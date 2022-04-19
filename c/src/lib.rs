use imsz::{imsz_from_reader, ImInfo, ImError, ImResult};
use std::os::raw::{c_int, c_char, c_uint, c_void};
use std::io::BufReader;
use std::fs::File;

#[repr(C)]
pub struct ImInfoC {
    format: c_uint,
    width:  u64,
    height: u64,
}

#[inline]
fn convert_result(result: ImResult<ImInfo>, info_ptr: *mut ImInfoC) -> c_int {
    match result {
        Ok(info) => {
            if info_ptr != std::ptr::null_mut() {
                unsafe {
                    (*info_ptr).format = info.format as c_uint;
                    (*info_ptr).width  = info.width;
                    (*info_ptr).height = info.height;
                }
            }
            return 0;
        },
        Err(ImError::IO(error)) => {
            if let Some(errnum) = error.raw_os_error() {
                return errnum;
            } else {
                return -1;
            }
        },
        Err(ImError::ParserError(format)) => {
            if info_ptr != std::ptr::null_mut() {
                unsafe {
                    (*info_ptr).format = format as c_uint;
                }
            }
            return -2;
        },
        Err(ImError::UnknownFormat) => {
            return -3;
        }
    }
}

#[no_mangle]
pub extern "C" fn imsz_from_path(path: *const c_char, info_ptr: *mut ImInfoC) -> c_int {
    #[cfg(target_family="unix")]
    use std::{os::unix::ffi::OsStrExt, ffi::OsStr};

    let path = unsafe { std::ffi::CStr::from_ptr(path) };

    #[cfg(target_family="unix")]
    let path = OsStr::from_bytes(path.to_bytes());

    #[cfg(not(target_family="unix"))]
    let path = unsafe { String::from_utf8_unchecked(Vec::from(path.to_bytes())) };

    return convert_result(imsz::imsz(path), info_ptr);
}

#[no_mangle]
pub extern "C" fn imsz_from_buffer(buf: *const c_void, len: libc::size_t, info_ptr: *mut ImInfoC) -> c_int {
    if buf == std::ptr::null() {
        #[cfg(target_family="unix")]
        return libc::EINVAL;

        #[cfg(target_family="windows")]
        return 0x000000A0; // ERROR_BAD_ARGUMENTS
    }

    let slice = unsafe { std::slice::from_raw_parts(buf as *const u8, len) };
    let mut reader = std::io::Cursor::new(slice);

    return convert_result(imsz::imsz_from_reader(&mut reader), info_ptr);
}

#[no_mangle]
#[cfg(target_family="unix")]
pub extern "C" fn imsz_from_fd(fd: c_int, info_ptr: *mut ImInfoC) -> c_int {
    use std::os::unix::io::FromRawFd;

    if fd < 0 {
        return libc::EBADF;
    }

    let file = unsafe { File::from_raw_fd(fd) };

    return convert_result(imsz_from_reader(&mut BufReader::new(file)), info_ptr);
}

#[no_mangle]
#[cfg(target_family="windows")]
pub extern "C" fn imsz_from_fd(fd: c_int, info_ptr: *mut ImInfoC) -> c_int {
    use std::os::windows::io::FromRawHandle;

    let hnd = unsafe { libc::get_osfhandle(fd) };

    if hnd == -1 {
        return 0x00000006; // ERROR_INVALID_HANDLE
    }

    let file = unsafe { File::from_raw_handle(hnd as std::os::windows::io::RawHandle) };

    return convert_result(imsz_from_reader(&mut BufReader::new(file)), info_ptr);
}

#[no_mangle]
#[cfg(target_family="windows")]
pub extern "C" fn imsz_from_handle(hnd: std::os::windows::io::RawHandle, info_ptr: *mut ImInfoC) -> c_int {
    use std::os::windows::io::FromRawHandle;

    if hnd == std::ptr::null_mut() {
        return 0x00000006; // ERROR_INVALID_HANDLE
    }

    let file = unsafe { File::from_raw_handle(hnd) };

    return convert_result(imsz_from_reader(&mut BufReader::new(file)), info_ptr);
}

#[no_mangle]
#[cfg(target_family="windows")]
pub extern "C" fn imsz_from_pathw(path: *const u16, info_ptr: *mut ImInfoC) -> c_int {
    use std::ffi::OsString;
    use std::os::windows::ffi::OsStringExt;

    let slice = unsafe { std::slice::from_raw_parts(path, libc::wcslen(path)) };
    let path = OsString::from_wide(slice);

    return convert_result(imsz::imsz(path), info_ptr);
}

const FORMAT_NAMES: &'static [&'static [u8]] = &[
    b"(unknown)\0",
    b"gif\0",
    b"png\0",
    b"bmp\0",
    b"jpeg\0",
    b"webp\0",
    b"qoi\0",
    b"psd\0",
    b"xcf\0",
    b"ico\0",
    b"avif\0",
    b"tiff\0",
    b"OpenEXR\0",
    b"pcx\0",
    b"tga\0",
    b"dds\0",
    b"heic\0",
];

#[cfg(target_family="windows")]
const fn w<const LEN: usize>(ascii: [u8; LEN]) -> [u16; LEN] {
    let mut wide = [0u16; LEN];

    let mut index = 0;
    while index < LEN {
        wide[index] = ascii[index] as u16;
        index += 1;
    }

    return wide;
}

#[cfg(target_family="windows")]
const FORMAT_NAMESW: &'static [&'static [u16]] = &[
    &w(*b"(unknown)\0"),
    &w(*b"gif\0"),
    &w(*b"png\0"),
    &w(*b"bmp\0"),
    &w(*b"jpeg\0"),
    &w(*b"webp\0"),
    &w(*b"qoi\0"),
    &w(*b"psd\0"),
    &w(*b"xcf\0"),
    &w(*b"ico\0"),
    &w(*b"avif\0"),
    &w(*b"tiff\0"),
    &w(*b"OpenEXR\0"),
    &w(*b"pcx\0"),
    &w(*b"tga\0"),
    &w(*b"dds\0"),
    &w(*b"heic\0"),
];

#[no_mangle]
pub extern "C" fn imsz_format_name(format: c_uint) -> *const c_char {
    return FORMAT_NAMES.get(format as usize).unwrap_or(&FORMAT_NAMES[0]).as_ptr() as *const c_char;
}

#[no_mangle]
#[cfg(target_family="windows")]
pub extern "C" fn imsz_format_namew(format: c_uint) -> *const u16 {
    return FORMAT_NAMESW.get(format as usize).unwrap_or(&FORMAT_NAMESW[0]).as_ptr();
}
