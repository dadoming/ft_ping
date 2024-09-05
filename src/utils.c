#include "../include/ft_ping.h"

// Converts the time from start to end into milliseconds
double calculate_elapsed_time(struct timeval start, struct timeval end) {
	return (double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_usec - start.tv_usec) / 1000.0;
}
void get_current_time(struct timeval *tv) {
    gettimeofday(tv, NULL);
}

// Calculate checksum for the ICMP packet
unsigned short calculate_checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *) buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// Fill the packet with the ICMP header
void fill_packet_header(t_ping_pkt *packet, int seq, int pid) {
    packet->hdr.type = ICMP_ECHO;
    packet->hdr.code = 0;
    packet->hdr.un.echo.id = pid;
    packet->hdr.un.echo.sequence = seq + 1;
    packet->hdr.checksum = calculate_checksum(packet, sizeof(*packet));
}
