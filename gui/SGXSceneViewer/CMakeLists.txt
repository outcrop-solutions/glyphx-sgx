CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${QT_INCLUDE_DIR}
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXImage
							../../GUI/SGXGraphics
)


file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
	"SceneViewer/*.h"
	"SceneViewer/*.cpp"
)

ADD_LIBRARY(SGXSceneViewer SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXSceneViewer ${Boost_LIBRARIES} ${QT_LIBRARIES} SGXUtility SGXFoundation SGXGraphics)

Set_Windows_Properties(SGXSceneViewer)
