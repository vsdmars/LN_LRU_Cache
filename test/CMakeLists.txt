find_package(TBB REQUIRED)
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED) # benchmark::benchmark
include(GoogleTest)

# -- LRUCache unit test --
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
# Scale-LRUCache header
target_include_directories(${SCALE_LRUCACHE_TEST} PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${SCALE_LRUCACHE_TEST} PRIVATE TBB::tbb)
target_link_libraries(${SCALE_LRUCACHE_TEST} PRIVATE GTest::gtest_main)
gtest_discover_tests(${SCALE_LRUCACHE_TEST})


# -- LRUCache benchmark test --
SET(LRUCACHE_BENCH_SRC "lrucache_bench.cpp")
add_executable(${LRUCACHE_BENCH} ${LRUCACHE_BENCH_SRC})
# LRUCache header
target_include_directories(${LRUCACHE_BENCH} PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${LRUCACHE_BENCH} PRIVATE TBB::tbb)
target_link_libraries(${LRUCACHE_BENCH} PRIVATE benchmark::benchmark)


# -- ScalableLRUCache benchmark test --
SET(SCALABLE_LRUCACHE_BENCH_SRC "scalable_lrucache_bench.cpp")
add_executable(${SCALE_LRUCACHE_BENCH} ${SCALABLE_LRUCACHE_BENCH_SRC})
# ScalableLRUCache header
target_include_directories(${SCALE_LRUCACHE_BENCH} PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${SCALE_LRUCACHE_BENCH} PRIVATE TBB::tbb)
target_link_libraries(${SCALE_LRUCACHE_BENCH} PRIVATE benchmark::benchmark)


# -- setup binary location --
set_property(TARGET ${LRUCACHE_TEST}
    PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test_bin")

set_property(TARGET ${LRUCACHE_BENCH}
    PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test_bin")

set_property(TARGET ${SCALE_LRUCACHE_TEST}
    PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test_bin")

set_property(TARGET ${SCALE_LRUCACHE_BENCH}
    PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test_bin")
