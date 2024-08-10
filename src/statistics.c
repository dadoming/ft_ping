#include "../include/ft_ping.h"

void calculate_statistics(t_ping *ping) {
	if (ping->stats.sent > 0) {
        ping->stats.packet_loss = (ping->stats.sent - ping->stats.received) * 100 / ping->stats.sent;
		ping->stats.avg = ping->stats.avg / ping->stats.received;
		// mdev = SQRT(SUM(RTT*RTT) / N – (SUM(RTT)/N)^2)
		double sum = 0;
		for (int i = 0; i < ping->stats.received; i++) {
			sum += ping->stats.all_rtt[i] * ping->stats.all_rtt[i];
		}
		ping->stats.mdev = sqrt(sum / ping->stats.received - pow(ping->stats.avg, 2));
	}
}

void print_statistics(t_ping *ping) {
	printf("\n--- %s statistics ---\n", ping->dest_addr);
    if (ping->stats.sent > 0) {
        printf("%d packets transmitted, %d received, %d%% packet loss, time %ldms\n", ping->stats.sent, ping->stats.received, ping->stats.packet_loss, ping->stats.total_msec);
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", ping->stats.min, ping->stats.avg, ping->stats.max, ping->stats.mdev);
	} else {
        printf("No packets were sent, time: %ldms\n", ping->stats.total_msec);
    }
}
