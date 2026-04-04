package math

import xyz.om3lette.rars.MemoryBaseAddresses
import xyz.om3lette.rars.MemoryEntry
import xyz.om3lette.rars.assertions.assertRegister
import xyz.om3lette.rars.enums.Reg
import xyz.om3lette.rars.testSuite.annotations.RarsTest
import xyz.om3lette.rars.testSuite.annotations.RarsTestSuite
import xyz.om3lette.rars.testSuite.testCase.extensions.withMemory
import xyz.om3lette.rars.testSuite.testCase.extensions.withRegister
import xyz.om3lette.rars.testSuite.testContext.RarsTestContext

@RarsTestSuite(name = "Math.max", defaultResource = "math/max/out.asm")
class MathMaxTests {
    private val baseAddress: Int = MemoryBaseAddresses.HEAP + 32

    @RarsTest("Positive values")
    fun positiveValue(ctx: RarsTestContext) =
        ctx
            .given {
                withRegister(Reg.A0, baseAddress)
                withMemory(MemoryEntry.Word(baseAddress, 3))
                withMemory(MemoryEntry.Word(baseAddress + 4, 10))
            }
            .execute()
            .assert {
                assertRegister(reg = Reg.A0, expected = 10)
            }

    @RarsTest("Negative values")
    fun negativeValue(ctx: RarsTestContext) =
        ctx
            .given {
                withRegister(Reg.A0, baseAddress)
                withMemory(MemoryEntry.Word(baseAddress, -10))
                withMemory(MemoryEntry.Word(baseAddress + 4, -42))
            }
            .execute()
            .assert {
                assertRegister(reg = Reg.A0, expected = -10)
            }
}
