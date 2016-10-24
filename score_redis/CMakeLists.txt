set(SCORE_REDIS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SCORE_ASYNC_DIR}/include
    ${SCORE_BASE_INCLUDE_DIRS}
)

FILE(GLOB SCORE_REDIS_SRC
    ${SCORE_REDIS_SRC_DIR}/*.cpp
    ${SCORE_REDIS_SRC_DIR}/hiredis_adapter/*.cpp
)

add_library(score_redis ${SCORE_REDIS_SRC})
add_dependencies(score_redis score_async score_core)
target_link_libraries(score_redis
    score_async
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
    ${SCORE_EXTRA_EXTERNAL_LIBS}
)

FILE(GLOB REDIS_INTEGRATION_TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_integration/*.cpp
)

add_executable(run_redis_integration_tests
    ${REDIS_INTEGRATION_TEST_SRC}
    ${SCORE_TEST_RUNNER_FILE}
)
add_dependencies(run_redis_integration_tests score_redis)
target_link_libraries(run_redis_integration_tests
    gmock
    score_redis
    score_core
    score_async
    ${SCORE_ALL_EXTERNAL_LIBS}
)
