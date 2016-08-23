cmake_minimum_required(VERSION 2.8)

project(asioz)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall")


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    /usr/local/include
)

link_directories(
    /usr/local/lib
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


add_executable(runner
    ${SRC}/main.cpp
)


# set(ALL_LIBS asiozcommon ${COMMON_LIBS})
set(ALL_LIBS ${COMMON_LIBS})
# add_dependencies(runner asiozcommon)

target_link_libraries(runner
    ${ALL_LIBS}
)

