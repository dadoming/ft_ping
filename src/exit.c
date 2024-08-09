# include "../include/ft_ping.h"

void exit_program(t_ping *ping) {
	// Add frees here
	if (!ping) {
		exit(0);
	}
	if (ping->sock_fd >= 0) {
		close(ping->sock_fd);
	}
	if (ping->reverse_hostname) {
		free(ping->reverse_hostname);
	}
	if (ping->stats.all_rtt) {
		free(ping->stats.all_rtt);
	}
	exit(0);
}

void usage_message(char *program_name, int where) {
	if (DEBUG) {
		printf("Where: %d\n", where);
	}
	printf("Usage: %s [options -%s] destination_address\n", program_name, AVAILABLE_FLAGS);
}
