#ifndef JACKC_CORE_LOCALIZATION_LOCALE_H
#define JACKC_CORE_LOCALIZATION_LOCALE_H

#include "compiler/diagnostics-engine/diagnostic.h"
#include "core/jackc_file_utils.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef JACKC_DEFAULT_LOCALE
#   define JACKC_DEFAULT_LOCALE JACKC_LANG_EN
#endif

typedef enum {
    CLI_SOURCE_DIR,
    CLI_OUT_DIR,
    CLI_STD_DIR,
    CLI_CODE_COMMENTS,
    CLI_LANGUAGE,
    CLI_REVERSED_STACK,
    CLI_STACK_SIZE,
    CLI_OPTION_COUNT
} jackc_cli_option;

typedef struct {
    jackc_cli_option code;
    const char* translation;
} jackc_cli_option_translation;

typedef struct {
    const char* usage;
    const char* options;
    const char* unknown_arg;
    const char* required_arg;

    const jackc_cli_option_translation option_descriptions[CLI_OPTION_COUNT];
} jackc_cli_translations;

typedef struct {
    const char* frontend_failed;
    const char* backend_failed;
    const char* parser_failed_with_exit_code;
} jackc_msg_translations;

typedef struct {
    const char* failed_open_base_dir;
    const char* failed_stat;
    const char* failed_close;
    const char* failed_read;
    const char* failed_rewind;
    const char* max_dir_depth;
    const char* failed_extract_filename;
    const char* failed_open_file;
} jackc_file_utils_translations;

typedef struct {
    jackc_diagnostic_code code;
    const char* fmt;
    const char* desc;
} jackc_diagnostic_translation;

typedef struct {
    const jackc_diagnostic_translation* entries;
    uint32_t count;
} jackc_diagnostic_translations;

typedef struct {
    const jackc_cli_translations cli;
    const jackc_file_utils_translations files;
    const jackc_diagnostic_translations diagnostics;
    const jackc_msg_translations msgs;
} jackc_locale;

typedef enum {
    JACKC_LANG_EN,
    JACKC_LANG_RU,
    JACKC_LANG_COUNT
} jackc_language_code;

extern const jackc_locale jackc_locale_en;
extern const jackc_locale jackc_locale_ru;

const jackc_locale* jackc_locale_get(jackc_language_code code);

void jackc_report_file_error(const jackc_locale* locale, jackc_file_return_code code, const char* path);

#endif
