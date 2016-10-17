cmake_minimum_required(VERSION 2.8)

project(score)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -g -O0 -mcx16 -march=native")

set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(BORINGSSL ${EXTERNAL}/boringssl)
set(GTEST ${EXTERNAL}/googletest)
set(SPDLOG ${EXTERNAL}/spdlog)
set(RE2 ${EXTERNAL}/re2)
include_directories(
    ${BORINGSSL}/include
    ${RE2}/build/include
    ${GTEST}/googletest/include
    ${GTEST}/googlemock/include
    ${SPDLOG}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    /usr/local/include
)

link_directories(
    ${BORINGSSL}/build/ssl
    ${BORINGSSL}/build/crypto
    ${BORINGSSL}/build/decrepit
    ${RE2}/build
    ${GTEST}/build/googlemock
    /usr/local/lib
)

set(CRYPTO_LIBS
    ssl
    crypto
)

set(COMMON_LIBS
    folly
    wangle
    re2
    boost_system
    boost_thread
    icuuc
    icudata
    glog
    curl
    event
    ${CRYPTO_LIBS}
    dl
    pthread
    gcc_s
    atomic
)

set(SCORE_ROOT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SCORE_CORE_SRC ${SCORE_ROOT_SRC}/score)

set(GUMBO_PARSER_SOURCES
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/attribute.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/char_ref.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/error.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/parser.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/string_buffer.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/string_piece.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/tag.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/tokenizer.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/utf8.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/util.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/vector.c
)

set(SMHASHER_SOURCES
    ${SCORE_CORE_SRC}/vendored/smhasher/Platform.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/sha1.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/md5.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/Types.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/Bitvec.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/Bitslice.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/Random.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/City.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/MurmurHash3.cpp
)


set(VENDORED_CHARSET_ROOT ${SCORE_CORE_SRC}/vendored/lgpl/charsetdetect)
FILE(GLOB CHARSETDETECT_SOURCES ${VENDORED_CHARSET_ROOT}/base/*.cpp)
set(CHARSETDETECT_SOURCES
    ${CHARSETDETECT_SOURCES} ${VENDORED_CHARSET_ROOT}/charsetdetect.cpp
)

FILE(GLOB VENDORED_FOLLY_SOURCES
    ${SCORE_CORE_SRC}/vendored/folly/*.cpp
    ${SCORE_CORE_SRC}/vendored/folly/**/*.cpp
)


set(SCORE_VENDORED_SRC
    ${SMHASHER_SOURCES}
    ${GUMBO_PARSER_SOURCES}
    ${CHARSETDETECT_SOURCES}
    ${VENDORED_FOLLY_SOURCES}
    ${SCORE_CORE_SRC}/vendored/http-parser/http_parser.c
    ${SCORE_CORE_SRC}/vendored/pugixml/pugixml.cpp
    ${SCORE_CORE_SRC}/vendored/modp_b64/modp_b64.cc
)

FILE(GLOB SCORE_COMMON_SRC
    ${SCORE_CORE_SRC}/*.cpp
    ${SCORE_CORE_SRC}/curl/**/*.cpp
    ${SCORE_CORE_SRC}/curl/*.cpp
    ${SCORE_CORE_SRC}/url/*.cpp
    ${SCORE_CORE_SRC}/url/**/*.cpp
    ${SCORE_CORE_SRC}/encodings/*.cpp
    ${SCORE_CORE_SRC}/html/*.cpp
    ${SCORE_CORE_SRC}/fs/*.cpp
    ${SCORE_CORE_SRC}/io/*.cpp
    ${SCORE_CORE_SRC}/locks/*.cpp
    ${SCORE_CORE_SRC}/mem/*.cpp
    ${SCORE_CORE_SRC}/posix/*.cpp
    ${SCORE_CORE_SRC}/re/*.cpp
    ${SCORE_CORE_SRC}/reactor/*.cpp
    ${SCORE_CORE_SRC}/unicode/*.cpp
    ${SCORE_CORE_SRC}/net/*.cpp
    ${SCORE_CORE_SRC}/hashing/*.cpp
    ${SCORE_CORE_SRC}/exceptions/*.cpp
)


set(SCORE_COMMON_SRC
    ${SCORE_COMMON_SRC}
    ${SCORE_VENDORED_SRC}
)

add_library(score_core ${SCORE_COMMON_SRC})
set(ALL_LIBS score_core ${COMMON_LIBS})



set(CORE_TEST_DIR ${SCORE_CORE_SRC}/test)

FILE(GLOB CORE_TEST_SRC
    ${CORE_TEST_DIR}/*.cpp
    ${CORE_TEST_DIR}/**/*.cpp
    ${CORE_TEST_DIR}/vendored/folly/*.cpp
)


add_executable(run_core_tests
    ${CORE_TEST_SRC}
    ${SCORE_ROOT_SRC}/run_tests.cpp
)

add_dependencies(run_core_tests score_core)
target_link_libraries(run_core_tests
    gmock
    ${ALL_LIBS}
)




set(SCORE_NLP_SRC_DIR ${SCORE_ROOT_SRC}/score_nlp)

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
target_link_libraries(score_nlp ${COMMON_LIBS} score_core)


FILE(GLOB SCORE_NLP_TEST_SRC
    ${SCORE_NLP_SRC_DIR}/test/*.cpp
    ${SCORE_NLP_SRC_DIR}/test/**/*.cpp
)

add_executable(run_nlp_tests
    ${SCORE_NLP_TEST_SRC}
    ${SCORE_ROOT_SRC}/run_tests.cpp
)
add_dependencies(run_nlp_tests score_nlp)
target_link_libraries(run_nlp_tests gmock ${COMMON_LIBS} score_core score_nlp)







set(SCORE_EXTRACT_SRC_DIR ${SCORE_ROOT_SRC}/score_extract)

FILE(GLOB SCORE_EXTRACT_SRC
    ${SCORE_EXTRACT_SRC_DIR}/goose/*.cpp
)
add_library(score_extract ${SCORE_EXTRACT_SRC})
add_dependencies(score_extract score)
add_dependencies(score_extract score_nlp)
target_link_libraries(score_extract ${COMMON_LIBS} score_core score_nlp)

FILE(GLOB SCORE_EXTRACT_TEST_SRC
    ${SCORE_EXTRACT_SRC_DIR}/test/*.cpp
    ${SCORE_EXTRACT_SRC_DIR}/test/**/*.cpp
)

add_executable(run_extract_tests
    ${SCORE_EXTRACT_TEST_SRC}
    ${SCORE_ROOT_SRC}/run_tests.cpp
)

add_dependencies(run_extract_tests score_extract)
target_link_libraries(run_extract_tests
    gmock
    ${COMMON_LIBS}
    score_extract
    score_core
    score_nlp
)




set(SCORE_ASYNC_SRC_DIR ${SCORE_ROOT_SRC}/score_async)

FILE(GLOB SCORE_ASYNC_SRC
    ${SCORE_EXTRACT_SRC_DIR}/EventFD.cpp
)
add_library(score_async ${SCORE_ASYNC_SRC})
add_dependencies(score_async score)
target_link_libraries(score_async ${COMMON_LIBS} score_core)

FILE(GLOB SCORE_ASYNC_TEST_SRC
    ${SCORE_ASYNC_SRC_DIR}/test/*.cpp
)
add_executable(run_async_tests
    ${SCORE_ASYNC_TEST_SRC}
    ${SCORE_ROOT_SRC}/run_tests.cpp
)

add_dependencies(run_async_tests score_async)
target_link_libraries(run_async_tests
    gmock
    ${COMMON_LIBS}
    score_async
    score_core
)



set(SCORE_CURL_SRC_DIR ${SCORE_ROOT_SRC}/score_curl)
FILE(GLOB SCORE_CURL_TEST_SRC
    ${SCORE_CURL_SRC_DIR}/test/*.cpp
)
add_executable(run_curl_tests
    ${SCORE_CURL_TEST_SRC}
    ${SCORE_ROOT_SRC}/run_tests.cpp
)

add_dependencies(run_curl_tests score_async)
target_link_libraries(run_curl_tests
    gmock
    ${COMMON_LIBS}
    score_async
    score_core
)



add_executable(runner
    ${SCORE_ROOT_SRC}/main.cpp
)
add_dependencies(runner score_core score_async)
target_link_libraries(runner
    ${ALL_LIBS}
    score_async
    score_core
)
