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
							  Qt6::Core
							  Qt6::Gui
							  Qt6::Network
							  Qt6::Widgets 
							  Qt6::Sql
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