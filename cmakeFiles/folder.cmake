include(cmakeFiles/var.cmake)

# For build directory
SET(BIN_DIR "${PROJECT_BINARY_DIR}/bin")
SET(BIN_DIR_TEST "${PROJECT_BINARY_DIR}/test_bin")

# setup project source
SET(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

set_property(TARGET ${EXEC_NAME}
   PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set_property(TARGET ${LIB_NAME}
   PROPERTY LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

define_property(DIRECTORY PROPERTY folder_configs INHERITED BRIEF_DOCS "build time generated config headers" FULL_DOCS "build time generated config headers")

set_directory_properties(PROPERTIES folder_configs ${CMAKE_SOURCE_DIR}/configs)
