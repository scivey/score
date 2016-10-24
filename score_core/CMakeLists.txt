set(SCORE_CORE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SMHASHER_SOURCES
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/Platform.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/sha1.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/md5.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/Types.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/Bitvec.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/Bitslice.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/Random.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/City.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/smhasher/MurmurHash3.cpp
)

set(VENDORED_CHARSET_ROOT ${SCORE_CORE_SRC_DIR}/vendored/lgpl/charsetdetect)
FILE(GLOB CHARSETDETECT_SOURCES ${VENDORED_CHARSET_ROOT}/base/*.cpp)
set(CHARSETDETECT_SOURCES
    ${CHARSETDETECT_SOURCES} ${VENDORED_CHARSET_ROOT}/charsetdetect.cpp
)

FILE(GLOB VENDORED_FOLLY_SOURCES
    ${SCORE_CORE_SRC_DIR}/vendored/folly/*.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/folly/**/*.cpp
)

set(SCORE_CORE_VENDORED_SRC
    ${SMHASHER_SOURCES}
    ${CHARSETDETECT_SOURCES}
    ${VENDORED_FOLLY_SOURCES}
    ${SCORE_CORE_SRC_DIR}/vendored/http-parser/http_parser.c
    ${SCORE_CORE_SRC_DIR}/vendored/pugixml/pugixml.cpp
    ${SCORE_CORE_SRC_DIR}/vendored/modp_b64/modp_b64.cc
)

FILE(GLOB SCORE_COMMON_SRC
    ${SCORE_CORE_SRC_DIR}/*.cpp
    ${SCORE_CORE_SRC_DIR}/curl/**/*.cpp
    ${SCORE_CORE_SRC_DIR}/curl/*.cpp
    ${SCORE_CORE_SRC_DIR}/url/*.cpp
    ${SCORE_CORE_SRC_DIR}/url/**/*.cpp
    ${SCORE_CORE_SRC_DIR}/encodings/*.cpp
    ${SCORE_CORE_SRC_DIR}/html/*.cpp
    ${SCORE_CORE_SRC_DIR}/fs/*.cpp
    ${SCORE_CORE_SRC_DIR}/io/*.cpp
    ${SCORE_CORE_SRC_DIR}/locks/*.cpp
    ${SCORE_CORE_SRC_DIR}/mem/*.cpp
    ${SCORE_CORE_SRC_DIR}/posix/*.cpp
    ${SCORE_CORE_SRC_DIR}/re/*.cpp
    ${SCORE_CORE_SRC_DIR}/reactor/*.cpp
    ${SCORE_CORE_SRC_DIR}/unicode/*.cpp
    ${SCORE_CORE_SRC_DIR}/net/*.cpp
    ${SCORE_CORE_SRC_DIR}/hashing/*.cpp
    ${SCORE_CORE_SRC_DIR}/exceptions/*.cpp
)

set(SCORE_COMMON_SRC
    ${SCORE_COMMON_SRC}
    ${SCORE_CORE_VENDORED_SRC}
)

add_library(score_core ${SCORE_COMMON_SRC})
target_link_libraries(score_core ${SCORE_COMMON_EXTERNAL_LIBS})



FILE(GLOB CORE_UNIT_TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_unit/**/*.cpp
)
add_executable(run_core_unit_tests
    ${CORE_UNIT_TEST_SRC}
    ${SCORE_ROOT_SRC_DIR}/run_tests.cpp
)
add_dependencies(run_core_unit_tests score_core)
target_link_libraries(run_core_unit_tests
    gmock
    score_core
    ${SCORE_COMMON_EXTERNAL_LIBS}
)

