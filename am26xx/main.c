#include <string.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include "tifs_modules_config.h"
#include <secure_ipc_notify/sipc_notify.h>
#include <modules/hsmserver/hsmserver.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfcrypt/src/port/ti-am26xx/ti-am26xx.h>
#ifdef WOLFSSL_TI_PKA
    #include <wolfssl/wolfcrypt/cryptocb.h>
    #if defined(WC_USE_DEVID) && (WC_USE_DEVID != WOLFSSL_TI_PKA_DEV_ID)
        #error "WC_USE_DEVID must equal WOLFSSL_TI_PKA_DEV_ID"
    #endif
#endif

void wolfssl_platform_init(void);

extern int wolfcrypt_test_main(int argc, char** argv);
extern int wolfcrypt_benchmark_main(int argc, char** argv);

/** common memory log base address */
#define MEMLOG_BASE_ADDRESS        (0x70080000U)

/** global memory log array */
extern char gDebugMemLog[DebugP_MEM_LOG_SIZE];

int main()
{
    System_init();
    Modules_init();
    Board_init();
    Drivers_open();
    Board_driversOpen();

    wolfssl_platform_init();

#ifdef WOLFSSL_TI_PKA
    {
        /* wolfCrypt_Init() first: it clears the crypto-callback table on its
         * first call only (test/benchmark call it again; it is refcounted). */
        int ret = wolfCrypt_Init();
        if (ret == 0)
            ret = wolfSSL_TI_PkaRegister(WOLFSSL_TI_PKA_DEV_ID);
        if (ret != 0)
            DebugP_log("TI PKA register failed: %d, continuing in software\r\n", ret);
    }
#endif

#if defined(WOLFCRYPT_BENCHMARK) && defined(WC_BENCH_ARGV)
    {
        /* BENCH_ARGS=... (Makefile): e.g. -asym, keeps output within the
         * 4KB DebugP memory log */
        char* argv[] = { (char*)"benchmark", WC_BENCH_ARGV };
        wolfcrypt_benchmark_main((int)(sizeof(argv) / sizeof(argv[0])), argv);
    }
#elif defined(WOLFCRYPT_BENCHMARK)
    wolfcrypt_benchmark_main(0, NULL);
#else
    wolfcrypt_test_main(0, NULL);
#endif

#ifdef WOLFSSL_TI_PKA
    wc_CryptoCb_UnRegisterDevice(WOLFSSL_TI_PKA_DEV_ID);
    wolfCrypt_Cleanup();
#endif

    HsmServer_sendBootNotify(SystemP_WAIT_FOREVER);

    /*copying Hsm memory log to R5 memory log*/
    memset((void *)MEMLOG_BASE_ADDRESS, 0x00, DebugP_MEM_LOG_SIZE);
    memcpy((void *)MEMLOG_BASE_ADDRESS, (void *)gDebugMemLog, DebugP_MEM_LOG_SIZE);

    Board_driversClose();
    Drivers_close();
    Board_deinit();
    Modules_deinit();
    System_deinit();

    return 0;
}
