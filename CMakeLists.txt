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
    /usr/local/include
)

set(SCORE_EXTERNAL_LINK_DIRS
    ${BORINGSSL}/build/ssl
    ${BORINGSSL}/build/crypto
    ${BORINGSSL}/build/decrepit
    ${RE2}/build
    ${GTEST}/build/googlemock
    /usr/local/lib
)

set(SCORE_COMMON_EXTERNAL_LIBS
    folly
    wangle
    re2
    boost_system
    boost_thread
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



# FILE(GLOB SCORE_CLIENT_INTEGRATION_TEST_SRC
#     ${SCORE_MEMCACHED_DIR}/src/test_integration/*.cpp
#     ${SCORE_REDIS_DIR}/src/test_integration/*.cpp
# )

# add_executable(run_client_integration_tests
#     ${SCORE_CLIENT_INTEGRATION_TEST_SRC}
#     ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
# )

# add_dependencies(run_client_integration_tests ${ALL_SCORE_LIBS})
# target_link_libraries(run_client_integration_tests
#     gmock
#     ${ALL_SCORE_LIBS}
#     ${SCORE_ALL_EXTERNAL_LIBS}
# )




# FILE(GLOB SCORE_MISC_INTEGRATION_TEST_SRC
#     ${SCORE_ASYNC_DIR}/src/test_integration/*.cpp
# )
# add_executable(run_misc_integration_tests
#     ${SCORE_MISC_INTEGRATION_TEST_SRC}
#     ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
# )
# add_dependencies(run_misc_integration_tests ${ALL_SCORE_LIBS})
# target_link_libraries(run_misc_integration_tests
#     gmock
#     ${ALL_SCORE_LIBS}
#     ${SCORE_ALL_EXTERNAL_LIBS}
# )





# add_executable(runner
#     ${SCORE_ROOT_SRC_DIR}/main.cpp
# )
# add_dependencies(runner ${ALL_SCORE_LIBS})
# target_link_libraries(runner
#     ${ALL_SCORE_LIBS}
#     ${COMMON_LIBS}
#     ${EXTRA_LIBS}
# )




# add_executable(unit_test_dummy ${SCORE_ROOT_SRC_DIR}/run_dummy.cpp)
# add_dependencies(unit_test_dummy
#     run_async_tests
#     run_core_tests
#     run_curl_tests
#     run_extract_tests
#     run_html_tests
#     run_nlp_tests
#     run_memcached_tests
#     run_bench_tests
# )

# target_link_libraries(unit_test_dummy glog)

# add_executable(integration_test_dummy ${SCORE_ROOT_SRC_DIR}/run_dummy.cpp)
# add_dependencies(integration_test_dummy
#     run_client_integration_tests
#     run_misc_integration_tests
# )
# target_link_libraries(integration_test_dummy glog)



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

