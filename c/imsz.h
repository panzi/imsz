#ifndef IMSZ_H
#define IMSZ_H
#pragma once

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    #include <windows.h>
#endif

#ifdef __cpluspluc
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
} ImFormat;

#define IMSZ_INIT { .format = 0, .width = (uint64_t)0, .height = (uint64_t)0 }

typedef struct ImInfo {
    unsigned int format;
    uint64_t width;
    uint64_t height;
} ImInfo;

IMSZ_EXPORT int imsz(const char *fname, ImInfo *info_ptr);
IMSZ_EXPORT int imszmem(const void *mem, size_t len, ImInfo *info_ptr);
IMSZ_EXPORT int imszfd(int fd, ImInfo *info_ptr);
IMSZ_EXPORT const char *imsz_format_name(int format);

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    IMSZ_EXPORT int imszw(const wchar_t *fname, ImInfo *info_ptr);
    IMSZ_EXPORT int imszhnd(HANDLE hnd, ImInfo *info_ptr);
    IMSZ_EXPORT const wchar_t *imsz_format_namew(int format);
#else
    #define imszf(fp, info_ptr) imszfd(fileno((fp)), (info_ptr))
#endif

#ifdef __cpluspluc
}
#endif

#endif
