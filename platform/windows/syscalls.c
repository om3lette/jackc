#include "std/jackc_syscalls.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static inline wchar_t* utf8_to_utf16(const char* str) {
    static wchar_t wide_str_buf[MAX_PATH * 2];

    int wide_length = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (wide_length == 0)
        return nullptr;
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wide_str_buf, wide_length);
    return wide_str_buf;
}

FD jackc_open(const char* path, FLAGS flags) {
    wchar_t* wide_path = utf8_to_utf16(path);
    if (wide_path == 0)
        return (FD)-1;

    DWORD cretion_disposition = flags & O_RDONLY ? OPEN_EXISTING : CREATE_ALWAYS;
    return (intptr_t)CreateFileW(wide_path, flags, FILE_SHARE_READ, nullptr, cretion_disposition, 0, nullptr);
}

long jackc_read(FD fd, void* buf, size_t n) {
    DWORD bytes_read;
    return ReadFile((HANDLE)fd, buf, (DWORD)n, &bytes_read, nullptr) ? (long)bytes_read : -1;
}

long jackc_write(FD fd, const void* buf, size_t n) {
    DWORD bytes_written;
    return WriteFile((HANDLE)fd, buf, (DWORD)n, &bytes_written, nullptr) ? (long)bytes_written : -1;
}

long jackc_lseek(FD fd, long offset, FLAGS whence) {
    return (long)SetFilePointer((HANDLE)fd, offset, nullptr, whence);
}

int jackc_close(FD fd) {
    return CloseHandle((HANDLE)fd);
}
