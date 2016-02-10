CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXImage
							../../Core/SGXGeospatial
							../../Core/SGXDataMapping
							../../GUI/MapDownloading)

SET(SRC_ALL csvcache.cpp
			databaseinfo.cpp
			inputfielddata.cpp
			sourcedatacache.cpp
			sourcedatamanager.cpp)

ADD_LIBRARY(SGXDataTransform SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXDataTransform ${Boost_LIBRARIES} 
											 Qt5::Core 
											 Qt5::Gui 
											 Qt5::Widgets 
											 Qt5::Sql
											 SGXImage 
											 SGXUtility 
											 SGXGeospatial
											 SGXDataMapping
											 SGXFoundation
											 MapDownloading)
											 
Set_Windows_Properties(SGXDataTransform)