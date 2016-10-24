
set(SCORE_REDIS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

FILE(GLOB SCORE_REDIS_SRC
    ${SCORE_REDIS_SRC_DIR}/*.cpp
    ${SCORE_REDIS_SRC_DIR}/hiredis_adapter/*.cpp
)

add_library(score_redis ${SCORE_REDIS_SRC})
add_dependencies(score_redis score_async score_core)
target_link_libraries(score_redis
    score_async
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
    ${SCORE_EXTRA_EXTERNAL_LIBS}
)