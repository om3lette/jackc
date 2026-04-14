#include "core/localization/locale.h"

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
