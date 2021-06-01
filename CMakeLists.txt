cmake_minimum_required(VERSION 3.10)

# include variables
include("cmakeFiles/var.cmake")
# setup version
include("cmakeFiles/version.cmake")

# set the project name / version
project(LRU_Cache VERSION ${LRUC_VERSION} LANGUAGES CXX)
message("test: ${PROJECT_SOURCE_DIR}")

add_subdirectory(src)

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
