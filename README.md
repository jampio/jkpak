# jkpak
A pk3 manager for jedi academy. I could see it being repurposed for other games as well.

Feel free to open issues for jkpak requests/bugs/etc.

To request a file be added, please send a pull request or open an issue here [jkpak-repo](https://github.com/jampio/jkpak-repo). (not in this repo please)

### future plans
* cross-platform support
* OpenJK integration

### building
Requirements
* CMake (ver. >= 3.10.2)
* Python 2 (ver. >= 2.7.9) or Python 3 (ver. >= 3.4)
* Conan (ver. >= 1) (a C++ dependency manager)
* C++17 compiler (should support either -std=c++17 or /std:c++17)
* unzip available in PATH
```shell
# install conan if you haven't already
pip install conan
# add bincrafters repository (contains libcurl)
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
# ...
git clone https://github.com/jampio/jkpak
cd jkpak && mkdir build && cd build
conan install ..
cmake ..
cmake --build .
# (for UNIX only)
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
