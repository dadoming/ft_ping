#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define FLAG_COUNT "c"
#define FLAG_VERBOSE "v"
#define AVAILABLE_FLAGS FLAG_COUNT FLAG_VERBOSE

#define DEBUG true

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

typedef struct s_ping
{
	int sock_fd;
	char* destination_addr;

	t_sockaddr_in addr_con;
	t_flags flags;
} t_ping;

void init_program(t_ping *ping);

void parse_input(t_ping *ping, int argc, char **argv);

void exit_program(t_ping *ping);
void usage_message(char *program_name, int where);

void setup_socket(t_ping *ping);
