#cmake file for project core
#author:T
#created:2015-01-12 15:07:04

SET(model
    "${PROJECT_SOURCE_DIR}/logic/model/memony.h"
    "${PROJECT_SOURCE_DIR}/logic/model/unit.h"
    "${PROJECT_SOURCE_DIR}/logic/model/unit.cpp"
    "${PROJECT_SOURCE_DIR}/logic/model/table.h"
    "${PROJECT_SOURCE_DIR}/logic/model/table.cpp"
    "${PROJECT_SOURCE_DIR}/logic/model/row.h"
    "${PROJECT_SOURCE_DIR}/logic/model/row.cpp"
    "${PROJECT_SOURCE_DIR}/logic/model/header.h"
    "${PROJECT_SOURCE_DIR}/logic/model/model.h"
    "${PROJECT_SOURCE_DIR}/logic/model/model.cpp"
    "${PROJECT_SOURCE_DIR}/logic/model/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(\\ FILES ${model})

ADD_LIBRARY(model SHARED
    ${model}
    ${api}
    ${public}
    ${tinyxml}
    ${proto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(model
    ${libprotobuf}
)

SET_PROPERTY(TARGET model PROPERTY FOLDER "component")