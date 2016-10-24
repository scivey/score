cmake_minimum_required(VERSION 2.8)

project(score)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -g -O0 -mcx16 -march=native")

set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(BORINGSSL ${EXTERNAL}/boringssl)
set(GTEST ${EXTERNAL}/googletest)
set(SPDLOG ${EXTERNAL}/spdlog)
set(RE2 ${EXTERNAL}/re2)

set(SCORE_EXTERNAL_INCLUDE_DIRS
    ${BORINGSSL}/include
    ${RE2}/build/include
    ${GTEST}/googletest/include
    ${GTEST}/googlemock/include
)

set(SCORE_EXTERNAL_LINK_DIRS
    ${BORINGSSL}/build/ssl
    ${BORINGSSL}/build/crypto
    ${BORINGSSL}/build/decrepit
    ${RE2}/build
    ${GTEST}/build/googlemock
)

set(SCORE_COMMON_EXTERNAL_LIBS
    iberty
    re2
    icuuc
    icudata
    glog
    event
    ssl
    crypto
    dl
    pthread
    gcc_s
    atomic
)

set(SCORE_EXTRA_EXTERNAL_LIBS
    curl
    hiredis
    memcached
)

set(SCORE_ALL_EXTERNAL_LIBS ${SCORE_COMMON_EXTERNAL_LIBS} ${SCORE_EXTRA_EXTERNAL_LIBS})

set(SCORE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SCORE_ROOT_SRC_DIR ${SCORE_ROOT_DIR}/src)

set(SCORE_ASYNC_DIR ${SCORE_ROOT_DIR}/score_async)
set(SCORE_BENCH_DIR ${SCORE_ROOT_DIR}/score_bench)
set(SCORE_CORE_DIR ${SCORE_ROOT_DIR}/score_core)
set(SCORE_CURL_DIR ${SCORE_ROOT_DIR}/score_curl)
set(SCORE_EXTRACT_DIR ${SCORE_ROOT_DIR}/score_extract)
set(SCORE_HTML_DIR ${SCORE_ROOT_DIR}/score_html)
set(SCORE_MEMCACHED_DIR ${SCORE_ROOT_DIR}/score_memcached)
set(SCORE_NLP_DIR ${SCORE_ROOT_DIR}/score_nlp)
set(SCORE_REDIS_DIR ${SCORE_ROOT_DIR}/score_redis)
set(SCORE_TEST_SUPPORT_DIR ${SCORE_ROOT_DIR}/test_support)
set(SCORE_TEST_RUNNER_FILE ${SCORE_TEST_SUPPORT_DIR}/src/run_tests.cpp)

set(SCORE_BASE_INCLUDE_DIRS
    ${SCORE_CORE_DIR}/include
    ${SCORE_EXTERNAL_INCLUDE_DIRS}
)

include_directories(
    ${SCORE_BASE_INCLUDE_DIRS}
)

link_directories(
    ${SCORE_EXTERNAL_LINK_DIRS}
)

set(ALL_SCORE_LIBS
    score_core
    score_memcached
    score_bench
    score_async
    score_redis
    score_extract
    score_html
    score_nlp
    score_curl
)

add_subdirectory(${SCORE_ASYNC_DIR})
add_subdirectory(${SCORE_BENCH_DIR})
add_subdirectory(${SCORE_CORE_DIR})
add_subdirectory(${SCORE_CURL_DIR})
add_subdirectory(${SCORE_EXTRACT_DIR})
add_subdirectory(${SCORE_HTML_DIR})
add_subdirectory(${SCORE_MEMCACHED_DIR})
add_subdirectory(${SCORE_NLP_DIR})
add_subdirectory(${SCORE_REDIS_DIR})

set(ALL_SCORE_INCLUDE_DIRS
    ${SCORE_ASYNC_DIR}/include
    ${SCORE_MEMCACHED_DIR}/include
    ${SCORE_BENCH_DIR}/include
    ${SCORE_CORE_DIR}/include
    ${SCORE_REDIS_DIR}/include
    ${SCORE_EXTRACT_DIR}/include
    ${SCORE_HTML_DIR}/include
    ${SCORE_NLP_DIR}/include
    ${SCORE_CURL_DIR}/include
)


# add_executable(bench_async
#     ${SCORE_ASYNC_SRC_DIR}/bench/bench_ThreadPool.cpp
# )
# add_dependencies(bench_async score_core score_async score_bench)
# target_link_libraries(bench_async
#     score_core
#     score_async
#     score_bench
#     wangle
#     folly
#     ${COMMON_LIBS}
# )

# add_executable(kgrind_threadpool
#     ${SCORE_ASYNC_SRC_DIR}/bench/kgrind_ThreadPool.cpp
# )
# add_dependencies(kgrind_threadpool score_core score_async)
# target_link_libraries(kgrind_threadpool
#     score_core
#     score_async
#     ${COMMON_LIBS}
# )

