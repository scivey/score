cmake_minimum_required(VERSION 2.8)

project(score)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -g -O2 -mcx16 -march=native")

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
    event
    ${CRYPTO_LIBS}
    dl
    pthread
    gcc_s
    atomic
)

set(EXTRA_LIBS
    curl
    hiredis
    memcached
)

set(SCORE_ROOT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SCORE_CORE_SRC_DIR ${SCORE_ROOT_SRC_DIR}/score)

set(SMHASHER_SOURCES
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/Platform.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/sha1.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/md5.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/Types.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/Bitvec.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/Bitslice.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/Random.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/City.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/MurmurHash3.cpp
)


set(VENDORED_CHARSET_ROOT ${SCORE_CORE_SRC_DIR}/vendored/lgpl/charsetdetect)
FILE(GLOB CHARSETDETECT_SOURCES ${VENDORED_CHARSET_ROOT}/base/*.cpp)
set(CHARSETDETECT_SOURCES
    ${CHARSETDETECT_SOURCES} ${VENDORED_CHARSET_ROOT}/charsetdetect.cpp
)

FILE(GLOB VENDORED_FOLLY_SOURCES
    ${SCORE_CORE_SRC_DIR}/vendored/folly/*.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/folly/**/*.cpp
)


set(SCORE_CORE_VENDORED_SRC
    ${SMHASHER_SOURCES}
    ${CHARSETDETECT_SOURCES}
    ${VENDORED_FOLLY_SOURCES}
    ${SCORE_CORE_SRC_DIR}/vendored/http-parser/http_parser.c
    ${SCORE_CORE_SRC_DIR}/vendored/pugixml/pugixml.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/modp_b64/modp_b64.cc
)

FILE(GLOB SCORE_COMMON_SRC
    ${SCORE_CORE_SRC_DIR}/*.cpp
    ${SCORE_CORE_SRC_DIR}/curl/**/*.cpp
    ${SCORE_CORE_SRC_DIR}/curl/*.cpp
    ${SCORE_CORE_SRC_DIR}/url/*.cpp
    ${SCORE_CORE_SRC_DIR}/url/**/*.cpp
    ${SCORE_CORE_SRC_DIR}/encodings/*.cpp
    ${SCORE_CORE_SRC_DIR}/html/*.cpp
    ${SCORE_CORE_SRC_DIR}/fs/*.cpp
    ${SCORE_CORE_SRC_DIR}/io/*.cpp
    ${SCORE_CORE_SRC_DIR}/locks/*.cpp
    ${SCORE_CORE_SRC_DIR}/mem/*.cpp
    ${SCORE_CORE_SRC_DIR}/posix/*.cpp
    ${SCORE_CORE_SRC_DIR}/re/*.cpp
    ${SCORE_CORE_SRC_DIR}/reactor/*.cpp
    ${SCORE_CORE_SRC_DIR}/unicode/*.cpp
    ${SCORE_CORE_SRC_DIR}/net/*.cpp
    ${SCORE_CORE_SRC_DIR}/hashing/*.cpp
    ${SCORE_CORE_SRC_DIR}/exceptions/*.cpp
)


set(SCORE_COMMON_SRC
    ${SCORE_COMMON_SRC}
    ${SCORE_CORE_VENDORED_SRC}
)

add_library(score_core ${SCORE_COMMON_SRC})
set(ALL_LIBS score_core ${COMMON_LIBS})



set(CORE_TEST_DIR ${SCORE_CORE_SRC_DIR}/test)

FILE(GLOB CORE_TEST_SRC
    ${CORE_TEST_DIR}/*.cpp
    ${CORE_TEST_DIR}/**/*.cpp
    ${CORE_TEST_DIR}/vendored/folly/*.cpp
)


add_executable(run_core_tests
    ${CORE_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)

add_dependencies(run_core_tests score_core)
target_link_libraries(run_core_tests
    gmock
    score_core
    ${COMMON_LIBS}
)








set(SCORE_HTML_VENDORED_SRC ${GUMBO_PARSER_SOURCES})
set(SCORE_HTML_SRC_DIR ${SCORE_ROOT_SRC_DIR}/score_html)
set(SCORE_HTML_VENDORED_DIR ${SCORE_HTML_SRC_DIR}/vendored)

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
target_link_libraries(score_html ${COMMON_LIBS} score_core)
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
    ${COMMON_LIBS}
)








set(SCORE_NLP_SRC_DIR ${SCORE_ROOT_SRC_DIR}/score_nlp)

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
target_link_libraries(score_nlp score_core ${COMMON_LIBS})


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
    ${COMMON_LIBS}
)







set(SCORE_EXTRACT_SRC_DIR ${SCORE_ROOT_SRC_DIR}/score_extract)

FILE(GLOB SCORE_EXTRACT_SRC
    ${SCORE_EXTRACT_SRC_DIR}/goose/*.cpp
)
add_library(score_extract ${SCORE_EXTRACT_SRC})
add_dependencies(score_extract score_core score_nlp score_html)
target_link_libraries(score_extract
    score_nlp
    score_html
    score_core
    ${COMMON_LIBS}
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
    ${COMMON_LIBS}
)




set(SCORE_ASYNC_SRC_DIR ${SCORE_ROOT_SRC_DIR}/score_async)

FILE(GLOB SCORE_ASYNC_SRC
    ${SCORE_ASYNC_SRC_DIR}/*.cpp

)
add_library(score_async ${SCORE_ASYNC_SRC})
add_dependencies(score_async score_core)
target_link_libraries(score_async score_core ${COMMON_LIBS})

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
    ${COMMON_LIBS}
)



set(SCORE_CURL_SRC_DIR ${SCORE_ROOT_SRC_DIR}/score_curl)

FILE(GLOB SCORE_CURL_SRC
    ${SCORE_CURL_SRC_DIR}/*.cpp
)

add_library(score_curl ${SCORE_CURL_SRC})
add_dependencies(score_curl score_async score_core)
target_link_libraries(score_curl
    score_async
    score_core
    ${COMMON_LIBS}
    ${EXTRA_LIBS}
)

FILE(GLOB SCORE_CURL_TEST_SRC
    ${SCORE_CURL_SRC_DIR}/test/*.cpp
)
add_executable(run_curl_tests
    ${SCORE_CURL_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)

add_dependencies(run_curl_tests score_curl)
target_link_libraries(run_curl_tests
    gmock
    score_curl
    score_async
    score_core
    ${COMMON_LIBS}
    ${EXTRA_LIBS}
)













set(SCORE_REDIS_SRC_DIR ${SCORE_ROOT_SRC_DIR}/score_redis)

FILE(GLOB SCORE_REDIS_SRC
    ${SCORE_REDIS_SRC_DIR}/*.cpp
    ${SCORE_REDIS_SRC_DIR}/hiredis_adapter/*.cpp
)

add_library(score_redis ${SCORE_REDIS_SRC})
add_dependencies(score_redis score_async score_core)
target_link_libraries(score_redis
    score_async
    score_core
    ${COMMON_LIBS}
    ${EXTRA_LIBS}
)





set(SCORE_MEMCACHED_SRC_DIR ${SCORE_ROOT_SRC_DIR}/score_memcached)

FILE(GLOB SCORE_MEMCACHED_SRC
    ${SCORE_MEMCACHED_SRC_DIR}/*.cpp
)

add_library(score_memcached ${SCORE_MEMCACHED_SRC})
add_dependencies(score_memcached score_async score_core)

target_link_libraries(score_memcached
    score_async
    score_core
    ${COMMON_LIBS}
    ${EXTRA_LIBS}
)


file(GLOB SCORE_MEMCACHED_TEST_SRC
    ${SCORE_MEMCACHED_SRC_DIR}/test/*.cpp
)
add_executable(run_memcached_tests
    ${SCORE_MEMCACHED_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)
add_dependencies(run_memcached_tests score_memcached)
target_link_libraries(run_memcached_tests
    gmock
    score_memcached
    score_async
    score_core
    ${COMMON_LIBS}
    ${EXTRA_LIBS}
)




set(ALL_SCORE_LIBS
    score_core
    score_memcached
    score_async
    score_redis
    score_extract
    score_html
    score_nlp
    score_curl
)

FILE(GLOB SCORE_CLIENT_INTEGRATION_TEST_SRC
    ${SCORE_MEMCACHED_SRC_DIR}/test_integration/*.cpp
    ${SCORE_REDIS_SRC_DIR}/test_integration/*.cpp
)

add_executable(run_client_integration_tests
    ${SCORE_CLIENT_INTEGRATION_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)

add_dependencies(run_client_integration_tests ${ALL_SCORE_LIBS})
target_link_libraries(run_client_integration_tests
    gmock
    ${ALL_SCORE_LIBS}
    ${COMMON_LIBS}
    ${EXTRA_LIBS}
)






FILE(GLOB SCORE_MISC_INTEGRATION_TEST_SRC
    ${SCORE_ASYNC_SRC_DIR}/test_integration/*.cpp
)
add_executable(run_misc_integration_tests
    ${SCORE_MISC_INTEGRATION_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)
add_dependencies(run_misc_integration_tests ${ALL_SCORE_LIBS})
target_link_libraries(run_misc_integration_tests
    gmock
    ${ALL_SCORE_LIBS}
    ${COMMON_LIBS}
    ${EXTRA_LIBS}
)





add_executable(runner
    ${SCORE_ROOT_SRC_DIR}/main.cpp
)
add_dependencies(runner ${ALL_SCORE_LIBS})
target_link_libraries(runner
    ${ALL_SCORE_LIBS}
    ${COMMON_LIBS}
    ${EXTRA_LIBS}
)




add_executable(unit_test_dummy ${SCORE_ROOT_SRC_DIR}/run_dummy.cpp)
add_dependencies(unit_test_dummy
    run_async_tests
    run_core_tests
    run_curl_tests
    run_extract_tests
    run_html_tests
    run_nlp_tests
    run_memcached_tests
)

target_link_libraries(unit_test_dummy glog)

add_executable(integration_test_dummy ${SCORE_ROOT_SRC_DIR}/run_dummy.cpp)
add_dependencies(integration_test_dummy
    run_client_integration_tests
    run_misc_integration_tests
)
target_link_libraries(integration_test_dummy glog)



add_executable(bench_async
    ${SCORE_ASYNC_SRC_DIR}/bench/bench_ThreadPool.cpp
)
add_dependencies(bench_async score_core score_async)
target_link_libraries(bench_async
    score_core
    score_async
    wangle
    folly
    ${COMMON_LIBS}
)

add_executable(kgrind_threadpool
    ${SCORE_ASYNC_SRC_DIR}/bench/kgrind_ThreadPool.cpp
)
add_dependencies(kgrind_threadpool score_core score_async)
target_link_libraries(kgrind_threadpool
    score_core
    score_async
    ${COMMON_LIBS}
)

