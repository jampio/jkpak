REMOTES = ( ('bincrafters', 'https://api.bintray.com/conan/bincrafters/public-conan')
          , ('conan-community', 'https://api.bintray.com/conan/conan-community/conan')
		  , ('boujee', 'https://api.bintray.com/conan/boujee/conan ')
          )

if __name__ == '__main__':
	from conans.client.conan_api import Conan
	conan, _, _ = Conan.factory()
	conan_remotes = conan.remote_list()
	def url_inuse(remotes, url):
		try:
			_ = next(r for r in remotes if r.url == url)
			return True
		except StopIteration:
			return False
	def name_inuse(remotes, name):
		try:
			_ = next(r for r in remotes if r.name == name)
			return True
		except StopIteration:
			return False
	for name, url in REMOTES:
		print('Checking {} {}'.format(name, url))
		if url_inuse(conan_remotes, url):
			print('Remote already registered')
		else:
			if name_inuse(conan_remotes, name):
				name += '_jkpak'
			print('Adding remote {} {}'.format(name, url))
			conan.remote_add(name, url)