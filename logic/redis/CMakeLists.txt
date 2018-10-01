#cmake file for project core
#author:^
#created:%
        
SET(redis
    "${PROJECT_SOURCE_DIR}/logic/redis/header.h"
    "${PROJECT_SOURCE_DIR}/logic/redis/redis.h"
    "${PROJECT_SOURCE_DIR}/logic/redis/redis.cpp"
    "${PROJECT_SOURCE_DIR}/logic/redis/rediser.h"
    "${PROJECT_SOURCE_DIR}/logic/redis/rediser.cpp"
    "${PROJECT_SOURCE_DIR}/logic/redis/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(nodeproto FILES ${nodeproto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${redis})

ADD_LIBRARY(redis SHARED
    ${redis}
    ${api}
    ${tinyxml}
    ${public}
    ${nodeproto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(redis
    ${libprotobuf}
    ${libredis}
)

SET_PROPERTY(TARGET redis PROPERTY FOLDER "component")