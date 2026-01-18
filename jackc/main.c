#include "main.h"
#include "common/logger.h"
#include "vm-translator/parser.h"
#include "jack_stdlib.h"
#include <stdio.h>

int main() {
    // TODO: Unhardcode file_path
    const char* file_path = "/home/nikita/Desktop/hse/course-project/jackc/tests/vm-translator/vm-code/BasicTest.vm";
    const char* file_content = jackc_read_file_content(file_path);
    if (!file_content) {
        LOG_ERROR("Failed to read file content.");
        LOG_ERROR(file_path);
        return 1;
    }
    jackc_parser* parser = jackc_parser_init(file_content);
    (void) parser;
    LOG_INFO("SUCCESS!");

    printf("%s", parser->buffer);
    return 0;
}
