#include "vm-translator/backend.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "std/jackc_stdio.h"
#include "std/jackc_string.h"
#include "std/jackc_syscalls.h"
#include "vm-translator/code-gen/asm_code_gen.h"
#include "vm-translator/parser/vm_parser.h"
#include "std/jackc_stdlib.h"
#include "core/jackc_file_utils.h"
#include "vm-translator/parser/vm_parser_utils.h"

jackc_backend_return_code jackc_backend_compile(
    const char* base_path,
    const char* output_dir,
    const jackc_config* config
) {
    char out_file_path[4096];
    jackc_sprintf(out_file_path, "%s/%s", output_dir, OUT_FILENAME);
    int fd = jackc_open(out_file_path, O_CREAT | O_WRONLY | O_TRUNC);
    if (fd < 0) {
        return BACKEND_FAILED_TO_OPEN_SAVE_FILE;
    }

    Allocator allocator = arena_allocator_adapter();
    asm_context* ctx = asm_context_init(fd, config, &allocator);

    asm_code_gen_bootstrap(ctx);

    size_t vm_files_cnt = 0;
    const char* source_file_path = NULL;
    while ((source_file_path = jackc_next_source_file(base_path, ".vm")) != NULL) {
        ++vm_files_cnt;

        const char* file_content = jackc_read_file_content(source_file_path);
        if (!file_content)
            return BACKEND_FAILED_TO_OPEN_SOURCE_FILE;
        vm_parser parser = jackc_parser_init(&jackc_string_from_str(file_content));

        while (vm_parser_has_more_lines(&parser)) {
            asm_code_gen_process_line(ctx, &parser);
            vm_parser_advance(&parser);
            if (!is_valid_state(&parser)) {
                jackc_printf("Parser failed with exit code %d: %s\n", parser.status, source_file_path);
                break;
            }
        }
        asm_code_gen_process_line(ctx, &parser);

        jackc_free((void*)file_content);
        jackc_free((void*)source_file_path);
    }
    asm_code_gen_finalize(ctx);

    arena_allocator_destroy(allocator.context);
    if (vm_files_cnt == 0)
        return BACKEND_NO_SOURCE_FILES;
    return BACKEND_OK;
}
