#ifndef IMSZ_H
#define IMSZ_H
#pragma once

#define _POSIX_C_SOURCE 1
#define _POSIX_SOURCE
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

typedef enum ImError {
    IMSZ_OK              =  0,
    IMSZ_ERR_IO          = -1,
    IMSZ_ERR_PARSER      = -2,
    IMSZ_ERR_UNSUPPORTED = -3,
} ImError;

typedef enum ImFormat {
    IMSZ_GIF     =  1u,
    IMSZ_PNG     =  2u,
    IMSZ_BMP     =  3u,
    IMSZ_JPEG    =  4u,
    IMSZ_WEBP    =  5u,
    IMSZ_QOI     =  6u,
    IMSZ_PSD     =  7u,
    IMSZ_XCF     =  8u,
    IMSZ_ICO     =  9u,
    IMSZ_AVIF    = 10u,
    IMSZ_TIFF    = 11u,
    IMSZ_OpenEXR = 12u,
    IMSZ_PCX     = 13u,
    IMSZ_TGA     = 14u,
    IMSZ_DDS     = 15u,
    IMSZ_HEIC    = 16u,
    IMSZ_JP2K    = 17u,
} ImFormat;

#define IMSZ_INIT { .format = 0, .width = (uint64_t)0, .height = (uint64_t)0 }

typedef struct ImInfo {
    unsigned int format;
    uint64_t width;
    uint64_t height;
} ImInfo;

IMSZ_EXPORT int imsz_from_path(const char *path, ImInfo *info_ptr);
IMSZ_EXPORT int imsz_from_buffer(const void *buf, size_t len, ImInfo *info_ptr);
IMSZ_EXPORT int imsz_from_fd(int fd, ImInfo *info_ptr);
IMSZ_EXPORT const char *imsz_format_name(unsigned int format);

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    IMSZ_EXPORT int imsz_from_pathw(const wchar_t *path, ImInfo *info_ptr);
    IMSZ_EXPORT int imsz_from_handle(HANDLE hnd, ImInfo *info_ptr);
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

#define imsz_3_(arg1, arg2, arg3) \
    imsz_from_buffer((arg1), (arg2), (arg3))

#define imsz_4th_(arg1, arg2, arg3, arg4, ...) arg4

/**
 * @brief All `imsz_from_*()` calls in one using `_Generic`.
 * 
 * Only for compilers that support C11.
 * 
 * Equivalent to:
 * ```
 * int imsz(const char *path, ImInfo *info_ptr);
 * int imsz(const void *buf, size_t len, ImInfo *info_ptr);
 * int imsz(int fd, ImInfo *info_ptr);
 * int imsz(FILE *file, ImInfo *info_ptr);
 * ```
 * 
 * In addition to that only on Windows:
 * ```
 * int imsz(const wchar_t *path, ImInfo *info_ptr);
 * int imsz(HANDLE hnd, ImInfo *info_ptr);
 * ```
 */
#define imsz(...) imsz_4th_(__VA_ARGS__, imsz_3_, imsz_2_, imsz_error_)(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
