# wolfCrypt for TI AM26xx (Sitara MCU)

Pure-software wolfCrypt test and benchmark for the TI AM26xx Sitara MCU using
the TI MCU+ SDK and TI ARM CLANG toolchain.

## Hardware Crypto Available

The AM26xx on-chip security accelerators support AES, SHA1, SHA2, HMAC, RSA,
ECDSA, and RNG. This project currently uses pure software crypto only.

## Prerequisites

- TI MCU+ SDK for AM261x (`MCU_PLUS_SDK_PATH`)
- TI ARM CLANG toolchain (bundled with CCS)
- wolfSSL source tree (`WOLFSSL_PATH`)

## Build

```bash
# Build test
make wolfcrypt-test

# Build benchmark
make wolfcrypt-benchmark

# Regenerate SysConfig files (from wolfssl.syscfg)
make syscfg
```

## Output

- `build/wolfcrypt-test.out` — wolfCrypt test executable
- `build/wolfcrypt-benchmark.out` — wolfCrypt benchmark executable
- Use the `.out` (ELF) files for flashing; `.bin` files are sparse.

## Project Structure

```
am26xx/
├── Makefile                     # Build system
├── wolfssl.syscfg               # SysConfig project (copy of hello_world)
├── main.c                       # Entry point: System_init → Drivers_open → test/bench → close
├── wolfssl_platform.c           # clock_gettime (ClockP) + xorshift64 PRNG
├── user_settings.h              # wolfSSL configuration
├── generated/                   # SysConfig output (UART, DPL, pinmux, clocks, board)
└── stubs/
    └── sys/time.h               # struct timeval + clock_gettime stub
```
