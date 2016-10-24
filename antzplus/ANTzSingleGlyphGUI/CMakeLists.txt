CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							${GLM_INCLUDE_DIR}
							../../3rdParty/include/SOIL_20080707
							../../GUI/SGXGlyphGUI
							../../GUI/SGXGUI
							../../ANTzPlus/ANTzBridge
							../../ANTzPlus/ANTzCore
							../../Core/SGXDataMapping
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXImage)

#zSpace is only available on windows							
#IF (${ZSPACE_FOUND})
#	INCLUDE_DIRECTORIES(AFTER ${ZSPACE_INCLUDE_DIR} ../../GUI/SGXzSpaceGUI)
#	add_definitions(-DUSE_ZSPACE)
#ENDIF()

file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)

ADD_LIBRARY(ANTzSingleGlyphGUI SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(ANTzSingleGlyphGUI ${Boost_LIBRARIES} 
										 ${OPENGL_LIBRARIES} 
										 Qt5::Core
										 Qt5::Gui
										 Qt5::OpenGL
										 Qt5::Widgets
										 SOIL 
										 SGXImage 
										 SGXUtility 
										 SGXGlyphGUI 
										 SGXGUI 
										 ANTzCore 
										 SGXFoundation 
										 SGXDataMapping 
										 ANTzBridge)

#zSpace is only available on windows
#IF (${ZSPACE_FOUND})
#	TARGET_LINK_LIBRARIES(ANTzSingleGlyphGUI ${ZSPACE_LIBRARIES} SGXzSpaceGUI)
#ENDIF()

Set_Windows_Properties(ANTzSingleGlyphGUI)