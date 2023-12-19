from conan import ConanFile
from conan.tools.cmake import cmake_layout

class Editor(ConanFile):
	name = "moth_ui renderer backends"
	version = "0.1"
	settings = "os", "compiler", "build_type", "arch"
	generators = "CMakeToolchain", "CMakeDeps"

	def requirements(self):
		self.requires("sdl/2.26.5")
		self.requires("sdl_image/2.0.5")
		self.requires("sdl_ttf/2.20.2")
		self.requires("nlohmann_json/3.11.2")
		self.requires("magic_enum/0.7.3")
		self.requires("range-v3/0.12.0")
		self.requires("fmt/[>10.0.0]")
		self.requires("vulkan-headers/1.3.243.0")
		self.requires("vulkan-loader/1.3.243.0")
		self.requires("spdlog/1.12.0")
		self.requires("glfw/3.3.8")
		self.requires("vulkan-memory-allocator/3.0.1")
		self.requires("freetype/2.12.1")
		self.requires("glm/cci.20230113")

	def layout(self):
		cmake_layout(self)