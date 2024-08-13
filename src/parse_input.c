#include "../include/ft_ping.h"

static long convert_flag_value_to_number(t_flag *flag, char *value_str);
static void enter_flag_data_with_value(t_flag *flag, char **argv, int argc, int *i);
static void check_invalid_input_arguments(t_ping *ping, char **argv, int argc);
static void set_destination_address(t_ping *ping, char **argv, int argc, int i);

void invalid_value(t_ping *ping, long value, char *flag, char *extra) {
	printf("Invalid value for flag -%s: %ld", flag, value);
	printf("%s\n", extra ? extra : "");
	exit_program(ping);
}

void parse_input(t_ping *ping, int argc, char **argv) {
	check_invalid_input_arguments(ping, argv, argc);
	help_flag(ping, argc, argv);

	int i = 1;
	while (i < argc - 1) {
		if (argv[i][0] != '-') {
			usage_message(argv[0], 3);
			exit_program(ping);
		}
		if (argv[i][0] == '-' && argv[i][1]) {
			switch (argv[i][1]) {
				case 'c':
					enter_flag_data_with_value(&(ping->flags.c), argv, argc, &i);
					break;
				case 'v':
					ping->flags.v.entered = true;
					break;
				case 'q':
					ping->flags.q.entered = true;
					break;
				case 'n':
					ping->flags.n.entered = true;
					break;
				default:
					printf("Unknown flag: %s\n", argv[i]);
					usage_message(argv[0], 4);
					exit_program(ping);
			}
		}
		i++;
	}
	set_destination_address(ping, argv, argc, i);
}

static void check_invalid_input_arguments(t_ping *ping, char **argv, int argc) {
	if (argc < 2) {
		usage_message(argv[0], 0);
		exit_program(ping);
	}
}

static void enter_flag_data_with_value(t_flag *flag, char **argv, int argc, int *i) {
	if (*i + 1 < argc) { // Because last argument is destination address, so, if required, there should be a value after the flag
		*i += 1;
		flag->value = convert_flag_value_to_number(flag, argv[*i]);
		flag->entered = true;
	} else {
		printf("Flag %s requires a value\n", flag->name);
		exit_program(NULL);
	}
}

static void set_destination_address(t_ping *ping, char **argv, int argc, int i) {
	if (i >= argc) {
		printf("Destination address not provided\n");
		usage_message(argv[0], 5);
		exit_program(ping);
	} else if (argv[i][0] != '-') {
		ping->dest_addr = argv[i];
	} else {
		usage_message(argv[0], 2);
		exit_program(ping);
	}
}

static long convert_flag_value_to_number(t_flag *flag, char *value_str) {
	char *endptr;
	long value = strtol(value_str, &endptr, 10);
	if (*endptr != '\0' || value < 0) {
		printf("Invalid value for flag -%s: %s\n", flag->name, value_str);
		exit_program(NULL);
	}
	return value;
}
