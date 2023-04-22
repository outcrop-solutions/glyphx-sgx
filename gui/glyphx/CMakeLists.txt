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
							${AWS_INCLUDE_DIR})
							

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

ADD_EXECUTABLE(GlyphX ${GUI_TYPE} ${SRC_ALL} ${RESOURCES_SRC})
TARGET_LINK_LIBRARIES(GlyphX ${Boost_LIBRARIES} 
									Qt6::Core 
									Qt6::Gui 
									Qt6::OpenGL 
									Qt6::OpenGLWidgets
									Qt6::Widgets 
									Qt6::WebSockets
									Qt6::Sql
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
									aws-cpp-sdk-core
							  		aws-cpp-sdk-s3)

IF (${BREAKPAD_FOUND})
	TARGET_LINK_LIBRARIES(GlyphX ${BREAKPAD_LIBRARIES})
ENDIF()

IF (${ZSPACE_FOUND})
	TARGET_LINK_LIBRARIES(GlyphX SGXzSpaceGUI)
ENDIF()	

IF (${LICENSING_FOUND})
	TARGET_LINK_LIBRARIES(GlyphX rlmez)
ENDIF()

if(${APPLE})
#	find_library(APP_SERVICES_LIBRARY ApplicationServices)
#	target_link_libraries(GlyphX ${APP_SERVICES_LIBRARY})

	#message("Setting custom Info.plist template for GlyphX...")
	set_target_properties(GlyphX PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../../Misc/osx_resources/GV.Info.plist.in)
endif()

Set_Windows_Properties(GlyphX)

