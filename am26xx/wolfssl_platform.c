#include <stdint.h>
#include <time.h>

#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/CycleCounterP.h>

/* HSM Cortex-M4 core clock, AM263Px */
#define CPU_FREQ_MHZ    200U

static uint64_t gRandState;

/* CycleCounterP_getCount32() wraps every ~21.4s at 200MHz; accumulate
 * deltas into a 64b running count so clock_gettime() stays monotonic
 * across a full benchmark run instead of jumping backwards on wrap. */
static uint64_t gTotalCycles;
static uint32_t gLastCycleCount;

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
    uint32_t count = CycleCounterP_getCount32();
    uint64_t nsec;
    (void)clk_id;

    gTotalCycles += (uint32_t)(count - gLastCycleCount);
    gLastCycleCount = count;

    nsec = (gTotalCycles * 1000ULL) / CPU_FREQ_MHZ;
    ts->tv_sec = nsec / 1000000000ULL;
    ts->tv_nsec = nsec % 1000000000ULL;
    return 0;
}

void wolfssl_platform_init(void)
{
    gRandState = ClockP_getTicks();
    CycleCounterP_reset();
    gLastCycleCount = CycleCounterP_getCount32();
    gTotalCycles = 0;
}
