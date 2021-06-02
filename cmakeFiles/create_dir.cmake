include("cmakeFiles/var.cmake")

# For build directory
SET(BIN_DIR "${PROJECT_BINARY_DIR}/bin")
SET(BIN_DIR_TEST "${PROJECT_BINARY_DIR}/test_bin")

SET(${EXEC_NAME}_DIRS include lib)

foreach(d ${${EXEC_NAME}_DIRS})
   file(MAKE_DIRECTORY "${BIN_DIR}/${d}")
   file(MAKE_DIRECTORY "${BIN_DIR_TEST}/${d}")
endforeach(d)
