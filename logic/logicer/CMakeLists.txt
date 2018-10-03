#cmake file for project kernel
#author:^
#created:%
        
SET(logicer
    "${PROJECT_SOURCE_DIR}/logic/logicer/header.h"
    "${PROJECT_SOURCE_DIR}/logic/logicer/logicer.h"
    "${PROJECT_SOURCE_DIR}/logic/logicer/logicer.cpp"
    "${PROJECT_SOURCE_DIR}/logic/logicer/main.cpp"
)

SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(nodeproto FILES ${nodeproto})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${logicer})


ADD_LIBRARY(logicer SHARED
    ${logicer}
    ${api}
    ${proto}
    ${nodeproto}
    ${interface}
    ${json}
    ${tinyxml}
    ${public}
    ${common}
)

TARGET_LINK_LIBRARIES(logicer
    ${libprotobuf}
)

SET_PROPERTY(TARGET logicer PROPERTY FOLDER "node")
