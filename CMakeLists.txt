cmake_minimum_required(VERSION 2.8)

project(score)

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
    curl
    event
    ${CRYPTO_LIBS}
    pthread
    gcc_s
    atomic
)

set(SCORE_ROOT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SCORE_CORE_SRC ${SCORE_ROOT_SRC}/score)

set(GUMBO_PARSER_SOURCES
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/attribute.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/char_ref.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/error.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/parser.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/string_buffer.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/string_piece.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/tag.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/tokenizer.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/utf8.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/util.c
    ${SCORE_CORE_SRC}/vendored/gumbo-parser/vector.c
)

set(SMHASHER_SOURCES
    ${SCORE_CORE_SRC}/vendored/smhasher/Platform.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/sha1.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/md5.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/Types.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/Bitvec.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/Bitslice.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/Random.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/City.cpp
    ${SCORE_CORE_SRC}/vendored/smhasher/MurmurHash3.cpp
)


set(VENDORED_CHARSET_ROOT ${SCORE_CORE_SRC}/vendored/lgpl/charsetdetect)
FILE(GLOB CHARSETDETECT_SOURCES ${VENDORED_CHARSET_ROOT}/base/*.cpp)
set(CHARSETDETECT_SOURCES
    ${CHARSETDETECT_SOURCES} ${VENDORED_CHARSET_ROOT}/charsetdetect.cpp
)



set(SCORE_VENDORED_SRC
    ${SMHASHER_SOURCES}
    ${GUMBO_PARSER_SOURCES}
    ${CHARSETDETECT_SOURCES}
    ${SCORE_CORE_SRC}/vendored/http-parser/http_parser.c
    ${SCORE_CORE_SRC}/vendored/pugixml/pugixml.cpp
)


set(SCORE_HTML_SRC
    ${SCORE_CORE_SRC}/html/GumboOutputWrapper.cpp
    ${SCORE_CORE_SRC}/html/GumboVectorWrapper.cpp
    ${SCORE_CORE_SRC}/html/HtmlDom.cpp
    ${SCORE_CORE_SRC}/html/MetaTags.cpp
    ${SCORE_CORE_SRC}/html/Node.cpp
    ${SCORE_CORE_SRC}/html/Tag.cpp
)


set(SCORE_ENCODING_SRC
    ${SCORE_CORE_SRC}/encodings/Encoding.cpp
    ${SCORE_CORE_SRC}/encodings/detection.cpp
    ${SCORE_CORE_SRC}/encodings/LibCharsetDetectorHandle.cpp
    ${SCORE_CORE_SRC}/encodings/EncodingNormalizer.cpp
    ${SCORE_CORE_SRC}/encodings/IConvHandle.cpp
    ${SCORE_CORE_SRC}/encodings/IConvException.cpp
    ${SCORE_CORE_SRC}/encodings/IConverter.cpp
)

set(SCORE_CURL_SRC
    ${SCORE_CORE_SRC}/curl/CurlError.cpp
    ${SCORE_CORE_SRC}/curl/detail/RealCurlAPI.cpp
)

set(SCORE_COMMON_SRC
    ${SCORE_CORE_SRC}/ScopeGuard.cpp
    ${SCORE_CORE_SRC}/FixedBuffer.cpp
    ${SCORE_CORE_SRC}/mem/standard.cpp
    ${SCORE_CORE_SRC}/io/UTF8Iterator.cpp
    ${SCORE_CORE_SRC}/io/string_utils.cpp
    ${SCORE_CORE_SRC}/fs/fs.cpp
    ${SCORE_CORE_SRC}/locks/ThreadBaton.cpp
    ${SCORE_CORE_SRC}/exceptions/exceptions.cpp
    ${SCORE_CORE_SRC}/formatters/formatters.cpp
    ${SCORE_CORE_SRC}/posix/FileDescriptor.cpp
    ${SCORE_CORE_SRC}/net/SocketAddr.cpp
    ${SCORE_CORE_SRC}/reactor/EpollFd.cpp
    ${SCORE_CORE_SRC}/reactor/EpollReactor.cpp
    ${SCORE_CORE_SRC}/reactor/TimerFd.cpp
    ${SCORE_CORE_SRC}/reactor/TimerSettings.cpp
    ${SCORE_CORE_SRC}/reactor/EventFd.cpp
    ${SCORE_CORE_SRC}/reactor/SignalFd.cpp
    ${SCORE_CORE_SRC}/hashing/hash_funcs.cpp
    ${SCORE_CORE_SRC}/net/TCPChannel.cpp
    ${SCORE_CORE_SRC}/net/TCPClient.cpp
    ${SCORE_CORE_SRC}/net/TCPAcceptSocket.cpp
    ${SCORE_CORE_SRC}/reactor/ReactorThread.cpp
    ${SCORE_HTML_SRC}
    ${SCORE_CURL_SRC}
    ${SCORE_VENDORED_SRC}
    ${SCORE_ENCODING_SRC}
)

add_library(score_core ${SCORE_COMMON_SRC})
set(ALL_LIBS score_core ${COMMON_LIBS})


add_executable(runner
    ${SCORE_ROOT_SRC}/main.cpp
)
add_dependencies(runner score_core)
target_link_libraries(runner
    ${ALL_LIBS}
)


set(CORE_TEST_DIR ${SCORE_CORE_SRC}/test)

set(TEST_SRC
    ${CORE_TEST_DIR}/test_ScopeGuard.cpp
    ${CORE_TEST_DIR}/test_ThreadLocalPtr.cpp
    ${CORE_TEST_DIR}/test_SingletonWrapper.cpp
    ${CORE_TEST_DIR}/test_NullablePointer.cpp
    ${CORE_TEST_DIR}/test_Maybe.cpp
    ${CORE_TEST_DIR}/test_MoveWrapper.cpp
    ${CORE_TEST_DIR}/test_DefaultValueMap.cpp
    ${CORE_TEST_DIR}/locks/test_ThreadBaton.cpp
    ${CORE_TEST_DIR}/reactor/test_tcp_integration.cpp
    ${CORE_TEST_DIR}/reactor/test_TimerFd.cpp
    ${CORE_TEST_DIR}/reactor/test_EventFd.cpp
    ${CORE_TEST_DIR}/reactor/test_SignalFd.cpp
    ${CORE_TEST_DIR}/mem/test_LSharedPtr.cpp
    ${CORE_TEST_DIR}/io/test_string_utils.cpp
    ${CORE_TEST_DIR}/io/test_BytesView.cpp
    ${CORE_TEST_DIR}/io/test_UTF8View.cpp
    ${CORE_TEST_DIR}/html/test_MetaTags.cpp
    ${CORE_TEST_DIR}/html/test_HtmlDomAndNode.cpp
    ${CORE_TEST_DIR}/http/test_SimpleHTTPResponseParser.cpp
    ${CORE_TEST_DIR}/atomic/test_IntrinsicAtomic.cpp
    ${CORE_TEST_DIR}/encodings/test_IConverter.cpp
    ${CORE_TEST_DIR}/fs/test_fs.cpp
    ${CORE_TEST_DIR}/prettyprint/test_prettyprint.cpp
    ${CORE_TEST_DIR}/curl/test_CurlEasyHandle.cpp
    ${CORE_TEST_DIR}/vendored/test_seastar_imports.cpp
    ${CORE_TEST_DIR}/vendored/test_utf8.cpp
    ${CORE_TEST_DIR}/vendored/test_smhasher_imports.cpp

)

add_executable(run_core_tests
    ${TEST_SRC}
    ${CORE_TEST_DIR}/run_tests.cpp
)

add_dependencies(run_core_tests score_core)
target_link_libraries(run_core_tests
    gmock
    ${ALL_LIBS}
)






set(SCORE_NLP_SRC_DIR ${SCORE_ROOT_SRC}/score_nlp)

set(VENDORED_LIBSTEMMER_ROOT ${SCORE_NLP_SRC_DIR}/vendored/libstemmer)
FILE(GLOB LIBSTEMMER_SOURCES ${VENDORED_LIBSTEMMER_ROOT}/**/*.c)
set(LIBSTEMMER_SOURCES ${LIBSTEMMER_SOURCES} ${VENDORED_LIBSTEMMER_ROOT}/libstemmer_utf8.c)

set(VENDORED_CLD2_ROOT ${SCORE_NLP_SRC_DIR}/vendored/cld2)
FILE(GLOB CLD2_SOURCES ${VENDORED_CLD2_ROOT}/**/*.cc)

set(SCORE_NLP_SRC
    ${LIBSTEMMER_SOURCES}
    ${CLD2_SOURCES}
    ${SCORE_NLP_SRC_DIR}/stemming/SbStemmerWrapper.cpp
    ${SCORE_NLP_SRC_DIR}/stemming/StemmerManager.cpp
    ${SCORE_NLP_SRC_DIR}/stemming/Utf8Stemmer.cpp
    ${SCORE_NLP_SRC_DIR}/Language.cpp
)

add_library(score_nlp ${SCORE_NLP_SRC})
add_dependencies(score_nlp score_core)
target_link_libraries(score_nlp ${COMMON_LIBS} score_core)


set(SCORE_NLP_TEST_SRC
    ${SCORE_NLP_SRC_DIR}/test/stemming/test_Utf8Stemmer.cpp
    ${SCORE_NLP_SRC_DIR}/test/test_Language.cpp
    ${SCORE_NLP_SRC_DIR}/test/run_tests.cpp
)

add_executable(run_nlp_tests
    ${SCORE_NLP_TEST_SRC}
)
add_dependencies(run_nlp_tests score_nlp)
target_link_libraries(run_nlp_tests gmock ${COMMON_LIBS} score_core score_nlp)
