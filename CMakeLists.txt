cmake_minimum_required(VERSION 3.0.0)
project( cvPipeline )
set(SOURCES
    src/ImageProcessor.cpp
    src/ImgProcSet.cpp
    src/VideoCapture.cpp
    src/pugixml-1.4/src/pugixml.cpp
    )

set(OpenCV_DIR D:/code/opencv-build)
find_package( OpenCV REQUIRED )

# show the message of OpenCV
message(STATUS "OpenCV library status:")
message(STATUS "    version: 	${OpenCV_VERSION}")
message(STATUS "    headers: 	${OpenCV_INCLUDE_DIRS}")
message(STATUS "    libraries: 	${OpenCV_LIBS}")

include_directories(inc)
add_library( cvPipeline SHARED ${SOURCES} )
target_link_libraries( cvPipeline ${OpenCV_LIBS} )
add_definitions(-DBUILD_DLL -D_DEBUG -D_MSC_VER)

add_executable(viewer sample/viewer.cpp)
#link_directories(D:/code/tmp/cvPipeline/build)
target_link_libraries( viewer cvPipeline ${OpenCV_LIBS} )

set(CMAKE_INSTALL_PREFIX .)
install (TARGETS cvPipeline DESTINATION .)
install (FILES
    inc/ImageProcessor.h
    inc/ImgProcSet.h
    inc/VideoCapture.h
    inc/cvPipeline.h
    inc/pugiconfig.hpp
    inc/pugixml.hpp
    DESTINATION include
)