#cmake file for project core
#author:^
#created:%
        
SET(match
    "${PROJECT_SOURCE_DIR}/logic/match/header.h"
    "${PROJECT_SOURCE_DIR}/logic/match/match.h"
    "${PROJECT_SOURCE_DIR}/logic/match/match.cpp"
    "${PROJECT_SOURCE_DIR}/logic/match/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(nodeproto FILES ${nodeproto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${match})

ADD_LIBRARY(match SHARED
    ${match}
    ${api}
    ${nodeproto}
    ${public}
    ${proto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(match
    ${libprotobuf}
)

SET_PROPERTY(TARGET match PROPERTY FOLDER "node")