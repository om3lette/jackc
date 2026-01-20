#include "jackc_stdlib.h"
#include "common/logger.h"
#include "rars/rars_syscalls.h"
#include <stdarg.h>
#include "common/logger.h"

void jackc_vprintf(const char* format, va_list args) {
    while (*format) {
        if (*format == '%') {
            ++format;
            switch (*format) {
                case 'd':
                    rars_print_int(va_arg(args, int));
                    break;
                case 'u':
                    rars_print_uint(va_arg(args, unsigned int));
                    break;
                case 'c':
                    // 'char' is promoted to 'int' when passed through
                    rars_print_char((char)va_arg(args, int));
                    break;
                case 's':
                    const char* str = va_arg(args, const char*);
                    if (!str) rars_print_string("(null)");
                    else      rars_print_string(str);
                    break;
                case 'f':
                    // 'float' is promoted to 'double' when passed through
                    rars_print_float((float)va_arg(args, double));
                    break;
                case 'p':
                    rars_print_int((int)va_arg(args, void*));
                    break;
                default:
                    LOG_ERROR("Invalid format specifier: %c.\n", *format);
                    break;
            }
        } else {
            rars_print_char(*format);
        }
        ++format;
    }
}

void jackc_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    jackc_vprintf(format, args);
    va_end(args);
}

/**
 * @todo: Normalize variable names accross platforms
 */
char* jackc_read_file_content(const char* file_path) {
    int fd = rars_open_file(file_path, READ_ONLY_MODE);
    if (fd < 0) {
        return NULL;
    }
    LOG_DEBUG("Opened source file.\n");

    long file_size = rars_lseek(fd, 0L, RARS_SEEK_END);
    if (file_size < 0) {
        rars_close_file(fd);
        return NULL;
    }
    if (rars_lseek(fd, 0L, RARS_SEEK_SET) < 0) {
        rars_close_file(fd);
        return NULL;
    }
    LOG_DEBUG("Calculated file content length.\n");

    char* content_buffer = jackc_alloc((size_t)(file_size + 1));

    long bytes_read = rars_read(fd, content_buffer, (size_t)file_size);
    content_buffer[bytes_read] = '\0';
    rars_close_file(fd);

    if (bytes_read != file_size) {
        LOG_ERROR("Invalid size.\n");
        return NULL;
    }

    LOG_DEBUG("Saved the content to a buffer.\n");
    return content_buffer;
}
