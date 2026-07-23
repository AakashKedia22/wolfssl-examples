#include <stdint.h>
#include <time.h>

#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/ClockP.h>

static uint64_t gRandState;

int cust_rand_generate_block(unsigned char *rndb, unsigned int sz)
{
    unsigned int i;
    if (rndb == NULL || sz == 0)
        return -1;
    for (i = 0; i < sz; i++) {
        gRandState ^= gRandState << 13;
        gRandState ^= gRandState >> 7;
        gRandState ^= gRandState << 17;
        rndb[i] = (unsigned char)(gRandState >> 32);
    }
    return 0;
}

int clock_gettime(int clk_id, struct timespec *ts)
{
    uint32_t ticks = ClockP_getTicks();
    uint64_t usec = ClockP_ticksToUsec(ticks);
    (void)clk_id;
    ts->tv_sec = usec / 1000000;
    ts->tv_nsec = (usec % 1000000) * 1000;
    return 0;
}

void wolfssl_platform_init(void)
{
    gRandState = ClockP_getTicks();
}
