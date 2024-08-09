#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <math.h>

#include <stdbool.h>

#define FLAG_COUNT "c"
#define FLAG_VERBOSE "v"
#define AVAILABLE_FLAGS FLAG_COUNT FLAG_VERBOSE

#define DEBUG true

#define PORT_NO 0 // Automatic port number
#define PING_DEFAULT_INTERVAL 1000000
// Gives the timeout delay for receiving packets
// in seconds
#define RECV_TIMEOUT 1
#define PING_PACKET_SIZE 64
#define DEFAULT_TIME_TO_LIVE 64

typedef struct sockaddr_in t_sockaddr_in;

typedef struct s_flag
{
	long value;
	bool entered;
	char *name;
} t_flag;

typedef struct s_flags
{
	t_flag c;
	t_flag v;
} t_flags;

typedef struct s_ping_pkt
{
	struct icmphdr hdr;
	char msg[PING_PACKET_SIZE - sizeof(struct icmphdr)];
} t_ping_pkt;

typedef struct s_ping_stats
{
	int sent;
	int received;
	double min;
	double max;
	double avg;
	double *all_rtt;
} t_ping_stats;

typedef struct s_timing
{
	//struct timespec start;
	//struct timespec end;
	struct timeval start;
	struct timeval end;
} t_timing;

typedef struct s_ping
{
	int sock_fd;
	int ttl;
	char *dest_addr;
	char *ip_addr;
	char *reverse_hostname;
	t_ping_stats stats;
	uint16_t pid;
	t_sockaddr_in addr_con;
	t_flags flags;
} t_ping;

void init_program(t_ping *ping);

void parse_input(t_ping *ping, int argc, char **argv);

void exit_program(t_ping *ping);
void usage_message(char *program_name, int where);

void setup_socket(t_ping *ping);

void start_ping(t_ping *ping);

//double calculate_elapsed_time(struct timespec start, struct timespec end);
double calculate_elapsed_time(struct timeval start, struct timeval end);
void get_current_time(struct timeval *tv);
