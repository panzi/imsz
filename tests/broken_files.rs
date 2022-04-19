use imsz::{imsz, ImError, ImFormat};

fn expect_broken(data: &[u8], expect_format: ImFormat) {
    match imsz(data) {
        Err(ImError::ParserError(format)) => {
            assert_eq!(format, expect_format);
        }
        other => {
            assert!(false, "expected: Err(ImError::ParserError(ImFormat::AVIF)), actual: {:?}", other);
        }
    }
}

#[test]
fn broken_avif() {
    let _ = imsz(b"\0\0\0\0ftypavif");
    let broken_images = [
        b"\0\0\0\0ftypavif" as &[u8],
        b"\x00\x00\x00\x0cftypavif",
        b"\x00\x00\x00\x0cftypavif\x00\x00\x00\x14meta....\x00\x00\x00\x08iprp",
        b"\x00\x00\x00\x0cftypavif\x00\x00\x00\x1cmeta....\x00\x00\x00\x10iprp\x00\x00\x00\x08ipco",
        b"\x00\x00\x00\x0cftypavif\x00\x00\x00$meta....\x00\x00\x00\x18iprp\x00\x00\x00\x10ipco\x00\x00\x00\x08ispe",
        b"\x00\x00\x00\x0cftypavif\x00\x00\x00/meta....\x00\x00\x00#iprp\x00\x00\x00\x1bipco\x00\x00\x00\x13ispe..........."
    ];
    for data in broken_images {
        expect_broken(data, ImFormat::AVIF);
    }
}

#[test]
fn broken_gif() {
    let broken_images = [
        b"GIF87a" as &[u8],
        b"GIF89a...",
    ];
    for data in broken_images {
        expect_broken(data, ImFormat::GIF);
    }
}

#[test]
fn broken_png() {
    let broken_images = [
        b"\x89PNG\r\n\x1a\n" as &[u8],
        b"\x89PNG\r\n\x1a\n\x00\x00\x00\x07IHDR.......",
    ];
    for data in broken_images {
        expect_broken(data, ImFormat::PNG);
    }
}

#[test]
fn broken_bmp() {
    let broken_images = [
        b"BM\x00\x00\x00\x0e\0\0\0\0\0\0\0\0" as &[u8],
        b"BM\x00\x00\x00\x19\0\0\0\0\x00\x00\x00\x0b...........",
    ];
    for data in broken_images {
        expect_broken(data, ImFormat::BMP);
    }
}
