/// @file
/// Get image width and height reading as few bytes as possible

#ifndef IMSZ_H
#define IMSZ_H
#pragma once

#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 1
#endif

#ifndef _POSIX_SOURCE
    #define _POSIX_SOURCE
#endif

#include <stdint.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    #include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    #ifdef IMSZ_STATIC
        #define IMSZ_EXPORT
    #else
        #define IMSZ_EXPORT __declspec(dllimport)
    #endif
#else
    #if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
        #define IMSZ_EXPORT __attribute__ ((visibility ("default")))
    #else
        #define IMSZ_EXPORT extern
    #endif
#endif

/// @brief Error values.
///
/// **NOTE:** The API doesn't use enum ImError as a return type, but int. This
/// is so to be certain about the ABI since the library is actually implemented
/// in Rust, which doesn't know about C enums. And C enums aren't guaranteed to
/// be any certain integer type.
///
/// Also a returned error value is not limited to these values, but can also be
/// a `errno` value under POSIX or a Windows error code under Windows. Both,
/// POSIX and Windows error codes are positive integers (or 0 for no error),
/// these custom error codes are negative values.
typedef enum ImError {
    IMSZ_OK              =  0, ///< No error.
    IMSZ_ERR_IO          = -1, ///< IO error happened, but no OS error (errno or WIndows error code) was reported. (Classic should never happen.)
    IMSZ_ERR_PARSER      = -2, ///< File format was detected, but there was an error parsing the file. ::ImInfo::format will be set to the detected file format.
    IMSZ_ERR_UNSUPPORTED = -3, ///< File format is not supported.
} ImError;

/// @brief All supported image formats.
typedef enum ImFormat {
    IMSZ_GIF     =  1u, ///< Graphics Interchange Format files in version GIF87a or GIF89a.
    IMSZ_PNG     =  2u, ///< Portable Network Graphics files. Requires the first chunk to be `IHDR`.
    IMSZ_BMP     =  3u, ///< Windows Bitmap, both for Windows 2.0 (BITMAPCOREHEADER) and for newer versions (BITMAPINFOHEADER).
    IMSZ_JPEG    =  4u, ///< Joint Photographic Experts Group files.
    IMSZ_WEBP    =  5u, ///< WebP files. Supported sub-formats: `VP8 `, `VP8L`, `VP8X`.
    IMSZ_QOI     =  6u, ///< Quite OK Image format files.
    IMSZ_PSD     =  7u, ///< Adobe Photoshop files.
    IMSZ_XCF     =  8u, ///< GIMP files.
    IMSZ_ICO     =  9u, ///< ICO files can contain multiple images. This returns the dimensions of the biggest image in the file.
    IMSZ_AVIF    = 10u, ///< AV1 Image File Format.
    IMSZ_TIFF    = 11u, ///< Tag Image File Format. Supports big endian and little endian TIFF files.
    IMSZ_OpenEXR = 12u, ///< OpenEXR files.
    IMSZ_PCX     = 13u, ///< PiCture eXchange files.
    IMSZ_TGA     = 14u, ///< TARGA (Truevision Advanced Raster Graphics Adapter) files.
    IMSZ_DDS     = 15u, ///< DirectDraw Surface files.
    IMSZ_HEIC    = 16u, ///< HEIC/HEIF files.
    IMSZ_JP2K    = 17u, ///< JPEG 2000 files.
} ImFormat;

/// Initialize an ImInfo variable with all 0 values.
///
/// **NOTE:** The API doesn't use enum ImFormat as a type, but unsigned int. This
/// is so to be certain about the ABI since the library is actually implemented
/// in Rust, which doesn't know about C enums. And C enums aren't guaranteed to
/// be any certain integer type.
#define IMSZ_INIT { .width = (uint64_t)0, .height = (uint64_t)0, .format = 0 }

/// @brief The width, height and format of an image.
typedef struct ImInfo {
    uint64_t width;  ///< Width of the image.
    uint64_t height; ///< Height of the image.

    /// @brief Values from ::ImFormat.
    ///
    /// It doesn't use enum ::ImFormat but unsigned int in order to be certain
    /// about the ABI since the library is actually implemented in Rust.
    ///
    /// This field is also set in the case of an ::IMSZ_ERR_PARSER, not just on
    /// ::IMSZ_OK. Then it specifies which file format was detected, but failed
    /// to parse.
    unsigned int format;
} ImInfo;

/// Get image width and height from file at @p path.
///
/// @param path Image file path.
/// @param info_ptr Pointer to where to write the result. Can be NULL.
/// @return ::ImError value or `errno` value under POSIX and Windows error code under Windows.
IMSZ_EXPORT int imsz_from_path(const char *path, ImInfo *info_ptr);

/// Get image width and height from file loaded in @p buf.
///
/// @param buf Image file loaded into memory.
/// @param len Size of @p buf in bytes.
/// @param info_ptr Pointer to where to write the result. Can be NULL.
/// @return ::ImError value or `errno` value under POSIX and Windows error code under Windows.
IMSZ_EXPORT int imsz_from_buffer(const void *buf, size_t len, ImInfo *info_ptr);

/// Get image width and height from file descriptor @p fd.
///
/// @param fd A file descriptor of an image file. Must be seekable.
/// @param info_ptr Pointer to where to write the result. Can be NULL.
/// @return ::ImError value or `errno` value under POSIX and Windows error code under Windows.
IMSZ_EXPORT int imsz_from_fd(int fd, ImInfo *info_ptr);

/// Get the name of an image file format.
///
/// @param format A ImFormat value.
/// @return The ASCII name of the image file format or `"(unknown)"` for an unknown value.
IMSZ_EXPORT const char *imsz_format_name(unsigned int format);

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__DOXYGEN__)
    /// Get image width and height from file at @p path. (Windows-only)
    ///
    /// @param path Image file path as a wide string.
    /// @param info_ptr Pointer to where to write the result. Can be NULL.
    /// @return ::ImError value or Windows error code.
    IMSZ_EXPORT int imsz_from_pathw(const wchar_t *path, ImInfo *info_ptr);

    /// Get image width and height from file handle @p hnd. (Windows-only)
    ///
    /// @param hnd File handle. Must be seekable.
    /// @param info_ptr Pointer to where to write the result. Can be NULL.
    /// @return ::ImError value or Windows error code.
    IMSZ_EXPORT int imsz_from_handle(HANDLE hnd, ImInfo *info_ptr);

    /// Get the name of an image file format. (Windows-only)
    ///
    /// @param format A ImFormat value.
    /// @return The name of the image file format or `"(unknown)"` for an unknown value.
    IMSZ_EXPORT const wchar_t *imsz_format_namew(unsigned int format);

    #define imsz_from_file(fp, info_ptr) imsz_from_fd(_fileno((fp)), (info_ptr))

    #define imsz_2_(arg1, arg2) \
        _Generic((arg1), \
            wchar_t*:       imsz_from_pathw((const wchar_t*)(arg1), (arg2)), \
            const wchar_t*: imsz_from_pathw((const wchar_t*)(arg1), (arg2)), \
            char*:          imsz_from_path((const char*)(arg1), (arg2)), \
            const char*:    imsz_from_path((const char*)(arg1), (arg2)), \
            FILE*:          imsz_from_file((FILE*)(arg1), (arg2)), \
            HANDLE:         imsz_from_handle((HANDLE)(arg1), (arg2)), \
            int:            imsz_from_fd((intptr_t)(arg1), (arg2)) \
        )
#else
    #define imsz_from_file(fp, info_ptr) imsz_from_fd(fileno((fp)), (info_ptr))

    #define imsz_2_(arg1, arg2) \
        _Generic((arg1), \
            char*:          imsz_from_path((const char*)(arg1), (arg2)), \
            const char*:    imsz_from_path((const char*)(arg1), (arg2)), \
            FILE*:          imsz_from_file((FILE*)(arg1), (arg2)), \
            int:            imsz_from_fd((intptr_t)(arg1), (arg2)) \
        )
#endif

/// @def imsz_from_file(fp,info_ptr)
/// Get image width and height from FILE pointer @p fp.
///
/// @param fp FILE pointer. Must be seekable.
/// @param info_ptr Pointer to where to write the result. Can be NULL.
/// @return ::ImError value or Windows error code.

#define imsz_3_(arg1, arg2, arg3) \
    imsz_from_buffer((arg1), (arg2), (arg3))

#define imsz_va_dispatch_(arg1, arg2, arg3, arg4, ...) arg4

#ifdef __cplusplus
}
#endif

#if defined(__cplusplus) || defined(__DOXYGEN__)

/// @brief Alias of ::imsz_from_path()
///
/// Under C this is a macro using `_Generic`, under C++ an overloaded function.
inline int imsz(const char *path, ImInfo *info_ptr) {
    return imsz_from_path(path, info_ptr);
}

/// @brief Alias of ::imsz_from_buffer()
///
/// Under C this is a macro using `_Generic`, under C++ an overloaded function.
inline int imsz(const void *buf, size_t len, ImInfo *info_ptr) {
    return imsz_from_buffer(buf, len, info_ptr);
}

/// @brief Alias of ::imsz_from_fd()
///
/// Under C this is a macro using `_Generic`, under C++ an overloaded function.
inline int imsz(int fd, ImInfo *info_ptr) {
    return imsz_from_fd(fd, info_ptr);
}

/// @brief Alias of ::imsz_from_file()
///
/// Under C this is a macro using `_Generic`, under C++ an overloaded function.
inline int imsz(FILE *file, ImInfo *info_ptr) {
    return imsz_from_file(file, info_ptr);
}

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__DOXYGEN__)
/// @brief Alias of ::imsz_from_pathw()
///
/// Under C this is a macro using `_Generic`, under C++ an overloaded function.
inline int imsz(const wchar_t *path, ImInfo *info_ptr) {
    return imsz_from_pathw(path, info_ptr);
}

/// @brief Alias of ::imsz_from_handle()
///
/// Under C this is a macro using `_Generic`, under C++ an overloaded function.
inline int imsz(HANDLE hnd, ImInfo *info_ptr) {
    return imsz_from_handle(hnd, info_ptr);
}
#endif
#endif

#if !defined(__cplusplus) || defined(__DOXYGEN__)

    /// @brief All `imsz_from_*()` calls in one using `_Generic`.
    /// 
    /// Only for compilers that support C11 and only in C-mode. For C++ see @fn imsz().
    /// 
    /// Equivalent to:
    /// ```
    /// int imsz(const char *path, ImInfo *info_ptr);
    /// int imsz(const void *buf, size_t len, ImInfo *info_ptr);
    /// int imsz(int fd, ImInfo *info_ptr);
    /// int imsz(FILE *file, ImInfo *info_ptr);
    /// ```
    /// 
    /// In addition to that only on Windows:
    /// ```
    /// int imsz(const wchar_t *path, ImInfo *info_ptr);
    /// int imsz(HANDLE hnd, ImInfo *info_ptr);
    /// ```
    #define imsz(...) imsz_va_dispatch_(__VA_ARGS__, imsz_3_, imsz_2_, imsz_error_)(__VA_ARGS__)

#endif


#endif
