#include "vm-translator/backend.h"
#include "jackc_stdio.h"
#include "vm-translator/code-generation/vm_code_generator.h"
#include "vm-translator/parser/vm_parser.h"
#include "jackc_stdlib.h"

jackc_backend_return_code jackc_backend_compile(
    const char* base_path,
    const char* output_dir,
    const jackc_config_t* config
) {
    char out_file_path[4096];
    jackc_sprintf(out_file_path, "%s/%s", output_dir, OUT_FILENAME);
    int fd = jackc_open(out_file_path, O_CREAT | O_WRONLY | O_TRUNC);
    if (fd < 0) {
        return BACKEND_FAILED_TO_OPEN_SAVE_FILE;
    }

    jackc_parser* parser = jackc_parser_init("");
    vm_code_generator* generator = jackc_vm_code_gen_init(out_file_path, fd, config);

    jackc_vm_code_bootstrap(generator);

    size_t vm_files_cnt = 0;
    const char* source_file_path = NULL;
    while ((source_file_path = jackc_next_source_file(base_path, ".vm")) != NULL) {
        ++vm_files_cnt;

        const char* file_content = jackc_read_file_content(source_file_path);
        if (!file_content) {
            jackc_free((void*)config);
            jackc_parser_free(parser); // TODO: Might not be neccesary as a separate method
            jackc_vm_code_gen_free(generator); // TODO: Might not be neccesary as a separate method
            return BACKEND_FAILED_TO_OPEN_SOURCE_FILE;
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


    jackc_free((void*)config);
    jackc_parser_free(parser);
    jackc_vm_code_gen_free(generator);
    if (vm_files_cnt == 0) {
        return BACKEND_NO_SOURCE_FILES;
    }
    return BACKEND_OK;
}
