#cmake file for project core
#author:^
#created:%
        
SET(configer
    "${PROJECT_SOURCE_DIR}/logic/configer/header.h"
    "${PROJECT_SOURCE_DIR}/logic/configer/configer.h"
    "${PROJECT_SOURCE_DIR}/logic/configer/configer.cpp"
    "${PROJECT_SOURCE_DIR}/logic/configer/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(\\ FILES ${configer})

ADD_LIBRARY(configer SHARED
    ${configer}
    ${api}
    ${public}
    ${tinyxml}
    ${interface}
    ${common}
)

SET_PROPERTY(TARGET configer PROPERTY FOLDER "component")