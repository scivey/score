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

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIB_SRC
    ${SRC}/TCPServer.cpp
    ${SRC}/TCPClient.cpp
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

# add_library(asiozcommon ${LIB_SRC})
# set(ALL_LIBS asiozcommon ${COMMON_LIBS})
set(ALL_LIBS ${COMMON_LIBS})



add_executable(runner
    ${SRC}/main.cpp
)
# add_dependencies(runner asiozcommon)
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
)

add_executable(tester
    ${TEST_SRC}
    ${TEST_DIR}/run_tests.cpp
)
target_link_libraries(tester
    gmock
    ${ALL_LIBS}
)
