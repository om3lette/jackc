#include "main.h"
#include "common/exit_code.h"
#include "common/logger.h"
#include "jackc_stdio.h"
#include "vm-translator/code-generation/vm_code_generator.h"
#include "vm-translator/parser/vm_parser.h"
#include "jackc_stdlib.h"

int main() {
    // TODO: Unhardcode file_path

    const char save_path[] = "gen/res.asm";
    int fd = jackc_open(save_path, O_CREAT | O_WRONLY | O_TRUNC);
    if (fd == -1) {
        LOG_ERROR("Failed to open file %s.\n", save_path);
        return 1;
    }

    // TODO: Unhardcode config variables
    const jackc_config_t* const jackc_config = jackc_config_create(true, 256 * 1024);

    jackc_parser* parser = jackc_parser_init("");
    vm_code_generator* generator = jackc_vm_code_gen_init(save_path, fd, jackc_config);

    jackc_vm_code_bootstrap(generator);

    size_t vm_files_cnt = 0;
    const char* source_file_path = NULL;
    while ((source_file_path = jackc_next_source_file("tests/vm-translator/vm-code/fibonacci-2", ".vm")) != NULL) {
        ++vm_files_cnt;

        const char* file_content = jackc_read_file_content(source_file_path);
        if (!file_content) {
            jackc_free((void*)jackc_config);
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
        jackc_free((void*)source_file_path);
    }
    jackc_vm_code_gen_finalize(generator);

    if (vm_files_cnt == 0) {
        LOG_WARNING("No VM source files found.\n");
    } else {
        LOG_INFO("Processed %u VM source files.\n", vm_files_cnt);
    }

    jackc_free((void*)jackc_config);
    jackc_parser_free(parser);
    jackc_vm_code_gen_free(generator);
    jackc_exit(JACKC_EXIT_SUCCESS);
}
