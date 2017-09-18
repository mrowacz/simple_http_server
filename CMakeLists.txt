cmake_minimum_required(VERSION 3.5.1)

option(test "Build all tests." ON) # Makes boolean 'test' available.

project(wp_interview)
include(ExternalProject)

# project versioning
set (Server_VERSION_MAJOR 0)
set (Server_VERSION_MINOR 9)

# get git HEAD sha1
exec_program(
        "git"
        ${CMAKE_CURRENT_SOURCE_DIR}
        ARGS "rev-parse HEAD"
        OUTPUT_VARIABLE VERSION_SHA1
)

add_definitions( -DGIT_SHA1="${VERSION_SHA1}" )

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
        "${PROJECT_SOURCE_DIR}/src/Config.h.in"
        "${PROJECT_BINARY_DIR}/Config.h"
)

# set compiler settings
SET(CMAKE_CXX_FLAGS_DEBUG "-O2 -g")
SET(CMAKE_C_FLAGS_DEBUG "-O2")

# external libraries
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
include_directories(
        ${EXTERNAL_INSTALL_LOCATION}/include
        ${PROJECT_BINARY_DIR}
)

# add boost
find_package(Boost COMPONENTS log_setup log system filesystem program_options REQUIRED)
add_definitions(-DBOOST_LOG_DYN_LINK)

# add google test
ExternalProject_Add(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
    UPDATE_DISCONNECTED 1
)

# add requests c++ library
ExternalProject_Add(request
        GIT_REPOSITORY https://github.com/mrtazz/restclient-cpp.git
        #-- Configure step --
        CONFIGURE_COMMAND ./autogen.sh && ./configure --prefix=${EXTERNAL_INSTALL_LOCATION}
        #-- Build Step --
        BUILD_COMMAND make
        BUILD_IN_SOURCE 1
        UPDATE_DISCONNECTED 1
        INSTALL_COMMAND make install
)

# add sql library
ExternalProject_Add(sql
        GIT_REPOSITORY https://github.com/aminroosta/sqlite_modern_cpp.git
        CONFIGURE_COMMAND ./configure --prefix=${EXTERNAL_INSTALL_LOCATION}
        BUILD_COMMAND make
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND make install
        UPDATE_DISCONNECTED 1
)

# add libuv - cross-platform asynchronous I/O
ExternalProject_Add(libuv
    GIT_REPOSITORY https://github.com/libuv/libuv.git
    #-- Configure step --
    CONFIGURE_COMMAND ./autogen.sh && ./configure --prefix=${EXTERNAL_INSTALL_LOCATION}
    #-- Build Step --
    BUILD_COMMAND make
    BUILD_IN_SOURCE 1
    UPDATE_DISCONNECTED 1
    INSTALL_COMMAND make install
)

# add ansi c http parsing library only for porting purposes
ExternalProject_Add(http_parser_git
    GIT_REPOSITORY https://github.com/mrowacz/http-parser.git
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    BUILD_IN_SOURCE 1
    UPDATE_DISCONNECTED 1
    INSTALL_COMMAND make install PREFIX=${EXTERNAL_INSTALL_LOCATION}
)

# lightweight uri c++ parser
ExternalProject_Add(nodeuv-uri
    GIT_REPOSITORY https://github.com/hij1nx/nodeuv-uri
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    BUILD_IN_SOURCE 1
    UPDATE_DISCONNECTED 1
    INSTALL_COMMAND mkdir -p ${EXTERNAL_INSTALL_LOCATION}/include && cp uri.h ${EXTERNAL_INSTALL_LOCATION}/include
)

set(CMAKE_CXX_STANDARD 14)
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
set(TESTING_SOURCES ${SOURCE_FILES})
list(REMOVE_ITEM TESTING_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/Server.cpp)

link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
add_executable(http_server ${SOURCE_FILES})
target_include_directories(http_server PUBLIC
        ${EXTERNAL_INSTALL_LOCATION}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
target_link_libraries(http_server
        http_parser
        sqlite3
        pthread
        ${Boost_LIBRARIES}
        uv
        )

set(PROJECT_DEPENDENCIES
        libuv
        http_parser_git
        nodeuv-uri
        sql
)
add_dependencies(http_server ${PROJECT_DEPENDENCIES})

if (test)
    enable_testing()

    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    file(GLOB_RECURSE TEST_SOURCE_FILES
            ${PROJECT_SOURCE_DIR}/test/*.cpp
    )

    add_executable(runTests
            ${TEST_SOURCE_FILES}
            ${TESTING_SOURCES}
    )
    target_include_directories(runTests PUBLIC ${CMAKE_SOURCE_DIR}/src)

    add_dependencies(runTests
        ${PROJECT_DEPENDENCIES}
        googletest
        request
    )
    target_link_libraries(runTests
            gtest
            gtest_main
            uv
            http_parser
            sqlite3
            pthread
            ${Boost_LIBRARIES}
            restclient-cpp
            pthread
    )
endif()
