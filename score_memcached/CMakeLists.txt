set(SCORE_MEMCACHED_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB SCORE_MEMCACHED_SRC
    ${SCORE_MEMCACHED_SRC_DIR}/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SCORE_ASYNC_DIR}/include
    ${SCORE_BASE_INCLUDE_DIRS}
)


add_library(score_memcached ${SCORE_MEMCACHED_SRC})
add_dependencies(score_memcached score_async score_core)

target_link_libraries(score_memcached
    score_async
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
    ${SCORE_EXTRA_EXTERNAL_LIBS}
)


file(GLOB SCORE_MEMCACHED_UNIT_TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/**/*.cpp
)
add_executable(run_memcached_tests
    ${SCORE_MEMCACHED_UNIT_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)
add_dependencies(run_memcached_tests score_memcached)
target_link_libraries(run_memcached_tests
    gmock
    score_memcached
    score_async
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
    ${SCORE_EXTRA_EXTERNAL_LIBS}
)
