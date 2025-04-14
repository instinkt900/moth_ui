from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain

class MothUI(ConanFile):
    name = "moth_ui"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    exports_sources = "CMakeLists.txt", "include/*", "src/*"
    package_type = "static-library"

    def requirements(self):
        self.requires("nlohmann_json/3.11.2", transitive_headers=True)
        self.requires("magic_enum/0.7.3")
        self.requires("range-v3/0.12.0")
        self.requires("fmt/10.0.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.9")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["moth_ui"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.includedirs = ["include"]
