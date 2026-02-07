from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import check_min_cppstd

class ScytheRecipe(ConanFile):
	name = "scythe"
	version = "1.0"

	# Optional metadata
	license = "MIT"
	author = "Shtille v.shtille@gmail.com"
	url = "https://github.com/Shtille/scythe"
	description = "A graphics framework"
	topics = ("framework", "graphics", "OpenGL")

	# Binary configuration
	settings = "os", "compiler", "build_type", "arch"
	options = {
		"shared": [True, False], 
		"fPIC": [True, False],
		"OpenGL": [True, False],
		}
	default_options = {
		"shared": False, 
		"fPIC": True,
		"OpenGL": True,
		}

	# Sources are located in the same place as this recipe, copy them to the recipe
	exports_sources = [
		"CMakeLists.txt",
		"conan_setup.cmake",
		"scythe.cmake",
		"src/*",
		"include/*",
		"examples/*",
		"tests/*"
	]

	scythe_defines = {
		"OpenGL": ["SCYTHE_USE_OPENGL"],
	}
	scythe_requires = {
		"OpenGL": ["glad/[>=2.0]"],
	}

	def config_options(self):
		if self.settings.os == "Windows":
			del self.options.fPIC

	def validate(self):
		check_min_cppstd(self, "17")

	def requirements(self):
		self.test_requires("gtest/[>=1.15.0]")
		if self.options.OpenGL == "True":
			for require in self.scythe_requires["OpenGL"]:
				self.requires(require, transitive_headers=True)

	def build_requirements(self):
		self.tool_requires("cmake/[>3.15]")

	def layout(self):
		cmake_layout(self)

	def generate(self):
		deps = CMakeDeps(self)
		deps.generate()
		tc = CMakeToolchain(self)
		if self.options.OpenGL == "True":
			for define in self.scythe_defines["OpenGL"]:
				tc.variables[define] = True
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
		if self.options.OpenGL == "True":
			for define in self.scythe_defines["OpenGL"]:
				self.cpp_info.defines.append(define)
		self.cpp_info.libs = ["scythe"]