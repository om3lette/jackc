include_directories(include)

add_library(tests_utils STATIC
    compiler/lexer/lexer_test_utils.c
    compiler/parser/test_parser_utils.c
    utils/test_path_utils.c
)

add_executable(compiler_lexer_tests
    main.c
    compiler/lexer/eof.c
    compiler/lexer/identifiers.c
    compiler/lexer/internal.c
    compiler/lexer/numbers.c
    compiler/lexer/comments.c
    compiler/lexer/operations.c
    compiler/lexer/tabs_and_spaces.c
    compiler/lexer/unknown_characters.c
    compiler/lexer/string_literals.c
    compiler/lexer/lexer_test_utils.c
)
target_link_libraries(compiler_lexer_tests
    PRIVATE
        Tau
        compiler
)

add_executable(compiler_symtab_tests
    main.c
    compiler/symtab/push_pop.c
    compiler/symtab/find.c
    compiler/symtab/insert.c
)
target_link_libraries(compiler_symtab_tests
    PRIVATE
        Tau
        compiler
)

add_executable(function_registry_tests
    main.c
    compiler/function-registry/insert.c
)
target_link_libraries(function_registry_tests
    PRIVATE
        Tau
        compiler
)

add_executable(data_structure_tests
    main.c
    data-structures/fixed_hashmap.c
)
target_link_libraries(data_structure_tests
    PRIVATE
        Tau
        core
)

add_executable(allocators_tests
    main.c
    allocators/arena_allocator.c
)
target_link_libraries(allocators_tests
    PRIVATE
        Tau
        core
)

add_executable(compiler_parser_tests
    main.c
    compiler/parser/var_dec.c
    compiler/parser/class_var_dec.c
    compiler/parser/param_list.c
    compiler/parser/expression.c
    compiler/parser/statements.c
    compiler/parser/subroutine.c
    compiler/parser/class.c
    compiler/parser/error_recovery.c
)
target_link_libraries(compiler_parser_tests
    PRIVATE
        Tau
        tests_utils
        compiler
)

add_executable(diagnostics_engine_tests
    main.c
    compiler/diagnostics-engine/parsing-errors/error_reporting.c
    compiler/diagnostics-engine/frontend-errors/error_reporting.c
)
target_link_libraries(diagnostics_engine_tests
    PRIVATE
        Tau
        tests_utils
        compiler
)

add_executable(compiler_integration_tests
    main.c
    compiler/integration/runner.c
)
target_link_libraries(compiler_integration_tests
    PRIVATE
        Tau
        tests_utils
        compiler
)

add_executable(vm_parser_tests
    main.c
    vm-translator/parser/instructions.c
    vm-translator/parser/multi_line.c
    vm-translator/parser/errors.c
)
target_link_libraries(vm_parser_tests
    PRIVATE
        Tau
        vm-translator
)

add_test(NAME vm_parser_tests COMMAND vm_parser_tests)
set_tests_properties(vm_parser_tests PROPERTIES LABELS "unit")

add_test(NAME compiler_lexer_tests COMMAND compiler_lexer_tests)
set_tests_properties(compiler_lexer_tests PROPERTIES LABELS "unit")

add_test(NAME compiler_symtab_tests COMMAND compiler_symtab_tests)
set_tests_properties(compiler_symtab_tests PROPERTIES LABELS "unit")

add_test(NAME data_structure_tests COMMAND data_structure_tests)
set_tests_properties(data_structure_tests PROPERTIES LABELS "unit")

add_test(NAME allocators_tests COMMAND allocators_tests)
set_tests_properties(allocators_tests PROPERTIES LABELS "unit")

add_test(NAME compiler_parser_tests COMMAND compiler_parser_tests)
set_tests_properties(compiler_parser_tests PROPERTIES LABELS "unit")

add_test(NAME diagnostics_engine_tests COMMAND diagnostics_engine_tests)
set_tests_properties(diagnostics_engine_tests PROPERTIES LABELS "unit")

add_test(NAME function_registry_tests COMMAND function_registry_tests)
set_tests_properties(function_registry_tests PROPERTIES LABELS "unit")

add_test(NAME compiler_integration_tests COMMAND compiler_integration_tests)
set_tests_properties(compiler_integration_tests PROPERTIES LABELS "unit")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(GRADLE_EXECUTABLE "gradlew.bat")
else()
    set(GRADLE_EXECUTABLE "gradlew")
endif()
add_test(NAME e2e_tests COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/e2e/${GRADLE_EXECUTABLE} -p ${CMAKE_CURRENT_SOURCE_DIR}/e2e run)
set_tests_properties(e2e_tests PROPERTIES LABELS "e2e")
