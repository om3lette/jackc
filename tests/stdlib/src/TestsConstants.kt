object TestsConstants {
    const val INITIAL_N_HEAP_WORDS: Int = (3.5 * 1024 * 1024 / 4).toInt() - 1
    const val HEAP_BASE_WORD: Int = 0x10040000 / 4
    const val LAST_HEAP_WORD = HEAP_BASE_WORD + INITIAL_N_HEAP_WORDS
}
