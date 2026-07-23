#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include <time.h>

struct timeval {
    long tv_sec;
    long tv_usec;
};

#define CLOCK_REALTIME 0

int clock_gettime(int clk_id, struct timespec *ts);

#endif /* _SYS_TIME_H_ */
