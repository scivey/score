
include_directories(
    ${ALLS_SCORE_INCLUDE_DIRS}
    ${SCORE_BASE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

link_directories(
    ${SCORE_EXTERNAL_LINK_DIRS}
)

add_executable(run_scratch
    ${SCORE_ROOT_SRC_DIR}/main.cpp
)
add_dependencies(run_scratch ${ALL_SCORE_LIBS})
target_link_libraries(run_scratch
    ${ALL_SCORE_LIBS}
    ${SCORE_ALL_EXTERNAL_LIBS}
)

