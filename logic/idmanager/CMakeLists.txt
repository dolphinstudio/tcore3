#cmake file for project core
#author:^
#created:%
        
SET(idmanager
    "${PROJECT_SOURCE_DIR}/logic/idmanager/header.h"
    "${PROJECT_SOURCE_DIR}/logic/idmanager/idmanager.h"
    "${PROJECT_SOURCE_DIR}/logic/idmanager/idmanager.cpp"
    "${PROJECT_SOURCE_DIR}/logic/idmanager/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${idmanager})

ADD_LIBRARY(idmanager SHARED
    ${idmanager}
    ${api}
    ${public}
    ${interface}
    ${common}
)

SET_PROPERTY(TARGET idmanager PROPERTY FOLDER "component")