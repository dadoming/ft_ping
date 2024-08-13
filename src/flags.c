# include "../include/ft_ping.h"

int count_flag(t_ping *ping) {
    if (ping->flags.c.entered) {
        return ping->flags.c.value > ping->stats.received && ping->flags.c.value > ping->stats.sent;
    }
    return true;
}

int verbose_flag(t_ping *ping) {
    return ping->flags.v.entered;
}

int quiet_flag(t_ping *ping) {
    return ping->flags.q.entered;
}

int numeric_flag(t_ping *ping) {
    return ping->flags.n.entered;
}

void help_flag(t_ping *ping, int argc, char **argv) {
    int i = 1;
	while (i < argc) {
		if (argv[i][0] == '-' && argv[i][1] && argv[i][1] == 'h' && !argv[i][2]) {
			printf("\n\
Usage:\n\
  ./ping [options] <destination>\n\
\n\
Options:\n\
  <destination>\t\tdns name or ip address\n\
  -c <count>\t\tstop after <count> replies\n\
  -v\t\t\tverbose output (Print details of each packet)\n\
  -q\t\t\tquiet output (Packets are not printed)\n\
  -n\t\t\tnumeric output (No reverse hostname lookup)\n\
  -h\t\t\thelp\n\
 \n");
			exit_program(ping);
		}
        i++;
	}
}
