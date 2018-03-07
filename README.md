# jkpak
A pk3 manager for jedi academy. I could see it being repurposed for other games as well.

Feel free to open issues for jkpak requests/bugs/etc.

To request a file be added, please send a pull request or open an issue here [jkpak-repo](https://github.com/jampio/jkpak-repo). (not in this repo please)

### future plans
* cross-platform support
* OpenJK integration

### building
Requirements
* Python2 or Python3 installed (for conan)
* C++17 compiler
```shell
# make sure conan is installed, can use pip3 for python 3
pip install conan
# add conan repo bincrafters (for libcurl)
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
# (...)
git clone https://github.com/jampio/jkpak
cd jkpak && mkdir build && cd build
conan install ..
conan build ..
# this will install to /usr/local/bin
sudo cmake --build . --target install
```

### usage
```shell
# point it to your base directory
# fs_game support is not added yet
jkpak set-install-path ~/.local/share/openjk/base/
# download the repo
jkpak update
# install something
jkpak install countryacademy
# or with url
jkpak install https://files.jkhub.org/jka/maps/ffa/countryacademy.zip
# list installed pk3s
jkpak ls
# remove a pk3
jkpak rm countryacademy.pk3
```
