#include "core/localization/locale.h"
#include "compiler/diagnostics-engine/diagnostic.h"

static const jackc_diagnostic_translation diagnostic_translations_ru[] = {
    { DIAG_UNEXPECTED_TOKEN, "Ожидалось '%s', получено '%.*s'", nullptr },
    { DIAG_MISSING_VARIABLE_KIND, "Отсутствует вид переменной", nullptr },
    { DIAG_MISSING_VARIABLE_NAME, "Отсутствует имя переменной", "Возможно, вы забыли указать тип?" },
    { DIAG_INVALID_VARIABLE_TYPE, "Неверный тип переменной.", "Ожидалось: 'int' | 'char' | 'boolean' | имя класса" },
    { DIAG_INVALID_RETURN_TYPE, "Неверный тип возврата.", "Ожидалось: 'void' | 'int' | 'char' | 'boolean' | имя класса" },
    { DIAG_INVALID_SUBROUTINE_KIND, "Неверный вид подпрограммы.", "Ожидалось: 'function' | 'method' | 'constructor'" },
    { DIAG_INVALID_TOKEN_SUBROUTINE_BODY, "Неверное тело подпрограммы: найдено '%.*s'.", "Ожидалось объявление переменной или оператор."},
    { DIAG_INVALID_TOKEN_CLASS_BODY, "Неверное объявление члена класса: найдено '%.*s'.", "Ожидалось объявление переменной класса или подпрограммы." },
    { DIAG_INVALID_TOKEN_TERM, "Неверный токен '%.*s' при разборе терма.", "Ожидалось начало терма: литерал, идентификатор, '(', или унарный оператор." },
    { DIAG_MISSING_SEMICOLON, "Отсутствует точка с запятой.", nullptr },
    { DIAG_MIXING_DECLARATIONS_AND_CODE, "Смешение объявлений и кода", nullptr },

    { DIAG_REDEFINITION, "Повторное определение '%.*s'.", nullptr },
    { DIAG_INCOMPLETE_TYPE, "Переменная '%.*s' имеет неполный тип.", nullptr },
    { DIAG_USE_OF_UNDECLARED_IDENTIFIER, "Необъявленный идентификатор '%.*s'.", nullptr },
    { DIAG_CALL_TO_UNDECLARED_SUBROUTINE, "Вызов необъявленной подпрограммы '%.*s'.", nullptr },
    { DIAG_NON_VOID_SUBROUTINE_SHOULD_RETURN_A_VALUE, "Не 'void' подпрограмма должна возвращать значение.", nullptr },
    { DIAG_CALLED_OBJECT_TYPE_IS_NOT_A_CLASS, "Вызываемый объект не имеет тип класса.", nullptr },
    { DIAG_TOO_FEW_ARGUMENTS_TO_FUNCTION_CALL, "Недостаточно аргументов при вызове подпрограммы, ожидалось %d, получено %d", nullptr },
    { DIAG_TOO_MANY_ARGUMENTS_TO_FUNCTION_CALL, "Слишком много аргументов при вызове подпрограммы, ожидалось %d, получено %d", nullptr },
    { DIAG_INCOMPATIBLE_TYPE_CONVERSION, "Невозможно преобразовать '%.*s' в '%.*s'", nullptr },

    { DIAG_EMPTY_IF_STATEMENT, "Пустое тело оператора if", nullptr },
    { DIAG_INVALID_OPERATION, "Неверная операция.", nullptr },
    { DIAG_CANNOT_CALL_METHOD_WITHOUT_AN_OBJECT, "Невозможно вызвать метод '%.*s' без объекта", nullptr },
    { DIAG_CLASS_NAME_DOES_NOT_MATCH_THE_FILENAME, "Имя класса '%.*s' не совпадает с именем файла '%.*s'.", nullptr },

    { DIAG_NOTE_PREVIOUS_DEFINITION_IS_HERE, "Предыдущее определение здесь", nullptr },
    { DIAG_NOTE_DECLARED_HERE, "Объявлено здесь", nullptr },

    { DIAG_WARNING_CONSTRUCTOR_WITH_NO_DISPOSE, "Нет метода 'dispose' для не статического класса", "Вызовет утечку памяти" },
};
static_assert(NUMBER_OF_DIAGNOSTICS == sizeof(diagnostic_translations_ru) / sizeof(diagnostic_translations_ru[0]), "Diagnostic translations array size mismatch");

const jackc_locale jackc_locale_ru = {
    .cli = {
        .usage = "Использование: jackc [параметры]",
        .options = "Параметры (* - обязательно):",
        .unknown_arg = "Неизвестный аргумент: %s",
        .required_arg = "Обязательный аргумент '%s' не был предоставлен",
        .option_descriptions = {
            {CLI_SOURCE_DIR, "Директория с исходным кодом"},
            {CLI_OUT_DIR, "Директория для сохранения выходных файлов"},
            {CLI_STD_DIR, "Путь до директории со стандартной библиотекой"},
            {CLI_CODE_COMMENTS,  "Включает генерацию комментариев в коде"},
            {CLI_LANGUAGE, "Язык сообщений (en/ru)"},
            {CLI_REVERSED_STACK, "Стэк будет расти в сторону высоких адресов"},
            {CLI_STACK_SIZE, "Фиксированный размер стека (работает только в связке с --reversed-stack)"}
        }
    },
    .msgs = {
        .frontend_failed = "Ошибка компилятора: код завершения %d",
        .backend_failed = "Ошибка генератора: код завершения %d",
        .parser_failed_with_exit_code = "Ошибка парсера: код завершения %d",
        .program_entrypoint_not_found = "Main.main не обнаружена в таблице символов"
    },
    .files = {
        .failed_open_base_dir = "Не удалось открыть базовую директорию %s",
        .failed_stat = "Не удалось получить статистику %s",
        .failed_close = "Не удалось закрыть файловый дескриптор",
        .failed_read = "Не удалось прочитать файл",
        .failed_rewind = "Не удалось перемотать файл",
        .max_dir_depth = "Достигнута максимальная глубина директорий",
        .failed_extract_filename = "Не удалось извлечь имя файла из %s",
        .failed_open_file = "Не удалось открыть файл %s"
    },
    .diagnostics = {
        .entries = diagnostic_translations_ru,
        .count = NUMBER_OF_DIAGNOSTICS,

        .error = "Ошибка",
        .warning = "Предупреждение",
        .note = "Замечание",
        .engine_overflow = "Диагностический модуль переполнен"
    }
};
