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

FILE(GLOB SCORE_EXTRACT_TEST_SRC
    ${SCORE_EXTRACT_SRC_DIR}/test/*.cpp
    ${SCORE_EXTRACT_SRC_DIR}/test/**/*.cpp
)
add_executable(run_extract_tests
    ${SCORE_EXTRACT_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)
add_dependencies(run_extract_tests score_extract)
target_link_libraries(run_extract_tests
    gmock
    score_extract
    score_html
    score_nlp
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)

