cmake_minimum_required(VERSION 2.8)
 
project(AutoMagesRoot)

MACRO(isDefined envArg result)
    
    set(${result} "FALSE")
    MESSAGE( STATUS "${envArg}:         " ${${envArg}} )
    IF(NOT ${${envArg}} MATCHES "") 
        IF(NOT ${${envArg}} MATCHES ${envArg}"_DIR-NOTFOUND")
            set(${result} "TRUE")
        ENDIF(NOT ${${envArg}} MATCHES ${envArg}"_DIR-NOTFOUND")
    ENDIF(NOT ${${envArg}} MATCHES "")

ENDMACRO(isDefined envArg result)

MACRO(find_package_IfNeed envArg)


    isDefined(${envArg}_DIR retVal)
    MESSAGE( STATUS "retVal:         " ${retVal} )

    IF(${retVal} MATCHES "FALSE")
        find_package(${envArg} REQUIRED) 
    ENDIF(${retVal} MATCHES "FALSE")

ENDMACRO(find_package_IfNeed envArg)

MESSAGE( STATUS "CMAKE_BUILD_TYPE:         " ${CMAKE_BUILD_TYPE} )
MESSAGE( STATUS "CMAKE_SYSTEM_NAME:         " ${CMAKE_SYSTEM_NAME} )
MESSAGE( STATUS "PROJECT_SOURCE_DIR:         " ${PROJECT_SOURCE_DIR} )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_MODULE_PATH "/usr/share/OGRE/cmake/modules;${CMAKE_MODULE_PATH}")
  set(CMAKE_MODULE_PATH "/usr/share/cmake-2.8/Modules;${CMAKE_MODULE_PATH}")
  set(CMAKE_MODULE_PATH "/home/manicqin/Dropbox/AutoMages/AutoMagesLink/cmake;${CMAKE_MODULE_PATH}")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(CMAKE_MODULE_PATH "C:/Users/boaz/Dropbox/AutoMages/AutoMagesLink/cmake;${CMAKE_MODULE_PATH}")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

find_package_IfNeed(OGRE) 
find_package_IfNeed(BULLET) 
find_package_IfNeed(OIS) 
find_package_IfNeed(TBB)
find_package_IfNeed(LOG4CXX)
find_package_IfNeed(PythonLibs)

find_package(CEGUI REQUIRED COMPONENTS Ogre)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.53.0 COMPONENTS filesystem program_options system date_time thread python) 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist/")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

SET(CMAKE_CXX_FLAGS "-std=c++0x")

add_subdirectory(AutoMages)
add_subdirectory(SystemsTest)
add_subdirectory(Viewer)
