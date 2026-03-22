#include "engine.h"

void jack_diagnostic_push(jack_diagnostic_engine* engine, jack_diagnostic diagnostic) {
    if (engine->size < MAX_DIAGNOSTICS) {
        engine->diagnostics[engine->size++] = diagnostic;
    } else {
        engine->overflow = true;
    }
}
