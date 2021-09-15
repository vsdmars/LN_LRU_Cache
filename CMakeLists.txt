cmake_minimum_required(VERSION 3.10)


# enable unit test
# https://stackoverflow.com/a/30264765
enable_testing()

# setup version
include("cmakeFiles/version.cmake")

# set the project name / version
project(LRU_Cache VERSION ${LRUC_VERSION} LANGUAGES CXX)

# unit test and benchmark binary
add_subdirectory(test)

# setup compile options
include("cmakeFiles/compile.cmake")

# setup create directories properties
include("cmakeFiles/create_dir.cmake")
