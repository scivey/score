cmake_minimum_required(VERSION 2.8)

project(aliens)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -fPIC")
set(EXTERNAL ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(GT_PATH ${EXTERNAL}/googletest)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${EXTERNAL}/asmjit/src
    ${GT_PATH}/googletest/include
    ${GT_PATH}/googlemock/include
    /usr/local/include
    ${CMAKE_CURRENT_SOURCE_DIR}/.env/include/python3.5m
)

link_directories(
    ${GT_PATH}/build/googlemock
    ${EXTERNAL}/asmjit/build
    /usr/local/lib
)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIB_SRC
    ${SRC}/WorkerChannel.cpp
    ${SRC}/exceptions.cpp

    ${SRC}/engine/Interpreter.cpp
    ${SRC}/engine/Func.cpp
    ${SRC}/engine/FilterFunc.cpp
    ${SRC}/runtime/AttrExperiment.cpp
    ${SRC}/WorkerCore.cpp
    ${SRC}/WorkerGroup.cpp
    ${SRC}/WorkerGroupHandle.cpp
    ${SRC}/WorkerMessageData.cpp

    ${SRC}/WorkerRequest.cpp
    ${SRC}/WorkerResponse.cpp
    ${SRC}/py_helpers.cpp
)

set(ASM)

set(COMMON_LIBS
    folly
    wangle
    boost_system
    boost_thread
    glog
    pthread
    atomic
)

add_library(alienstuff
    ${LIB_SRC}
)

add_library(dyntest SHARED
    ${SRC}/dyntest/dyntest.c
)
# target_link_libraries(alienstuff, ${COMMON_LIBS})

add_executable(runner
    ${SRC}/main.cpp
)

# add_dependencies(runner alienstuff)
target_link_libraries(runner asmjit ${COMMON_LIBS})

# set(TEST_SRC
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/testing/run_tests.cpp
# )

# add_executable(run_tests ${TEST_SRC})
# target_link_libraries(run_tests
#     gmock
#     ${COMMON_LIBS}
# )
