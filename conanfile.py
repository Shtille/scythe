from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.build import check_min_cppstd
from conan.errors import ConanException
from scythe_features import ScytheFeatures

# ConanFile requires options to be static, thus we store it outside of main class
scythe_options = {
	"shared": [True, False], 
	"fPIC": [True, False],
	}
scythe_default_options = {
	"shared": False, 
	"fPIC": True,
	}

scythe_features = ScytheFeatures()
scythe_features.configure()
scythe_features.add_features_to_options(scythe_options, scythe_default_options)


class ScytheRecipe(ScytheFeatures, ConanFile):
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
	options = scythe_options
	default_options = scythe_default_options

	exports = "scythe_features.py"

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

	def add_scythe_features_requirements(self):
		for feature, requirements_list in scythe_features.features_requirements.items():
			feature_value = scythe_features.features.get(feature, False)
			if feature_value:
				for requirement_pair in requirements_list:
					requirement = requirement_pair[0]
					use_transitive_headers = requirement_pair[1]
					self.requires(requirement, transitive_headers=use_transitive_headers)

	def add_scythe_defines_to_toolchain(self, toolchain):
		for feature, defines_list in scythe_features.features_defines.items():
			feature_value = scythe_features.features.get(feature, False)
			for define in defines_list:
				toolchain.variables[define] = "ON" if feature_value == True else "OFF"

	def add_scythe_defines_to_cpp_info(self):
		for feature, defines_list in scythe_features.features_defines.items():
			feature_value = scythe_features.features.get(feature, False)
			if feature_value:
				for define in defines_list:
					self.cpp_info.defines.append(define)

	def config_options(self):
		if self.settings.os == "Windows":
			del self.options.fPIC

	def validate(self):
		check_min_cppstd(self, "17")

	def requirements(self):
		self.test_requires("gtest/[>=1.15.0 <cci]")
		self.add_scythe_features_requirements()

	def build_requirements(self):
		self.tool_requires("cmake/[>3.15]")

	def layout(self):
		cmake_layout(self)

	def generate(self):
		deps = CMakeDeps(self)
		deps.generate()
		tc = CMakeToolchain(self)
		self.add_scythe_defines_to_toolchain(tc)
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
		self.add_scythe_defines_to_cpp_info()
		self.cpp_info.includedirs = ['include']
		self.cpp_info.libs = ["scythe"]