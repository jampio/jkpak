from conans import ConanFile, CMake

class JKPAK(ConanFile):
	name = 'jkpak'
	requires = 'jsonformoderncpp/3.1.1@vthiery/stable' \
	         , 'libcurl/7.52.1@bincrafters/stable' \
	         , 'Poco/1.9.0@boujee/stable'
	default_options = 'Poco:enable_xml=False' \
	                , 'Poco:enable_json=False' \
	                , 'Poco:enable_mongodb=False' \
	                , 'Poco:enable_redis=False' \
	                , 'Poco:enable_encodings=False' \
	                , 'Poco:enable_util=False' \
	                , 'Poco:enable_net=False' \
	                , 'Poco:enable_netssl=False' \
	                , 'Poco:enable_netssl_win=False' \
	                , 'Poco:enable_crypto=False' \
	                , 'Poco:enable_data=False' \
	                , 'Poco:enable_data_sqlite=False' \
	                , 'Poco:force_openssl=False'
	generators = 'cmake'
	settings = 'os', 'compiler', 'build_type', 'arch'

	def build(self):
		cmake = CMake(self)
		cmake.configure()
		cmake.build()
		cmake.install()