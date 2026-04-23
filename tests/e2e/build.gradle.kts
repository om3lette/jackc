plugins {
    kotlin("jvm") version "2.3.0"
    application
}

repositories {
    mavenLocal()
    mavenCentral()
}

sourceSets.main {
    kotlin.srcDir("src")
    resources.srcDir("tests")
}

application {
    mainClass.set("MainKt")
}

dependencies {
    implementation(files("libs/rars.jar", "libs/rars-test-0.1.0.jar"))
    implementation("org.jetbrains.kotlin:kotlin-reflect:2.3.0")
    implementation("io.github.classgraph:classgraph:4.8.184")
}
