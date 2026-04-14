#ifndef JACKC_CORE_LOCALIZATION_LOCALE_H
#define JACKC_CORE_LOCALIZATION_LOCALE_H

#include <stdint.h>
#include <stdbool.h>

#ifndef JACKC_DEFAULT_LOCALE
#   define JACKC_DEFAULT_LOCALE JACKC_LANG_EN
#endif

typedef struct {
    const char* usage;
    const char* options;
    const char* unknown_arg;
    const char* required_arg;
} jackc_cli_translations;

typedef struct {
    const char* code;
    const char* fmt;
    const char* desc;
} jackc_diagnostic_translation;

typedef struct {
    const jackc_diagnostic_translation* entries;
    uint32_t count;
} jackc_diagnostic_translations;

typedef struct {
    const jackc_cli_translations cli;
    const jackc_diagnostic_translations diagnostics;
} jackc_locale;

typedef enum {
    JACKC_LANG_EN,
    JACKC_LANG_RU,
    JACKC_LANG_COUNT
} jackc_language_code;

extern const jackc_locale jackc_locale_en;
extern const jackc_locale jackc_locale_ru;

const jackc_locale* jackc_locale_get(jackc_language_code code);

#endif
