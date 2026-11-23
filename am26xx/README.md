# wolfCrypt for TI AM263Px HSM (Sitara MCU)

wolfCrypt test and benchmark (software by default, optional hardware offload) running directly on the TI AM263Px
HSM (secure) Cortex-M4 core (`hsm0-0`), built from the TI TIFS SDK sources.

## Hardware Crypto

The AM263Px on-chip security accelerators (DTHE, PKA, TRNG) support AES, SHA1,
SHA2, HMAC, RSA, ECDSA, ECDH, and RNG. By default this project is pure software
(the engines are left idle). Each offload is an opt-in Makefile option, see
"Build options" below. With every option at its default the build is the same
as the software-only build.

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

- TI TIFS SDK for AM263Px (`TIFS_PATH`, default `../../../ti-sdk/TI/tifs_sdk_am263px`, relative to this directory)
- TI MCU+ SDK for AM263Px (`MCU_PLUS_SDK_PATH`, default `../../../ti-sdk/mcu_plus_sdk_am263px`, relative to this directory)
- TI ARM CLANG toolchain (via `CCS_PATH`, default `/Applications/ti/ccs2050/ccs`)
- SysConfig 1.28.0 or newer (`SYSCFG_PATH`, `SYSCFG_NODE`; see "SysConfig regeneration")
- wolfSSL source tree (`WOLFSSL_PATH`, default `../../wolfssl`)

All of these are `?=` defaults and can be overridden on the command line or in the environment.

## Build

```bash
# Build test
make wolfcrypt-test

# Build benchmark
make wolfcrypt-benchmark

# Regenerate SysConfig files (from wolfssl.syscfg, see "SysConfig regeneration")
make syscfg

# Clean
make clean
```

### Explicit paths

All SDK locations can be given on the command line. Example, using the TIFS and MCU+ SDKs in this
workspace, the TI ARM CLANG toolchain at `/Applications/ti/ti-cgt-armllvm_4.0.4.LTS` and SysConfig from CCS 2050:

```bash
make wolfcrypt-test \
    TIFS_PATH=../../../ti-sdk/TI/tifs_sdk_am263px \
    MCU_PLUS_SDK_PATH=../../../ti-sdk/mcu_plus_sdk_am263px \
    CGT_TI_ARM_CLANG_PATH=/Applications/ti/ti-cgt-armllvm_4.0.4.LTS \
    SYSCFG_PATH=/Applications/ti/ccs2050/ccs/utils/sysconfig_1.27.0 \
    SYSCFG_NODE=/Applications/ti/ccs2050/ccs/tools/node/node
```

SysConfig 1.27.0 is the version bundled with CCS 2050. TIFS 26.01 needs SysConfig 1.28.0 or newer, so use it
only for the `make syscfg` workaround in "SysConfig regeneration". Build (`make wolfcrypt-test`) does not
run SysConfig.

## Optimisation

`OPT` sets the optimisation level for every object file. It defaults to `-Os`
(`OPT ?= -Os`). Without it the objects are built at `-O0`, which does not fit the
HSM RAM code/data region for most configurations.

Changing `OPT` does not trigger a rebuild of existing objects, so run `make clean`
first. The table gives the HSM RAM code/data usage (code, rodata, data and keyring
sections) in K bytes, for the test and benchmark images at each level. The limit is
227.0K (`HSM_RAM_CODE_DATA`).

| Config | -O0 test / bench | -Os test / bench | -Oz test / bench |
|---|---|---|---|
| sw (AES+SHA software, no RSA/ECC) | 225.9K / 142.9K | 151.7K / 100.3K | 133.0K / 91.2K |
| hw_rng | 235.0K / 148.7K | 156.7K / 104.4K | 137.0K / 94.4K |
| hw_aes_sha | 188.1K / 105.9K | 118.8K / 69.7K | 104.1K / 64.4K |
| RSA + HW_RSA | 311.8K / 221.8K | 212.7K / 156.6K | 187.5K / 141.7K |
| ECC + HW_ECC + HW_RNG | 328.0K / 222.6K | 223.3K / 156.9K | 197.7K / 142.7K |
| all (RSA+ECC+CHACHA, HW AES/RSA/ECC/RNG; HW_SHA off) | 383.1K / 261.9K | 252.4K / 173.7K | 223.0K / 157.6K |
| all + ECC384/521 + RSA4096 | 425.0K / 299.3K | 283.1K / 200.1K | 248.2K / 179.2K |

- At `-O0`, `hw_rng` overflows the region by about 8 KB. `-Os` brings it to 156.7K.
- `-Os` (the default) fits every config except the "all" test image, and the
  "all + ECC384/521 + RSA4096" test image does not fit at any level.
- The "all" test image needs `OPT=-Oz` (223.0K, about 4K spare):

```bash
make clean
make wolfcrypt-test OPT=-Oz ENABLE_RSA=yes ENABLE_ECC=yes ENABLE_CHACHA=yes \
    ENABLE_HW_AES=yes ENABLE_HW_RSA=yes ENABLE_HW_ECC=yes ENABLE_HW_RNG=yes
```

- The benchmark image is smaller and fits with `-Os` for every config except the
  `-O0` builds. Use `-Os` for benchmark figures: `-Oz` trades speed for size.

## SysConfig regeneration

`generated/` is produced from `wolfssl.syscfg` by SysConfig, against the TIFS and MCU+ SDK
`product.json` files. The TIFS 26.01 `product.json` declares `minToolVersion` 1.28.0, so
SysConfig must be 1.28.0 or newer.

`make syscfg` takes these overrides:

| Variable | Default | Purpose |
|---|---|---|
| `SYSCFG_PATH` | `$(HOME)/ti/sysconfig_1.28.0` | SysConfig install root (contains `dist/cli.js`) |
| `SYSCFG_NODE` | `$(CCS_PATH)/tools/node/node` | Node binary used to run SysConfig |
| `SYSCFG_PRODUCT_JSON` | `$(TIFS_PATH)/.metadata/product.json` | TIFS product file passed to SysConfig |
| `MCU_PLUS_SDK_PATH` | `../../../ti-sdk/mcu_plus_sdk_am263px` | MCU+ SDK; its `.metadata/product.json` is also passed |

The command-line values are used as given: `imports.mak` does not clobber them.

### Using SysConfig 1.27 (workaround)

If only SysConfig 1.27 is available (for example the copy bundled with CCS), copy the TIFS
`product.json` to a scratch directory, lower its `minToolVersion`, and link `source` and `docs`
next to it:

```bash
TIFS=../../../ti-sdk/TI/tifs_sdk_am263px
SCRATCH=$HOME/sysconfig-workaround
mkdir -p $SCRATCH/.metadata
cp $TIFS/.metadata/product.json $SCRATCH/.metadata/product.json
sed -i '' 's/"minToolVersion" : "1.28.0"/"minToolVersion" : "1.25.0"/' $SCRATCH/.metadata/product.json
ln -sfn "$(cd $TIFS && pwd)/source" $SCRATCH/source
ln -sfn "$(cd $TIFS && pwd)/docs" $SCRATCH/docs
```

### Full command

```bash
make syscfg \
    SYSCFG_PATH=/Applications/ti/ccs2050/ccs/utils/sysconfig_1.27.0 \
    SYSCFG_NODE=/Applications/ti/ccs2050/ccs/tools/node/node \
    SYSCFG_PRODUCT_JSON=$HOME/sysconfig-workaround/.metadata/product.json \
    MCU_PLUS_SDK_PATH=../../../ti-sdk/mcu_plus_sdk_am263px
```

`make syscfg` always runs, so there is no need for `-B`. The rule removes the unused
`ti_enet_*`, `ti_usb_*`, `ti_sdl_*` and networking files that SysConfig writes.

After regeneration the `keyring_externs.h` workaround is no longer needed. The regenerated
`tifs_modules_config.h` declares the keyring externs and `KEYRING_init()` itself. If you
added that workaround header or its include, remove it.

To refresh the `@cliArgs` header in `wolfssl.syscfg`, re-save the file from SysConfig 1.28
against the new SDK when it is available.

## Build options

Pass on the make command line, e.g.
`make wolfcrypt-test ENABLE_RSA=yes ENABLE_ECC=yes ENABLE_HW_RSA=yes ENABLE_HW_ECC=yes ENABLE_HW_RNG=yes`.
All default to `no` unless noted.

| Option | Effect |
|---|---|
| `OPT` (default `-Os`) | Optimisation level for all objects. See "Optimisation" |
| `ENABLE_AES` / `ENABLE_SHA` (default yes), `ENABLE_RSA`, `ENABLE_ECC`, `ENABLE_CHACHA` | Include the algorithm (software) |
| `ENABLE_HW_AES` | DTHE AES/GCM/CCM (`-DWOLFSSL_TI_CRYPT`) |
| `ENABLE_HW_SHA` | DTHE SHA/MD5/HMAC (`-DWOLFSSL_TI_HASH`) |
| `ENABLE_HW_RSA` | PKA RSA via a crypto callback (`-DWOLFSSL_TI_RSA`). Requires `ENABLE_RSA=yes` (build error otherwise) |
| `ENABLE_HW_ECC` | PKA ECDSA sign/verify, ECDH, EC key generation (public part) and EC public-key derivation via a crypto callback (`-DWOLFSSL_TI_ECC`). Requires `ENABLE_ECC=yes` (build error otherwise) |
| `ENABLE_ED25519` | Software Ed25519 (`ed25519.c` over `fe/ge_low_mem.c`, `-DHAVE_ED25519 -DED25519_SMALL`). Requires `ENABLE_SHA=yes` |
| `ENABLE_HW_ED25519` | PKA Ed25519 keygen/make-pub/sign/verify via a crypto callback (`ti-ed25519.c`, `-DWOLFSSL_TI_ED25519`). Requires `ENABLE_ED25519=yes`. With everything else enabled the image needs `OPT=-Oz` and `ENABLE_AES=no` to fit HSM RAM |
| `BENCH_ARGS` | Benchmark options, e.g. `-asym` or `-ed25519`: the HSM log is a 4 KB ring, a full run with RSA/ECC overflows it |
| `ENABLE_HW_RNG` | AM263Px TRNG seeds Hash_DRBG (`-DWOLFSSL_TI_RNG`) instead of the xorshift generator. Also the shared `gRngHandle` used by the PKA (see "TIFS 26.01 notes") |
| `ENABLE_ECC384`, `ENABLE_ECC521` | Also enable P-384 / P-521 (software SP code enabled to match). Require `ENABLE_ECC=yes` |
| `ENABLE_RSA4096` | Allow 4096-bit RSA keys (`-DWOLFSSL_SP_4096`; raises `SP_INT_BITS` to 4096, more stack/RAM). Requires `ENABLE_RSA=yes` |
| `TI_PKA_DEVID` | wolfCrypt devId of the PKA device, default `0x2D`. Must not be `1` (used by the wolfCrypt cryptocb test) |

`ENABLE_HW_RSA` / `ENABLE_HW_ECC` additionally define `WOLFSSL_TI_PKA_DEV_ID` and `WC_USE_DEVID` (so the
wolfCrypt test and benchmark create their keys with the PKA devId and exercise the hardware path), and add
`cryptocb.c` and the port files `ti-am26xx.c`, `ti-pka.c`, `ti-rsa.c` / `ti-ecc.c`. The software `rsa.c` /
`ecc.c` stay in the build. The TIFS PKA driver, firmware, `ecdsa.c` (curve table) and `rng.c` are already part
of the TIFS source list. `main.c` registers the device (`wolfSSL_TI_PkaRegister`) after `wolfCrypt_Init()` and
unregisters it at the end; if registration fails it prints a message and continues in software.

`ENABLE_HW_ECC` without `ENABLE_HW_RNG` prints a make warning: the DRBG that supplies ECDSA nonces and EC
private keys would then be seeded from the non-cryptographic xorshift generator. That generator is for
test/benchmark only and is **not secure**; use `ENABLE_HW_RNG=yes` for anything real.

### What is offloaded vs software

| Operation | Where |
|---|---|
| RSA private (CRT) and public op, 1024 to 4096 bit as supported by SP math (PKA: n up to 4096 bit, e up to 64 bit) | PKA (padding, ASN.1 stay in `rsa.c`) |
| ECDSA sign / verify on the curves in the TIFS table (P-256; P-384 / P-521 when enabled) | PKA |
| ECDH shared secret on the curves in the TIFS table | PKA. wolfSSL's input checks run first (curve, cofactor flag, peer point on curve, key range) |
| EC key generation | Private scalar from the wolfSSL DRBG; public point computed on the PKA |
| EC public-key derivation (`make_pub`) | PKA |
| RSA key generation, EC public-key check | Software (not implemented by the TIFS PKA driver) |
| Unsupported sizes/curves, or PKA reporting "unavailable" | Falls back to software |
| Ed25519 (plain RFC 8032) keygen, make-pub, sign, verify | PKA (`ENABLE_HW_ED25519`). Ed25519ctx/ph fall back to software. Signatures with s >= L are rejected before the PKA (TIFS `PKA_ECMontMulX` hangs on a PKA error) |
| TRNG | Seed source only (feeds Hash_DRBG) |

## TIFS 26.01 notes

The port is built against TIFS 26.01.00 (`TIFS_PATH`) and the MCU+ SDK 26.01.00 (`MCU_PLUS_SDK_PATH`).
Changes from the earlier TIFS that affect this project:

- **PKA needs a shared TRNG handle.** TIFS `pka.c` requires a non-NULL `RNG_Handle gRngHandle`.
  `AsymCrypt_open()` returns NULL without one, which makes the PKA unavailable for that boot, and
  `AsymCrypt_close()` closes the RNG, so the port never calls it. The port defines `gRngHandle` in
  `ti-am26xx.c` whenever the PKA or the TRNG offload is built. It opens and sets up the RNG once, before the
  first `AsymCrypt_open()`, and `ti-rng.c` shares the same handle. To opt out, define
  `WOLFSSL_TI_EXTERN_RNG_HANDLE`: the application then defines `RNG_Handle gRngHandle` and opens it
  (`RNG_open(0)` and `RNG_setup()`) before any PKA call.
- **Random numbers.** The raw TRNG (TIFS mode 0) seeds wolfSSL's Hash_DRBG. The TIFS DRBG mode is not used:
  it stops returning data after about 64 KB and has no reseed or health-test API.
- **AES / GCM / CCM limits (DTHE).** CBC, CTR, ECB and CFB inputs of any length are split internally into
  hardware-sized pieces. GCM and CCM inputs and AAD are limited to 0xFFFFF bytes (1 MiB - 1) per call; a
  larger length returns `BAD_FUNC_ARG`. CCM tag and nonce lengths that the DTHE does not support are
  rejected. CMAC accepts 16, 24 and 32-byte keys.
- **SHA / HMAC limits (DTHE).** The DTHE has one SHA context. Hash state for each hash object is kept
  separately by the port, so interleaved update streams are supported (see `ti-hash.c`). Hardware HMAC is
  used only for SHA-256 and SHA-512. HMAC with MD5, SHA-1, SHA-224 or SHA-384 is built on the port's hash
  (ipad/opad) in the same API. `ENABLE_HW_SHA` keeps SHA-512/224 and SHA-512/256 disabled
  (`WOLFSSL_NOSHA512_224`, `WOLFSSL_NOSHA512_256`).
- **RSA.** TIFS RSA keygen is not implemented, so RSA key generation stays in software.

## Known limitations

- The PKA is a single serialized engine; hardware calls are mutex-protected
  (no-ops under `SINGLE_THREADED`, which this example uses).
- Software RSA here is the SP-math code only (no `mp_exptmod`), so RSA sizes
  are limited to 2048/3072 (plus 4096 with `ENABLE_RSA4096`).
- With `ENABLE_HW_RSA` the build drops `RSA_LOW_MEM` (the PKA needs dP/dQ/u),
  which increases `RsaKey` RAM use; the 16 KB stack and 16 KB heap are tight.
- The wolfCrypt test/benchmark run with `WC_USE_DEVID`, so the HW path is what
  gets measured; keygen/ECDH figures are software numbers.
- Brainpool curves are not enabled.
- Builds with tiarmclang could not be done on the authoring machine; only
  `make` dry runs and syntax checks were run, nothing was flashed or tested.

## Output

- `build/wolfcrypt-test.release.out` + `.bin` — wolfCrypt test image (loads at `0x0`)
- `build/wolfcrypt-benchmark.release.out` + `.bin` — wolfCrypt benchmark image
- The `.out`/`.bin` must be signed into an `.hsmimage` (via `mcu_rom_image_gen.py`)
  and loaded by the R5F SBL as the HSM runtime firmware, then flashed.

## Project Structure

```
am26xx/
├── Makefile                     # Build system (TIFS HSM sources + wolfCrypt, HW options)
├── wolfssl.syscfg               # SysConfig project for hsm0-0 (AM263P4, ZCZ_S)
├── main.c                       # HSM boot: System_init → Modules_init → … → test/bench
├── wolfssl_platform.c           # clock_gettime (ClockP) + xorshift64 PRNG (unless ENABLE_HW_RNG)
├── user_settings.h              # wolfSSL configuration
├── ti-arm-clang/
│   └── linker.cmd               # HSM M4 memory map (256KB HSM RAM, OCRAM NOLOAD overflow)
├── generated/                   # SysConfig output (DPL, pinmux, clocks, board, tifs_modules)
└── stubs/
    └── sys/time.h               # struct timeval + clock_gettime stub
```
