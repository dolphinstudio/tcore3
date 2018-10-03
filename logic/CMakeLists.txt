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
    "${PROJECT_SOURCE_DIR}/logic/interface/attribute.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/auto_config.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iAi.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iBuff.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iCards.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iCombiner.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iConfiger.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iEffect.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iEvent.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iGame.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/IGate.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iIdmanager.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iLogicer.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iMaster.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iMatch.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iModel.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iNoder.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iOrbit.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iPublic.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iRedis.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iReference.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iRobit.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iRole.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iScene.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iScore.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iSession.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iSkill.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iStorage.h"
    "${PROJECT_SOURCE_DIR}/logic/interface/iWebSocket.h"
)

set(nodeproto
    "${PROJECT_SOURCE_DIR}/logic/proto/noder.pb.h"
    "${PROJECT_SOURCE_DIR}/logic/proto/noder.pb.cc"
)

set(proto
    "${PROJECT_SOURCE_DIR}/logic/proto/data.pb.h"
    "${PROJECT_SOURCE_DIR}/logic/proto/data.pb.cc"
    "${PROJECT_SOURCE_DIR}/logic/proto/game.pb.h"
    "${PROJECT_SOURCE_DIR}/logic/proto/game.pb.cc"
)

IF(WIN32)
    SET(libprotobuf
        debug "${PROJECT_SOURCE_DIR}/library/debug/libprotobuf.lib"
        optimized "${PROJECT_SOURCE_DIR}/library/release/libprotobuf.lib"
    )
    
    SET(libredis  
        debug "${PROJECT_SOURCE_DIR}/library/debug/hiredis.lib"
        debug "${PROJECT_SOURCE_DIR}/library/debug/Win32_Interop.lib"
        optimized "${PROJECT_SOURCE_DIR}/library/release/hiredis.lib"
        optimized "${PROJECT_SOURCE_DIR}/library/release/Win32_Interop.lib"
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
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/websocket)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/redis)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/configer)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/ai)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/buff)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/cards)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/effect)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/game)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/gate)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/logicer)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/match)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/relation)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/scene)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/skill)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/storage)


