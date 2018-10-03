#cmake file for project core
#author:^
#created:%
        
SET(effect
    "${PROJECT_SOURCE_DIR}/logic/effect/header.h"
    "${PROJECT_SOURCE_DIR}/logic/effect/effect.h"
    "${PROJECT_SOURCE_DIR}/logic/effect/effect.cpp"
    "${PROJECT_SOURCE_DIR}/logic/effect/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(\\ FILES ${effect})

ADD_LIBRARY(effect SHARED
    ${effect}
    ${api}
    ${public}
    ${proto}
    ${tinyxml}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(effect
    ${libprotobuf}
)

SET_PROPERTY(TARGET effect PROPERTY FOLDER "logic")