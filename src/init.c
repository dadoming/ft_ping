#include "../include/ft_ping.h"

static void init_flags(t_ping *ping);

void init_program(t_ping *ping) {
	memset(ping, 0, sizeof(t_ping));
	if (!ping) {
		perror("malloc() failed");
		exit_program(ping);
	}
	init_flags(ping);
	ping->ttl = DEFAULT_TIME_TO_LIVE;
	bzero(&ping->stats, sizeof(t_ping_stats));
	ping->stats.all_rtt = malloc(sizeof(double));
	if (!ping->stats.all_rtt) {
		perror("malloc() failed");
		exit_program(ping);
	}
}

static void init_flags(t_ping *ping) {
	ping->flags.c.value = 0;
	ping->flags.c.name = FLAG_COUNT;
	ping->flags.c.entered = false;

	ping->flags.v.value = 0;
	ping->flags.v.name = FLAG_VERBOSE;
	ping->flags.v.entered = false;

	ping->flags.q.value = 0;
	ping->flags.q.name = FLAG_QUIET;
	ping->flags.q.entered = false;

	ping->flags.n.value = 0;
	ping->flags.n.name = FLAG_NUMERIC;
	ping->flags.n.entered = false;
}
