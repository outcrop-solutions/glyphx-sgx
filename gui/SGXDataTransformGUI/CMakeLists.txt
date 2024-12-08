CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							${GLM_INCLUDE_DIR}
							../../GUI/SGXGraphics
							../../GUI/SGXSceneViewer
							../../GUI/SGXGUI
							../../GUI/SGXGlyphGUI
							../../Core/SGXImage
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXDataTransform
							../../Core/SGXDataMapping
							../../ANTzPlus/ANTzBridge)


file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)

ADD_LIBRARY(SGXDataTransformGUI SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXDataTransformGUI ${Boost_LIBRARIES} 
							  Qt5::Core
							  Qt5::Gui
							  Qt5::Network
							  Qt5::Widgets 
							  Qt5::Sql
							  ANTzBridge
							  SGXImage 
							  SGXUtility 
							  SGXGraphics
							  SGXGUI 
							  SGXGlyphGUI
							  SGXSceneViewer
							  SGXFoundation
							  SGXDataMapping)
							 
Set_Windows_Properties(SGXDataTransformGUI)