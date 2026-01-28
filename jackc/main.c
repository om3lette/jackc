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
    int files_cnt = 1;
    const char* file_path[] = {
        "tests/vm-translator/vm-code/StaticTest.vm"
        // "tests/vm-translator/vm-code/FibonacciSys.vm",
        // "tests/vm-translator/vm-code/FibonacciSeries.vm"
    };
    // vm code generator initialization
    const char save_path[] = "gen/res.asm";
    int fd = jackc_open(save_path, O_CREAT | O_WRONLY | O_TRUNC);
    if (fd == -1) {
        LOG_ERROR("Failed to open file %s.\n", save_path);
        return 1;
    }
    jackc_parser* parser = jackc_parser_init("");
    vm_code_generator* generator = jackc_vm_code_gen_init(save_path, fd);

    jackc_vm_code_bootstrap(generator);
    for (int i = 0; i < files_cnt; i++) {
        const char* file_content = jackc_read_file_content(file_path[i]);
        if (!file_content) {
            jackc_parser_free(parser); // TODO: Might not be neccesary as a separate method
            jackc_vm_code_gen_free(generator); // TODO: Might not be neccesary as a separate method
            LOG_ERROR("Failed to read file content.\n");
            return 1;
        }
        jackc_parser_update_source(parser, file_content);

        while (jackc_parser_has_more_lines(parser)) {
            jackc_vm_parser_advance(parser);
            jackc_vm_code_gen_line(generator, parser);
        }

        jackc_free((void*)file_content);
    }
    jackc_vm_code_gen_finalize(generator);

    jackc_parser_free(parser);
    jackc_vm_code_gen_free(generator);
    jackc_exit(JACKC_EXIT_SUCCESS);
}
