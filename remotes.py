REMOTES = ( ('bincrafters', 'https://api.bintray.com/conan/bincrafters/public-conan')
          , ('conan-community', 'https://api.bintray.com/conan/conan-community/conan')
          )

if __name__ == '__main__':
	from conans.client.conan_api import Conan
	conan, _, _ = Conan.factory()
	conan_remotes = conan.remote_list()
	def remote_exists(remotes, url):
		try:
			_ = next(r for r in remotes if r.url == url)
			return True
		except StopIteration:
			return False
	for name, url in REMOTES:
		print 'Checking {} {}'.format(name, url)
		if remote_exists(conan_remotes, url):
			print 'Remote exists'
		else:
			print 'Adding remote {} {}'.format(name, url)
			conan.remote_add(name, url)