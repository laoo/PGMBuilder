[![Build](https://github.com/laoo/PGMBuilder/actions/workflows/build.yml/badge.svg)](https://github.com/laoo/PGMBuilder/actions/workflows/build.yml)

# PGMBuilder

A tool that converts IGS PGM ROM sets into a single .pgm cartridge image.

## Getting Source

Clone with submodules:

```bash
git clone --recurse-submodules <repo-url>
```

If you already cloned without submodules:

```bash
git submodule update --init --recursive
```

The project depends on submodule `libextern/zip`.

## Requirements

- CMake 3.25+
- C++23 compiler
- Git with submodule support

## Build

Linux/macOS (native preset):

```bash
cmake --preset native
cmake --build --preset native
```

Windows (MSVC preset):

```bash
cmake --preset vs-windows
cmake --build --preset vs-windows
```

Binary location:

```text
out/build/<preset>/PGMBuilder
```

## CI (GitHub Actions)

Repository includes a GitHub Actions workflow that builds release binaries on:

- Linux x64 (Ubuntu; libstdc++ linked statically)
- macOS universal (Apple silicon and Intel, macOS 11 or later)
- Windows x64 (MSVC; runtime linked statically)

It runs on every push and pull request.

### Build artifacts

Each workflow run uploads build outputs as downloadable artifacts:

- `PGMBuilder-linux-x64`
- `PGMBuilder-macos-universal`
- `PGMBuilder-windows-x64`

To download:

1. Open GitHub repository `Actions` tab.
2. Open a finished `Build` workflow run.
3. In the `Artifacts` section, download the package for your platform.

### Releases

Pushing a tag that starts with `v` builds the three packages and publishes them as a GitHub
release named after the tag, with notes generated from the commits since the previous release:

```bash
git tag v0.22
git push origin v0.22
```

Each package holds the binary, `README.md` and `LICENSE`. The macOS binary is not notarized; if
macOS refuses to open it, run `xattr -d com.apple.quarantine PGMBuilder`.

## Usage

PGMBuilder supports two operational modes.

1. ZIP mode (existing behavior):
   - single ZIP input
   - directory batch input
2. Manual ROM mode (no ZIP):
   - build a single .pgm directly from ROM files and explicit mappings

ZIP mode syntax (positional):

```bash
PGMBuilder input [output]
```

ZIP mode syntax (named options):

```bash
PGMBuilder --input <path-to-zip-or-directory> [--output <output-directory>]
```

Manual ROM mode syntax:

```bash
PGMBuilder --rom-prg=<path@mapping> [--rom-int=<path@mapping>] [--rom-ext=<path@mapping>] [--rom-tle=<path@mapping>] [--rom-spm=<path@mapping>] [--rom-spc=<path@mapping>] [--rom-aud=<path@mapping>] --output <absolute-path-to-file.pgm>
```

## CLI Parameters

- `-h, --help`: show help
- `-i, --input <path>`: ZIP mode source (ZIP file or directory)
- `-o, --output <path>`:
  - ZIP mode: output directory (default: parent of input)
  - Manual ROM mode: required full target file path ending with `.pgm`

Manual ROM mode arguments:

- `--rom-prg=<path@mapping>`: required
- `--rom-int=<path@mapping>`: optional
- `--rom-ext=<path@mapping>`: optional
- `--rom-tle=<path@mapping>`: optional
- `--rom-spm=<path@mapping>`: optional
- `--rom-spc=<path@mapping>`: optional
- `--rom-aud=<path@mapping>`: optional

Rules:

- `--input` and `--rom-*` are mutually exclusive
- each `--rom-*` option can be used only once
- `path@mapping` must contain `@`
- `mapping` supports decimal or hexadecimal (`0x...`) numbers

Optional header info parameters for manual ROM mode:

- `--manufacturer=<text>`
- `--short-name=<text>`
- `--ascii-long-name=<text>`
- `--utf8-long-name=<text>`
- `--year=<text>`
- `--hardware=<value>`
- `--vertical`: mark the game as vertical (see [Header flags](#header-flags))

Accepted `--hardware` values:

- `state`
- `asic3`
- `012_025`
- `022_025`
- `arm_type1`
- `arm_type2`
- `arm_type3`
- `028_025`

## Examples

Single ZIP:

```bash
PGMBuilder ./roms/kov.zip ./out
```

Batch from directory:

```bash
PGMBuilder --input ./roms --output ./out
```

Manual ROM minimal:

```bash
PGMBuilder --rom-prg=/tmp/rom.bin@0x1000 --output /tmp/manual.pgm
```

Manual ROM with extra sections and metadata:

```bash
PGMBuilder \
  --rom-prg=/tmp/prg.bin@0x1000 \
  --rom-tle=/tmp/tle.bin@0x400000 \
  --rom-spm=/tmp/spm.bin@0x800000 \
  --manufacturer="IGS" \
  --short-name="custom" \
  --ascii-long-name="Custom Build" \
  --year="2026" \
  --hardware=state \
  --vertical \
  --output /tmp/custom.pgm
```

## Header flags

Since format version `0x0022` the header info (`pgm::Header::Info` in `pgm.hpp`) ends with a
32-bit `flags` field, and `infoSize` grows from 76 to 80 bytes. A reader finds the field only
when `infoSize` covers it; in older images every flag is clear.

| Bit | Name | Meaning |
|---|---|---|
| 0 | `InfoFlags_Vertical` | The game was made for a monitor turned on its side (TATE). The board draws its 448x224 picture lying with its top at the left; it is shown upright turned 90 degrees anticlockwise (MAME's `ROT270`). |

Other bits are reserved and written as zero.

In ZIP mode the vertical flag comes from MAME's driver list: it is set for every set MAME draws
`ROT270`, which on PGM are DoDonPachi II, DoDonPachi III, Ketsui and Espgaluda with their clones.
In manual ROM mode it is set by `--vertical`.
