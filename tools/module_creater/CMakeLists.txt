#cmake file for project core
#author:alax
#created:2015-05-12 15:07:04

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}/api"
    "${PROJECT_SOURCE_DIR}/public")
    
SET(module_creater
    "${PROJECT_SOURCE_DIR}/tools/module_creater/header.h"
    "${PROJECT_SOURCE_DIR}/tools/module_creater/template.h"
    "${PROJECT_SOURCE_DIR}/tools/module_creater/main.cpp"
)

SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(\\ FILES ${module_creater})

ADD_EXECUTABLE(module_creater
    ${api}
    ${tinyxml}
    ${public}
    ${module_creater}
)

SET_PROPERTY(TARGET module_creater PROPERTY FOLDER "tools")
