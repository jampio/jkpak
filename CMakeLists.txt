cmake_minimum_required(VERSION 3.10)
project(jkpak CXX)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
add_executable(jkpak src/download.cpp src/lib.cpp src/main.cpp src/pkg.cpp src/config.cpp src/util.cpp)
set_target_properties(jkpak PROPERTIES CXX_STANDARD 17)
target_link_libraries(jkpak ${CONAN_LIBS})
if (UNIX)
	# "conan build" overwrites CMAKE_INSTALL_PREFIX
	# not using "conan build" for now
	# set(CMAKE_INSTALL_PREFIX /usr/local/)
	install(TARGETS jkpak DESTINATION bin)
endif()