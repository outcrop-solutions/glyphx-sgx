CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${GLM_INCLUDE_DIR}
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../GUI/SGXGraphics
							../../GUI/SGXGlyphGUI
							../../GUI/SGXGUI
)


file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
	"SceneViewer/*.h"
	"SceneViewer/*.cpp"
	"../../antzplus/ANTzTopology/*.h"
	"../../antzplus/ANTzTopology/*.cpp"
)

# Enable high warning level on OSX (PC eventually...)
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

ADD_LIBRARY(SGXSceneViewer SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXSceneViewer
	${Boost_LIBRARIES}
	Qt6::Core
	Qt6::Gui
	Qt6::OpenGL
	Qt6::OpenGLWidgets
	Qt6::Widgets
	SGXUtility 
	SGXFoundation
	SGXGraphics
	SGXGlyphGUI
)

Set_Windows_Properties(SGXSceneViewer)
