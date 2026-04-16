import xyz.om3lette.rars.assertions.assertMemoryWord
import xyz.om3lette.rars.assertions.assertRegister
import xyz.om3lette.rars.enums.Reg
import xyz.om3lette.rars.testSuite.annotations.RarsTest
import xyz.om3lette.rars.testSuite.annotations.RarsTestSuite
import xyz.om3lette.rars.testSuite.testCase.extensions.withRegister
import xyz.om3lette.rars.testSuite.testContext.RarsTestContext

@RarsTestSuite(name = "Array")
class ArrayTests {
    @RarsTest("Array is correctly allocated and disposed", resource = "std/array/creation/out.asm")
    fun oneDeallocation(ctx: RarsTestContext) {
        val arraySize = 11
        // Every word is its own allocation. See the test file
        ctx
            .given {
                withRegister(Reg.A0, arraySize)
            }
            .execute()
            .assert {
                assertMemoryWord(
                    TestsConstants.HEAP_BASE,
                    TestsConstants.INITIAL_N_HEAP_WORDS - arraySize - 1
                )
            }
    }

    @RarsTest("Nested array accesses", resource = "std/array/assignment/out.asm")
    fun nestedAccess(ctx: RarsTestContext) =
        // Every word is its own allocation. See the test file
        ctx
            .given()
            .execute()
            .assert {
                assertRegister(Reg.A0, 40)
            }
}
