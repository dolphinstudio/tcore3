#cmake file for project core
#author:^
#created:%
        
SET(scene
    "${PROJECT_SOURCE_DIR}/logic/scene/header.h"
    "${PROJECT_SOURCE_DIR}/logic/scene/scene.h"
    "${PROJECT_SOURCE_DIR}/logic/scene/scene.cpp"
    "${PROJECT_SOURCE_DIR}/logic/scene/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${scene})

ADD_LIBRARY(scene SHARED
    ${scene}
    ${api}
    ${public}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(scene
    ${libprotobuf}
)

SET_PROPERTY(TARGET scene PROPERTY FOLDER "logic")