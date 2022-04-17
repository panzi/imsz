![bear](https://github.com/scardine/imsz/blob/master/tenor.gif?raw=true)

# imsz

[![Test Status](https://img.shields.io/github/workflow/status/panzi/imsz/Tests)](https://github.com/panzi/imsz/actions/workflows/tests.yml)
[![License](https://img.shields.io/github/license/panzi/imsz)](https://github.com/panzi/imsz/blob/main/LICENSE)

Get width and height from an image file reading as few bytes as possible.

This is a fork of [scardine/imsz](https://github.com/scardine/imsz) that adds
support for more file formats, but also breaks API compatibility in order to be
more idiomatic Rust. It also provides a C library wrapper. For more information
on how this started see the mentioned link.

The library itself has zero dependencies, but the example binary uses
[clap](https://crates.io/crates/clap).

## Usage

There is a simple example binary:

```bash
$ cargo run --example imsz tenor.gif
tenor.gif: gif, 220 x 159

$ cargo run --example imsz -- --help
imsz 0.2.0
Paulo Scardine <paulo@scardine.com.br>, Mathias Panzenböck <grosser.meister.morti@gmx.net>

USAGE:
    imsz [FILES]...

ARGS:
    <FILES>...    

OPTIONS:
    -h, --help       Print help information
    -V, --version    Print version information
```

The relevant parts:

```Rust
use imsz::imsz;

let info = imsz(fname)?;
println!("{}: {}, {} x {}", fname, info.format, info.width, info.height);
// tenor.gif: gif, 220 x 159

// alternatively if you have something implementing Read and Seek:
use imsz::imsz_from_reader;

let mut file = File::open(fname)?;
let info = imsz_from_reader(&mut file)?;
```

## Supported File Types

* AVIF
* BMP
* DDS
* GIF
* HEIC/HEIF
* ICO
* JPEG
* PCX
* PNG
* PSD
* OpenEXR
* QOI
* TGA
* TIFF
* WEBP
* XCF

No guarantees of correct or complete implementation are made.
