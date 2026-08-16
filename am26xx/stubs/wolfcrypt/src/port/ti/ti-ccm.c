/* Empty override for wolfcrypt/src/port/ti/ti-ccm.c.
 *
 * wc_port.c unconditionally does:
 *   #include <wolfcrypt/src/port/ti/ti-ccm.c>
 * whenever WOLFSSL_TI_CRYPT or WOLFSSL_TI_HASH is defined. That file is the
 * old CC3220/TM4C (TivaWare "driverlib") TI port, which this project does
 * not use and cannot build (no driverlib headers in the TIFS/MCU+ tree).
 * It also defines wolfSSL_TI_CCMInit(), which would collide with the
 * AM263Px DTHE port's own definition in
 * wolfcrypt/src/port/ti-am26xx/ti-aes.c.
 *
 * -I./stubs is listed before -I$(WOLFSSL_PATH) in the Makefile, so this
 * empty file shadows the real one for the angle-bracket include above,
 * without touching the wolfSSL vendor tree. It has no effect on builds
 * that don't define WOLFSSL_TI_CRYPT/WOLFSSL_TI_HASH, since wc_port.c
 * skips the #include entirely in that case.
 */
