CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${GLM_INCLUDE_DIR}
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../GUI/SGXGraphics
							../../GUI/SGXGlyphGUI
)


file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
	"SceneViewer/*.h"
	"SceneViewer/*.cpp"
	"../../antzplus/ANTzTopology/*.h"
	"../../antzplus/ANTzTopology/*.cpp"
)

# Enable high warning level and warnings-as-errors.
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
endif()

ADD_LIBRARY(SGXSceneViewer SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXSceneViewer
	${Boost_LIBRARIES}
	Qt5::Core
	Qt5::Gui
	Qt5::OpenGL
	Qt5::Widgets
	SGXUtility 
	SGXFoundation
	SGXGlyphGUI
	SGXGraphics
)

Set_Windows_Properties(SGXSceneViewer)
