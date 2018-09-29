#cmake file for project core
#author:^
#created:%
        
SET(noder
    "${PROJECT_SOURCE_DIR}/logic/noder/header.h"
    "${PROJECT_SOURCE_DIR}/logic/noder/noder.h"
    "${PROJECT_SOURCE_DIR}/logic/noder/noder.cpp"
    "${PROJECT_SOURCE_DIR}/logic/noder/noderSession.h"
    "${PROJECT_SOURCE_DIR}/logic/noder/noderSession.cpp"
    "${PROJECT_SOURCE_DIR}/logic/noder/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(nodeproto FILES ${nodeproto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(\\ FILES ${noder})

ADD_LIBRARY(noder SHARED
    ${noder}
    ${api}
    ${tinyxml}
    ${public}
    ${nodeproto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(noder
    ${libprotobuf}
)

SET_PROPERTY(TARGET noder PROPERTY FOLDER "architecture")