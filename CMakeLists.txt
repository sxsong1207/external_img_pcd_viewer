cmake_minimum_required(VERSION 2.8.1)

project(ONR_VIS)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(LINUX)
    set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(BUILD_SHARED_LIBS ON)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

find_package(OpenSceneGraph REQUIRED COMPONENTS osgUtil osgDB osgFX osgGA osgTerrain osgViewer osgText osgWidget osgManipulator osg osgShadow)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(GDAL 1.9.6 EXACT REQUIRED)
find_package(LibLAS 1.0.0 REQUIRED)


link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lib)
include_directories(src
${OPENSCENEGRAPH_INCLUDE_DIR}
${GDAL_INCLUDE_DIR}
${PCL_INCLUDE_DIRS}
${LIBLAS_INCLUDE_DIR})

add_subdirectory(src)
add_subdirectory(app)
