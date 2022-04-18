#define _POSIX_C_SOURCE 1
#define _POSIX_SOURCE

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "imsz.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)

#include <windows.h>

bool print_result(const LPWSTR fname, int error, const ImInfo *info) {
    switch (error) {
        case IMSZ_OK:
            wprintf(L"%ls: %ls, %I64u x %I64u\n", fname, imsz_format_namew(info->format), info->width, info->height);
            return true;

        case IMSZ_ERR_IO:
            fwprintf(stderr, L"%ls: IO Error\n", fname);
            return false;

        case IMSZ_ERR_PARSER:
            fwprintf(stderr, L"%ls: Parser Error %ls\n", fname, imsz_format_namew(info->format));
            return false;

        case IMSZ_ERR_UNSUPPORTED:
            fwprintf(stderr, L"%ls: Unsupported Format\n", fname);
            return false;

        default:
        {
            WCHAR   wszMsgBuff[1024];  // Buffer for text.
            DWORD   dwChars;  // Number of chars returned.

            // Try to get the message from the system errors.
            dwChars = FormatMessageW(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                error,
                0,
                wszMsgBuff,
                sizeof(wszMsgBuff) / sizeof(wszMsgBuff[0]),
                NULL );

            if (dwChars == 0) {
                fwprintf(stderr, L"%ls: error retreiving error message for error code %d\n", fname, error);
            } else {
                fwprintf(stderr, L"%ls: %ls\n", fname, wszMsgBuff);
            }
            return false;
        }
    }
}

int main() {
    LPWSTR *szArglist;
    int nArgs = 0;
    int status = 0;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (szArglist == NULL) {
        fwprintf(stderr, L"CommandLineToArgvW() failed!\n");
        return 1;
    }

    if (nArgs <= 1) {
        ImInfo info = IMSZ_INIT;

        int error = imsz_from_handle(GetStdHandle(STD_INPUT_HANDLE), &info);
        // Only very recent MSVC support _Generic, apparently.
        // int error = imsz(GetStdHandle(STD_INPUT_HANDLE), &info);
        if (!print_result(L"<stdin>", error, &info)) {
            status = 1;
        }
    } else {
        for (int index = 1; index < nArgs; ++ index) {
            const LPWSTR fname = szArglist[index];
            ImInfo info = IMSZ_INIT;

            int error = imsz_from_pathw(fname, &info);
            // int error = imsz(fname, &info);
            if (!print_result(fname, error, &info)) {
                status = 1;
            }
        }
    }

    LocalFree(szArglist);

    return status;
}

#else

#include <unistd.h>

bool print_result(const char *fname, int error, const ImInfo *info) {
    switch (error) {
        case IMSZ_OK:
            printf("%s: %s, %" PRIu64 " x %" PRIu64 "\n", fname, imsz_format_name(info->format), info->width, info->height);
            return true;

        case IMSZ_ERR_IO:
            fprintf(stderr, "%s: IO Error\n", fname);
            return false;

        case IMSZ_ERR_PARSER:
            fprintf(stderr, "%s: Parser Error %s\n", fname, imsz_format_name(info->format));
            return false;

        case IMSZ_ERR_UNSUPPORTED:
            fprintf(stderr, "%s: Unsupported Format\n", fname);
            return false;

        default:
            fprintf(stderr, "%s: %s\n", fname, strerror(error));
            return false;
        }
}

int main(int argc, char *argv[]) {
    int status = 0;

    if (argc <= 1) {
        ImInfo info = IMSZ_INIT;

        // int error = imsz_from_fd(STDIN_FILENO, &info);
        int error = imsz(STDIN_FILENO, &info);
        if (!print_result("<stdin>", error, &info)) {
            status = 1;
        }
    } else {
        for (int index = 1; index < argc; ++ index) {
            const char *fname = argv[index];
            ImInfo info = IMSZ_INIT;

            // int error = imsz_from_path(fname, &info);
            int error = imsz(fname, &info);
            if (!print_result(fname, error, &info)) {
                status = 1;
            }
        }
    }

    return status;
}
#endif
