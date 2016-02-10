CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							../../ANTzPlus/ANTzCore
							../../ANTzPlus/ANTzBridge
							../../ANTzPlus/ANTzSingleGlyphGUI
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXDataMapping
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

				
#SET(RESOURCES_SRC glyphdesignerwindow.qrc)
IF (${WIN32})
#	LIST(APPEND ${RESOURCES_SRC} GlyphDesigner.rc)
	SET(RESOURCES_SRC GlyphDesigner.rc)
ENDIF()

ADD_EXECUTABLE(GlyphDesigner ${GUI_TYPE} ${SRC_ALL} ${RESOURCES_SRC})
TARGET_LINK_LIBRARIES(GlyphDesigner ${Boost_LIBRARIES} 
									Qt5::Core 
									Qt5::Gui 
									Qt5::OpenGL 
									Qt5::Widgets 
									ANTzCore 
									ANTzBridge 
									ANTzSingleGlyphGUI 
									SGXUtility 
									SGXFoundation 
									SGXDataMapping 
									SGXImage 
									SGXGUI 
									SGXGlyphGUI)

IF (${BREAKPAD_FOUND})
	TARGET_LINK_LIBRARIES(GlyphDesigner ${BREAKPAD_LIBRARIES})
ENDIF()