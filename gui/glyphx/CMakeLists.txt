CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)
message("The third party direcrtory is ${THIRDPARTYDIR}")
INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							${GLM_INCLUDE_DIR}
							../../antzplus/ANTzCore
							../../antzplus/ANTzBridge
							../../antzplus/ANTzGUI
							../../core/SGXUtility
							../../core/SGXFoundation
							../../core/SGXDataMapping
							../../core/SGXDataTransform
							../../core/SGXImage
							../../core/SGXGeospatial
							../../gui/SGXGraphics
							../../gui/SGXSceneViewer
							../../gui/SGXGUI
							../../gui/SGXGlyphGUI
							../../gui/SGXDataTransformGUI
							../../gui/MapDownloading
							${AWS_INCLUDE_DIR})
							

#Enable breakpad if it is available						
IF (${BREAKPAD_FOUND})
	INCLUDE_DIRECTORIES(AFTER ${BREAKPAD_INCLUDE_DIR})
	add_definitions(-DUSE_BREAKPAD)
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
									Qt5::Core 
									Qt5::Gui 
									Qt5::OpenGL 
									Qt5::Widgets 
									Qt5::WebSockets
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
									aws-cpp-sdk-core
							  		aws-cpp-sdk-s3)

IF (${BREAKPAD_FOUND})
	TARGET_LINK_LIBRARIES(GlyphX ${BREAKPAD_LIBRARIES})
ENDIF()

IF (${ZSPACE_FOUND})
	TARGET_LINK_LIBRARIES(GlyphX SGXzSpaceGUI)
ENDIF()	


if(${APPLE})
#	find_library(APP_SERVICES_LIBRARY ApplicationServices)
#	target_link_libraries(GlyphX ${APP_SERVICES_LIBRARY})

	#message("Setting custom Info.plist template for GlyphX...")
	set_target_properties(GlyphX PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../../Misc/osx_resources/GV.Info.plist.in)
endif()
if(UNIX)
    add_custom_command(TARGET GlyphX POST_BUILD
        COMMAND echo "This is a post-build command for Linux" 
	COMMAND ln -snf ${THIRDPARTYDIR}/textures ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/textures
	COMMAND ln -snf ${THIRDPARTYDIR}/meshes ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/meshes
	COMMAND ln -snf ${THIRDPARTYDIR}/fonts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fonts
	COMMAND ln -snf ${THIRDPARTYDIR}/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
	COMMAND ln -snf ${THIRDPARTYDIR}/DefaultBaseImages  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/DefaultBaseImages 
	COMMAND ln -snf  /usr/share/qt/resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
	COMMAND ln -snf  /usr/share/qt/translations ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translations
    )
endif()
Set_Windows_Properties(GlyphX)

