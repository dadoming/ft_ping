#include "../include/ft_ping.h"
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

// Function prototypes
static unsigned short calculate_checksum(void *b, int len);
static void send_ping(t_ping *ping, t_timing *single);
static void receive_ping(t_ping *ping, t_timing *single);
static void handle_alarm(int sig);
static void handle_interrupt(int sig);
int g_is_running = true;

// New global variables to hold the `t_ping` and `t_timing` structures
static t_ping *global_ping = NULL;
static t_timing single;

int count_flag(t_ping *ping) {
    if (ping->flags.c.entered) {
        return ping->flags.c.value > ping->stats.sent;
    }
    return true;
}

void start_ping(t_ping *ping) {
    global_ping = ping;

    // Set up the signal handlers
    signal(SIGALRM, handle_alarm);
    signal(SIGINT, handle_interrupt);

    // Start the pinging process
    t_timing session;
    get_current_time(&(session.start));
    printf("PING %s(%s (%s)) %ld data bytes.\n", ping->dest_addr, ping->reverse_hostname, ping->ip_addr, PING_PACKET_SIZE - sizeof(struct icmphdr));

    // Trigger the first ping immediately
    handle_alarm(SIGALRM);

    // Wait until interrupted
    while (g_is_running) {
        pause();  // Suspend the program until a signal is caught
		if (count_flag(global_ping) == false) {
			break;
		}
	}

    // Calculate total time and print statistics
    get_current_time(&(session.end));
    double total_msec = calculate_elapsed_time(session.start, session.end);

    printf("\n--- %s statistics ---\n", ping->dest_addr);
    if (ping->stats.sent > 0) {
        int packet_loss = (ping->stats.sent - ping->stats.received) * 100 / ping->stats.sent;
        printf("%d packets transmitted, %d received, %d%% packet loss, time %ldms\n", ping->stats.sent, ping->stats.received, packet_loss, (long)total_msec);
		double average = ping->stats.avg / ping->stats.received;
		// mdev = SQRT(SUM(RTT*RTT) / N – (SUM(RTT)/N)^2)
		double sum = 0;
		for (int i = 0; i < ping->stats.received; i++) {
			sum += ping->stats.all_rtt[i] * ping->stats.all_rtt[i];
		}
		double mdev = sqrt(sum / ping->stats.received - average * average);

		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", ping->stats.min, average, ping->stats.max, mdev);
	} else {
        printf("No packets were sent, time: %ldms\n", (long)total_msec);
    }
}

// Handle the alarm signal, sending and receiving a ping
static void handle_alarm(int sig) {
    (void)sig;  // Avoid unused parameter warning

    if (g_is_running && count_flag(global_ping)) {
        send_ping(global_ping, &single);
        receive_ping(global_ping, &single);
//        printf("start:\t%ld.%05ld\nend:\t%ld.%05ld\n\n", single.start.tv_sec, single.start.tv_usec, single.end.tv_sec, single.end.tv_usec);

        // Set the next alarm for PING_DEFAULT_INTERVAL microseconds
        alarm(PING_DEFAULT_INTERVAL / 1000000);
    }
}

// Handle the interrupt signal, stopping the ping process
static void handle_interrupt(int sig) {
    (void)sig;  // Avoid unused parameter warning
    g_is_running = false;
}

static void send_ping(t_ping *ping, t_timing *single) {
    t_ping_pkt packet;

    bzero(&packet, sizeof(packet));
    packet.hdr.type = ICMP_ECHO;
    packet.hdr.code = 0;
    packet.hdr.un.echo.id = getpid();
    packet.hdr.un.echo.sequence = ping->stats.sent++;
    size_t i;
    for (i = 0; i < sizeof(packet.msg) - 1; i++) {
        packet.msg[i] = i + '0';
    }
    packet.msg[i] = 0;
    packet.hdr.checksum = calculate_checksum(&packet, sizeof(packet));
    get_current_time(&(single->start));

    if (sendto(ping->sock_fd, &packet, sizeof(packet), 0, (struct sockaddr *) &ping->addr_con, sizeof(ping->addr_con)) <= 0) {
        perror("sendto() failed");
    }
}

static void receive_ping(t_ping *ping, t_timing *single) {
    t_sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    char buffer[1024];

    // Use select to add a timeout for recvfrom
    fd_set fds;
    struct timeval timeout;

    FD_ZERO(&fds);
    FD_SET(ping->sock_fd, &fds);

    timeout.tv_sec = 1;  // Wait for 1 second
    timeout.tv_usec = 0;

    int ret = select(ping->sock_fd + 1, &fds, NULL, NULL, &timeout);
    if (ret > 0) {
        ssize_t bytes_received = recvfrom(ping->sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &recv_addr, &addr_len);
        if (bytes_received <= 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Request timed out\n");
            } else {
                perror("recvfrom() failed");
            }
        }

        get_current_time(&(single->end));
        double rtt_msec = calculate_elapsed_time(single->start, single->end);

        struct iphdr *ip_hdr = (struct iphdr *) buffer;
        struct icmphdr *icmp_hdr = (struct icmphdr *) (buffer + (ip_hdr->ihl * 4));
        if (icmp_hdr->type == ICMP_ECHOREPLY) {
            printf("%ld bytes from %s (%s): icmp_seq=%d ttl=%d time=%.3f ms\n", bytes_received - sizeof(struct iphdr), ping->reverse_hostname, ping->ip_addr, icmp_hdr->un.echo.sequence, ip_hdr->ttl, rtt_msec);
            ping->stats.received++;
			if (ping->stats.min == 0 || rtt_msec < ping->stats.min) {
				ping->stats.min = rtt_msec;
			}
			if (rtt_msec > ping->stats.max) {
				ping->stats.max = rtt_msec;
			}
			ping->stats.avg += rtt_msec;
			ping->stats.all_rtt = realloc(ping->stats.all_rtt, ping->stats.received * sizeof(double));
			ping->stats.all_rtt[ping->stats.received - 1] = rtt_msec;
        } else if (icmp_hdr->type == ICMP_DEST_UNREACH) {
            printf("Error..Packet received with ICMP type %d code %d\n", icmp_hdr->type, icmp_hdr->code);
        } else {
            printf("Received unexpected ICMP packet: type=%d code=%d\n", icmp_hdr->type, icmp_hdr->code);
        }
    } else if (ret == 0) {
        printf("Request timed out\n");
    } else {
        perror("select() failed");
    }
}

// Calculate checksum for the ICMP packet
static unsigned short calculate_checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *) buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}
