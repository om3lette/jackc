package memory

import xyz.om3lette.rars.MemoryBaseAddresses
import xyz.om3lette.rars.MemoryEntry
import xyz.om3lette.rars.assertions.assertMemoryWord
import xyz.om3lette.rars.assertions.assertRegister
import xyz.om3lette.rars.enums.Reg
import xyz.om3lette.rars.testSuite.annotations.RarsTest
import xyz.om3lette.rars.testSuite.annotations.RarsTestSuite
import xyz.om3lette.rars.testSuite.testCase.extensions.withMemory
import xyz.om3lette.rars.testSuite.testCase.extensions.withRegister
import xyz.om3lette.rars.testSuite.testContext.RarsTestContext

@RarsTestSuite(name = "Memory.poke", defaultResource = "memory/poke/out.asm")
class MemoryPokeTests {
    @RarsTest("Poke assigns the value at the given address")
    fun test(ctx: RarsTestContext) {
        val wordIdx = 100
        val address = MemoryBaseAddresses.HEAP + wordIdx * 4
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
