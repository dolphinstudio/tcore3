#cmake file for project core
#author:^
#created:%
        
SET(game
    "${PROJECT_SOURCE_DIR}/logic/game/header.h"
    "${PROJECT_SOURCE_DIR}/logic/game/game.h"
    "${PROJECT_SOURCE_DIR}/logic/game/game.cpp"
    "${PROJECT_SOURCE_DIR}/logic/game/combiner.h"
    "${PROJECT_SOURCE_DIR}/logic/game/combiner.cpp"
    "${PROJECT_SOURCE_DIR}/logic/game/role.h"
    "${PROJECT_SOURCE_DIR}/logic/game/role.cpp"
    "${PROJECT_SOURCE_DIR}/logic/game/orbit.h"
    "${PROJECT_SOURCE_DIR}/logic/game/orbit.cpp"
    "${PROJECT_SOURCE_DIR}/logic/game/score.h"
    "${PROJECT_SOURCE_DIR}/logic/game/score.cpp"
    "${PROJECT_SOURCE_DIR}/logic/game/main.cpp"
    )
    
SOURCE_GROUP(api FILES ${api})
SOURCE_GROUP(common FILES ${common})
SOURCE_GROUP(proto FILES ${proto})
SOURCE_GROUP(interface FILES ${interface})
SOURCE_GROUP(public\\json FILES ${json})
SOURCE_GROUP(public FILES ${public})
SOURCE_GROUP(\\ FILES ${game})

ADD_LIBRARY(game SHARED
    ${game}
    ${api}
    ${json}
    ${public}
    ${proto}
    ${interface}
    ${common}
)

TARGET_LINK_LIBRARIES(game
    ${libprotobuf}
)

SET_PROPERTY(TARGET game PROPERTY FOLDER "logic")