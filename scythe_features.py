"""
Module: scythe_features.py
Description: Scythe features.
Author: Shtille
Created: 2026-03-21
"""

class ScytheFeatures:
	# List of features
	FEATURE_MATH = "Math"
	FEATURE_OPENGL = "OpenGL"

	# Features paired with its default values
	features = {
		FEATURE_MATH: True,
		FEATURE_OPENGL: True,
	}
	# Dependencies to other features
	features_dependecies = {
		FEATURE_MATH: [],
		FEATURE_OPENGL: [],
	}
	# First element is dependency, second is usage of transitive headers (if library headers are passed outside)
	features_requirements = {
		FEATURE_MATH: [],
		FEATURE_OPENGL: [["glad/[>=2.0]", True]],
	}
	# Maps feature to list of defines
	features_defines = {
		FEATURE_MATH: ["SCYTHE_USE_MATH"],
		FEATURE_OPENGL: ["SCYTHE_USE_OPENGL"],
	}

	def __all_dicts_have_same_keys(self, dicts):
		if not dicts:
			return True
		first_keys = set(dicts[0].keys())
		return all(set(d.keys()) == first_keys for d in dicts[1:])

	def __validate_features_keys(self):
		return self.__all_dicts_have_same_keys([
			self.features,
			self.features_dependecies,
			self.features_requirements,
			self.features_defines,
			])

	def __has_circular_dependency(self, feature, visited=None):
		if visited is None:
			visited = set()
		if feature in visited:
			return True
		visited.add(feature)
		deps = self.features_dependecies.get(feature, [])
		for dep in deps:
			if (self.__has_circular_dependency(dep, visited)):
				return True
		return False

	def __validate_features_dependencies_graph(self):
		for feature in self.features:
			if self.__has_circular_dependency(feature):
				return False
		return True

	def __sync_features_with_options(self, options):
		for feature in self.features:
			self.features[feature] = options[feature]

	def __enable_option(self, feature, options):
		setattr(options, feature, True)
		self.features[feature] = True
		deps = self.features_dependecies.get(feature, [])
		for dep in deps:
			self.__enable_option(dep, options)

	def __enable_dependent_options(self, options):
		for feature, value in self.features.items():
			if value:
				self.__enable_option(feature, options)

	def validate(self):
		result = self.__validate_features_keys()
		if not result:
			raise Exception("Features keys do not match.")
		result = self.__validate_features_dependencies_graph()
		if not result:
			raise Exception("Circular dependency detected.")

	def configure(self, options):
		self.__sync_features_with_options(options)
		self.__enable_dependent_options(options)

	def add_features_to_options(self, options, default_options):
		for feature, value in self.features.items():
			options[feature] = [True, False]
			default_options[feature] = value