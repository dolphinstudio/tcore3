#cmake file for project core
#author:^
#created:%
        
SET(event
    "${PROJECT_SOURCE_DIR}/logic/event/event.h"
    "${PROJECT_SOURCE_DIR}/logic/event/event.cpp"
    "${PROJECT_SOURCE_DIR}/logic/event/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${event})

ADD_LIBRARY(event SHARED
    ${event}
    ${api}
    ${public}
    ${interface}
    ${common}
)

SET_PROPERTY(TARGET event PROPERTY FOLDER "component")