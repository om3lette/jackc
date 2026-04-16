import xyz.om3lette.rars.assertions.assertRegister
import xyz.om3lette.rars.enums.Reg
import xyz.om3lette.rars.testSuite.annotations.RarsTest
import xyz.om3lette.rars.testSuite.annotations.RarsTestSuite
import xyz.om3lette.rars.testSuite.testCase.extensions.withRegister
import xyz.om3lette.rars.testSuite.testContext.RarsTestContext

@RarsTestSuite(name = "Fibonacci", defaultResource = "fibonacci/out.asm")
class FibonacciTests {
    @RarsTest("fib(0), fib(1)")
    fun baseCases(ctx: RarsTestContext) {
        for (i in 0..1) {
            ctx
                .given {
                    withRegister(Reg.A0, i)
                }
                .execute()
                .assert {
                    assertRegister(Reg.A0, i)
                }
        }
    }

    @RarsTest("Recursive calls")
    fun recursion(ctx: RarsTestContext) {
        val tests: List<Pair<Int, Int>> = listOf(
            2 to 1, 3 to 2, 4 to 3, 5 to 5,
            6 to 8, 7 to 13, 8 to 21, 9 to 34
        )
        for (test in tests) {
            ctx
                .given {
                    withRegister(Reg.A0, test.first)
                }
                .execute()
                .assert {
                    assertRegister(Reg.A0, test.second)
                }
        }
    }
}
