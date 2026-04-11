object TestsConstants {
    const val SCREEN_BASE_WORD = 0x10040000 / 4
    const val SCREEN_SIZE = 512 * 256
    const val INITIAL_N_HEAP_WORDS = 3 * 1024 * 1024 / 4

    const val HEAP_BASE_WORD = SCREEN_BASE_WORD + SCREEN_SIZE
    const val HEAP_BASE = HEAP_BASE_WORD * 4
    const val LAST_HEAP_WORD = HEAP_BASE_WORD + INITIAL_N_HEAP_WORDS
}
