#include "main.h"
#include "common/exit_code.h"
#include "common/logger.h"
#include "vm-translator/parser.h"
#include "jackc_stdlib.h"

int main() {
    // TODO: Unhardcode file_path
    const char* file_path = "/tmp/StaticTest2.vm";
    const char* file_content = jackc_read_file_content(file_path);
    if (!file_content) {
        LOG_ERROR("Failed to read file content.");
        LOG_ERROR(file_path);
        return 1;
    }
    jackc_parser* parser = jackc_parser_init(file_content);
    (void) parser;
    LOG_DEBUG("Buffer content:");
    LOG_DEBUG(parser->buffer);
    jackc_parser_free(parser);
    jackc_exit(JACKC_EXIT_SUCCESS);
}
