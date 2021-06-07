# -- lruc --
add_executable(${EXEC_NAME} main.cpp)

# for lru cache config
target_include_directories(${EXEC_NAME} PRIVATE "${PROJECT_BINARY_DIR}")

# LRUCache header
target_include_directories(${EXEC_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(${EXEC_NAME} PRIVATE TBB::tbb)

find_package(TBB REQUIRED)
# message("TBBFOUND: ${TBB_FOUND}")
# message("TBBVER: ${TBB_VERSION}")
# message("TBBIMPORTED: ${TBB_IMPORTED_TARGETS}")
# message("TBB::tbb FOUND: ${TBB_tbb_FOUND}")


# -- setup binary location --
set_property(TARGET ${EXEC_NAME}
   PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
