#include "../include/ft_ping.h"

// use volatile for the loop is_running variable to prevent compiler optimizations:
// https://stackoverflow.com/questions/246127/why-is-volatile-needed-in-c


int main(int argc, char **argv) {
	t_ping ping;

	init_program(&ping);
	parse_input(&ping, argc, argv);
	setup_socket(&ping);
	start_ping(&ping);
	exit_program(&ping);
	return 0;
}
