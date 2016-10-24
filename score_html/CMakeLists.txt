set(SCORE_HTML_VENDORED_SRC ${GUMBO_PARSER_SOURCES})
set(SCORE_HTML_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SCORE_HTML_VENDORED_DIR ${SCORE_HTML_SRC_DIR}/vendored)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SCORE_BASE_INCLUDE_DIRS}
)

set(SCORE_HTML_GUMBO_PARSER_SRC
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/attribute.c
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/char_ref.c
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/error.c
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/parser.c
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/string_buffer.c
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/string_piece.c
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/tag.c
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/tokenizer.c
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/utf8.c
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/util.c
    ${SCORE_HTML_VENDORED_DIR}/gumbo-parser/vector.c
)
FILE(GLOB SCORE_HTML_SRC
    ${SCORE_HTML_SRC_DIR}/*.cpp
    ${SCORE_HTML_SRC_DIR}/detail/*.cpp
)
set(SCORE_HTML_SRC
    ${SCORE_HTML_SRC}
    ${SCORE_HTML_GUMBO_PARSER_SRC}
)
add_library(score_html ${SCORE_HTML_SRC})
add_dependencies(score_html score_core)
target_link_libraries(score_html score_core ${SCORE_COMMON_EXTERNAL_LIBS})



FILE(GLOB SCORE_HTML_TEST_SRC
    ${SCORE_HTML_SRC_DIR}/test/*.cpp
)
add_executable(run_html_tests
    ${SCORE_HTML_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)
add_dependencies(run_html_tests score_html)
target_link_libraries(run_html_tests
    gmock
    score_html
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)
