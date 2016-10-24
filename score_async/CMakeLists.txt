set(SCORE_ASYNC_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB SCORE_ASYNC_SRC
    ${SCORE_ASYNC_SRC_DIR}/*.cpp

)
add_library(score_async ${SCORE_ASYNC_SRC})
add_dependencies(score_async score_core)
target_link_libraries(score_async score_core ${SCORE_COMMON_EXTERNAL_LIBS})

FILE(GLOB SCORE_ASYNC_TEST_SRC
    ${SCORE_ASYNC_SRC_DIR}/test/*.cpp
)
add_executable(run_async_tests
    ${SCORE_ASYNC_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)

add_dependencies(run_async_tests score_async score_core)
target_link_libraries(run_async_tests
    gmock
    score_async
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)
