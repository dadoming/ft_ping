#include "../include/ft_ping.h"
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

static void send_ping(t_ping *ping, t_timing *single);
static void receive_ping(t_ping *ping, t_timing *single);
static void handle_alarm(int sig);
static void handle_interrupt(int sig);

int g_is_running = true;
static t_ping *global_ping = NULL;
static t_timing single;
static t_timing session;

void start_ping(t_ping *ping) {
    global_ping = ping;

    signal(SIGALRM, handle_alarm);
    signal(SIGINT, handle_interrupt);

    printf("PING %s(%s (%s)) %ld data bytes.\n", \
        ping->dest_addr, ping->reverse_hostname, \
        ping->ip_addr, PING_PACKET_SIZE - sizeof(struct icmphdr));

    handle_alarm(SIGALRM); // Trigger the first ping
    while (g_is_running && count_flag(ping)) {
        pause();
	}

    get_current_time(&(session.end));
    ping->stats.total_msec = calculate_elapsed_time(session.start, session.end);

    calculate_statistics(ping);
    print_statistics(ping);
}

static void send_ping(t_ping *ping, t_timing *single) {
    t_ping_pkt packet;

    bzero(&packet, sizeof(packet));
    fill_packet_header(&packet, ping->stats.sent);
    get_current_time(&(single->start));
    if (sendto(ping->sock_fd, &packet, sizeof(packet), 0, (struct sockaddr *) &ping->addr_con, sizeof(ping->addr_con)) <= 0) {
        perror("sendto() failed");
		return;
    }
	if (++(ping->stats.sent) == 1) {
		get_current_time(&(session.start));
	}
}

static void receive_ping(t_ping *ping, t_timing *single) {
    t_sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    char buffer[1024];

    ssize_t bytes_received = recvfrom(ping->sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &recv_addr, &addr_len);
    if (bytes_received <= 0) {
        // return ; uncomment for evaluation
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
        ping->stats.received++;
        printf("%ld bytes from %s (%s): icmp_seq=%d ttl=%d time=%.3f ms\n", bytes_received - sizeof(struct iphdr), ping->reverse_hostname, ping->ip_addr, icmp_hdr->un.echo.sequence, ip_hdr->ttl, rtt_msec);
        if (ping->stats.min == 0 || rtt_msec < ping->stats.min) {
            ping->stats.min = rtt_msec;
        }
        if (rtt_msec > ping->stats.max) {
            ping->stats.max = rtt_msec;
        }
        ping->stats.avg += rtt_msec;
        ping->stats.all_rtt = realloc(ping->stats.all_rtt, ping->stats.received * sizeof(double));
        ping->stats.all_rtt[ping->stats.received - 1] = rtt_msec;
        // comment lines below for evaluation
    } else if (icmp_hdr->type == ICMP_DEST_UNREACH) {
        printf("Error..Packet received with ICMP type %d code %d\n", icmp_hdr->type, icmp_hdr->code);
    } else {
        printf("Received unexpected ICMP packet: type=%d code=%d\n", icmp_hdr->type, icmp_hdr->code);
    }
}


static void handle_alarm(int sig) {
    (void)sig;
    if (g_is_running && count_flag(global_ping)) {
        send_ping(global_ping, &single);
        receive_ping(global_ping, &single);
        alarm(PING_DEFAULT_INTERVAL_SEC);
    }
}

static void handle_interrupt(int sig) {
    (void)sig;
    g_is_running = false;
}
