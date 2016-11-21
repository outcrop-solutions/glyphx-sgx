CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							${GLM_INCLUDE_DIR}
							../../ANTzPlus/ANTzCore
							../../ANTzPlus/ANTzBridge
							../../ANTzPlus/ANTzGUI
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXDataMapping
							../../Core/SGXDataTransform
							../../Core/SGXImage
							../../Core/SGXGeospatial
							../../GUI/SGXGraphics
							../../GUI/SGXSceneViewer
							../../GUI/SGXGUI
							../../GUI/SGXGlyphGUI
							../../GUI/SGXDataTransformGUI
							../../GUI/MapDownloading
							../../DataEngine/DataEngine
							${JAVA_INCLUDE_DIRS})
							
LINK_DIRECTORIES(${JAVA_LINK_DIRS})

#Enable breakpad if it is available						
IF (${BREAKPAD_FOUND})
	INCLUDE_DIRECTORIES(AFTER ${BREAKPAD_INCLUDE_DIR})
	add_definitions(-DUSE_BREAKPAD)
ENDIF()

IF (${ZSPACE_FOUND})
	INCLUDE_DIRECTORIES(AFTER ${ZSPACE_INCLUDE_DIR} ../../GUI/SGXzSpaceGUI)
	add_definitions(-DUSE_ZSPACE)
ENDIF()

IF (${LICENSING_FOUND})
	include_directories(AFTER ${LICENSE_INCLUDE_DIR})
	add_definitions(-DUSE_LICENSING)
ENDIF()

file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
	"../GlyphViewer/*.h"
	"../GlyphViewer/*.cpp"
)
				
SET(RESOURCES_SRC ../GlyphViewer/glyphviewer.qrc)
IF (${WIN32})
	LIST(APPEND RESOURCES_SRC GlyphEd.rc)
ENDIF()

ADD_EXECUTABLE(GlyphEd ${GUI_TYPE} ${SRC_ALL} ${RESOURCES_SRC})
TARGET_LINK_LIBRARIES(GlyphEd ${Boost_LIBRARIES} 
									Qt5::Core 
									Qt5::Gui 
									Qt5::OpenGL 
									Qt5::Widgets 
									Qt5::Sql
									ANTzCore 
									ANTzBridge 
									SGXUtility 
									SGXFoundation 
									SGXDataMapping 
									SGXImage 
									SGXGUI 
									SGXGlyphGUI
									SGXDataTransformGUI
									SGXGeospatial
									SGXGraphics
									SGXSceneViewer
									MapDownloading
									DataEngine)

IF (${BREAKPAD_FOUND})
	TARGET_LINK_LIBRARIES(GlyphEd ${BREAKPAD_LIBRARIES})
ENDIF()

IF (${ZSPACE_FOUND})
	TARGET_LINK_LIBRARIES(GlyphEd SGXzSpaceGUI)
ENDIF()	

IF (${LICENSING_FOUND})
	TARGET_LINK_LIBRARIES(GlyphEd rlmez)
ENDIF()

if(${APPLE})
#	find_library(APP_SERVICES_LIBRARY ApplicationServices)
#	target_link_libraries(GlyphEd ${APP_SERVICES_LIBRARY})

	#message("Setting custom Info.plist template for GlyphEd...")
	set_target_properties(GlyphEd PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../../Misc/osx_resources/GV.Info.plist.in)
endif()

Set_Windows_Properties(GlyphEd)

