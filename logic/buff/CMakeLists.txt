#cmake file for project core
#author:^
#created:%
        
SET(buff
    "${PROJECT_SOURCE_DIR}/logic/buff/header.h"
    "${PROJECT_SOURCE_DIR}/logic/buff/buff.h"
    "${PROJECT_SOURCE_DIR}/logic/buff/buff.cpp"
    "${PROJECT_SOURCE_DIR}/logic/buff/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${buff})

ADD_LIBRARY(buff SHARED
    ${buff}
    ${api}
    ${public}
    ${proto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(buff
    ${libprotobuf}
)

SET_PROPERTY(TARGET buff PROPERTY FOLDER "logic")