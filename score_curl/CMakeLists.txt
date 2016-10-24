set(SCORE_CURL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB SCORE_CURL_SRC
    ${SCORE_CURL_SRC_DIR}/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SCORE_ASYNC_DIR}/include
    ${SCORE_BASE_INCLUDE_DIRS}
)

add_library(score_curl ${SCORE_CURL_SRC})
add_dependencies(score_curl score_async score_core)
target_link_libraries(score_curl
    score_async
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
    ${SCORE_EXTRA_EXTERNAL_LIBS}
)

FILE(GLOB SCORE_CURL_UNIT_TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/**/*.cpp
)
add_executable(run_curl_unit_tests
    ${SCORE_CURL_UNIT_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)

add_dependencies(run_curl_unit_tests score_curl)
target_link_libraries(run_curl_unit_tests
    gmock
    score_curl
    score_async
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
    ${SCORE_EXTRA_EXTERNAL_LIBS}
)
