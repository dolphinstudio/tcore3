#cmake file for project core
#author:T
#created:2015-01-12 15:07:04

SET(Model
    "${PROJECT_SOURCE_DIR}/logic/model/Memony.h"
    "${PROJECT_SOURCE_DIR}/logic/model/Unit.h"
    "${PROJECT_SOURCE_DIR}/logic/model/Unit.cpp"
    "${PROJECT_SOURCE_DIR}/logic/model/Table.h"
    "${PROJECT_SOURCE_DIR}/logic/model/Table.cpp"
    "${PROJECT_SOURCE_DIR}/logic/model/Row.h"
    "${PROJECT_SOURCE_DIR}/logic/model/Row.cpp"
    "${PROJECT_SOURCE_DIR}/logic/model/Header.h"
    "${PROJECT_SOURCE_DIR}/logic/model/Model.h"
    "${PROJECT_SOURCE_DIR}/logic/model/Model.cpp"
    "${PROJECT_SOURCE_DIR}/logic/model/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(\\ FILES ${Model})

ADD_LIBRARY(Model SHARED
    ${Model}
    ${api}
    ${public}
    ${tinyxml}
    ${proto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(Model
    ${libprotobuf}
)

SET_PROPERTY(TARGET Model PROPERTY FOLDER "component")