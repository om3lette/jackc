import xyz.om3lette.rars.RarsTestEngine

import java.io.File

fun main() {
    val root = File(".")
    val libsPath = root.resolve("libs")
    val stdPath = libsPath.resolve("../../../stdlib")
    val compilerPath = libsPath.resolve("jackc")
    compileAllJackPrograms(root, compilerPath.absolutePath, stdPath.absolutePath)
    RarsTestEngine().apply {
        discoverTestSuites()
        run()
    }
}

fun compileAllJackPrograms(root: File, compilerPath: String, stdPath: String) {
    val jackDirs = mutableSetOf<File>()
    root.walkTopDown().forEach { file ->
        if (file.isFile) {
            when {
                file.extension in setOf("vm", "asm") -> file.delete()
                file.name == "Main.jack" -> jackDirs.add(file.parentFile)
            }
        }
    }

    for (dir in jackDirs) {
        println("Compiling ${dir.path}...")

        val process = ProcessBuilder()
            .command(compilerPath, "-s", dir.path, "-o", dir.path, "-std", stdPath)
            .inheritIO()
            .start()

        val exitCode = process.waitFor()

        if (exitCode != 0) {
            error("Compilation failed in ${dir.path} (exit code $exitCode)")
        }
    }
}
