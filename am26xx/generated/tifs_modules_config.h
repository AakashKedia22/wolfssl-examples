/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Auto generated file 
 */

#ifndef TIFS_MODULES_CONFIG_H_
#define TIFS_MODULES_CONFIG_H_

#include <stdint.h>
#include <drivers/hw_include/cslr_soc.h>
#include "ti_dpl_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Common Functions
 */
void Modules_init(void);
void Modules_deinit(void);

/*
 *  HSM Server
 */
#include <modules/hsmserver/hsmserver.h>

/* sysconfig generated parameter QUEUE LENGTH */
#define SIPC_QUEUE_LENGTH   (32u)
/* Total number of secure cores */
#define SIPC_NUM_R5_CORES   (2u)

/* -------------------------------------------------------------------------
 * Fault Injection Hardening (FIH) - SysConfig generated defines
 * ------------------------------------------------------------------------- */

/* Maximum number of NOP iterations for the FIH delay loop.
 * Actual loop count is randomised in [0, FIH_DELAY_LOOP_COUNT] when
 * FAULT_INJECTION_HARDENING_ENABLED is defined.
 * Wrapped in #undef / #define to override any fallback default that may
 * have been set by hsm_utils.h before this generated header was included. */
#undef  FIH_DELAY_LOOP_COUNT
#define FIH_DELAY_LOOP_COUNT    (16U)

/* Enable FIH macros (FIH_EQ, FIH_NOT_EQ, FIH_SET) and random delay in
 * HsmServer_FIH_delay(). */
#define FAULT_INJECTION_HARDENING_ENABLED

/* Use hardware RNG (RNG_setup / RNG_read) for the FIH delay random count.
 * When not defined, C stdlib rand() is used instead. */
#define HW_RANDOM_NUM

/* Enable secure asset and key erasure inside HsmServer_FIH_panic().
 * Overrides and locks the KEK, then zeroes keyring and asset structures. */
#define FIH_ERASE_SECURE_ASSETS

/*
 *  Keyring
 */
#include <modules/keyring/keyring.h>

/* sysconfig generated parameter KEYRING_ASYMM_SIZE */
#define KEYRING_ASYMM_SIZE   (48U)
/* sysconfig generated parameter KEYRING_SYMM_SIZE */
#define KEYRING_SYMM_SIZE   (48U)


#ifdef __cplusplus
}
#endif

#endif /* TIFS_MODULES_CONFIG_H_ */
