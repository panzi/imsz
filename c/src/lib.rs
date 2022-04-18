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
pub extern "C" fn imsz(fname: *const c_char, info_ptr: *mut ImInfoC) -> c_int {
    #[cfg(target_family="unix")]
    use std::{os::unix::ffi::OsStrExt, ffi::OsStr};

    let fname = unsafe { std::ffi::CStr::from_ptr(fname) };

    #[cfg(target_family="unix")]
    let fname = OsStr::from_bytes(fname.to_bytes());

    #[cfg(not(target_family="unix"))]
    let fname = unsafe { String::from_utf8_unchecked(Vec::from(fname.to_bytes())) };

    return convert_result(imsz::imsz(fname), info_ptr);
}

#[no_mangle]
pub extern "C" fn imszmem(mem: *const c_void, len: libc::size_t, info_ptr: *mut ImInfoC) -> c_int {
    if mem == std::ptr::null() {
        #[cfg(target_family="unix")]
        return libc::EINVAL;

        #[cfg(target_family="windows")]
        return 0x000000A0; // ERROR_BAD_ARGUMENTS
    }

    let slice = unsafe { std::slice::from_raw_parts(mem as *const u8, len) };
    let mut reader = std::io::Cursor::new(slice);

    return convert_result(imsz::imsz_from_reader(&mut reader), info_ptr);
}

#[no_mangle]
#[cfg(target_family="unix")]
pub extern "C" fn imszfd(fd: c_int, info_ptr: *mut ImInfoC) -> c_int {
    use std::os::unix::io::FromRawFd;

    if fd < 0 {
        return libc::EBADF;
    }

    let file = unsafe { File::from_raw_fd(fd) };

    return convert_result(imsz_from_reader(&mut BufReader::new(file)), info_ptr);
}

#[no_mangle]
#[cfg(target_family="windows")]
pub extern "C" fn imszfd(fd: c_int, info_ptr: *mut ImInfoC) -> c_int {
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
pub extern "C" fn imszhnd(hnd: std::os::windows::io::RawHandle, info_ptr: *mut ImInfoC) -> c_int {
    use std::os::windows::io::FromRawHandle;

    if hnd == std::ptr::null_mut() {
        return 0x00000006; // ERROR_INVALID_HANDLE
    }

    let file = unsafe { File::from_raw_handle(hnd) };

    return convert_result(imsz_from_reader(&mut BufReader::new(file)), info_ptr);
}

#[no_mangle]
#[cfg(target_family="windows")]
pub extern "C" fn imszw(fname: *const u16, info_ptr: *mut ImInfoC) -> c_int {
    use std::ffi::OsString;
    use std::os::windows::ffi::OsStringExt;

    let slice = unsafe { std::slice::from_raw_parts(fname, libc::wcslen(fname)) };
    let fname = OsString::from_wide(slice);

    return convert_result(imsz::imsz(fname), info_ptr);
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

// print(''.join('&['+','.join('0x%02X' % c for c in name)+'],\n' for name in FORMAT_NAMES))
#[cfg(target_family="windows")]
const FORMAT_NAMESW: &'static [&'static [u16]] = &[
    &[0x28,0x75,0x6E,0x6B,0x6E,0x6F,0x77,0x6E,0x29,0x00],
    &[0x67,0x69,0x66,0x00],
    &[0x70,0x6E,0x67,0x00],
    &[0x62,0x6D,0x70,0x00],
    &[0x6A,0x70,0x65,0x67,0x00],
    &[0x77,0x65,0x62,0x70,0x00],
    &[0x71,0x6F,0x69,0x00],
    &[0x70,0x73,0x64,0x00],
    &[0x78,0x63,0x66,0x00],
    &[0x69,0x63,0x6F,0x00],
    &[0x61,0x76,0x69,0x66,0x00],
    &[0x74,0x69,0x66,0x66,0x00],
    &[0x4F,0x70,0x65,0x6E,0x45,0x58,0x52,0x00],
    &[0x70,0x63,0x78,0x00],
    &[0x74,0x67,0x61,0x00],
    &[0x64,0x64,0x73,0x00],
    &[0x68,0x65,0x69,0x63,0x00],
];

#[no_mangle]
pub extern "C" fn imsz_format_name(format: c_int) -> *const c_char {
    if format < 0 {
        return FORMAT_NAMES[0].as_ptr() as *const c_char;
    }
    return FORMAT_NAMES.get(format as usize).unwrap_or(&FORMAT_NAMES[0]).as_ptr() as *const c_char;
}

#[no_mangle]
#[cfg(target_family="windows")]
pub extern "C" fn imsz_format_namew(format: c_int) -> *const u16 {
    if format < 0 {
        return FORMAT_NAMESW[0].as_ptr();
    }
    return FORMAT_NAMESW.get(format as usize).unwrap_or(&FORMAT_NAMESW[0]).as_ptr();
}
