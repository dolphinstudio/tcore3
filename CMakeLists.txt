#cmake file for project core
#author:T
#created:2015-05-12 15:07:04

cmake_minimum_required(VERSION 2.8)
project(tcore3)
set_property(GLOBAL PROPERTY USE_FOLDERS On)

IF (WIN32)
    MESSAGE(STATUS "Now is windows")  
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/windows/")
    SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/windows/")
ELSEIF (UNIX)
    MESSAGE(STATUS "Now is unix")
    option(USE_32BITS "Compile for 32bits processors(linux only)" OFF)
    SET(CMAKE_CXX_FLAGS "-g -fuse-ld=gold -std=c++11 -Wall -lpthread -rdynamic -ldl -Dlinux")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -fuse-ld=gold -std=c++11 -lpthread -rdynamic -ldl -Dlinux")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBUGINFO "-g -o2 -fuse-ld=gold -std=c++11 -lpthread -rdynamic -ldl -Dlinux")
    SET(CMAKE_CXX_FLAGS_RELEASE "-g -o3 -fuse-ld=gold -std=c++11 -lpthread -rdynamic -ldl -Dlinux")
    SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/linux/")
    SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/linux/")

    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        add_definitions(
            -D_DEBUG
        )
    endif ()
ENDIF ()  

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}/api"
    "${PROJECT_SOURCE_DIR}/public"
)

SET(api
    "${PROJECT_SOURCE_DIR}/api/api.h"
)

SET(tinyxml
    "${PROJECT_SOURCE_DIR}/public/tinyxml/tinystr.h"
    "${PROJECT_SOURCE_DIR}/public/tinyxml/tinystr.cpp"
    "${PROJECT_SOURCE_DIR}/public/tinyxml/tinyxml.h"
    "${PROJECT_SOURCE_DIR}/public/tinyxml/tinyxml.cpp"
    "${PROJECT_SOURCE_DIR}/public/tinyxml/tinyxmlerror.cpp"
    "${PROJECT_SOURCE_DIR}/public/tinyxml/tinyxmlparser.cpp"
)

SET(json
    "${PROJECT_SOURCE_DIR}/public/json/autolink.h"
    "${PROJECT_SOURCE_DIR}/public/json/config.h"
    "${PROJECT_SOURCE_DIR}/public/json/features.h"
    "${PROJECT_SOURCE_DIR}/public/json/forwards.h"
    "${PROJECT_SOURCE_DIR}/public/json/json.h"
    "${PROJECT_SOURCE_DIR}/public/json/json_batchallocator.h"
    "${PROJECT_SOURCE_DIR}/public/json/reader.h"
    "${PROJECT_SOURCE_DIR}/public/json/value.h"
    "${PROJECT_SOURCE_DIR}/public/json/writer.h"
    "${PROJECT_SOURCE_DIR}/public/json/json_reader.cpp"
    "${PROJECT_SOURCE_DIR}/public/json/json_value.cpp"
    "${PROJECT_SOURCE_DIR}/public/json/json_writer.cpp"
)

SET(zlib
    "${PROJECT_SOURCE_DIR}/public/zlib/zlib.h"
    "${PROJECT_SOURCE_DIR}/public/zlib/zconf.h"
)

IF (WIN32)
SET(public
    "${PROJECT_SOURCE_DIR}/public/cfile.h"
    "${PROJECT_SOURCE_DIR}/public/base64.h"
    "${PROJECT_SOURCE_DIR}/public/base64.cpp"
    "${PROJECT_SOURCE_DIR}/public/autolock.h"
    "${PROJECT_SOURCE_DIR}/public/dumper.h"
    "${PROJECT_SOURCE_DIR}/public/dumper.cpp"
    "${PROJECT_SOURCE_DIR}/public/cthread.h"
    "${PROJECT_SOURCE_DIR}/public/cthread.cpp"
    "${PROJECT_SOURCE_DIR}/public/multisys.h"
    "${PROJECT_SOURCE_DIR}/public/multisys.cpp"
    "${PROJECT_SOURCE_DIR}/public/ilock.h"
    "${PROJECT_SOURCE_DIR}/public/spinlock.h"
    "${PROJECT_SOURCE_DIR}/public/tqcsort.h"
    "${PROJECT_SOURCE_DIR}/public/tools.h"
    "${PROJECT_SOURCE_DIR}/public/tools-win.cpp"
    "${PROJECT_SOURCE_DIR}/public/tpool.h"
    "${PROJECT_SOURCE_DIR}/public/tqueue.h"
    "${PROJECT_SOURCE_DIR}/public/uuid.h"
    "${PROJECT_SOURCE_DIR}/public/cbuffer.h"
    "${PROJECT_SOURCE_DIR}/public/md5.h"
    "${PROJECT_SOURCE_DIR}/public/md5.cpp"
    "${PROJECT_SOURCE_DIR}/public/sha1.h"
    "${PROJECT_SOURCE_DIR}/public/sha1.cpp"
    "${PROJECT_SOURCE_DIR}/public/blockallocator.h"
    "${PROJECT_SOURCE_DIR}/public/blockallocator.cpp"
)
ELSEIF (APPLE)
ELSEIF (UNIX)
SET(public
    "${PROJECT_SOURCE_DIR}/public/cfile.h"
    "${PROJECT_SOURCE_DIR}/public/base64.h"
    "${PROJECT_SOURCE_DIR}/public/base64.cpp"
    "${PROJECT_SOURCE_DIR}/public/autolock.h"
    "${PROJECT_SOURCE_DIR}/public/cthread.h"
    "${PROJECT_SOURCE_DIR}/public/cthread.cpp"
    "${PROJECT_SOURCE_DIR}/public/multisys.h"
    "${PROJECT_SOURCE_DIR}/public/multisys.cpp"
    "${PROJECT_SOURCE_DIR}/public/ilock.h"
    "${PROJECT_SOURCE_DIR}/public/spinlock.h"
    "${PROJECT_SOURCE_DIR}/public/tqcsort.h"
    "${PROJECT_SOURCE_DIR}/public/tools.h"
    "${PROJECT_SOURCE_DIR}/public/tools-linux.cpp"
    "${PROJECT_SOURCE_DIR}/public/tpool.h"
    "${PROJECT_SOURCE_DIR}/public/tqueue.h"
    "${PROJECT_SOURCE_DIR}/public/uuid.h"
    "${PROJECT_SOURCE_DIR}/public/cbuffer.h"
    "${PROJECT_SOURCE_DIR}/public/md5.h"
    "${PROJECT_SOURCE_DIR}/public/md5.cpp"
    "${PROJECT_SOURCE_DIR}/public/sha1.h"
    "${PROJECT_SOURCE_DIR}/public/sha1.cpp"
    "${PROJECT_SOURCE_DIR}/public/blockallocator.h"
    "${PROJECT_SOURCE_DIR}/public/blockallocator.cpp"
)
ENDIF ()

IF(WIN32)
    SET(libcurl
        debug "${PROJECT_SOURCE_DIR}/library/debug/libcurl.lib"
        optimized "${PROJECT_SOURCE_DIR}/library/release/libcurl.lib"
    )
ELSEIF(UNIX)
    SET(libcurl
        debug "/usr/lib64/libcurl.so"
        optimized "/usr/lib64/libcurl.so"
    )
ENDIF()

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/tools)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/tcore3)

