#cmake file for project core
#author:^
#created:%
        
SET(master
    "${PROJECT_SOURCE_DIR}/logic/master/header.h"
    "${PROJECT_SOURCE_DIR}/logic/master/idmanager.h"
    "${PROJECT_SOURCE_DIR}/logic/master/idmanager.cpp"
    "${PROJECT_SOURCE_DIR}/logic/master/master.h"
    "${PROJECT_SOURCE_DIR}/logic/master/master.cpp"
    "${PROJECT_SOURCE_DIR}/logic/master/masterSession.h"
    "${PROJECT_SOURCE_DIR}/logic/master/masterSession.cpp"
    "${PROJECT_SOURCE_DIR}/logic/master/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(nodeproto FILES ${nodeproto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${master})

ADD_LIBRARY(master SHARED
    ${master}
    ${api}
    ${tinyxml}
    ${public}
    ${nodeproto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(master
    ${libprotobuf}
)

SET_PROPERTY(TARGET master PROPERTY FOLDER "architecture")