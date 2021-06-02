include(cmakeFiles/var.cmake)

define_property(DIRECTORY PROPERTY folder_configs INHERITED BRIEF_DOCS "build time generated config headers" FULL_DOCS "build time generated config headers")

set_directory_properties(PROPERTIES folder_configs ${CMAKE_SOURCE_DIR}/configs)
