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
#### Linux (with APT)
```shell
# install unzip (required at runtime)
sudo apt install unzip
# install cmake (cmake from APT is dated)
wget -O cmake.sh https://cmake.org/files/v3.10/cmake-3.10.2-Linux-x86_64.sh
sudo sh cmake.sh --skip-license --exclude-subdir --prefix=/usr/local
# install conan & add remotes
pip install conan && python remotes.py
# build (will install into /usr/local/bin)
make && sudo make install
```
#### Windows
```shell
# install conan & add remotes
pip install conan && python remotes.py
# build
bin\build
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
# install jampgamei386.so and libcxa.so.1
sudo jkpak install basejka
# install asset pk3s from steam
jkpak install steam-assets
```
