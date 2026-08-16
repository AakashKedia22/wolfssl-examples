/* wc_local_AesGcmCheckTagSz() is normally defined in wolfcrypt/src/aes.c,
 * but that file is excluded from this build when ENABLE_HW_AES=yes (its AES
 * implementation is replaced by wolfcrypt/src/port/ti-am26xx/ti-aes.c, which
 * calls this helper but does not define it itself). Reproduced verbatim
 * from aes.c so tag-size validation stays identical to the software path.
 */
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/misc.h>

int wc_local_AesGcmCheckTagSz(word32 authTagSz) {
#ifdef WC_AES_GCM_ALLOW_NONSTANDARD_TAG_LENGTH
    #ifdef HAVE_FIPS
        #error WC_AES_GCM_ALLOW_NONSTANDARD_TAG_LENGTH not allowed with FIPS 140.
    #endif
    wc_static_assert(WOLFSSL_MIN_AUTH_TAG_SZ >= 4);
    if ((authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ) ||
        (authTagSz > WC_AES_BLOCK_SIZE))
    {
        WOLFSSL_MSG("AES-GCM unsupported authTagSz");
        return BAD_FUNC_ARG;
    }
    else
        return 0;
#else
    switch (authTagSz) {
#if WOLFSSL_MIN_AUTH_TAG_SZ <= 4
    case 4:
#endif
#if WOLFSSL_MIN_AUTH_TAG_SZ <= 8
    case 8:
#endif
#if WOLFSSL_MIN_AUTH_TAG_SZ <= 12
    case 12:
#endif
#if WOLFSSL_MIN_AUTH_TAG_SZ <= 13
    case 13:
#endif
#if WOLFSSL_MIN_AUTH_TAG_SZ <= 14
    case 14:
#endif
#if WOLFSSL_MIN_AUTH_TAG_SZ <= 15
    case 15:
#endif
#if WOLFSSL_MIN_AUTH_TAG_SZ <= 16
    case 16:
#endif
        return 0;
    default:
        WOLFSSL_MSG("AES-GCM unsupported authTagSz");
        return BAD_FUNC_ARG;
    }
#endif
}
