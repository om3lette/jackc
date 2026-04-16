#include "core/localization/locale.h"
#include "core/jackc_file_utils.h"
#include "std/jackc_stdlib.h"

static const jackc_locale* locales[JACKC_LANG_COUNT] = {
    [JACKC_LANG_EN] = &jackc_locale_en,
    [JACKC_LANG_RU] = &jackc_locale_ru,
};

const jackc_locale* jackc_locale_get(jackc_language_code code) {
    if (code >= JACKC_LANG_COUNT) {
        return &jackc_locale_en;
    }
    return locales[code];
}

void jackc_report_file_error(const jackc_locale* locale, jackc_file_return_code code, const char* path) {
    jackc_file_utils_translations t = locale->files;
    const char* text = nullptr;

    switch (code) {
        case FILE_OK:
        case FILE_EXHAUSTED_DIR: return;
        case FILE_FAILED_TO_OPEN: text = path ? t.failed_open_file : t.failed_open_base_dir; break;
        case FILE_FAILED_TO_READ: text = t.failed_read; break;
        case FILE_FAILED_TO_CLOSE: text = t.failed_close; break;
        case FILE_FAILED_LSEEK:
        case FILE_FAILED_REWIND: text = t.failed_rewind; break;
        case FILE_FAILED_STAT: text = t.failed_stat; break;
        case FILE_FAILED_TO_OPEN_DIR: text = t.failed_open_base_dir; break;
        case FILE_MAX_DIR_DEPTH_REACHED: text = t.max_dir_depth; break;
        case FILE_FAILED_TO_EXTRACT_NAME: text = t.failed_extract_filename; break;
    }

    if (!text) return;
    if (path) {
        jackc_printf(text, path);
    } else {
        jackc_printf(text);
    }
    jackc_putchar('\n');
}
