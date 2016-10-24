set(SCORE_BENCH_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SCORE_BASE_INCLUDE_DIRS}
)

FILE(GLOB SCORE_BENCH_SRC
    ${SCORE_BENCH_SRC_DIR}/*.cpp
)

add_library(score_bench ${SCORE_BENCH_SRC})
add_dependencies(score_bench score_core)

target_link_libraries(score_bench
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)

FILE(GLOB SCORE_BENCH_UNIT_TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/*.cpp
)
add_executable(run_bench_unit_tests
    ${SCORE_BENCH_UNIT_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)
add_dependencies(run_bench_unit_tests score_bench)
target_link_libraries(run_bench_unit_tests
    gmock
    score_bench
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)


