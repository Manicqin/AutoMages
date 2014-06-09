cmake_minimum_required(VERSION 2.6)
 
project(Viewer)


set(HDRS
    src/config.h)

set(SRCS
    src/viewerMain.cpp)

set (Scripts_Path "${PROJECT_SOURCE_DIR}/Scripts")
set (Config_Path "${PROJECT_SOURCE_DIR}/Configurations")

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_SOURCE_DIR}/src/config.h"
  )

include_directories("${AutoMages_SOURCE_DIR}/src")
include_directories( ${BULLET_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}
    ${CEGUI_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
 )


add_executable( Viewer ${SRCS})
target_link_libraries(Viewer AutoMages ${PYTHON_LIBRARIES} ${BULLET_LIBRARIES} ${Boost_LIBRARIES} ${LOG4CXX_LIBRARIES} ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGRE_Overlay_LIBRARIES} ${TBB_LIBRARIES})
