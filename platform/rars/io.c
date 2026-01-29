#include "jackc_stdio.h"
#include "jackc_stdlib.h"
#include "common/logger.h"
#include "jackc_string.h"
#include "rars/rars_syscalls.h"
#include <stdarg.h>
#include "common/logger.h"

void jackc_putchar(char c) {
    rars_print_char(c);
}

int jackc_open(const char *path, int flags) {
    file_mode mode = WRITE_CREATE_MODE;

    if (flags & O_RDONLY) mode = READ_ONLY_MODE;
    else if (flags & O_APPEND) mode = WRITE_APPEND_MODE;

    return rars_open_file(path, mode);
}

long jackc_write(int fd, const void *buf, size_t count) {
    return rars_write(fd, buf, count);
}

int jackc_close(int fd) {
    rars_close_file(fd);
    return 0;
}

static void str_reverse(char* begin, char* end) {
    while (begin < end) {
        char tmp = *begin;
        *begin++ = *end;
        *end-- = tmp;
    }
}

static char* write_int(char* buf, int n) {
    if (n == 0) {
        *buf++ = '0';
        *buf = '\0';
        return buf;
    }
    int neg = n < 0;
    if (neg) n = -n;

    char* start = buf;
    while (n != 0) {
        char digit = (char)(n % 10);
        *buf++ = '0' + digit;
        n /= 10;
    }

    if (neg) {
        *buf++ = '-';
    }
    str_reverse(start, buf - 1);
    *buf = '\0';
    return buf;
}

static char* write_uint(char* buf, unsigned int n) {
    if (n == 0) {
        *buf++ = '0';
        *buf = '\0';
        return buf;
    }
    char* start = buf;
    while (n) {
        *buf++ = '0' + (char)(n % 10);
        n /= 10;
    }
    str_reverse(start, buf - 1);
    *buf = '\0';
    return buf;
}

void jackc_vsprintf(char* buffer, const char* format, va_list args) {
    char* out = buffer;

    while (*format) {
        if (*format == '%') {
            ++format;
            long max_len = -1;

            // Parse .*
            if (*format == '.') {
                ++format;
                if (*format == '*') {
                    max_len = va_arg(args, int);
                    ++format;
                }
            }

            switch (*format) {
                case 'd': {
                    int val = va_arg(args, int);
                    out = write_int(out, val);
                    break;
                }
                case 'u': {
                    unsigned int val = va_arg(args, unsigned int);
                    out = write_uint(out, val);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    *out++ = c;
                    break;
                }
                case 's': {
                    const char* str = va_arg(args, const char*);
                    if (!str) str = "(null)";

                    if (max_len < 0) {
                        while (*str) {
                            *out++ = *str++;
                        }
                    } else {
                        int count = 0;
                        while (*str && count < max_len) {
                            *out++ = *str++;
                            count++;
                        }
                    }
                    break;
                }
                default:
                    *out++ = '%';
                    *out++ = *format;
                    break;
            }
        } else {
            *out++ = *format;
        }
        ++format;
    }

    *out = '\0';
}

void jackc_sprintf(char* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    jackc_vsprintf(buffer, format, args);
    va_end(args);
}

void jackc_vfprintf(int fd, const char* format, va_list args) {
    while (*format) {
        if (*format == '%') {
            ++format;
            long max_len = -1;

            if (*format == '.') {
                ++format;
            }

            if (*format == '*') {
                max_len = va_arg(args, int);
                ++format;
            }
            char s_buf[32];

            switch (*format) {
                case 'd':
                    write_int(s_buf, va_arg(args, int));
                    rars_write(fd, s_buf, jackc_strlen(s_buf));
                    break;
                case 'u':
                    write_uint(s_buf, va_arg(args, unsigned int));
                    rars_write(fd, s_buf, jackc_strlen(s_buf));
                    break;
                case 'c':
                    s_buf[0] = (char)va_arg(args, int);
                    rars_write(fd, s_buf, sizeof(char));
                    break;
                case 's': {
                    const char* str = va_arg(args, const char*);
                    if (!str) str = "(null)";

                    size_t bytes_to_write = max_len < 0 ? jackc_strlen(str) : (size_t)max_len;
                    rars_write(fd, str, bytes_to_write);
                    break;
                }
                default:
                    s_buf[0] = '%';
                    s_buf[1] = *format;
                    rars_write(fd, &s_buf, 2);
                    break;
            }
        } else {
            rars_write(fd, format, 1);
        }
        ++format;
    }
}

void jackc_fprintf(int fd, const char* format, ...) {
    va_list args;
    va_start(args, format);
    jackc_vfprintf(fd, format, args);
    va_end(args);
}

void jackc_vprintf(const char* format, va_list args) {
    while (*format) {
        if (*format == '%') {
            ++format;
            int max_len = -1;

            if (*format == '.') {
                ++format;
            }

            if (*format == '*') {
                max_len = va_arg(args, int);
                ++format;
            }

            switch (*format) {
                case 'd':
                    rars_print_int(va_arg(args, int));
                    break;
                case 'u':
                    rars_print_uint(va_arg(args, unsigned int));
                    break;
                case 'c':
                    rars_print_char((char)va_arg(args, int));
                    break;
                case 's': {
                    const char* str = va_arg(args, const char*);
                    if (!str) str = "(null)";

                    if (max_len < 0) {
                        // No limit found, print the whole string normally
                        rars_print_string(str);
                    } else {
                        // Limit found, print char-by-char up to max_len
                        int count = 0;
                        while (*str && count < max_len) {
                            rars_print_char(*str++);
                            count++;
                        }
                    }
                    break;
                }
                case 'f':
                    rars_print_float((float)va_arg(args, double));
                    break;
                case 'p':
                    rars_print_int((int)va_arg(args, void*));
                    break;
                default:
                    rars_print_char('%');
                    rars_print_char(*format);
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
