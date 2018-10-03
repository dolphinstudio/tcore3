#cmake file for project core
#author:^
#created:%
        
SET(storage
    "${PROJECT_SOURCE_DIR}/logic/storage/header.h"
    "${PROJECT_SOURCE_DIR}/logic/storage/storage.h"
    "${PROJECT_SOURCE_DIR}/logic/storage/storage.cpp"
    "${PROJECT_SOURCE_DIR}/logic/storage/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(\\ FILES ${storage})

ADD_LIBRARY(storage SHARED
    ${storage}
    ${api}
    ${public}
    ${tinyxml}
    ${interface}
    ${common}
)

SET_PROPERTY(TARGET storage PROPERTY FOLDER "logic")