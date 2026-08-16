#
# wolfCrypt test/benchmark for TI AM263Px HSM core (hsm0-0, Cortex-M4)
#
# This builds a custom TIFS-MCU style firmware image that runs wolfCrypt
# directly on the AM263Px HSM (secure) Cortex-M4 core, instead of the
# standard TIFS-MCU firmware. It is built from the TI TIFS SDK sources.
#
# Prerequisites:
#   - TI TIFS SDK for AM263Px     (TIFS_PATH)
#   - TI MCU+ SDK for AM263Px     (MCU_PLUS_SDK_PATH)
#   - TI ARM CLANG compiler       (via CCS_PATH / CGT_TI_ARM_CLANG_PATH)
#   - SysConfig 1.27.x            (via SYSCFG_PATH)
#   - wolfSSL source tree         (WOLFSSL_PATH)
#

TIFS_PATH          ?= /home/aakashk/aakashk/tifs_4
MCU_PLUS_SDK_PATH  ?= /home/aakashk/aakashk/mcu_plus_sdk
WOLFSSL_PATH       ?= ../../wolfssl
CCS_PATH           ?= /home/aakashk/ti/ccs2100/ccs
SYSCFG_PATH        ?= /home/aakashk/ti/sysconfig_1.28.0
SYSCFG_NODE        ?= $(CCS_PATH)/tools/node/node
PROFILE            ?= release
DEVICE_TYPE        ?= HS

#
# Selective wolfCrypt algorithm build config.
# AES/RSA/SHA(+MD5/HMAC) are opt-out in wolfCrypt (disabled via -DNO_x), while
# SHA512/ECC/ChaChaPoly are opt-in defines living in user_settings.h; those are
# instead suppressed by defining a WOLFCRYPT_DISABLE_x guard that
# user_settings.h checks before enabling them. Declared here, before the
# source lists below, since they gate which .c files get compiled.
#
ENABLE_AES    ?= yes
ENABLE_SHA    ?= yes
ENABLE_RSA    ?= no
ENABLE_ECC    ?= no
ENABLE_CHACHA ?= no

# When yes, AES is offloaded to the AM263Px's DTHE hardware crypto engine
# (wolfcrypt/src/port/ti-am26xx/ti-aes.c) instead of running in software
# (wolfcrypt/src/aes.c). Mutually exclusive with the software AES source.
ENABLE_HW_AES ?= no

include $(TIFS_PATH)/imports.mak
include $(TIFS_PATH)/devconfig/devconfig.mak

CG_TOOL_ROOT := $(CGT_TI_ARM_CLANG_PATH)
CC           := $(CG_TOOL_ROOT)/bin/tiarmclang
LNK          := $(CG_TOOL_ROOT)/bin/tiarmclang
OBJCOPY      := $(CG_TOOL_ROOT)/bin/tiarmobjcopy
BIN2C        := $(TIFS_PATH)/tools/bin2c/bin2c.py
PYTHON       ?= python3

OUT_DIR := ./build

OUTNAME_TEST   := wolfcrypt-test.$(PROFILE).out
OUTNAME_BENCH  := wolfcrypt-benchmark.$(PROFILE).out

TARGET_CPU := cortex-m4
ISA_FLAGS  := -mcpu=$(TARGET_CPU) -mfloat-abi=soft

#
# TIFS-MCU HSM firmware sources (from the TIFS crypto example file list).
# These provide the HSM runtime: boot, DPL (M4), hsmserver, dthe, sipc, etc.
#
TIFS_SRCS := \
    hsm_version.c \
    AddrTranslateP.c \
    CacheP_null.c \
    ClockP_nortos.c \
    ClockP_nortos_m4.c \
    cpu_boot_config.c \
    CycleCounterP_m4.c \
    DebugP_log.c \
    DebugP_memTraceLogWriter.c \
    DebugP_nortos.c \
    DebugP_shmLogWriter.c \
    DebugP_uartLogWriter.c \
    DebugP_uartScanf.c \
    ext_otp_interface.c \
    firewall_interface.c \
    HeapP_internal.c \
    HeapP_nortos.c \
    HwiP_armv7m.c \
    HwiP_armv7m_handlers_nortos.c \
    MpuP_armv7m.c \
    SemaphoreP_nortos.c \
    SysTickTimerP.c \
    TaskP_nortos.c \
    TimerP_rti_nortos.c \
    aes_cmac.c \
    aes_ecb.c \
    aes_cmac_kdf.c \
    asn1_parse.c \
    asn1_utils.c \
    asset.c \
    boot_armv7m.c \
    cert_auth.c \
    crypto_rng_interface.c \
    crypto_util.c \
    dkek.c \
    dma.c \
    dthe.c \
    dthe_aes.c \
    dthe_edma.c \
    dthe_secure_enable.c \
    dthe_sha.c \
    dthe_soc_ctrl.c \
    edma.c \
    ecdsa.c \
    nvm.c \
    otfa_hw.c \
    efuse.c \
    eip29t2_firmware.c \
    hkdf.c \
    otfa_interface.c \
    hsm_dkek.c \
    hsm_dthe_service.c \
    hsm_firewall_srv.c \
    hsm_otfa_service.c \
    hsm_soc_ctrl.c \
    hsm_srv_debug.c \
    hsm_swrev.c \
    hsm_proc_auth_boot.c \
    hsm_rot_switching.c \
    hsm_utils.c \
    hsm_ext_otp.c \
    hsmserver.c \
    hsmserver_send_boot_notify.c \
    hsm_defines.c \
    led.c \
    memlog.c \
    mpu_firewall.c \
    mpu_firewall_v0_cfg.c \
    otp_defines.c \
    anti_rollback.c \
    pinmux.c \
    pka.c \
    pka_eddsa.c \
    printf.c \
    sec_boot.c \
    sec_dbg.c \
    secmgr.c \
    secure_asset_exclusion_list.c \
    sipc_notify_cfg.c \
    sipc_notify_src.c \
    soc.c \
    soc_rcm.c \
    soc_tifs.c \
    stack_check.c \
    uart_v0.c \
    update_key_rev.c \
    utils.c \
    watchdog_rti.c \
    watchdog_soc.c \
    rot_switching.c \
    rng.c \
    hsm_keyring.c \
    keyring.c \
    keyring_soc.c

#
# SysConfig generated sources (hsm0-0 context)
#
GENERATED_SRCS := \
    generated/ti_drivers_config.c \
    generated/ti_drivers_open_close.c \
    generated/ti_board_config.c \
    generated/ti_board_open_close.c \
    generated/ti_dpl_config.c \
    generated/ti_pinmux_config.c \
    generated/ti_power_clock_config.c \
    generated/tifs_modules_config.c

#
# wolfSSL / wolfCrypt sources.
# Core glue (RNG, logging, generic hash/encrypt dispatch, math) is always
# built; per-algorithm sources are pulled in only when their ENABLE_x flag
# (see ALGO_DEFINES above) is "yes", so a disabled algorithm's .c files are
# neither compiled nor linked.
#
#
# asn.c/sp_int.c/sp_c32.c/wolfmath.c/pkcs12.c are gated by NO_ASN/NO_BIG_INT/
# HAVE_PKCS12 in wolfCrypt itself, not by NO_RSA/WOLFCRYPT_DISABLE_ECC (e.g.
# wolfcrypt/test/test.c's PKCS12 and CTC-settings checks run regardless of
# whether RSA/ECC are enabled). Since none of ENABLE_RSA/ENABLE_ECC being
# "no" disables those wolfCrypt features, these sources must stay in the
# always-built group even when RSA and ECC are both off.
#
WOLFCRYPT_SRC_NAMES := \
    wc_port.c \
    wc_encrypt.c \
    random.c \
    logging.c \
    coding.c \
    hash.c \
    pwdbased.c \
    fe_low_mem.c \
    ge_low_mem.c \
    asn.c \
    sp_int.c \
    sp_c32.c \
    wolfmath.c \
    pkcs12.c \

ifeq ($(ENABLE_AES),yes)
ifeq ($(ENABLE_HW_AES),yes)
WOLFCRYPT_SRC_NAMES += port/ti-am26xx/ti-aes.c port/ti-am26xx/ti-gcm.c port/ti-am26xx/ti-ccm.c
else
WOLFCRYPT_SRC_NAMES += aes.c
endif
endif

ifeq ($(ENABLE_SHA),yes)
WOLFCRYPT_SRC_NAMES += sha256.c sha512.c sha.c md5.c hmac.c
endif

ifeq ($(ENABLE_RSA),yes)
WOLFCRYPT_SRC_NAMES += rsa.c
endif

ifeq ($(ENABLE_ECC),yes)
WOLFCRYPT_SRC_NAMES += ecc.c
endif

ifeq ($(ENABLE_CHACHA),yes)
WOLFCRYPT_SRC_NAMES += chacha.c chacha20_poly1305.c poly1305.c
endif

WOLFSSL_SRCS := $(addprefix $(WOLFSSL_PATH)/wolfcrypt/src/, $(WOLFCRYPT_SRC_NAMES))

PLATFORM_SRC := wolfssl_platform.c
ifeq ($(ENABLE_HW_AES),yes)
PLATFORM_SRC += stubs/aes_gcm_checktagsz_stub.c
endif
MAIN_SRC     := main.c

WOLFSSL_TEST_SRC  := $(WOLFSSL_PATH)/wolfcrypt/test/test.c
WOLFSSL_BENCH_SRC := $(WOLFSSL_PATH)/wolfcrypt/benchmark/benchmark.c

#
# vpath directories for TIFS sources
#
TIFS_SRC_DIRS := \
    $(TIFS_PATH)/source \
    $(TIFS_PATH)/source/drivers \
    $(TIFS_PATH)/source/drivers/crypto/dthe \
    $(TIFS_PATH)/source/drivers/hsm_ctrl \
    $(TIFS_PATH)/source/drivers/nvm/efuse \
    $(TIFS_PATH)/source/drivers/otfa \
    $(TIFS_PATH)/source/drivers/efuse/f021 \
    $(TIFS_PATH)/source/drivers/mpu_firewall/v0 \
    $(TIFS_PATH)/source/drivers/mpu_firewall/v0/soc/am263px \
    $(TIFS_PATH)/source/drivers/secmgr \
    $(TIFS_PATH)/source/drivers/stack_canaries \
    $(TIFS_PATH)/source/imports \
    $(TIFS_PATH)/source/imports/board/led \
    $(TIFS_PATH)/source/imports/drivers/edma/v0 \
    $(TIFS_PATH)/source/imports/drivers/pinmux/am263px \
    $(TIFS_PATH)/source/imports/drivers/soc/am263px \
    $(TIFS_PATH)/source/imports/drivers/uart/v0 \
    $(TIFS_PATH)/source/imports/drivers/watchdog/v0 \
    $(TIFS_PATH)/source/imports/drivers/watchdog/v0/soc/am263px \
    $(TIFS_PATH)/source/imports/kernel/nortos/dpl/common \
    $(TIFS_PATH)/source/imports/kernel/nortos/dpl/m4 \
    $(TIFS_PATH)/source/imports/kernel/nortos/dpl/m4/ram \
    $(TIFS_PATH)/source/imports/security_common/drivers/crypto \
    $(TIFS_PATH)/source/imports/security_common/drivers/crypto/dthe \
    $(TIFS_PATH)/source/imports/security_common/drivers/crypto/dthe/dma/edma \
    $(TIFS_PATH)/source/imports/security_common/drivers/crypto/pka \
    $(TIFS_PATH)/source/imports/security_common/drivers/crypto/rng \
    $(TIFS_PATH)/source/imports/security_common/drivers/secure_ipc_notify \
    $(TIFS_PATH)/source/imports/security_common/drivers/secure_ipc_notify/soc/am263px \
    $(TIFS_PATH)/source/modules/aes \
    $(TIFS_PATH)/source/modules/asset \
    $(TIFS_PATH)/source/modules/anti_rollback \
    $(TIFS_PATH)/source/modules/cert_auth \
    $(TIFS_PATH)/source/modules/crypto \
    $(TIFS_PATH)/source/modules/dkek \
    $(TIFS_PATH)/source/modules/ecdsa \
    $(TIFS_PATH)/source/modules/ext_otp \
    $(TIFS_PATH)/source/modules/firewall \
    $(TIFS_PATH)/source/modules/firewall/soc/am263px \
    $(TIFS_PATH)/source/modules/hkdf \
    $(TIFS_PATH)/source/modules/hsmserver \
    $(TIFS_PATH)/source/modules/hsmserver/soc/am263px \
    $(TIFS_PATH)/source/modules/hsmserver/tifs/dthe \
    $(TIFS_PATH)/source/modules/kdf \
    $(TIFS_PATH)/source/modules/keyring \
    $(TIFS_PATH)/source/modules/keyring/soc/am263px \
    $(TIFS_PATH)/source/modules/memlog \
    $(TIFS_PATH)/source/modules/otfa \
    $(TIFS_PATH)/source/modules/rot_switching \
    $(TIFS_PATH)/source/modules/rot_switching/soc/am263px \
    $(TIFS_PATH)/source/modules/sec_boot \
    $(TIFS_PATH)/source/modules/sec_boot/soc/am263px \
    $(TIFS_PATH)/source/modules/sec_debug \
    $(TIFS_PATH)/source/modules/utils \
    $(TIFS_PATH)/source/soc \
    $(TIFS_PATH)/source/soc/am263px

vpath %.c $(TIFS_SRC_DIRS)
vpath %.c generated

#
# Object lists
#
TIFS_OBJS      := $(addprefix $(OUT_DIR)/tifs/, $(TIFS_SRCS:.c=.o))
GENERATED_OBJS := $(patsubst generated/%.c, $(OUT_DIR)/generated/%.o, $(GENERATED_SRCS))
WOLFSSL_OBJS   := $(patsubst $(WOLFSSL_PATH)/%, $(OUT_DIR)/%, $(WOLFSSL_SRCS:.c=.o))
PLATFORM_OBJS  := $(patsubst %.c, $(OUT_DIR)/%.o, $(PLATFORM_SRC))
MAIN_TEST_OBJ  := $(OUT_DIR)/main-test.o
MAIN_BENCH_OBJ := $(OUT_DIR)/main-bench.o
TEST_OBJ       := $(OUT_DIR)/wolfcrypt/test/test.o
BENCH_OBJ      := $(OUT_DIR)/wolfcrypt/benchmark/benchmark.o

# Pull in -MMD-generated per-object dependency files so header changes
# (e.g. to user_settings.h) trigger a rebuild of everything that includes
# them, instead of only .c file changes.
DEPFILES := $(TIFS_OBJS:.o=.d) $(GENERATED_OBJS:.o=.d) $(WOLFSSL_OBJS:.o=.d) \
    $(PLATFORM_OBJS:.o=.d) $(MAIN_TEST_OBJ:.o=.d) $(MAIN_BENCH_OBJ:.o=.d) \
    $(TEST_OBJ:.o=.d) $(BENCH_OBJ:.o=.d)
-include $(DEPFILES)

#
# Include paths
#
INCLUDES := \
    -I$(CG_TOOL_ROOT)/include/c \
    -I$(TIFS_PATH)/source/drivers \
    -I$(TIFS_PATH)/source \
    -I$(TIFS_PATH)/source/imports \
    -I$(TIFS_PATH)/source/imports/security_common/drivers \
    -I$(TIFS_PATH)/source/modules/hsmserver/tifs/hsse \
    -I$(TIFS_PATH)/source/modules/cert_auth/tifs \
    -I$(TIFS_PATH)/source/modules/hsmserver/tifs/hsse/soc/am263px \
    -Igenerated \
    -I. \
    -I./stubs \
    -I$(WOLFSSL_PATH)

#
# Selective wolfCrypt algorithm build config (ENABLE_x vars declared at the
# top of this file, before the source lists that depend on them).
#
ALGO_DEFINES :=
ifeq ($(ENABLE_AES),no)
ALGO_DEFINES += -DNO_AES
endif
ifeq ($(ENABLE_SHA),no)
ALGO_DEFINES += -DNO_SHA -DNO_SHA256 -DNO_MD5 -DNO_HMAC -DWOLFCRYPT_DISABLE_SHA512
endif
ifeq ($(ENABLE_RSA),no)
ALGO_DEFINES += -DNO_RSA
endif
ifeq ($(ENABLE_ECC),no)
ALGO_DEFINES += -DWOLFCRYPT_DISABLE_ECC
endif
ifeq ($(ENABLE_CHACHA),no)
ALGO_DEFINES += -DWOLFCRYPT_DISABLE_CHACHA
endif
ifeq ($(ENABLE_HW_AES),yes)
ALGO_DEFINES += -DWOLFSSL_TI_CRYPT
endif

#
# Defines / flags
#
DEFINES := \
    -DSOC_AM263PX \
    -DOS_NORTOS \
    -DDEV_TYPE_HS=1 \
    -D__DEVELOPMENT_FLOW \
    -DWOLFSSL_USER_SETTINGS \
    $(ALGO_DEFINES)

CFLAGS := \
    $(ISA_FLAGS) \
    -fstack-protector-strong \
    -Wall \
    -Wno-gnu-variable-sized-type-not-at-end \
    -Wno-unused-function \
    -g \
    -Os \
    $(DEFINES) \
    $(INCLUDES)

LDFLAGS := \
    $(ISA_FLAGS) \
    -Wl,--ram_model \
    -Wl,--reread_libs \
    -Wl,-i$(CG_TOOL_ROOT)/lib

LIBC := -llibc.a

LINKER_CMD := ti-arm-clang/linker.cmd

#
# SysConfig
#
SYSCFG_GEN_FILES := \
    generated/ti_drivers_config.c \
    generated/ti_drivers_config.h \
    generated/ti_drivers_open_close.c \
    generated/ti_drivers_open_close.h \
    generated/ti_dpl_config.c \
    generated/ti_dpl_config.h \
    generated/ti_pinmux_config.c \
    generated/ti_power_clock_config.c \
    generated/ti_board_config.c \
    generated/ti_board_config.h \
    generated/ti_board_open_close.c \
    generated/ti_board_open_close.h \
    generated/tifs_modules_config.c \
    generated/tifs_modules_config.h

.PHONY: all clean syscfg wolfcrypt-test wolfcrypt-benchmark install

all: wolfcrypt-test wolfcrypt-benchmark

wolfcrypt-test: $(OUT_DIR)/$(OUTNAME_TEST)
wolfcrypt-benchmark: $(OUT_DIR)/$(OUTNAME_BENCH)

#
# Compile rules
#
$(OUT_DIR)/tifs/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -o $@ $<

$(OUT_DIR)/generated/%.o: generated/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -o $@ $<

$(OUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -o $@ $<

$(OUT_DIR)/wolfcrypt/%.o: $(WOLFSSL_PATH)/wolfcrypt/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -o $@ $<

$(TEST_OBJ): $(WOLFSSL_TEST_SRC)
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -DNO_MAIN_FUNCTION -o $@ $<

$(BENCH_OBJ): $(WOLFSSL_BENCH_SRC)
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -DNO_MAIN_FUNCTION -o $@ $<

$(MAIN_TEST_OBJ): main.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -o $@ $<

$(MAIN_BENCH_OBJ): main.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -MMD -DWOLFCRYPT_BENCHMARK -o $@ $<

#
# Link rules
#
$(OUT_DIR)/$(OUTNAME_TEST): $(TIFS_OBJS) $(GENERATED_OBJS) $(WOLFSSL_OBJS) $(PLATFORM_OBJS) $(MAIN_TEST_OBJ) $(TEST_OBJ)
	@mkdir -p $(OUT_DIR)
	$(LNK) $(LDFLAGS) -Wl,-m=$(basename $@).map -o $@ $^ $(LIBC) $(LINKER_CMD)
	$(OBJCOPY) -O binary $@ $(basename $@).bin

$(OUT_DIR)/$(OUTNAME_BENCH): $(TIFS_OBJS) $(GENERATED_OBJS) $(WOLFSSL_OBJS) $(PLATFORM_OBJS) $(MAIN_BENCH_OBJ) $(BENCH_OBJ)
	@mkdir -p $(OUT_DIR)
	$(LNK) $(LDFLAGS) -Wl,-m=$(basename $@).map -o $@ $^ $(LIBC) $(LINKER_CMD)
	$(OBJCOPY) -O binary $@ $(basename $@).bin

#
# SysConfig generation
#
$(SYSCFG_GEN_FILES): wolfssl.syscfg
	@echo Generating SysConfig files ...
	$(SYSCFG_NODE) $(SYSCFG_PATH)/dist/cli.js \
	    --product $(SYSCFG_TIFSPRODUCT) \
	    --product $(SYSCFG_SDKPRODUCT) \
	    --context hsm0-0 --part AM263P4 --package ZCZ_S \
	    --output generated/ wolfssl.syscfg
	@rm -f generated/ti_enet_*.c generated/ti_enet_*.h \
	      generated/ti_usb_*.c generated/ti_usb_*.h \
	      generated/ti_sdl_*.c generated/ti_sdl_*.h \
	      generated/ti_drivers_networking_config.c generated/ti_drivers_networking_config.h

syscfg: $(SYSCFG_GEN_FILES)

#
# Post-build: sign the linked binary into a boot-loadable .hsmimage and,
# for HS devices, install it as the HSM runtime image other cores load at
# boot. BUILD_CONFIG picks which app (benchmark or test) gets processed,
# since the install step overwrites the single shared hsmRtImg.h.
#
BUILD_CONFIG ?= test

ifeq ($(BUILD_CONFIG),benchmark)
POSTBUILD_OUTNAME := $(OUTNAME_BENCH)
POSTBUILD_APP     := wolfcrypt-benchmark
else
POSTBUILD_OUTNAME := $(OUTNAME_TEST)
POSTBUILD_APP     := wolfcrypt-test
endif

POSTBUILD_BIN := $(OUT_DIR)/$(basename $(POSTBUILD_OUTNAME)).bin
POSTBUILD_IMG := $(OUT_DIR)/$(basename $(POSTBUILD_OUTNAME)).hsmimage

BOOTIMAGE_CERT_GEN_CMD := $(PYTHON) $(TIFS_PATH)/tools/boot/signing/mcu_rom_image_gen.py
RUN_ADDRESS            := 0x00000000
LIC_TSPA               := ti_tspa

ifeq ($(DEVICE_TYPE),HS)
BOOTIMAGE_CERT_KEY := $(HSM_SIGNING_KEY)
else
BOOTIMAGE_CERT_KEY := $(MCU_PLUS_SDK_PATH)/tools/boot/signing/mcu_gpkey.pem
endif

HSMRT_IMG      := $(MCU_PLUS_SDK_PATH)/source/security/security_common/drivers/hsmclient/soc/am263px/hsmRtImg.h
HSMRT_BIN_PATH := $(MCU_PLUS_SDK_PATH)/source/security/security_common/drivers/hsmclient/soc/am263px/

$(POSTBUILD_BIN): $(OUT_DIR)/$(POSTBUILD_OUTNAME)
	$(OBJCOPY) -O binary $< $@

$(POSTBUILD_IMG): $(POSTBUILD_BIN)
	@echo  Boot image \($(BUILD_CONFIG)\): $@ ...
ifeq ($(DEVICE_TYPE),HS)
ifeq ($(DBG_ENABLED),no)
ifeq ($(ENC_ENABLED),yes)
	$(BOOTIMAGE_CERT_GEN_CMD) --tifs-enc --enc-key $(HSM_ENCRYPTION_KEY) --image-bin $(POSTBUILD_BIN) --core HSM --swrv 1 --loadaddr $(RUN_ADDRESS) --sign-key $(BOOTIMAGE_CERT_KEY) --out-image $@ --kd-salt $(KD_SALT) --debug DBG_SOC_DEFAULT --device am263px
else
	$(BOOTIMAGE_CERT_GEN_CMD) --image-bin $(POSTBUILD_BIN) --core HSM --swrv 1 --loadaddr $(RUN_ADDRESS) --sign-key $(BOOTIMAGE_CERT_KEY) --out-image $@ --debug DBG_SOC_DEFAULT --device am263px
endif
else
ifeq ($(ENC_ENABLED),yes)
	$(BOOTIMAGE_CERT_GEN_CMD) --tifs-enc --enc-key $(HSM_ENCRYPTION_KEY) --image-bin $(POSTBUILD_BIN) --core HSM --swrv 1 --loadaddr $(RUN_ADDRESS) --sign-key $(BOOTIMAGE_CERT_KEY) --out-image $@ --kd-salt $(KD_SALT) --debug DBG_PUBLIC_ENABLE --device am263px
else
	$(BOOTIMAGE_CERT_GEN_CMD) --image-bin $(POSTBUILD_BIN) --core HSM --swrv 1 --loadaddr $(RUN_ADDRESS) --sign-key $(BOOTIMAGE_CERT_KEY) --out-image $@ --debug DBG_PUBLIC_ENABLE --device am263px
endif
endif
endif
	@echo  Boot image \($(BUILD_CONFIG)\): $@ Done !!!

install: $(POSTBUILD_IMG)
ifeq ($(DEVICE_TYPE),HS)
	@echo initiating HSMRt image bin2c conversion \($(BUILD_CONFIG)\).....
	$(RM) $(HSMRT_IMG)
	$(COPY) $(POSTBUILD_IMG) $(HSMRT_BIN_PATH)
	$(PYTHON) $(BIN2C) $(POSTBUILD_IMG) $(HSMRT_IMG) HSMRT_IMG $(LIC_TSPA)
	@echo  HSM runtime image installed \($(BUILD_CONFIG)\): $(HSMRT_IMG)
endif

clean:
	rm -rf $(OUT_DIR)
