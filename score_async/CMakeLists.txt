set(SCORE_ASYNC_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB SCORE_ASYNC_SRC
    ${SCORE_ASYNC_SRC_DIR}/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SCORE_BASE_INCLUDE_DIRS}
)

add_library(score_async ${SCORE_ASYNC_SRC})
add_dependencies(score_async score_core)
target_link_libraries(score_async score_core ${SCORE_COMMON_EXTERNAL_LIBS})

FILE(GLOB SCORE_ASYNC_UNIT_TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/**/*.cpp
)

add_executable(run_async_unit_tests
    ${SCORE_ASYNC_UNIT_TEST_SRC}
    ${SCORE_TEST_RUNNER_FILE}
)

add_dependencies(run_async_unit_tests score_async score_core)
target_link_libraries(run_async_unit_tests
    gmock
    score_async
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)



FILE(GLOB ASYNC_INTEGRATION_TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_integration/*.cpp
)

add_executable(run_async_integration_tests
    ${ASYNC_INTEGRATION_TEST_SRC}
    ${SCORE_TEST_RUNNER_FILE}
)
add_dependencies(run_async_integration_tests score_async)
target_link_libraries(run_async_integration_tests
    gmock
    score_async
    score_core
    ${SCORE_ALL_EXTERNAL_LIBS}
)
