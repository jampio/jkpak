from conans import ConanFile, CMake

class JKPAK(ConanFile):
	name = 'jkpak'
	requires = 'jsonformoderncpp/3.1.1@vthiery/stable' \
	         , 'libcurl/7.52.1@bincrafters/stable'
	generators = 'cmake'
	settings = 'os', 'compiler', 'build_type', 'arch'

	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()