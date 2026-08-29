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

#include "tifs_modules_config.h"
/*
 * HSM server
 */

/* memory assigned for each R5x <-> HSM channel */
uint8_t gQueue_HsmToSecureHost[SIPC_NUM_R5_CORES][SIPC_QUEUE_LENGTH*SIPC_MSG_SIZE] __attribute__((aligned(8),section(".bss.sipc_hsm_queue_mem")));
uint8_t gQueue_SecureHostToHsm[SIPC_NUM_R5_CORES][SIPC_QUEUE_LENGTH*SIPC_MSG_SIZE] __attribute__((aligned(8),section(".bss.sipc_secure_host_queue_mem")));

void HsmServer_config(void);
void HsmServer_config(void)
{
    SIPC_Params sipcParams;
    int32_t status;

    /* initialize parameters to default */
    SIPC_Params_init(&sipcParams);

    sipcParams.ipcQueue_eleSize_inBytes = SIPC_MSG_SIZE;
    sipcParams.ipcQueue_length = SIPC_QUEUE_LENGTH ;
    /* list the cores that will do IPC Notify with this core
    * Make sure to NOT list 'self' core in the list below
    */
    sipcParams.numCores = 2;

    /* specify the priority of SIPC Notify interrupt */
    sipcParams.intrPriority = 7U;
    
    sipcParams.coreIdList[0] = CORE_INDEX_SEC_MASTER_0 ;
    /* This is HSM -> R5F queue */
    sipcParams.tx_SipcQueues[CORE_INDEX_SEC_MASTER_0] = (uintptr_t)gQueue_HsmToSecureHost[0] ;

    sipcParams.rx_SipcQueues[CORE_INDEX_SEC_MASTER_0] = (uintptr_t)gQueue_SecureHostToHsm[0] ;
    sipcParams.secHostCoreId[CORE_INDEX_SEC_MASTER_0] = CORE_ID_R5FSS0_0;
    sipcParams.coreIdList[1] = CORE_INDEX_SEC_MASTER_1 ;
    /* This is HSM -> R5F queue */
    sipcParams.tx_SipcQueues[CORE_INDEX_SEC_MASTER_1] = (uintptr_t)gQueue_HsmToSecureHost[1] ;

    /* This will be R5 -> HSM queue */
    sipcParams.rx_SipcQueues[CORE_INDEX_SEC_MASTER_1] = (uintptr_t)gQueue_SecureHostToHsm[1] ;
    sipcParams.secHostCoreId[CORE_INDEX_SEC_MASTER_1] = CORE_ID_R5FSS0_1 ;

    status = HsmServer_init(&sipcParams);
    DebugP_assert(status==SystemP_SUCCESS);
}

/**
 * \brief  Pointer to the keyring information structure 
 */
Keyring_Info_t   gKeyringInfo __attribute__((section(".keyring_info_mem")));

/**
 * \brief Pointer to the Asymmetric public key hash at index 0
 */
Keyring_AsymmKey_t   gKeyringAsymmKey[KEYRING_ASYMM_SIZE] __attribute__((section(".keyring_asymm_mem")));

/**
 * \brief  Pointer to the Asymmetric key at index 0
 */
Keyring_SymmKey_t   gKeyringSymmKey[KEYRING_SYMM_SIZE] __attribute__((section(".keyring_symm_mem")));

/**
 * \brief  Pointer to the Asymmetric private key at index 0
 */
Keyring_AsymmPrivKey_t   gKeyringAsymmPrivKey[KEYRING_ASYMM_PRIVATE_SIZE] __attribute__((section(".keyring_asymm_private_mem")));

/**
 * \brief  Pointer to the custom keyring data at index 0
 */
uint8_t gKeyringCustomData[KEYRING_CUSTOM_DATA_SIZE] __attribute__((section(".keyring_custom_data_mem")));


extern Asset_Info gAssetInfo;

/* ========================================================================== */
/*                        Function Prototypes                                 */
/* ========================================================================== */


void KEYRING_populate_root_keys(uint8_t keyRevision, uint8_t keyCount);


void KEYRING_init_secMgrBased(void)
{
    uint8_t keyRevision;
    uint8_t keyCount;


    /* fetch key revision from secure manager */
	SecMgr_getKeyRevision(&keyRevision);

    /* fetch key count from secure manager */
    SecMgr_getKeyCount(&keyCount);

    /* Populate root keys */
    KEYRING_populate_root_keys(keyRevision, keyCount);


}

void KEYRING_init(void) {

    KEYRING_init_secMgrBased();
}

void KEYRING_populate_root_keys(uint8_t keyRevision, uint8_t keyCount) {
    switch (keyCount)
    {
        case 0U:
            /* Keys are not programmed */
            break;
        case 2U:
            /* BMPK can be used for both application authentication and debug authorization 
            0bXXXXXXXX_XXXXXXXX_XXXXXXXX_XXXXXXXX
            <-----------rsvd(8b)------>|<-- imageAuth(4b)-->|<-- debugAuth (4b) →   */
            gKeyringAsymmKey[KEYRING_BMPKH_INDEX].keyRights  = KEYRING_BMPKH_KEY_RIGHTS;
            gKeyringAsymmKey[KEYRING_BMPKH_INDEX].keyId  = KEYRING_BMPKH_INDEX;
            /* Populate Hash Algorithm for BMPK */
            gKeyringAsymmKey[KEYRING_BMPKH_INDEX].hashAlgo = HASH_ALGO_SHA2_512;
            /* Populate BMPKH from secure manager at index 1 in keyring */
            SecMgr_enableReadAccess(CSL_SecMgrDeviceKey_CUST_BMPK1);
            SecMgr_getKey(CSL_SecMgrDeviceKey_CUST_BMPK1, (uint32_t *) &gKeyringAsymmKey[KEYRING_BMPKH_INDEX].publicKeyHash);
            SecMgr_enableReadAccess(CSL_SecMgrDeviceKey_CUST_BMPK2);
            SecMgr_getKey(CSL_SecMgrDeviceKey_CUST_BMPK2, (uint32_t *) &gKeyringAsymmKey[KEYRING_BMPKH_INDEX].publicKeyHash[8U]);
            SecMgr_disableReadAccess(CSL_SecMgrDeviceKey_CUST_BMPK1);
            SecMgr_disableReadAccess(CSL_SecMgrDeviceKey_CUST_BMPK2);
            /* SMPK can be used for both application authentication and debug authorization 
            0bXXXXXXXX_XXXXXXXX_XXXXXXXX_XXXXXXXX
            <-----------rsvd(8b)------>|<-- imageAuth(4b)-->|<-- debugAuth (4b) →   */
            gKeyringAsymmKey[KEYRING_SMPKH_INDEX].keyRights  = KEYRING_SMPKH_KEY_RIGHTS;
            gKeyringAsymmKey[KEYRING_SMPKH_INDEX].keyId  = KEYRING_SMPKH_INDEX;
            /* Populate Hash Algorithm for SMPK */
            gKeyringAsymmKey[KEYRING_SMPKH_INDEX].hashAlgo = HASH_ALGO_SHA2_512;
            /* Populate SMPKH from secure manager at index 0 in keyring */
            SecMgr_enableReadAccess(CSL_SecMgrDeviceKey_CUST_SMPK1);
            SecMgr_getKey(CSL_SecMgrDeviceKey_CUST_SMPK1, (uint32_t *) &gKeyringAsymmKey[KEYRING_SMPKH_INDEX].publicKeyHash);
            SecMgr_enableReadAccess(CSL_SecMgrDeviceKey_CUST_SMPK2);
            SecMgr_getKey(CSL_SecMgrDeviceKey_CUST_SMPK2, (uint32_t *) &gKeyringAsymmKey[KEYRING_SMPKH_INDEX].publicKeyHash[8U]);
            SecMgr_disableReadAccess(CSL_SecMgrDeviceKey_CUST_SMPK1);
            SecMgr_disableReadAccess(CSL_SecMgrDeviceKey_CUST_SMPK2);
            if(FIH_EQ(keyRevision, 2U))
            {
                if(SecMgr_isLocked(CSL_SecMgrDeviceKey_CUST_BMEK) == SEC_MGR_BOOL_FALSE)
                {
                     /* Populate BMEK in keyring */
                    gKeyringSymmKey[KEYRING_BMEK_INDEX].keyRights  = KEYRING_BMEK_KEY_RIGHTS;
                    gKeyringSymmKey[KEYRING_BMEK_INDEX].keyLength  = sizeof(gKeyringSymmKey[KEYRING_BMEK_INDEX].aesKey);
                    SecMgr_enableReadAccess(CSL_SecMgrDeviceKey_CUST_BMEK);
                    SecMgr_getKey(CSL_SecMgrDeviceKey_CUST_BMEK, (uint32_t*)&gKeyringSymmKey[KEYRING_BMEK_INDEX].aesKey);
                    SecMgr_disableReadAccess(CSL_SecMgrDeviceKey_CUST_BMEK);
                }
                gKeyringSymmKey[KEYRING_DBMEK_INDEX].keyId  = KEYRING_DBMEK_INDEX;
                gKeyringSymmKey[KEYRING_DBMEK_INDEX].keyRights  = KEYRING_BMEK_KEY_RIGHTS;
                gKeyringSymmKey[KEYRING_DBMEK_INDEX].keyLength  = sizeof(gKeyringSymmKey[KEYRING_DBMEK_INDEX].aesKey);
                /* Populate derived active root key from Assets */
                (void) HsmServer_memcpy((void *) &gKeyringSymmKey[KEYRING_DBMEK_INDEX].aesKey, 
                                        sizeof(gKeyringSymmKey[KEYRING_DBMEK_INDEX].aesKey), 
                                        (void *) gAssetInfo.derivedKey, 
                                        sizeof(gKeyringSymmKey[KEYRING_DBMEK_INDEX].aesKey));
            } else if(FIH_EQ(keyRevision, 1U)) {
                if(SecMgr_isLocked(CSL_SecMgrDeviceKey_CUST_SMEK) == SEC_MGR_BOOL_FALSE) {
                    /* Populate SMEK in keyring */
                    gKeyringSymmKey[KEYRING_SMEK_INDEX].keyId  = KEYRING_SMEK_INDEX;
                    gKeyringSymmKey[KEYRING_SMEK_INDEX].keyRights  = KEYRING_SMEK_KEY_RIGHTS;
                    gKeyringSymmKey[KEYRING_SMEK_INDEX].keyLength  = sizeof(gKeyringSymmKey[KEYRING_SMEK_INDEX].aesKey);
                    SecMgr_enableReadAccess(CSL_SecMgrDeviceKey_CUST_SMEK);
                    SecMgr_getKey(CSL_SecMgrDeviceKey_CUST_SMEK, (uint32_t*)&gKeyringSymmKey[KEYRING_SMEK_INDEX].aesKey);
                    SecMgr_disableReadAccess(CSL_SecMgrDeviceKey_CUST_SMEK);
                }
                gKeyringSymmKey[KEYRING_DSMEK_INDEX].keyId  = KEYRING_DSMEK_INDEX;
                gKeyringSymmKey[KEYRING_DSMEK_INDEX].keyRights  = KEYRING_SMEK_KEY_RIGHTS;
                gKeyringSymmKey[KEYRING_DSMEK_INDEX].keyLength  = sizeof(gKeyringSymmKey[KEYRING_DSMEK_INDEX].aesKey);
                /* Populate derived active root key from Assets */
                (void) HsmServer_memcpy((void *) &gKeyringSymmKey[KEYRING_DSMEK_INDEX].aesKey, 
                                        sizeof(gKeyringSymmKey[KEYRING_DSMEK_INDEX].aesKey), 
                                        (void *) gAssetInfo.derivedKey, 
                                        sizeof(gKeyringSymmKey[KEYRING_DSMEK_INDEX].aesKey));
            } else {
                /* Do nothing */
            }
            break;
        case 1U:
            /*Case 1 will be executed if the keyCount > 0 and  keyRevision < 2*/
            
            /* exit if keyRev is bumped */
            if(keyRevision > 1U)
            {
                break;
            }
            /* SMPK can be used for both application authentication and debug authorization 
            0bXXXXXXXX_XXXXXXXX_XXXXXXXX_XXXXXXXX
            <-----------rsvd(8b)------>|<-- imageAuth(4b)-->|<-- debugAuth (4b) →   */
            gKeyringAsymmKey[KEYRING_SMPKH_INDEX].keyRights  = KEYRING_SMPKH_KEY_RIGHTS;
            gKeyringAsymmKey[KEYRING_SMPKH_INDEX].keyId  = KEYRING_SMPKH_INDEX;
            /* Populate Hash Algorithm for SMPK */
            gKeyringAsymmKey[KEYRING_SMPKH_INDEX].hashAlgo = HASH_ALGO_SHA2_512;
            /* Populate SMPKH from secure manager at index 0 in keyring */
            SecMgr_enableReadAccess(CSL_SecMgrDeviceKey_CUST_SMPK1);
            SecMgr_getKey(CSL_SecMgrDeviceKey_CUST_SMPK1, (uint32_t *) &gKeyringAsymmKey[KEYRING_SMPKH_INDEX].publicKeyHash);
            SecMgr_enableReadAccess(CSL_SecMgrDeviceKey_CUST_SMPK2);
            SecMgr_getKey(CSL_SecMgrDeviceKey_CUST_SMPK2, (uint32_t *) &gKeyringAsymmKey[KEYRING_SMPKH_INDEX].publicKeyHash[8U]);
            SecMgr_disableReadAccess(CSL_SecMgrDeviceKey_CUST_SMPK1);
            SecMgr_disableReadAccess(CSL_SecMgrDeviceKey_CUST_SMPK2);
            if(FIH_EQ(keyRevision, 1U))
            {
                 if(SecMgr_isLocked(CSL_SecMgrDeviceKey_CUST_SMEK) == SEC_MGR_BOOL_FALSE)
                {
                    /* Populate SMEK in keyring */
                    gKeyringSymmKey[KEYRING_SMEK_INDEX].keyId  = KEYRING_SMEK_INDEX;
                    gKeyringSymmKey[KEYRING_SMEK_INDEX].keyRights  = KEYRING_SMEK_KEY_RIGHTS;
                    gKeyringSymmKey[KEYRING_SMEK_INDEX].keyLength  = sizeof(gKeyringSymmKey[KEYRING_SMEK_INDEX].aesKey);
                    SecMgr_enableReadAccess(CSL_SecMgrDeviceKey_CUST_SMEK);
                    SecMgr_getKey(CSL_SecMgrDeviceKey_CUST_SMEK, (uint32_t*)&gKeyringSymmKey[KEYRING_SMEK_INDEX].aesKey);
                    SecMgr_disableReadAccess(CSL_SecMgrDeviceKey_CUST_SMEK);
                }
                gKeyringSymmKey[KEYRING_DSMEK_INDEX].keyId  = KEYRING_DSMEK_INDEX;
                gKeyringSymmKey[KEYRING_DSMEK_INDEX].keyRights  = KEYRING_SMEK_KEY_RIGHTS;
                gKeyringSymmKey[KEYRING_DSMEK_INDEX].keyLength  = sizeof(gKeyringSymmKey[KEYRING_DSMEK_INDEX].aesKey);
                /* Populate derived active root key from Assets */
                (void) HsmServer_memcpy((void *) &gKeyringSymmKey[KEYRING_DSMEK_INDEX].aesKey, 
                                        sizeof(gKeyringSymmKey[KEYRING_DSMEK_INDEX].aesKey), 
                                        (void *) gAssetInfo.derivedKey, 
                                        sizeof(gKeyringSymmKey[KEYRING_DSMEK_INDEX].aesKey));
            }
            break;
        default:
            break;
    }

    return;
}



/*
 * Common Functions
 */
void Modules_init(void)
{
    HsmServer_config();
    KEYRING_init();
}

void Modules_deinit(void)
{
    HsmServer_deInit();

}
