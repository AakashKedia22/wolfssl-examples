MCU_PLUS_SDK_PATH ?= /Users/shreyaagarwal/Personal/mcu_plus_sdk_am261x
WOLFSSL_PATH      ?= ../../wolfssl

include $(MCU_PLUS_SDK_PATH)/imports.mak

CG_TOOL_ROOT := $(CGT_TI_ARM_CLANG_PATH)
CC  := $(CG_TOOL_ROOT)/bin/tiarmclang
LD  := $(CG_TOOL_ROOT)/bin/tiarmclang
OBJCOPY := $(CG_TOOL_ROOT)/bin/tiarmobjcopy

OUT_DIR := ./build
PROFILE ?= release

TARGET_CPU := cortex-r5
ISA_FLAGS  := -mcpu=$(TARGET_CPU) -mfloat-abi=hard -mfpu=vfpv3-d16 -mthumb

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

GENERATED_SRCS := \
    generated/ti_drivers_config.c \
    generated/ti_drivers_open_close.c \
    generated/ti_dpl_config.c \
    generated/ti_pinmux_config.c \
    generated/ti_power_clock_config.c \
    generated/ti_board_config.c \
    generated/ti_board_open_close.c

MAIN_SRC := main.c

WOLFSSL_TEST_SRC  := $(WOLFSSL_PATH)/wolfcrypt/test/test.c
WOLFSSL_BENCH_SRC := $(WOLFSSL_PATH)/wolfcrypt/benchmark/benchmark.c

WOLFSSL_OBJS := $(patsubst $(WOLFSSL_PATH)/%, $(OUT_DIR)/%, $(WOLFSSL_SRCS:.c=.o))
PLATFORM_OBJS := $(patsubst %.c, $(OUT_DIR)/%.o, $(PLATFORM_SRC))
GENERATED_OBJS := $(patsubst %.c, $(OUT_DIR)/%.o, $(GENERATED_SRCS))
MAIN_TEST_OBJ := $(OUT_DIR)/main-test.o
MAIN_BENCH_OBJ := $(OUT_DIR)/main-bench.o
TEST_OBJ      := $(OUT_DIR)/wolfcrypt/test/test.o
BENCH_OBJ     := $(OUT_DIR)/wolfcrypt/benchmark/benchmark.o

SDK_INCLUDES := \
    -I$(MCU_PLUS_SDK_PATH)/source \
    -I$(MCU_PLUS_SDK_PATH)/source/security \
    -I. \
    -I./stubs \
    -I./generated \
    -I$(WOLFSSL_PATH)

SDK_LIBS_PATH := \
    -Wl,-i$(MCU_PLUS_SDK_PATH)/source/kernel/nortos/lib \
    -Wl,-i$(MCU_PLUS_SDK_PATH)/source/drivers/lib \
    -Wl,-i$(MCU_PLUS_SDK_PATH)/source/board/lib \
    -Wl,-i$(MCU_PLUS_SDK_PATH)/source/security/lib \
    -Wl,-i$(CG_TOOL_ROOT)/lib

SDK_LIBS := \
    -lnortos.am261x.r5f.ti-arm-clang.$(PROFILE).lib \
    -ldrivers.am261x.r5f.ti-arm-clang.nortos.$(PROFILE).lib \
    -lboard.am261x.r5f.ti-arm-clang.nortos.$(PROFILE).lib \
    -lsecurity.am261x.r5f.ti-arm-clang.nortos.$(PROFILE).lib \
    -llibc.a \
    -llibsysbm.a

COMMON_FLAGS := \
    $(ISA_FLAGS) \
    -Wall \
    -Wno-unused-function \
    -ffunction-sections \
    -fdata-sections \
    -fno-common \
    -g \
    -Os \
    -DSOC_AM261X \
    -DOS_NORTOS \
    -DWOLFSSL_USER_SETTINGS

CFLAGS := $(COMMON_FLAGS) $(SDK_INCLUDES)

LDFLAGS := $(COMMON_FLAGS) \
    -Wl,--ram_model \
    -Wl,--reread_libs \
    -Wl,--diag_suppress=10063

LINKER_CMD := generated/linker.cmd

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
    generated/linker.cmd

SYSCFG_NODE_CMD := $(shell command -v node 2>/dev/null || echo $(SYSCFG_NODE))
SYSCFG_CLI_PATH := $(shell ls -d /Applications/ti/ccs2050/ccs/utils/sysconfig_1.27.0 2>/dev/null || echo $(SYSCFG_CLI_PATH))

.PHONY: all clean syscfg wolfcrypt-test wolfcrypt-benchmark

all: wolfcrypt-test wolfcrypt-benchmark

wolfcrypt-test: $(OUT_DIR)/wolfcrypt-test.out
wolfcrypt-benchmark: $(OUT_DIR)/wolfcrypt-benchmark.out

$(OUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/generated/%.o: generated/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SYSCFG_GEN_FILES): wolfssl.syscfg
	@echo Generating SysConfig files ...
	$(SYSCFG_NODE_CMD) $(SYSCFG_CLI_PATH)/dist/cli.js \
	    --product $(SYSCFG_SDKPRODUCT) \
	    --context r5fss0-0 --part AM2612 --package ZFG \
	    --output generated/ wolfssl.syscfg
	@rm -f generated/ti_enet_*.c generated/ti_enet_*.h \
	      generated/ti_usb_*.c generated/ti_usb_*.h \
	      generated/ti_sdl_*.c generated/ti_sdl_*.h
	@# Reduce heap/stack to fit in OCRAM
	$(SED) -i '' 's/--stack_size=16384/--stack_size=4096/; s/--heap_size=32768/--heap_size=4096/' generated/linker.cmd

syscfg: $(SYSCFG_GEN_FILES)

$(OUT_DIR)/wolfcrypt/%.o: $(WOLFSSL_PATH)/wolfcrypt/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TEST_OBJ): $(WOLFSSL_TEST_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Dmain=wolfcrypt_test_entry -c -o $@ $<

$(BENCH_OBJ): $(WOLFSSL_BENCH_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Dmain=wolfcrypt_benchmark_entry -c -o $@ $<

$(MAIN_TEST_OBJ): main.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(MAIN_BENCH_OBJ): main.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DWOLFCRYPT_BENCHMARK -c -o $@ $<

$(OUT_DIR)/wolfcrypt-test.out: $(WOLFSSL_OBJS) $(PLATFORM_OBJS) $(GENERATED_OBJS) $(MAIN_TEST_OBJ) $(TEST_OBJ)
	@mkdir -p $(OUT_DIR)
	$(LD) $(LDFLAGS) $(SDK_LIBS_PATH) -o $@ $^ $(SDK_LIBS) $(LINKER_CMD) \
	    -Wl,-m=$(OUT_DIR)/wolfcrypt-test.map
	$(OBJCOPY) -O binary $@ $(OUT_DIR)/wolfcrypt-test.bin 2>/dev/null; \
	  echo "Warning: .bin may be sparse (vectors at 0x0, code at 0x70040000); use .out for flashing"

$(OUT_DIR)/wolfcrypt-benchmark.out: $(WOLFSSL_OBJS) $(PLATFORM_OBJS) $(GENERATED_OBJS) $(MAIN_BENCH_OBJ) $(BENCH_OBJ)
	@mkdir -p $(OUT_DIR)
	$(LD) $(LDFLAGS) $(SDK_LIBS_PATH) -o $@ $^ $(SDK_LIBS) $(LINKER_CMD) \
	    -Wl,-m=$(OUT_DIR)/wolfcrypt-benchmark.map
	$(OBJCOPY) -O binary $@ $(OUT_DIR)/wolfcrypt-benchmark.bin 2>/dev/null; \
	  echo "Warning: .bin may be sparse (vectors at 0x0, code at 0x70040000); use .out for flashing"

clean:
	rm -rf $(OUT_DIR)
