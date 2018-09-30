#cmake file for project core
#author:^
#created:%
        
SET(tcptester
    "${PROJECT_SOURCE_DIR}/logic/tcptester/header.h"
    "${PROJECT_SOURCE_DIR}/logic/tcptester/tcptester.h"
    "${PROJECT_SOURCE_DIR}/logic/tcptester/tcptester.cpp"
    "${PROJECT_SOURCE_DIR}/logic/tcptester/tcpsession.h"
    "${PROJECT_SOURCE_DIR}/logic/tcptester/tcpsession.cpp"
    "${PROJECT_SOURCE_DIR}/logic/tcptester/tcpserver.h"
    "${PROJECT_SOURCE_DIR}/logic/tcptester/tcpserver.cpp"
    "${PROJECT_SOURCE_DIR}/logic/tcptester/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${tcptester})

ADD_LIBRARY(tcptester SHARED
    ${tcptester}
    ${api}
    ${json}
    ${public}
    ${interface}
    ${common}
)


SET_PROPERTY(TARGET tcptester PROPERTY FOLDER "logic")