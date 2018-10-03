#cmake file for project core
#author:^
#created:%
        
SET(cards
    "${PROJECT_SOURCE_DIR}/logic/cards/header.h"
    "${PROJECT_SOURCE_DIR}/logic/cards/cards.h"
    "${PROJECT_SOURCE_DIR}/logic/cards/cards.cpp"
    "${PROJECT_SOURCE_DIR}/logic/cards/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(\\ FILES ${cards})

ADD_LIBRARY(cards SHARED
    ${cards}
    ${api}
    ${public}
    ${json}
    ${interface}
    ${common}
)

SET_PROPERTY(TARGET cards PROPERTY FOLDER "logic")