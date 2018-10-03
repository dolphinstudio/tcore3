#cmake file for project kernel
#author:^
#created:%
        
SET(gate
    "${PROJECT_SOURCE_DIR}/logic/gate/header.h"
    "${PROJECT_SOURCE_DIR}/logic/gate/gate.h"
    "${PROJECT_SOURCE_DIR}/logic/gate/gate.cpp"
    "${PROJECT_SOURCE_DIR}/logic/gate/wsDelegate.h"
    "${PROJECT_SOURCE_DIR}/logic/gate/wsDelegate.cpp"
    "${PROJECT_SOURCE_DIR}/logic/gate/wbServerDelegate.h"
    "${PROJECT_SOURCE_DIR}/logic/gate/wbServerDelegate.cpp"
    "${PROJECT_SOURCE_DIR}/logic/gate/main.cpp"
)

SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(nodeproto FILES ${nodeproto})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(public\\tinyxml FILES ${tinyxml})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${gate})


ADD_LIBRARY(gate SHARED
    ${gate}
    ${api}
    ${proto}
    ${nodeproto}
    ${interface}
    ${json}
    ${tinyxml}
    ${public}
    ${common}
)

TARGET_LINK_LIBRARIES(gate
    ${libprotobuf}
)

SET_PROPERTY(TARGET gate PROPERTY FOLDER "node")
