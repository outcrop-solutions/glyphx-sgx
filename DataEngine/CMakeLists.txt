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
							${JAVA_INCLUDE_DIRS})

LINK_DIRECTORIES(${JAVA_LINK_DIRS})
#message("JAVA_LINK_DIRS = ${JAVA_LINK_DIRS}")

SET(SRC_ALL 	dataengineconnection.cpp
				dataenginestatement.cpp
				glyphengine.cpp)

ADD_LIBRARY(DataEngine SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(DataEngine ${Boost_LIBRARIES} 
							  Qt5::Core
							  SGXImage 
							  SGXUtility 
							  SGXGUI 
							  SGXGeospatial
							  SGXFoundation
							  SGXDataMapping
							  MapDownloading)
							  
TARGET_LINK_LIBRARIES(DataEngine jvm jawt)

Set_Windows_Properties(DataEngine)

IF(${WIN32})
	ADD_CUSTOM_COMMAND(TARGET DataEngine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DEVELOPMENT_TOP_DIR}/DataEngine/Java DataEngine/dataengine.jar" $<TARGET_FILE_DIR:DataEngine>)
ELSE()
	ADD_CUSTOM_COMMAND(TARGET DataEngine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DEVELOPMENT_TOP_DIR}/DataEngine/Java DataEngine/dataengine.jar" $<TARGET_FILE_DIR:DataEngine>)
ENDIF()
