CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							../../GUI/SGXGUI
							../../Core/SGXGeospatial
							../../Core/SGXImage
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXDataMapping
							../../GUI/MapDownloading
							${AWS_INCLUDE_DIR}
							${JAVA_INCLUDE_DIRS})

LINK_DIRECTORIES(${JAVA_LINK_DIRS})
#message("JAVA_LINK_DIRS = ${JAVA_LINK_DIRS}")

file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)

ADD_LIBRARY(DataEngine SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(DataEngine ${Boost_LIBRARIES} 
							  Qt5::Core
							  SGXImage 
							  SGXUtility 
							  SGXGUI 
							  SGXGeospatial
							  SGXFoundation
							  SGXDataMapping
							  MapDownloading
							  aws-cpp-sdk-core
							  aws-cpp-sdk-s3)
							  
TARGET_LINK_LIBRARIES(DataEngine)

Set_Windows_Properties(DataEngine)

#Custom build steps to build data engine JAR and copy to build path
if(${APPLE}})
	set(PYTHON_CMD python3)
else()
	set(PYTHON_CMD python)
endif()

add_custom_command(TARGET DataEngine PRE_BUILD COMMAND ${PYTHON_CMD} ${DEVELOPMENT_TOP_DIR}/dataengine/build.py WORKING_DIRECTORY ${DEVELOPMENT_TOP_DIR}/dataengine)
add_custom_command(TARGET DataEngine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DEVELOPMENT_TOP_DIR}/classes/dataengine.jar" $<TARGET_FILE_DIR:DataEngine> WORKING_DIRECTORY ${DEVELOPMENT_TOP_DIR}/dataengine)
