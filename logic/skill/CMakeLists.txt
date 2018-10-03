#cmake file for project core
#author:^
#created:%
        
SET(skill
    "${PROJECT_SOURCE_DIR}/logic/skill/header.h"
    "${PROJECT_SOURCE_DIR}/logic/skill/skill.h"
    "${PROJECT_SOURCE_DIR}/logic/skill/skill.cpp"
    "${PROJECT_SOURCE_DIR}/logic/skill/reciver.h"
    "${PROJECT_SOURCE_DIR}/logic/skill/reciver.cpp"
    "${PROJECT_SOURCE_DIR}/logic/skill/caster.h"
    "${PROJECT_SOURCE_DIR}/logic/skill/caster.cpp"
    "${PROJECT_SOURCE_DIR}/logic/skill/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${skill})

ADD_LIBRARY(skill SHARED
    ${skill}
    ${api}
    ${public}
    ${proto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(skill
    ${libprotobuf}
)

SET_PROPERTY(TARGET skill PROPERTY FOLDER "logic")