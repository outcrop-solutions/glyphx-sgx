CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							../../GUI/SGXGUI
							../../Core/SGXGeospatial
							../../Core/SGXImage
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXDataMapping
							../jdk1.7.0_79/include)
							
IF (${WIN32})
	INCLUDE_DIRECTORIES(AFTER ../jdk1.7.0_79/include/win32)
ENDIF()

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
							  SGXDataMapping)
							  
IF (${WIN32})
	TARGET_LINK_LIBRARIES(DataEngine jvm jawt)
ENDIF()