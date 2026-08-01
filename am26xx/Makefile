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

TIFS_PATH          ?= /Users/shreyaagarwal/Personal/TI/tifs_sdk_am263px
MCU_PLUS_SDK_PATH  ?= /Users/shreyaagarwal/Personal/mcu_plus_sdk_am263px
WOLFSSL_PATH       ?= ../../wolfssl
CCS_PATH           ?= /Applications/ti/ccs2050/ccs
SYSCFG_PATH        ?= $(CCS_PATH)/utils/sysconfig_1.27.0
SYSCFG_NODE        ?= $(CCS_PATH)/tools/node/node
PROFILE            ?= release
DEVICE_TYPE        ?= HS

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
# wolfSSL / wolfCrypt sources
#
WOLFSSL_SRCS := \
    $(WOLFSSL_PATH)/wolfcrypt/src/wc_port.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/wc_encrypt.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/random.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/logging.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/sp_int.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/sp_c32.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/ecc.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/rsa.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/aes.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/asn.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/sha256.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/sha512.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/sha.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/md5.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/chacha.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/chacha20_poly1305.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/poly1305.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/hmac.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/coding.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/hash.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/pwdbased.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/wolfmath.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/fe_low_mem.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/ge_low_mem.c \
    $(WOLFSSL_PATH)/wolfcrypt/src/pkcs12.c

PLATFORM_SRC := wolfssl_platform.c
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
# Defines / flags
#
DEFINES := \
    -DSOC_AM263PX \
    -DOS_NORTOS \
    -DDEV_TYPE_HS=1 \
    -DWOLFSSL_USER_SETTINGS

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

.PHONY: all clean syscfg wolfcrypt-test wolfcrypt-benchmark

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
	$(CC) -c $(CFLAGS) -DNO_MAIN_FUNCTION -o $@ $<

$(BENCH_OBJ): $(WOLFSSL_BENCH_SRC)
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -DNO_MAIN_FUNCTION -o $@ $<

$(MAIN_TEST_OBJ): main.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

$(MAIN_BENCH_OBJ): main.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -DWOLFCRYPT_BENCHMARK -o $@ $<

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

clean:
	rm -rf $(OUT_DIR)
