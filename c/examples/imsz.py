#!/usr/bin/env python3

import io
import cffi
import platform

from typing import NamedTuple, Optional, Union, Any, Callable
from enum import Enum
from os.path import join as join_path, abspath, dirname
from os import fsencode, PathLike

__all__ = 'ImInfo', 'ImError', 'ImFormat', 'imsz', 'imsz_from_path', 'imsz_from_buffer', 'imsz_from_fd', 'imsz_from_file'

IS_WINDOWS = platform.system() == 'Windows'

ffi = cffi.FFI()
ffi.cdef("""
typedef struct ImInfo {
    unsigned int format;
    uint64_t width;
    uint64_t height;
} ImInfo;
int imsz_from_path(const char *fname, ImInfo *info_ptr);
int imsz_from_buffer(const void *mem, size_t len, ImInfo *info_ptr);
int imsz_from_fd(int fd, ImInfo *info_ptr);
const char *imsz_format_name(int format);
""")

format_error: Callable[[int], str]

if IS_WINDOWS:
    from ctypes import FormatError # type: ignore
    format_error = FormatError # type: ignore

    ffi.cdef("""
int imsz_from_pathw(const wchar_t *fname, ImInfo *info_ptr);
const wchar_t *imsz_format_namew(int format);
""")

else:
    from os import strerror
    format_error = strerror

libpath = join_path(dirname(abspath(__file__)), "..", "target", "debug" if __debug__ else "release", "libimsz.so")

_imsz = ffi.dlopen(libpath)

class ImFormat(Enum):
    GIF     =  1
    PNG     =  2
    BMP     =  3
    JPEG    =  4
    WEBP    =  5
    QOI     =  6
    PSD     =  7
    XCF     =  8
    ICO     =  9
    AVIF    = 10
    TIFF    = 11
    OpenEXR = 12
    PCX     = 13
    TGA     = 14
    DDS     = 15
    HEIC    = 16
    JP2K    = 17

    def __str__(self) -> str:
        return ffi.string(_imsz.imsz_format_name(self.value)).decode('ASCII')

class ImInfo(NamedTuple):
    format: ImFormat
    width:  int
    height: int

class ImError(Exception):
    __slots__ = ()

    def __str__(self) -> str:
        return f"Unknown Error (outdated Python bindings?)"

class IOError(ImError):
    __slots__ = 'error',
    error: Optional[int]

    def __init__(self, error: Optional[int]=None) -> None:
        super().__init__()
        self.error = error

    def __str__(self) -> str:
        error = self.error
        if error is None:
            return "IO Error"
        return format_error(error)

class ParserError(ImError):
    __slots__ = 'format',
    format: ImFormat

    def __init__(self, format: ImFormat) -> None:
        super().__init__()
        self.format = format

    def __str__(self) -> str:
        return f"Parser Error {self.format}"

class UnsupportedFormat(ImError):
    __slots__ = ()

    def __str__(self) -> str:
        return "Unsupported Format"

def _convert_result(result: int, cinfo: Any) -> ImInfo:
    if result == 0:
        return ImInfo(ImFormat(cinfo.format), cinfo.width, cinfo.height)
    elif result == -1:
        raise IOError()
    elif result == -2:
        raise ParserError(ImFormat(cinfo.format))
    elif result == -3:
        raise UnsupportedFormat()
    elif result > 0:
        raise IOError(result)
    else:
        raise ImError()

if IS_WINDOWS:
    def imsz_from_path(path: Union[str, PathLike]) -> ImInfo:
        info_ptr = ffi.new("ImInfo*")
        result = _imsz.imsz_from_pathw(ffi.new("wchar_t[]", path), info_ptr)
        return _convert_result(result, info_ptr)

else:
    def imsz_from_path(path: Union[str, PathLike]) -> ImInfo:
        info_ptr = ffi.new("ImInfo*")
        result = _imsz.imsz_from_path(ffi.new("char[]", fsencode(path)), info_ptr)
        return _convert_result(result, info_ptr)

def imsz_from_buffer(data: Union[bytes, bytearray, memoryview]) -> ImInfo:
    info_ptr = ffi.new("ImInfo*")
    result = _imsz.imsz_from_buffer(data, len(data), info_ptr)
    return _convert_result(result, info_ptr)

def imsz_from_fd(fd: int) -> ImInfo:
    info_ptr = ffi.new("ImInfo*")
    result = _imsz.imsz_from_fd(fd, info_ptr)
    return _convert_result(result, info_ptr)

def imsz_from_file(fp: Union[io.BytesIO, io.FileIO]) -> ImInfo:
    if isinstance(fp, io.BytesIO):
        return imsz_from_buffer(fp.getbuffer())

    return imsz_from_fd(fp.fileno())

def imsz(input: Union[str, PathLike, bytes, bytearray, memoryview, int, io.BytesIO, io.FileIO]) -> ImInfo:
    if isinstance(input, (bytes, bytearray, memoryview)):
        return imsz_from_buffer(input)

    if isinstance(input, int):
        return imsz_from_fd(input)

    if isinstance(input, (io.BytesIO, io.FileIO)):
        return imsz_from_file(input)

    return imsz_from_path(input)

if __name__ == '__main__':
    import sys
    if len(sys.argv) <= 1:
        try:
            info = imsz(sys.stdin.fileno())
        except Exception as error:
            print(f"<stdin>: {error}", file=sys.stderr)
        else:
            print(f"<stdin>: {info.format}, {info.width} x {info.height}")
    else:
        for fname in sys.argv[1:]:
            try:
                info = imsz(fname)
            except Exception as error:
                print(f"{fname}: {error}", file=sys.stderr)
            else:
                print(f"{fname}: {info.format}, {info.width} x {info.height}")
