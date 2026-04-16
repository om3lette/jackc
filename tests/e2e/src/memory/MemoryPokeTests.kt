package memory

import TestsConstants
import xyz.om3lette.rars.assertions.assertMemoryWord
import xyz.om3lette.rars.enums.Reg
import xyz.om3lette.rars.testSuite.annotations.RarsTest
import xyz.om3lette.rars.testSuite.annotations.RarsTestSuite
import xyz.om3lette.rars.testSuite.testCase.extensions.withRegister
import xyz.om3lette.rars.testSuite.testContext.RarsTestContext

@RarsTestSuite(name = "Memory.poke", defaultResource = "std/memory/poke/out.asm")
class MemoryPokeTests {
    @RarsTest("Poke assigns the value at the given address")
    fun test(ctx: RarsTestContext) {
        val wordIdx = TestsConstants.HEAP_BASE_WORD + 100
        val address = wordIdx * 4
        ctx
            .given {
                withRegister(Reg.A0, wordIdx)
            }
            .execute()
            .assert {
                assertMemoryWord(address, 1)
            }
    }
}
