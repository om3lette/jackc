package math

import xyz.om3lette.rars.assertions.assertRegister
import xyz.om3lette.rars.enums.Reg
import xyz.om3lette.rars.testSuite.annotations.RarsTest
import xyz.om3lette.rars.testSuite.annotations.RarsTestSuite
import xyz.om3lette.rars.testSuite.testCase.extensions.withRegister
import xyz.om3lette.rars.testSuite.testContext.RarsTestContext

@RarsTestSuite(name = "Math.sqrt", defaultResource = "math/sqrt/out.asm")
class MathSqrtTests {
    @RarsTest("Full square")
    fun fullSquare(ctx: RarsTestContext) =
        ctx
            .given {
                withRegister(Reg.A0, 4)
            }
            .execute()
            .assert {
                assertRegister(Reg.A0, 2)
            }

    @RarsTest("sqrt(15)")
    fun sqrtFloor(ctx: RarsTestContext) =
        ctx
            .given {
                withRegister(Reg.A0, 15)
            }
            .execute()
            .assert {
                assertRegister(Reg.A0, 3)
            }

    @RarsTest("sqrt(1)")
    fun sqrtOne(ctx: RarsTestContext) =
        ctx
            .given {
                withRegister(Reg.A0, 1)
            }
            .execute()
            .assert {
                assertRegister(Reg.A0, 1)
            }

    @RarsTest("sqrt(0)")
    fun sqrtZero(ctx: RarsTestContext) =
        ctx
            .given {
                withRegister(Reg.A0, 0)
            }
            .execute()
            .assert {
                assertRegister(Reg.A0, 0)
            }

    @RarsTest("sqrt(2000000)")
    fun sqrtIntMax(ctx: RarsTestContext) =
        ctx
            .given {
                withRegister(Reg.A0, 2000000)
            }
            .execute()
            .assert {
                assertRegister(Reg.A0, 1414)
            }
}
