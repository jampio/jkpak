# jkpak [![Build Status](https://travis-ci.org/jampio/jkpak.svg?branch=master)](https://travis-ci.org/jampio/jkpak)
A pk3 manager for jedi academy. I could see it being repurposed for other games as well.

Feel free to open issues for jkpak requests/bugs/etc.

To request a file be added, please send a pull request or open an issue here [jkpak-repo](https://github.com/jampio/jkpak-repo). (not in this repo please)

### future plans
* OpenJK integration

### building
Requirements
* cmake (ver. >= 3.10)
* c++17 compiler (should support either -std=c++17 or /std:c++17)
* unzip available in PATH
#### installing cmake 3.10.2 on linux x86_64
```shell
wget -O cmake.sh https://cmake.org/files/v3.10/cmake-3.10.2-Linux-x86_64.sh
sudo sh cmake.sh --skip-license --exclude-subdir --prefix=/usr/local
```
#### APT deps
```shell
dpkg-checkbuilddeps || sudo apt build-dep .
```
#### building
```shell
pip install -r requirements.txt
python remotes.py
mkdir build
cd build
# download & install conan dep binaries
# use "conan install --build .." to build only from source
# use "conan install --build=missing .." to build only missing binaries from source
conan install ..
# build project
# use "sudo conan build -pf /usr/local .." to install to /usr/local after
conan build ..
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
