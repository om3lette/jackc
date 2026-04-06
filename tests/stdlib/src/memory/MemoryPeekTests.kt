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

@RarsTestSuite(name = "Memory.peek", defaultResource = "memory/peek/out.asm")
class MemoryPeekTests {
    @RarsTest("Peek returns the value at the address with no side-effect")
    fun test(ctx: RarsTestContext) {
        val wordIdx = 100
        val address = MemoryBaseAddresses.HEAP + wordIdx * 4
        ctx
            .given {
                withRegister(Reg.A0, wordIdx)
                withMemory(MemoryEntry.Word(address, 42))
            }
            .execute()
            .assert {
                assertMemoryWord(address, 42)
                assertRegister(Reg.A0, 42)
            }
    }
}
