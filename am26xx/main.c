#include <string.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include "tifs_modules_config.h"
#include <secure_ipc_notify/sipc_notify.h>
#include <modules/hsmserver/hsmserver.h>

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

#if defined(WOLFCRYPT_BENCHMARK)
    wolfcrypt_benchmark_main(0, NULL);
#else
    wolfcrypt_test_main(0, NULL);
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
