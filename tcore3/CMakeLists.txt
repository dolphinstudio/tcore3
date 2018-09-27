#cmake file for project core
#author:alax
#created:2015-05-12 15:07:04

IF (WIN32)
    INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/tcore3"
        "${PROJECT_SOURCE_DIR}/tcore3/net"
        "${PROJECT_SOURCE_DIR}/tcore3/net/win"
    )

    SET(net
        "${PROJECT_SOURCE_DIR}/tcore3/net/win/header.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/win/interface.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/win/iocper.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/win/iocper.cpp"
    )

    SET(udp
        "${PROJECT_SOURCE_DIR}/tcore3/net/win/udp/udper.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/win/udp/udper.cpp"
    )

    SET(tcp
        "${PROJECT_SOURCE_DIR}/tcore3/net/win/tcp/tcper.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/win/tcp/tcper.cpp"
        "${PROJECT_SOURCE_DIR}/tcore3/net/win/tcp/accepter.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/win/tcp/accepter.cpp"
    )
ELSEIF (APPLE)
ELSEIF (UNIX)

    INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/tcore3"
        "${PROJECT_SOURCE_DIR}/tcore3/net"
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux"
    )

    SET(net
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux/header.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux/interface.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux/epoller.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux/epoller.cpp"
    )

    SET(udp
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux/udp/udper.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux/udp/udper.cpp"
    )

    SET(tcp
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux/tcp/tcper.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux/tcp/tcper.cpp"
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux/tcp/accepter.h"
        "${PROJECT_SOURCE_DIR}/tcore3/net/linux/tcp/accepter.cpp"
    )
ENDIF ()

SET(httper
    "${PROJECT_SOURCE_DIR}/tcore3/httper/httpData.h"
    "${PROJECT_SOURCE_DIR}/tcore3/httper/httper.h"
    "${PROJECT_SOURCE_DIR}/tcore3/httper/httper.cpp"
    "${PROJECT_SOURCE_DIR}/tcore3/httper/httpRequest.h"
    "${PROJECT_SOURCE_DIR}/tcore3/httper/httpRequest.cpp"
)

SET(logic
    "${PROJECT_SOURCE_DIR}/tcore3/logic/logic.h"
    "${PROJECT_SOURCE_DIR}/tcore3/logic/logic.cpp"
)

SET(logger
    "${PROJECT_SOURCE_DIR}/tcore3/logger/logfile.h"
    "${PROJECT_SOURCE_DIR}/tcore3/logger/logfile.cpp"
    "${PROJECT_SOURCE_DIR}/tcore3/logger/logger.h"
    "${PROJECT_SOURCE_DIR}/tcore3/logger/logger.cpp"
)

SET(timer
    "${PROJECT_SOURCE_DIR}/tcore3/timer/tbase.h"
    "${PROJECT_SOURCE_DIR}/tcore3/timer/tbase.cpp"
    "${PROJECT_SOURCE_DIR}/tcore3/timer/tgear.h"
    "${PROJECT_SOURCE_DIR}/tcore3/timer/tgear.cpp"
    "${PROJECT_SOURCE_DIR}/tcore3/timer/tlist.h"
    "${PROJECT_SOURCE_DIR}/tcore3/timer/tlist.cpp"
    "${PROJECT_SOURCE_DIR}/tcore3/timer/timermgr.h"
    "${PROJECT_SOURCE_DIR}/tcore3/timer/timermgr.cpp"
)

SET(main 
    "${PROJECT_SOURCE_DIR}/tcore3/instance.h"
    "${PROJECT_SOURCE_DIR}/tcore3/core.h"
    "${PROJECT_SOURCE_DIR}/tcore3/core.cpp"
    "${PROJECT_SOURCE_DIR}/tcore3/main.cpp"
)

SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(public\\zlib FILES ${zlib})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(core\\net FILES ${net})
SOURCE_GROUP(core\\net\\udp FILES ${udp})
SOURCE_GROUP(core\\net\\tcp FILES ${tcp})
SOURCE_GROUP(core\\logic FILES ${logic})
SOURCE_GROUP(core\\logger FILES ${logger})
SOURCE_GROUP(core\\httper FILES ${httper})
SOURCE_GROUP(core\\timer FILES ${timer})
SOURCE_GROUP(core FILES ${main})

ADD_EXECUTABLE(tcore3
    ${api}
    ${tinyxml}
    ${json}
    ${zlib}
    ${public}
    ${net}
    ${udp}
    ${tcp}
    ${httper}
    ${logic}
    ${logger}
    ${timer}
    ${main}
)

TARGET_LINK_LIBRARIES(tcore3
    ${libcurl}
)
