#cmake file for project core
#author:^
#created:%
        
SET(ai
    "${PROJECT_SOURCE_DIR}/logic/ai/header.h"
    "${PROJECT_SOURCE_DIR}/logic/ai/ai.h"
    "${PROJECT_SOURCE_DIR}/logic/ai/ai.cpp"
    "${PROJECT_SOURCE_DIR}/logic/ai/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${ai})

ADD_LIBRARY(ai SHARED
    ${ai}
    ${api}
    ${json}
    ${public}
    ${proto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(ai
    ${libprotobuf}
)

SET_PROPERTY(TARGET ai PROPERTY FOLDER "logic")