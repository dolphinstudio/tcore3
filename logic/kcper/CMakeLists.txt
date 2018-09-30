#cmake file for project core
#author:^
#created:%
        
set(ikcp
    "${PROJECT_SOURCE_DIR}/logic/kcper/ikcp/ikcp.h"
    "${PROJECT_SOURCE_DIR}/logic/kcper/ikcp/ikcp.c"
)

SET(kcper
    "${PROJECT_SOURCE_DIR}/logic/kcper/header.h"
    "${PROJECT_SOURCE_DIR}/logic/kcper/kcper.h"
    "${PROJECT_SOURCE_DIR}/logic/kcper/kcper.cpp"
    "${PROJECT_SOURCE_DIR}/logic/kcper/kcpServer.h"
    "${PROJECT_SOURCE_DIR}/logic/kcper/kcpServer.cpp"
    "${PROJECT_SOURCE_DIR}/logic/kcper/kcpSession.h"
    "${PROJECT_SOURCE_DIR}/logic/kcper/kcpSession.cpp"
    "${PROJECT_SOURCE_DIR}/logic/kcper/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ikcp FILES ${ikcp})
SOURCE_GROUP(\\ FILES ${kcper})

ADD_LIBRARY(kcper SHARED
    ${ikcp}
    ${kcper}
    ${api}
    ${json}
    ${public}
    ${interface}
    ${common}
)


SET_PROPERTY(TARGET kcper PROPERTY FOLDER "logic")