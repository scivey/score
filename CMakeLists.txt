cmake_minimum_required(VERSION 2.8)

project(aliens)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -g -O0")

set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(GTEST ${EXTERNAL}/googletest)
set(SPDLOG ${EXTERNAL}/spdlog)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTEST}/googletest/include
    ${GTEST}/googlemock/include
    ${SPDLOG}/include
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
    ${SRC}/mem/standard.cpp
    ${SRC}/locks/ThreadBaton.cpp
    ${SRC}/exceptions/exceptions.cpp
    ${SRC}/reactor/FileDescriptor.cpp
    ${SRC}/reactor/EpollFd.cpp
    ${SRC}/reactor/EpollReactor.cpp
    ${SRC}/reactor/SocketAddr.cpp
    ${SRC}/reactor/AcceptSocketTask.cpp
    ${SRC}/reactor/ServerSocketTask.cpp
    ${SRC}/reactor/ClientSocketTask.cpp
    ${SRC}/reactor/TimerFd.cpp
    ${SRC}/reactor/TimerSettings.cpp
    ${SRC}/reactor/EventFd.cpp
    ${SRC}/reactor/SignalFd.cpp
    ${SRC}/reactor/TCPSocket.cpp
    ${SRC}/reactor/ReactorThread.cpp
)

add_library(alienscommon ${LIB_SRC})
# set(ALL_LIBS ${COMMON_LIBS})
set(ALL_LIBS alienscommon ${COMMON_LIBS})



add_executable(runner
    ${SRC}/main.cpp
)
add_dependencies(runner alienscommon)
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
    ${TEST_DIR}/test_NullablePointer.cpp
    ${TEST_DIR}/test_Maybe.cpp
    ${TEST_DIR}/locks/test_ThreadBaton.cpp
    # ${TEST_DIR}/async/test_ThreadExecutorCore.cpp
    ${TEST_DIR}/reactor/test_EpollReactor.cpp
    ${TEST_DIR}/reactor/test_ReactorThread.cpp
    ${TEST_DIR}/reactor/test_TCPSocket.cpp
    ${TEST_DIR}/reactor/test_TimerFd.cpp
    ${TEST_DIR}/reactor/test_EventFd.cpp
    ${TEST_DIR}/reactor/test_SignalFd.cpp
    ${TEST_DIR}/reactor/test_experimental.cpp
    # ${TEST_DIR}/test_UninitializedArray.cpp
    ${TEST_DIR}/test_casts.cpp
)

add_executable(tester
    ${TEST_SRC}
    ${TEST_DIR}/run_tests.cpp
)
add_dependencies(tester alienscommon)
target_link_libraries(tester
    gmock
    ${ALL_LIBS}
)
