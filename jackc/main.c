#include "main.h"
#include "common/exit_code.h"
#include "common/logger.h"
#include "vm-translator/parser.h"
#include "jackc_stdlib.h"

int main() {
    // TODO: Unhardcode file_path

    const char* file_path = "/tmp/FibonacciSeries.vm";
    const char* file_content = jackc_read_file_content(file_path);
    if (!file_content) {
        LOG_ERROR("Failed to read file content.\n");
        return 1;
    }
    jackc_parser* parser = jackc_parser_init(file_content);
    LOG_DEBUG("Buffer content:\n%s\n", parser->buffer);

    while (jackc_parser_has_more_lines(parser)) {
        jackc_vm_parser_advance(parser);
    }

    jackc_parser_free(parser);
    jackc_exit(JACKC_EXIT_SUCCESS);
}
