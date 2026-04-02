package math

import xyz.om3lette.rars.assertions.assertRegister
import xyz.om3lette.rars.enums.Reg
import xyz.om3lette.rars.testSuite.annotations.RarsTest
import xyz.om3lette.rars.testSuite.annotations.RarsTestSuite
import xyz.om3lette.rars.testSuite.testCase.extensions.withRegister
import xyz.om3lette.rars.testSuite.testContext.RarsTestContext

@RarsTestSuite(name = "Math.abs", defaultResource = "math/abs/out.asm")
class MathAbsTests {
    @RarsTest("Positive value returns itself")
    fun symbolicRegisterAssertion(ctx: RarsTestContext) =
        ctx
            .given {
                withRegister(reg = Reg.A0, value = 5)
            }
            .execute()
            .assert {
                assertRegister(reg = Reg.A0, expected = 5)
            }
}