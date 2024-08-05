#include "../include/ft_ping.h"

static void init_flags(t_ping *ping);

void init_program(t_ping *ping) {
	memset(ping, 0, sizeof(t_ping));
	if (!ping) {
		perror("malloc() failed");
		exit_program(ping);
	}
	init_flags(ping);
}

static void init_flags(t_ping *ping) {
	ping->flags.c.value = 0;
	ping->flags.c.name = FLAG_COUNT;
	ping->flags.c.entered = false;

	ping->flags.v.value = 0;
	ping->flags.v.name = FLAG_VERBOSE;
	ping->flags.v.entered = true;
}
