cmake_minimum_required(VERSION 2.8)

project(asioz)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall")

set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(GTEST ${EXTERNAL}/googletest)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTEST}/googletest/include
    ${GTEST}/googlemock/include
    /usr/local/include
)

link_directories(
    /usr/local/lib
    ${GTEST}/build/googlemock
)


set(COMMON_LIBS
    folly
    wangle
    boost_system
    boost_thread
    glog
    pthread
    atomic
)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIB_SRC
    ${SRC}/ScopeGuard.cpp
    ${SRC}/FixedBuffer.cpp

)

add_library(asiozcommon ${LIB_SRC})
# set(ALL_LIBS ${COMMON_LIBS})
set(ALL_LIBS asiozcommon ${COMMON_LIBS})



add_executable(runner
    ${SRC}/main.cpp
)
add_dependencies(runner asiozcommon)
target_link_libraries(runner
    ${ALL_LIBS}
)

add_executable(bencher
    ${SRC}/bench/main.cpp
)
target_link_libraries(bencher
    benchmark
    ${ALL_LIBS}
)

set(TEST_DIR ${SRC}/test)
set(TEST_SRC
    ${TEST_DIR}/test_nothing.cpp
    ${TEST_DIR}/test_ScopeGuard.cpp
    ${TEST_DIR}/test_ThreadLocalPtr.cpp
    ${TEST_DIR}/test_SingletonWrapper.cpp
    ${TEST_DIR}/test_casts.cpp
)

add_executable(tester
    ${TEST_SRC}
    ${TEST_DIR}/run_tests.cpp
)
add_dependencies(tester asiozcommon)
target_link_libraries(tester
    gmock
    ${ALL_LIBS}
)
