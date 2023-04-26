CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							../../gui/SGXGUI
							../../core/SGXGeospatial
							../../core/SGXImage
							../../core/SGXUtility)


file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)

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
							  
Set_Windows_Properties(MapDownloading)
