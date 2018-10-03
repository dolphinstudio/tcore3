#cmake file for project kernel
#author:^
#created:%
        
SET(relation
    "${PROJECT_SOURCE_DIR}/logic/relation/header.h"
    "${PROJECT_SOURCE_DIR}/logic/relation/relation.h"
    "${PROJECT_SOURCE_DIR}/logic/relation/relation.cpp"
    "${PROJECT_SOURCE_DIR}/logic/relation/main.cpp"
)

SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(nodeproto FILES ${nodeproto})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${relation})


ADD_LIBRARY(relation SHARED
    ${relation}
    ${api}
    ${proto}
    ${nodeproto}
    ${interface}
    ${json}
    ${tinyxml}
    ${public}
    ${common}
)

TARGET_LINK_LIBRARIES(relation
    ${libprotobuf}
)

SET_PROPERTY(TARGET relation PROPERTY FOLDER "node")
