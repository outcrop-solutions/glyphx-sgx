CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							../../GUI/SGXGUI
							../../Core/SGXGeospatial
							../../Core/SGXImage
							../../Core/SGXUtility)


file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)

ADD_LIBRARY(MapDownloading SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(MapDownloading ${Boost_LIBRARIES} 
							  Qt6::Core
							  Qt6::Gui
							  Qt6::Network
							  Qt6::Widgets 
							  SGXImage 
							  SGXUtility 
							  SGXGUI 
							  SGXGeospatial)
							  
Set_Windows_Properties(MapDownloading)