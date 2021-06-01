# specify the C++ standard
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED True)

# debugging purpose, should be removed in production
# https://stackoverflow.com/a/7032021
set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# compile with C++ 17 standard
# https://cmake.org/cmake/help/latest/manual/cmake-compile-features.7.html#requiring-language-standards
target_compile_features(${EXEC_NAME} PRIVATE cxx_std_17)

# extra compile options
target_compile_options(${EXEC_NAME} PRIVATE -O3 -Wall -Wconversion -Wdouble-promotion -Weffc++ -Wextra -Wfloat-equal -Wformat=2 -Wno-error=unused-variable -Wnull-dereference -Wold-style-cast -Wpedantic -Wshadow -Wuninitialized -Wunreachable-code -Wredundant-move -Wpessimizing-move -Wthread-safety)

# RPATH setup
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
