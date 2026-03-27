#ifndef JACKC_COMPILER_DIAGNOSTICS_ENGINE_TRANSLATION_H
#define JACKC_COMPILER_DIAGNOSTICS_ENGINE_TRANSLATION_H

#include "compiler/diagnostics-engine/diagnostic.h"

typedef struct {
    jackc_diagnostic_code code;
    const char* fmt;
    const char* desc;
} jackc_diagnostic_translation;

#endif
