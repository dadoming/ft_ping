#include "../include/ft_ping.h"

int main(int argc, char **argv) {
	t_ping ping;

	init_program(&ping);
	parse_input(&ping, argc, argv);
	setup_socket(&ping);

}
