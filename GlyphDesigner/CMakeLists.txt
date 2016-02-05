CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${QT_INCLUDE_DIR}
							${STLPLUS_INCLUDE_DIR}
							../../ANTzPlus/ANTzCore
							../../ANTzPlus/ANTzBridge
							../../ANTzPlus/ANTzSingleGlyphGUI
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXDataMapping
							../../Core/SGXDataTransform
							../../Core/SGXImage
							../../GUI/SGXGUI
							../../GUI/SGXGlyphGUI)
							
#Enable breakpad if it is available						
IF (${BREAKPAD_FOUND})
	INCLUDE_DIRECTORIES(AFTER ${BREAKPAD_INCLUDE_DIR})
	add_definitions(-DUSE_BREAKPAD)
ENDIF()

SET(SRC_ALL 	glyphdesignerwindow.cpp
				glyphtreeview.cpp
				main.cpp
				modalglyphwidget.cpp)

ADD_EXECUTABLE(GlyphDesigner ${SRC_ALL})
TARGET_LINK_LIBRARIES(GlyphDesigner ${Boost_LIBRARIES} ${QT_LIBRARIES} ANTzCore ANTzBridge ANTzSingleGlyphGUI SGXUtility SGXFoundation SGXDataMapping SGXImage SGXGUI SGXDataTransform SGXGlyphGUI)

IF (${BREAKPAD_FOUND})
	TARGET_LINK_LIBRARIES(GlyphDesigner ${BREAKPAD_LIBRARIES})
ENDIF()