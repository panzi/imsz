![bear](https://github.com/scardine/imsz/blob/master/tenor.gif?raw=true)

# imsz

[![Test Status](https://img.shields.io/github/workflow/status/panzi/imsz/Tests)](https://github.com/panzi/imsz/actions/workflows/tests.yml)
[![License](https://img.shields.io/github/license/panzi/imsz)](https://github.com/panzi/imsz/blob/main/LICENSE)

Get width and height from an image file reading as few bytes as possible.

## Why?

A few years ago I answered a Stackoverflow question ["Get Image size WITHOUT loading image into memory"](https://stackoverflow.com/questions/15800704/get-image-size-without-loading-image-into-memory) (using Python). The canonical answer for dealing with images in Python is the PIL library but it is a huge library if you just want to get width and height. I rolled up my sleeves and wrote a small function to do just this. Over the years people sent patches implementing new image formats and refactoring the code until I could not recognize it anymore. I always wanted to tidy it up a bit, may be reimplement it as a C module...

## Am I rusty?

In the last 10 years I've used Python for everything. This saturday afternoon I wanted to answer the question: "at my age, can I still learn a new computer language?". So I decided to try Rust and this is the result. It was a pleasant surprise, if you are familiar with C/C++ Rust is really easy to pick up - and I see some Python influence here and there.

I don't expect it to be very idiomatic Rust, it is my first Rust project so be kind!

## First impressions

There are many things I like in Rust. I'm still looking for a good debugger, the one I was using with VSCode is unable to show the values in a HashMap, for example.

## Usage

There is a simple example binary:

```plain
> cargo run --example imsz tenor.gif
tenor.gif: gif, 220 x 159

> cargo run --example imsz -- -h
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

// alternatively if you have someting implementing Read and Seek:
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
