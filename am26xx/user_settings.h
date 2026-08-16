/* user_settings.h
 *
 * Custom configuration for pure software wolfCrypt on TI AM26xx (Sitara MCU).
 * Enabled via WOLFSSL_USER_SETTINGS.
 *
 * Copyright (C) 2025 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#ifndef H_USER_SETTINGS_
#define H_USER_SETTINGS_

/* ---- System ---- */
#define WOLFSSL_GENERAL_ALIGNMENT   4
#define SINGLE_THREADED
#define WOLFCRYPT_ONLY
#define SIZEOF_LONG_LONG           8
#define NO_WOLFSSL_MEMORY
#define WOLFSSL_NO_CURRDIR

/* HSM M4 heap is only 16KB (see --heap_size in ti-arm-clang/linker.cmd);
 * without this the benchmark defaults to 1MB test buffers and its
 * malloc()/aligned_alloc() calls fail immediately. */
#define BENCH_EMBEDDED

/* ---- Debug output -- route printf through the TIFS debug log ----
 * <stdio.h> is pre-included here so its own printf prototype is parsed
 * before the macro below exists; otherwise later includes of <stdio.h>
 * (e.g. from wolfssl/wolfcrypt/logging.h) would macro-expand their own
 * printf declaration and fail to compile. */
#include <stdio.h>
#include <kernel/dpl/DebugP.h>
#ifdef printf
#undef printf
#endif
#define printf(format, ...) DebugP_log(format, ##__VA_ARGS__)

/* ---- Math -- SP Math (optimized for Cortex-R5) ---- */
#define WOLFSSL_SP
#define WOLFSSL_SP_MATH
#define WOLFSSL_SP_SMALL
#define SP_WORD_SIZE                32

/* ---- ECC ---- */
#ifndef WOLFCRYPT_DISABLE_ECC
#define HAVE_ECC
#define ECC_TIMING_RESISTANT
#define HAVE_ECC256
#define WOLFSSL_HAVE_SP_ECC
#endif

/* ---- RSA ---- */
#define HAVE_RSA
#define RSA_LOW_MEM
#define WOLFSSL_HAVE_SP_RSA
#define WC_RSA_BLINDING

/* ---- SHA ---- */
#define WOLFSSL_SHA256
#ifndef WOLFCRYPT_DISABLE_SHA512
#define WOLFSSL_SHA512
#endif

/* ---- AES ---- */
#ifndef NO_AES
#define HAVE_AESGCM
#define HAVE_AES_ECB
#define WOLFSSL_AES_DIRECT
#define HAVE_AES_CBC
#define WOLFSSL_AES_COUNTER
#endif

/* ---- Other crypto ---- */
#ifndef WOLFCRYPT_DISABLE_CHACHA
#define HAVE_CHACHA
#define HAVE_POLY1305
#endif
#define HAVE_PWDBASED
#define HAVE_HASHDRBG

/* ---- Disables -- for a minimal wolfCrypt build ---- */
#define WOLFSSL_NO_PEM
#define NO_RC4
#define NO_DSA
#define NO_MD4
#define NO_DH
#define NO_RABBIT
#define NO_SIG_WRAPPER
#define NO_CERT
#define NO_SESSION_CACHE
#define NO_HC128
#define NO_DES3
#define NO_WRITEV
#define NO_DEV_RANDOM
#define NO_FILESYSTEM
#define NO_OLD_RNGNAME
#define NO_WOLFSSL_DIR
#define WOLFSSL_NO_SOCK
#define WOLFSSL_IGNORE_FILE_WARN
#define NO_ERROR_STRINGS

/* ---- RNG -- TRNG via MCU+ SDK ---- */
int cust_rand_generate_block(unsigned char *rndb, unsigned int sz);
#define CUSTOM_RAND_GENERATE_BLOCK  cust_rand_generate_block
#ifndef WC_RESEED_INTERVAL
    #define WC_RESEED_INTERVAL (1000000)
#endif

#endif /* !H_USER_SETTINGS_ */
