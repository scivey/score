set(SCORE_EXTRACT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
FILE(GLOB SCORE_EXTRACT_SRC
    ${SCORE_EXTRACT_SRC_DIR}/goose/*.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SCORE_HTML_DIR}/include
    ${SCORE_NLP_DIR}/include
    ${SCORE_BASE_INCLUDE_DIRS}
)

add_library(score_extract ${SCORE_EXTRACT_SRC})
add_dependencies(score_extract score_core score_nlp score_html)
target_link_libraries(score_extract
    score_nlp
    score_html
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)

FILE(GLOB SCORE_EXTRACT_UNIT_TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/**/*.cpp
)
add_executable(run_extract_unit_tests
    ${SCORE_EXTRACT_UNIT_TEST_SRC}
    ${SCORE_TEST_RUNNER_FILE}
)
add_dependencies(run_extract_unit_tests score_extract)
target_link_libraries(run_extract_unit_tests
    gmock
    score_extract
    score_html
    score_nlp
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)

