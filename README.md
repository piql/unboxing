# Boxinglib

**boxinglib** is a cross-platform software library for **decoding**
high-capacity 2D barcode images. The flexbible barcode format is fully
customizeable in terms of **frame geometry**, number of **symbols per pixel**
and **forward-error-correction** (FEC) method. This makes it a sutiable choice
for storing large amounts of any kind of digital data on storage mediums such
paper, photographic film or similar.

The 2D barcode format consists of a rectangular **border frame** and a **content
area** inside the frame. The border has **corner marks** for detecting the image
area and **synchronization marks** along the border for accurately tracking the
content of the frame. The border has areas reserved for metadata in analog and
digital format. The **digital metadata section** is at the bottom border and the
**analog metadata section** is at the left and right borders. Metadata tags are
fully customizeable and can be tags like a frame identification number or boxing
format name.

The content area can contain either **analog images** or **digital data**.
Digital data is encoded with FEC and written as a image where each pixel encodes
two or more symbols. Analog data is written as monochrome images with 8bit
resolution.

To decode the images the **reading device** needs to capture the 2D barcode from
the storage medium and convert it to a digital image with 8bit resolution per
pixel. The image should be read with ~2.7x oversampling compared to the original
source resolution.

The library is released under GPLv3 licence. If you want a closed source
commercial licence, please contact Piql AS.

## Table of Contents

1. [Boxinglib](#boxinglib)
2. [Features](#features)
3. [Requirements](#requirements)
4. [Installation](#installation)
   1. [iVM](#ivm)
5. [Sample Applications](#sample-applications)
6. [Contact](#contact)

## Features

- Flexible and user configurable 2D barcode format, including bit depth,
  geometry and forward error correction method.
- Support for interleaved error correction where the data (and error correction
  codes) are spread across multiple barcodes.
- Support for digital or analog payload.
- Robust frame format with multiple features to aid tracking.
- Frame has support for user configurable machine readable and human readable
  metadata.
- Cross-platform: Tested on Windows, Linux and OS-X.
- Stand alone: Minimal dependecies to other libraries or operation system
  functions.

## Requirements

C language compiler supporting C99 (ISO/IEC 9899:1999).

The following C99 header files are used:

```c
#include <alloca.h>
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <malloc.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
```

A design goal is to have as few dependencies as possible.

The following 3rd party libraries are included:

- glib - LGPLv2
- reedsolomon - GPLv3, see thirdparty/reedsolomon/gpl.txt
- ldpc - see thirdparty/LDPC-codes/COPYRIGHT
- bch - GPLv2

The following dependencies must be installed to compile:

- [Git](https://git-scm.com/)
- [CMake](https://cmake.org/)
- [libcheck](https://libcheck.github.io/check/)
- [doxygen](https://www.doxygen.nl/) (for documentation)

For detailed copyright and licencing description, inspect source code under
thirdparty/.

## Installation

```sh
git clone https://github.com/piql/unboxing
cd unboxing
cmake -Bbuild
cmake --build build
```

## iVM

The iVM virtual machine is a minimal execution environment for file format
decoders. It's design goal is to minimize future implementation effort by
providing an unabmbigious and compact implementation description.

The boxing library is one of the formats the iVM natively supports.

Targeting the iVM compiler since switching to CMake is untested.

## Sample Applications

- **tests/unboxingdata**: Unittests for the unboxing library.
- **tests/unboxer**: Command line sample application for decoding 2D barcodes.
- **tests/testdata**: Sample frames in RAW and analog image formats.

## Contact

**Piql AS** Grønland 56 3045 Drammen NORWAY

_office@piql.com_ _+47 905 33 432_
