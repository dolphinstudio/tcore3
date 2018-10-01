#cmake file for project core
#author:alax
#created:2015-05-12 15:07:04

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}/api"
    "${PROJECT_SOURCE_DIR}/public")
    
SET(model_maker
    "${PROJECT_SOURCE_DIR}/tools/model_maker/header.h"
    "${PROJECT_SOURCE_DIR}/tools/model_maker/main.cpp"
)

SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(\\ FILES ${model_maker})

ADD_EXECUTABLE(model_maker
    ${api}
    ${tinyxml}
    ${public}
    ${model_maker}
)

SET_PROPERTY(TARGET model_maker PROPERTY FOLDER "tools")
