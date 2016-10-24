set(SCORE_NLP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SCORE_COMMON_INCLUDE_DIRS}
)

set(VENDORED_LIBSTEMMER_ROOT ${SCORE_NLP_SRC_DIR}/vendored/libstemmer)
FILE(GLOB LIBSTEMMER_SOURCES ${VENDORED_LIBSTEMMER_ROOT}/**/*.c)
set(LIBSTEMMER_SOURCES ${LIBSTEMMER_SOURCES} ${VENDORED_LIBSTEMMER_ROOT}/libstemmer_utf8.c)

set(VENDORED_CLD2_ROOT ${SCORE_NLP_SRC_DIR}/vendored/cld2)
FILE(GLOB CLD2_SOURCES ${VENDORED_CLD2_ROOT}/**/*.cc)

FILE(GLOB SCORE_NLP_SRC
    ${SCORE_NLP_SRC_DIR}/stemming/*.cpp
    ${SCORE_NLP_SRC_DIR}/stopwords/*.cpp
    ${SCORE_NLP_SRC_DIR}/tokenize/*.cpp
)

set(SCORE_NLP_SRC
    ${SCORE_NLP_SRC}
    ${SCORE_NLP_SRC_DIR}/Language.cpp
    ${SCORE_NLP_SRC_DIR}/UTF8UTextRef.cpp
    ${LIBSTEMMER_SOURCES}
    ${CLD2_SOURCES}
)

add_library(score_nlp ${SCORE_NLP_SRC})
add_dependencies(score_nlp score_core)
target_link_libraries(score_nlp score_core ${SCORE_COMMON_EXTERNAL_LIBS})


FILE(GLOB SCORE_NLP_TEST_SRC
    ${SCORE_NLP_SRC_DIR}/test/*.cpp
    ${SCORE_NLP_SRC_DIR}/test/**/*.cpp
)

add_executable(run_nlp_tests
    ${SCORE_NLP_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)
add_dependencies(run_nlp_tests score_nlp)
target_link_libraries(run_nlp_tests
    gmock
    score_nlp
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)


