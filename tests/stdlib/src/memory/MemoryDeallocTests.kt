package memory

import TestsConstants
import xyz.om3lette.rars.assertions.assertMemoryWord
import xyz.om3lette.rars.assertions.assertRegister
import xyz.om3lette.rars.enums.Reg
import xyz.om3lette.rars.testSuite.annotations.RarsTest
import xyz.om3lette.rars.testSuite.annotations.RarsTestSuite
import xyz.om3lette.rars.testSuite.testCase.extensions.withRegister
import xyz.om3lette.rars.testSuite.testContext.RarsTestContext

@RarsTestSuite(name = "Memory.dealloc")
class MemoryDeallocTests {
    @RarsTest("Many allocations/deallocations", resource = "memory/dealloc/one/out.asm")
    fun oneDeallocation(ctx: RarsTestContext) {
        // Every word is its own allocation. See the test file
        ctx
            .given()
            .execute()
            .assert {
                assertMemoryWord(
                    TestsConstants.HEAP_BASE,
                    TestsConstants.INITIAL_N_HEAP_WORDS - 11
                )
            }
    }

    @RarsTest("Deallocated memory is reused", resource = "memory/dealloc/many/out.asm")
    fun manyDeallocations(ctx: RarsTestContext) {
        // Every word is its own allocation. See the test file
        val wordsToAllocate = 42
        ctx
            .given {
                withRegister(Reg.A0, wordsToAllocate)
            }
            .execute()
            .assert {
                assertMemoryWord(
                    TestsConstants.HEAP_BASE,
                    TestsConstants.INITIAL_N_HEAP_WORDS - wordsToAllocate - 1
                )
                assertRegister(Reg.A0, TestsConstants.LAST_HEAP_WORD - wordsToAllocate)
            }
    }
}
