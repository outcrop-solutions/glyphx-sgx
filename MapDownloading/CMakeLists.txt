CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							../../GUI/SGXGUI
							../../Core/SGXGeospatial
							../../Core/SGXImage
							../../Core/SGXUtility)

SET(SRC_ALL 	downloadedmap.cpp
				downloadexception.cpp
				downloadoptionsdialog.cpp
				mapoptionswidget.cpp
				networkdownloader.cpp)

ADD_LIBRARY(MapDownloading SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(MapDownloading ${Boost_LIBRARIES} 
							  Qt5::Core
							  Qt5::Gui
							  Qt5::Network
							  Qt5::Widgets 
							  SGXImage 
							  SGXUtility 
							  SGXGUI 
							  SGXGeospatial)