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

Linux (GCC preset):

```bash
cmake --preset gcc-linux
cmake --build --preset gcc-linux
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
  --output /tmp/custom.pgm
```
