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

#ifndef TI_DRIVERS_CONFIG_H_
#define TI_DRIVERS_CONFIG_H_

#include <stdint.h>
#include <drivers/hw_include/cslr_soc.h>
#include <drivers/hw_include/hw_types.h>
#include "ti_dpl_config.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Common Functions
 */
void System_init(void);
void System_deinit(void);

/*
 *  DTHE
 */
#include <crypto/dthe/dthe.h>
#include <crypto/dthe/dthe_aes.h>
#include <crypto/dthe/dthe_sha.h>
#include <crypto/rng/rng.h>
#include <crypto/asym_crypt.h>
#include <crypto/dthe/dma/edma/dthe_edma.h>

/* Edma macros */
#define EDMA_TEST_EVT_QUEUE_NO                        (0U)
#define CONFIG_EDMA_NUM_INSTANCES                     (1U)
#define HSM_TPCC_A_INTAGG_MASK                        (0x470000E0)
#define HSM_TPCC_A_INTAGG_STATUS                      (0x470000E4)
#define HSM_TPCC_A_INTAGG                             (18U + 16U)

/* Edma handler*/
EDMA_Handle gEdmaHandle[CONFIG_EDMA_NUM_INSTANCES];


/*
 * MPU Firewall
 */
#include <drivers/mpu_firewall.h>

/* MPU Firewall Macros */
#define CONFIG_MPU_FIREWALL_NUM_REGIONS (66U)



#include <drivers/soc.h>
#include <kernel/dpl/CycleCounterP.h>

/*
 * MCU_LBIST
 */
void SDL_lbist_selftest(void);

#ifdef __cplusplus
}
#endif

#endif /* TI_DRIVERS_CONFIG_H_ */
