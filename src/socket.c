#include "../include/ft_ping.h"

void setup_socket(t_ping *ping) {
	int sock_fd;
	sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock_fd < 0) {
		perror("socket() failed");
		exit_program(ping);
	}

	t_sockaddr_in addr_con;
	memset(&addr_con, 0, sizeof(addr_con));
	addr_con.sin_family = AF_INET;
	addr_con.sin_addr.s_addr = inet_addr(ping->destination_addr);

}
