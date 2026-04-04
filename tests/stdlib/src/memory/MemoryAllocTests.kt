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

@RarsTestSuite(name = "Math.min")
class MemoryAllocTests {
    @RarsTest("One allocation", resource = "memory/alloc/one/out.asm")
    fun positiveValue(ctx: RarsTestContext) {
        val wordsToAlloc = 4
        ctx
            .given {
                withRegister(Reg.A0, wordsToAlloc)
            }
            .execute()
            .assert {
                // Expect initial words - allocated - 1 for the header of the new allocation
                assertMemoryWord(
                    MemoryBaseAddresses.HEAP,
                    TestsConstants.INITIAL_N_HEAP_WORDS - wordsToAlloc - 1
                )
                // Expect the words to be allocated from the back of the free list
                assertRegister(Reg.A0, TestsConstants.INITIAL_N_HEAP_WORDS - wordsToAlloc)
            }
    }

    // FIXME:
    // @RarsTest("Many allocations", resource = "memory/alloc/many/out.asm")
    // fun manyAllocations(ctx: RarsTestContext) {
    //     // Every word is its own allocation. See the test file
    //     val wordsToAlloc = 10
    //     ctx
    //         .given {
    //             withRegister(Reg.A0, wordsToAlloc)
    //         }
    //         .execute()
    //         .assert {
    //             // * 2 to account for headers
    //             assertMemoryWord(
    //                 MemoryBaseAddresses.HEAP,
    //                 TestsConstants.INITIAL_N_HEAP_WORDS - wordsToAlloc * 2
    //             )
    //             // Last allocated word
    //             assertRegister(Reg.A0, TestsConstants.INITIAL_N_HEAP_WORDS - wordsToAlloc * 2 + 1)
    //         }
    // }
}
