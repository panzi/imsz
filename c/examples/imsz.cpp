#define _POSIX_C_SOURCE 1
#define _POSIX_SOURCE

#include <string.h>
#include <unistd.h>

#include <iostream>

#include "imsz.h"

bool print_result(const char *fname, int error, const ImInfo *info) {
    switch (error) {
        case IMSZ_OK:
            std::cout << fname << ": " << imsz_format_name(info->format) << ", " << info->width << " x " <<  info->height << std::endl;
            return true;

        case IMSZ_ERR_IO:
            std::cerr << fname << ": IO Error" << std::endl;
            return false;

        case IMSZ_ERR_PARSER:
            std::cerr << fname << ": Parser Error " << imsz_format_name(info->format) << std::endl;
            return false;

        case IMSZ_ERR_UNSUPPORTED:
            std::cerr << fname << ": Unsupported Format" << std::endl;
            return false;

        default:
            std::cerr << fname << ": " << strerror(error) << std::endl;
            return false;
        }
}

int main(int argc, char *argv[]) {
    int status = 0;

    if (argc <= 1) {
        ImInfo info = IMSZ_INIT;

        int error = imsz(STDIN_FILENO, &info);
        if (!print_result("<stdin>", error, &info)) {
            status = 1;
        }
    } else {
        for (int index = 1; index < argc; ++ index) {
            const char *fname = argv[index];
            ImInfo info = IMSZ_INIT;

            int error = imsz(fname, &info);
            if (!print_result(fname, error, &info)) {
                status = 1;
            }
        }
    }

    return status;
}
