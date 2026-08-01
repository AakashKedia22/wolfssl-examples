# wolfCrypt for TI AM263Px HSM (Sitara MCU)

Pure-software wolfCrypt test and benchmark running directly on the TI AM263Px
HSM (secure) Cortex-M4 core (`hsm0-0`), built from the TI TIFS SDK sources.

## Hardware Crypto Available

The AM263Px on-chip security accelerators (DTHE, PKA) support AES, SHA1, SHA2,
HMAC, RSA, ECDSA, and RNG. This project currently uses pure software crypto
only (wolfCrypt runs on the HSM core; the DTHE/PKA hardware engines are left
idle).

## How It Works

This project replaces the standard TIFS HSM runtime firmware with a custom
TIFS-style image that boots on the HSM core and runs the wolfCrypt self-test or
benchmark directly, instead of serving security requests from the R5F cores.

The HSM M4 boots the image as a single contiguous flat binary loaded at address
`0x00000000` by the HSM ROM. The firmware follows the standard TIFS HSM boot
sequence (`System_init → Modules_init → Board_init → Drivers_open →
HsmServer_sendBootNotify`) so the R5F bootloader sees the HSM as booted, then
runs `wolfcrypt_test_main()` / `wolfcrypt_benchmark_main()`.

### Memory model

The HSM M4 has 256KB of secure RAM at `0x00000000`. The HSM ROM loads the whole
image (code, rodata, data) there, so **all loadable content must fit in HSM
RAM**; only NOLOAD sections (`.bss`, `.sysmem`, `.bss.filebuf`) may overflow
into L2 OCRAM (`0x70000000`, 512KB, opened to the M4 by the `mpu_firewall`
`L2OCRAM_BANK0_SLV` config in `wolfssl.syscfg`). Code/data cannot live in OCRAM
because the flat load image must be contiguous from `0x0`.

## Prerequisites

- TI TIFS SDK for AM263Px (`TIFS_PATH`, default `../TI/tifs_sdk_am263px`)
- TI MCU+ SDK for AM263Px (`MCU_PLUS_SDK_PATH`, default `../mcu_plus_sdk_am263px`)
- TI ARM CLANG toolchain (via `CCS_PATH`, default `/Applications/ti/ccs2050/ccs`)
- SysConfig 1.27.x (`SYSCFG_PATH`, `SYSCFG_NODE`)
- wolfSSL source tree (`WOLFSSL_PATH`, default `../../wolfssl`)

## Build

```bash
# Build test
make wolfcrypt-test

# Build benchmark
make wolfcrypt-benchmark

# Regenerate SysConfig files (from wolfssl.syscfg)
make syscfg

# Clean
make clean
```

## Output

- `build/wolfcrypt-test.release.out` + `.bin` — wolfCrypt test image (loads at `0x0`)
- `build/wolfcrypt-benchmark.release.out` + `.bin` — wolfCrypt benchmark image
- The `.out`/`.bin` must be signed into an `.hsmimage` (via `mcu_rom_image_gen.py`)
  and loaded by the R5F SBL as the HSM runtime firmware, then flashed.

## Project Structure

```
am26xx/
├── Makefile                     # Build system (TIFS HSM sources + wolfCrypt)
├── wolfssl.syscfg               # SysConfig project for hsm0-0 (AM263P4, ZCZ_S)
├── main.c                       # HSM boot: System_init → Modules_init → … → test/bench
├── wolfssl_platform.c           # clock_gettime (ClockP) + xorshift64 PRNG
├── user_settings.h              # wolfSSL configuration
├── ti-arm-clang/
│   └── linker.cmd               # HSM M4 memory map (256KB HSM RAM, OCRAM NOLOAD overflow)
├── generated/                   # SysConfig output (DPL, pinmux, clocks, board, tifs_modules)
└── stubs/
    └── sys/time.h               # struct timeval + clock_gettime stub
```
