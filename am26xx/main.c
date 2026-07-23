#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_config.h"
#include "ti_board_open_close.h"

void wolfssl_platform_init(void);

extern int wolfcrypt_test_main(int argc, char** argv);
extern int wolfcrypt_benchmark_main(int argc, char** argv);

int main(void)
{
    System_init();
    Board_init();
    Drivers_open();
    Board_driversOpen();
    wolfssl_platform_init();

#if defined(WOLFCRYPT_BENCHMARK)
    wolfcrypt_benchmark_main(0, NULL);
#else
    wolfcrypt_test_main(0, NULL);
#endif

    Board_driversClose();
    Drivers_close();
    Board_deinit();
    System_deinit();

    return 0;
}
