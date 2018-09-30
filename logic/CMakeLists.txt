#cmake file for project core
#author:T
#created:2015-05-12 15:07:04

include_directories(
    "${PROJECT_SOURCE_DIR}/logic/common"
    "${PROJECT_SOURCE_DIR}/logic/proto"
    "${PROJECT_SOURCE_DIR}/logic/interface"
)

set(common

)

set(interface
	${interface}
    "${PROJECT_SOURCE_DIR}/logic/interface/iIdmanager.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iEvent.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iModel.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iMaster.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iNoder.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iSession.h"
)

set(nodeproto
    "${PROJECT_SOURCE_DIR}/logic/proto/Noder.pb.h"
    "${PROJECT_SOURCE_DIR}/logic/proto/Noder.pb.cc"
)

set(proto
    "${PROJECT_SOURCE_DIR}/logic/proto/Data.pb.h"
    "${PROJECT_SOURCE_DIR}/logic/proto/Data.pb.cc"
    "${PROJECT_SOURCE_DIR}/logic/proto/Game.pb.h"
    "${PROJECT_SOURCE_DIR}/logic/proto/Game.pb.cc"
)

IF(WIN32)
    SET(libprotobuf
        debug "${PROJECT_SOURCE_DIR}/library/debug/libprotobuf.lib"
        optimized "${PROJECT_SOURCE_DIR}/library/release/libprotobuf.lib"
    )

ELSEIF(UNIX)
    SET(libprotobuf
        debug "/usr/local/lib64/libprotobuf.a"
        optimized "/usr/local/lib64/libprotobuf.a"
    )
    
    SET(libredis    
        debug "${PROJECT_SOURCE_DIR}/library/libhiredis.a"
        optimized "${PROJECT_SOURCE_DIR}/library/libhiredis.a"
    )
    
    SET(liblua
        debug "${PROJECT_SOURCE_DIR}/library/libluabind.a"
        debug "${PROJECT_SOURCE_DIR}/library/liblua.a"
        optimized "${PROJECT_SOURCE_DIR}/library/libluabind.a"
        optimized "${PROJECT_SOURCE_DIR}/library/liblua.a"
    )
ENDIF()

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/master)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/noder)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/idmanager)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/event)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/model)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/tcptester)
##ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/udptester)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/kcper)
