#cmake file for project core
#author:^
#created:%

SET(webSocket
    "${PROJECT_SOURCE_DIR}/logic/websocket/header.h"
    "${PROJECT_SOURCE_DIR}/logic/websocket/webSocketServer.h"
    "${PROJECT_SOURCE_DIR}/logic/websocket/webSocketServer.cpp"
    "${PROJECT_SOURCE_DIR}/logic/websocket/webSocketSession.h"
    "${PROJECT_SOURCE_DIR}/logic/websocket/webSocketSession.cpp"
    "${PROJECT_SOURCE_DIR}/logic/websocket/webSocket.h"
    "${PROJECT_SOURCE_DIR}/logic/websocket/webSocket.cpp"
    "${PROJECT_SOURCE_DIR}/logic/websocket/main.cpp"
)
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${webSocket})

ADD_LIBRARY(webSocket SHARED
    ${webSocket}
    ${api}
    ${interface}
    ${public}
)

SET_PROPERTY(TARGET webSocket PROPERTY FOLDER "component")
