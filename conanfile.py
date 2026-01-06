from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import check_min_cppstd

class ScytheRecipe(ConanFile):
	name = "scythe"
	version = "2.0"

	# Optional metadata
	license = "MIT"
	author = "Shtille v.shtille@gmail.com"
	url = "https://github.com/Shtille/scythe"
	description = "A graphics framework"
	topics = ("framework", "graphics", "OpenGL")

	# Binary configuration
	settings = "os", "compiler", "build_type", "arch"
	options = {"shared": [True, False], "fPIC": [True, False]}
	default_options = {"shared": False, "fPIC": True}

	# Sources are located in the same place as this recipe, copy them to the recipe
	exports_sources = "CMakeLists.txt", "src/*", "include/*", "tests/*"

	def config_options(self):
		if self.settings.os == "Windows":
			del self.options.fPIC

	def validate(self):
		check_min_cppstd(self, "17")

	def requirements(self):
		self.test_requires("gtest/[>=1.15.0]")

	def build_requirements(self):
		self.tool_requires("cmake/[>3.15]")

	def layout(self):
		cmake_layout(self)

	def generate(self):
		deps = CMakeDeps(self)
		deps.generate()
		tc = CMakeToolchain(self)
		tc.generate()

	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()
		cmake.test()

	def package(self):
		cmake = CMake(self)
		cmake.install()

	def package_info(self):
		self.cpp_info.libs = ["scythe"]