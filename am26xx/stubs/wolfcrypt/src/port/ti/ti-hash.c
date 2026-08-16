/* Empty override for wolfcrypt/src/port/ti/ti-hash.c.
 *
 * See ti-ccm.c in this same directory for why this shadow exists: wc_port.c
 * unconditionally includes both old CC3220/TM4C TI port files whenever
 * WOLFSSL_TI_CRYPT or WOLFSSL_TI_HASH is defined, and this project doesn't
 * use or build that port (no TivaWare driverlib in the TIFS/MCU+ tree).
 */
