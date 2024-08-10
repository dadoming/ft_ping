# include "../include/ft_ping.h"

int count_flag(t_ping *ping) {
    if (ping->flags.c.entered) {
        return ping->flags.c.value > ping->stats.sent;
    }
    return true;
}
