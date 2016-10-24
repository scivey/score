set(SCORE_BENCH_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/score_bench)

FILE(GLOB SCORE_BENCH_SRC
    ${SCORE_BENCH_SRC_DIR}/*.cpp
)

add_library(score_bench ${SCORE_BENCH_SRC})
add_dependencies(score_bench score_core)

target_link_libraries(score_bench
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)

FILE(GLOB SCORE_BENCH_TEST_SRC
    ${SCORE_BENCH_SRC_DIR}/test/*.cpp
)
add_executable(run_bench_tests
    ${SCORE_BENCH_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)
add_dependencies(run_bench_tests score_bench)
target_link_libraries(run_bench_tests
    gmock
    score_bench
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)


