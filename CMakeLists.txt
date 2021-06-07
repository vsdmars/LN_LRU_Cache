cmake_minimum_required(VERSION 3.10)

# enable unit test
# https://stackoverflow.com/a/30264765
enable_testing()

# include variables
include("cmakeFiles/var.cmake")
# setup version
include("cmakeFiles/version.cmake")

# set the project name / version
project(LRU_Cache VERSION ${LRUC_VERSION} LANGUAGES CXX)

# binary to test LRUCache
add_subdirectory(src)

# unit test and benchmark binary
add_subdirectory(test)

# setup config options
include("cmakeFiles/config.cmake")

# setup compile options
include("cmakeFiles/compile.cmake")

# setup folder properties
include("cmakeFiles/folder.cmake")

# setup create directories properties
include("cmakeFiles/create_dir.cmake")

# setup module properties
include("cmakeFiles/modules.cmake")
