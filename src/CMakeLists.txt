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


# -- LRUCache unit test --
include(GoogleTest)
find_package(GTest REQUIRED)

SET(LRUCACHE_TEST_SRC "lrucache_test.cpp")

add_executable(${LRUCACHE_TEST} ${LRUCACHE_TEST_SRC})

# LRUCache header
target_include_directories(${LRUCACHE_TEST} PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(${LRUCACHE_TEST} PRIVATE TBB::tbb)
target_link_libraries(${LRUCACHE_TEST} PRIVATE GTest::gtest_main)
gtest_discover_tests(${LRUCACHE_TEST})


# -- Scale-LRUCache unit test --
SET(SCALE_LRUCACHE_TEST_SRC "scale-lrucache_test.cpp")

add_executable(${SCALE_LRUCACHE_TEST} ${SCALE_LRUCACHE_TEST_SRC})

# LRUCache header
target_include_directories(${SCALE_LRUCACHE_TEST} PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(${SCALE_LRUCACHE_TEST} PRIVATE GTest::gtest_main)
gtest_discover_tests(${SCALE_LRUCACHE_TEST})


# -- setup binary location --
set_property(TARGET ${EXEC_NAME}
   PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

set_property(TARGET ${LRUCACHE_TEST}
    PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test_bin")

set_property(TARGET ${SCALE_LRUCACHE_TEST}
    PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test_bin")

set_property(TARGET ${LIB_NAME}
   PROPERTY LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
