#include "main.h"
#include "common/exit_code.h"
#include "common/logger.h"
#include "jackc_stdio.h"
#include "vm-translator/code-generation/code_gen.h"
#include "vm-translator/parser.h"
#include "jackc_stdlib.h"

int main() {
    // TODO: Unhardcode file_path

    // vm parser initialization
    const char file_path[] = "/tmp/FibonacciSeries.vm";
    const char* file_content = jackc_read_file_content(file_path);
    if (!file_content) {
        LOG_ERROR("Failed to read file content.\n");
        return 1;
    }
    jackc_parser* parser = jackc_parser_init(file_content);
    LOG_DEBUG("Buffer content:\n%s\n", parser->buffer);

    // vm code generator initialization
    const char save_path[] = "/home/nikita/code_gen.asm";
    int fd = jackc_open(save_path, O_CREAT | O_WRONLY | O_TRUNC);
    if (fd == -1) {
        LOG_ERROR("Failed to open file %s.\n", save_path);
        return 1;
    }
    vm_code_generator* generator = jackc_vm_code_gen_init(save_path, fd);

    while (jackc_parser_has_more_lines(parser)) {
        jackc_vm_parser_advance(parser);
        jackc_vm_code_gen_line(generator, parser);
    }
    jackc_vm_code_gen_finalize(generator);

    jackc_parser_free(parser);
    jackc_vm_code_gen_free(generator);
    jackc_exit(JACKC_EXIT_SUCCESS);
}
