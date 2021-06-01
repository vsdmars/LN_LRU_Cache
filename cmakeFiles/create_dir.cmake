include("cmakeFiles/var.cmake")

SET(${EXEC_NAME}_DIRS include lib)

foreach(d ${${EXEC_NAME}_DIRS})
   file(MAKE_DIRECTORY "${BIN_DIR}/${d}")
   file(MAKE_DIRECTORY "${BIN_DIR_TEST}/${d}")
endforeach(d)
