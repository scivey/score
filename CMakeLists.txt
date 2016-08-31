cmake_minimum_required(VERSION 2.8)

project(aliens)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -g -O0 -mcx16 -march=native")

set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(BORINGSSL ${EXTERNAL}/boringssl)
set(GTEST ${EXTERNAL}/googletest)
set(SPDLOG ${EXTERNAL}/spdlog)

include_directories(
    ${BORINGSSL}/include
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
    boost_system
    boost_thread
    glog
    ${CRYPTO_LIBS}
    pthread
    gcc_s
    atomic
)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(LIB_SRC
    ${SRC}/ScopeGuard.cpp
    ${SRC}/FixedBuffer.cpp
    ${SRC}/mem/standard.cpp
    ${SRC}/io/UTF8Iterator.cpp
    ${SRC}/locks/ThreadBaton.cpp
    ${SRC}/exceptions/exceptions.cpp
    ${SRC}/formatters/formatters.cpp
    ${SRC}/posix/FileDescriptor.cpp
    ${SRC}/net/SocketAddr.cpp
    ${SRC}/reactor/EpollFd.cpp
    ${SRC}/reactor/EpollReactor.cpp
    ${SRC}/reactor/TimerFd.cpp
    ${SRC}/reactor/TimerSettings.cpp
    ${SRC}/reactor/EventFd.cpp
    ${SRC}/reactor/SignalFd.cpp
    ${SRC}/net/TCPChannel.cpp
    ${SRC}/net/TCPClient.cpp
    ${SRC}/net/TCPAcceptSocket.cpp
    ${SRC}/reactor/ReactorThread.cpp

    ${SRC}/vendored/smhasher/Platform.cpp
    ${SRC}/vendored/smhasher/sha1.cpp
    ${SRC}/vendored/smhasher/md5.cpp
    ${SRC}/vendored/smhasher/Types.cpp
    ${SRC}/vendored/smhasher/Bitvec.cpp
    ${SRC}/vendored/smhasher/Bitslice.cpp
    ${SRC}/vendored/smhasher/Random.cpp
    ${SRC}/vendored/http-parser/http_parser.c

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
    # ${TEST_DIR}/reactor/test_EpollReactor.cpp
    # ${TEST_DIR}/reactor/test_ReactorThread.cpp
    ${TEST_DIR}/reactor/test_tcp_integration.cpp
    ${TEST_DIR}/reactor/test_TimerFd.cpp
    ${TEST_DIR}/reactor/test_EventFd.cpp
    ${TEST_DIR}/reactor/test_SignalFd.cpp
    ${TEST_DIR}/reactor/test_experimental.cpp
    ${TEST_DIR}/mem/test_LSharedPtr.cpp
    ${TEST_DIR}/io/test_string_utils.cpp
    ${TEST_DIR}/io/test_BytesView.cpp
    ${TEST_DIR}/io/test_UTF8View.cpp

    ${TEST_DIR}/http/test_SimpleHTTPResponseParser.cpp
    ${TEST_DIR}/atomic/test_IntrinsicAtomic.cpp
    ${TEST_DIR}/vendored/test_seastar_imports.cpp
    ${TEST_DIR}/vendored/test_utf8.cpp
    # ${TEST_DIR}/vendored/test_folly_imports.cpp
    ${TEST_DIR}/test_MoveWrapper.cpp
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
