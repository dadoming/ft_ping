#include "../include/ft_ping.h"

// Converts the time from start to end into milliseconds
double calculate_elapsed_time(struct timeval start, struct timeval end) {
	return (double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_usec - start.tv_usec) / 1000.0;
}
void get_current_time(struct timeval *tv) {
    gettimeofday(tv, NULL);
}
