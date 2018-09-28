#cmake file for project core
#author:T
#created:2015-05-12 15:07:04

INCLUDE_DIRECTORIES(

)

SET(common

)

SET(interface
	${interface}
)

SET(proto
    ${proto}
)

SET(define
    ${define}
)

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/tcptester)
##ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/udptester)
ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/kcper)
