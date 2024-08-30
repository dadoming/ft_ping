#include "../include/ft_ping.h"

//static int resolve_hostname(const char *hostname, char *ip_str, size_t max_len);
static void dns_lookup(t_ping *ping);
static void reverse_dns_lookup(t_ping *ping);

void setup_socket(t_ping *ping) {
    int sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock_fd < 0) {
		perror("socket() failed");
		exit_program(ping);
	}
	ping->sock_fd = sock_fd;

    dns_lookup(ping);
    if (ping->flags.n.entered == false) {
        reverse_dns_lookup(ping);
    } else { // if -n flag is entered then reverse_hostname does not need to be resolved
        ping->reverse_hostname = NULL;
    }

    if (setsockopt(ping->sock_fd, SOL_IP, IP_TTL, &(ping->ttl), sizeof(ping->ttl)) != 0) {
        printf("\nSetting socket options to TTL failed!\n");
        exit_program(ping);
    }

    struct timeval tv_out = {RECV_TIMEOUT, 0};
    if (setsockopt(ping->sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out)) < 0) {
        perror("setsockopt() failed");
        exit_program(ping);
    }
}

static void dns_lookup(t_ping *ping) {
    struct hostent *host_entity = gethostbyname(ping->dest_addr);
    if (host_entity == NULL) {
        fprintf(stderr, "Failed to resolve hostname: %s\n", ping->dest_addr);
        exit_program(ping);
    }

    char *ip = inet_ntoa(*(struct in_addr *) host_entity->h_addr);
    ping->addr_con.sin_family = host_entity->h_addrtype;
    ping->addr_con.sin_port = htons(PORT_NO);
    ping->addr_con.sin_addr.s_addr = *(long *) host_entity->h_addr;
    ping->ip_addr = ip;
}

static void reverse_dns_lookup(t_ping *ping) {
    t_sockaddr_in temp;
    socklen_t len;
    char buf[NI_MAXHOST];

    temp.sin_family = AF_INET;
    temp.sin_addr.s_addr = inet_addr(ping->ip_addr);
    len = sizeof(t_sockaddr_in);

    if (getnameinfo((struct sockaddr *) &temp, len, buf, sizeof(buf), NULL, 0, NI_NAMEREQD)) {
        printf("\nCould not resolve reverse lookup of hostname\n");
        exit_program(ping);
    }
    ping->reverse_hostname = strdup(buf);
}
