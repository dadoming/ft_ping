# include "../include/ft_ping.h"

void exit_program(t_ping *ping) {
	// Add frees here
	(void)ping;

	exit(EXIT_FAILURE);
}

void usage_message(char *program_name, int where) {
	if (DEBUG) {
		printf("Where: %d\n", where);
	}
	printf("Usage: %s [options -%s] destination_address\n", program_name, AVAILABLE_FLAGS);
}
