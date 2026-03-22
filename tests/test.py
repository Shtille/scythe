class Features:
	features = {
		'feature1': False,
		'feature2': True,
		'feature3': False,
	}
	features_dependecies = {
		'feature1': [],
		'feature2': ['feature1','feature3'],
		'feature3': [],
	}

	def enable_one(self, feature):
		self.features[feature] = True
		deps = self.features_dependecies.get(feature, [])
		for dep in deps:
			self.enable_one(dep)

	def enable_all(self):
		for feature, value in self.features.items():
			if value:
				self.enable_one(feature)

	def print_all(self):
		for feature, value in self.features.items():
			print(feature, value)

f = Features()
print("--- Before ---")
f.print_all()
print("--- After ---")
f.enable_all()
f.print_all()
