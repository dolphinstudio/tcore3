#cmake file for project core
#author:^
#created:%
        
SET(udptest
    "${PROJECT_SOURCE_DIR}/logic/udptester/header.h"
    "${PROJECT_SOURCE_DIR}/logic/udptester/udptester.h"
    "${PROJECT_SOURCE_DIR}/logic/udptester/udptester.cpp"
    "${PROJECT_SOURCE_DIR}/logic/udptester/udpsession.h"
    "${PROJECT_SOURCE_DIR}/logic/udptester/udpsession.cpp"
    "${PROJECT_SOURCE_DIR}/logic/udptester/ikcp/ikcp.h"
    "${PROJECT_SOURCE_DIR}/logic/udptester/ikcp/ikcp.c"
    "${PROJECT_SOURCE_DIR}/logic/udptester/ikcp/test.h"
    "${PROJECT_SOURCE_DIR}/logic/udptester/ikcp/test.cpp"
    "${PROJECT_SOURCE_DIR}/logic/udptester/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${udptest})

ADD_LIBRARY(udptest SHARED
    ${udptest}
    ${api}
    ${json}
    ${public}
    ${interface}
    ${common}
)


SET_PROPERTY(TARGET udptest PROPERTY FOLDER "logic")