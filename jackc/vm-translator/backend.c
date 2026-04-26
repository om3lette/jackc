#include "vm-translator/backend.h"
#include "core/allocators/allocators.h"
#include "core/localization/locale.h"
#include "std/jackc_limits.h"
#include "std/jackc_stdio.h"
#include "std/jackc_string.h"
#include "std/jackc_syscalls.h"
#include "vm-translator/code-gen/asm_code_gen.h"
#include "vm-translator/parser/vm_parser.h"
#include "core/jackc_file_utils.h"
#include "vm-translator/parser/vm_parser_utils.h"

jackc_backend_return_code jackc_backend_compile(
    const char* base_path,
    const char* output_dir,
    const char* std_path,
    const jackc_config* config,
    const jackc_locale* locale,
    Allocator* allocator
) {
    if (!std_path)
        std_path = base_path;

    size_t std_dir_length = jackc_strlen(std_path);
    size_t output_dir_length = jackc_strlen(output_dir);

    char file_path[PATH_MAX];
    if (output_dir_length + jackc_strlen(OUT_FILENAME) >= PATH_MAX)
        return BACKEND_SAVE_PATH_TOO_LONG;

    jackc_sprintf(file_path, "%s/%s", output_dir, OUT_FILENAME);
    FD fd = jackc_open(file_path, O_WRONLY);
    if (fd < 0)
        return BACKEND_FAILED_TO_OPEN_SAVE_FILE;

    asm_context* ctx = asm_context_init(fd, config, allocator);


    if (std_dir_length + jackc_strlen(STD_ASM_FILENAME) >= PATH_MAX)
        return BACKEND_BASE_PATH_TOO_LONG;
    jackc_sprintf(file_path, "%s%c%s", std_path, DELIMITER, STD_ASM_FILENAME);

    jackc_file_return_code source_file_ret_code, file_read_ret_code;
    char* std_native_content = nullptr;
    file_read_ret_code = jackc_read_file_content(file_path, &std_native_content, allocator);
    if (file_read_ret_code != FILE_OK) {
        jackc_report_file_error(locale, file_read_ret_code, file_path);
        return BACKEND_FAILED_TO_OPEN_STD_NATIVE;
    }
    asm_code_gen_bootstrap(ctx, std_native_content);

    size_t vm_files_cnt = 0;
    const char* source_file_path = nullptr;
    jackc_file_return_code iter_ret_code = FILE_OK;
    jackc_dir_iterator iter;
    if ((iter_ret_code = jackc_dir_iterator_create(base_path, allocator, &iter)) != FILE_OK) {
        jackc_report_file_error(locale, iter_ret_code, base_path);
        return BACKEND_FAILED_TO_OPEN_SOURCE_FILE;
    }
    while (
        (source_file_ret_code = jackc_dir_iterator_next_file_with_ext(&iter, ".vm", &source_file_path)) == FILE_OK
    ) {
        ++vm_files_cnt;

        char* file_content = nullptr;
        file_read_ret_code = jackc_read_file_content(source_file_path, &file_content, allocator);
        if (file_read_ret_code != FILE_OK) {
            jackc_report_file_error(locale, file_read_ret_code, source_file_path);
            return BACKEND_FAILED_TO_OPEN_SOURCE_FILE;
        }
        vm_parser parser = vm_parser_init(&jackc_string_from_str(file_content));

        while (vm_parser_has_more_lines(&parser)) {
            asm_code_gen_process_line(ctx, &parser);
            vm_parser_advance(&parser);
            if (!is_valid_state(&parser)) {
                jackc_printf(locale->msgs.parser_failed_with_exit_code, parser.status);
                jackc_putchar('\n');
                break;
            }
        }
        asm_code_gen_process_line(ctx, &parser);
    }
    asm_code_gen_finalize(ctx);

    if (vm_files_cnt == 0)
        return BACKEND_NO_SOURCE_FILES;
    return BACKEND_OK;
}
