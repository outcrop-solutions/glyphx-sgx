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

SET(SRC_ALL 	antzimportdialog.cpp
				cachemanager.cpp
				datamappingloadingfiltermodel.cpp
				glyphpropertieswidgetscontainer.cpp
				glyphtreelistview.cpp
				glyphvieweroptions.cpp
				glyphviewerwindow.cpp
				glyph3dview.cpp
				filteringwidget.cpp
				filteringmanager.cpp
				filteringparameters.cpp
				filteredsourcedatawidget.cpp
				rangefilterlistwidget.cpp
				keywordfilter.cpp
				keywordfilterwidget.cpp
				keywordfilterlistwidget.cpp
				linkedwidgetsmanager.cpp
				loadingscreenwidget.cpp
				legendsdisplaywidget.cpp
				main.cpp
				multitableelasticlistswidget.cpp
				optionswidget.cpp
				pseudotimefilterwidget.cpp
				singletableelasticlistswidget.cpp
				selectedsourcedatawidget.cpp
				sourcedatacache.cpp
				sourcedatainfomodel.cpp
				sourcedatawidget.cpp)
				
#SET(RESOURCES_SRC glyphviewer.qrc)
IF (${WIN32})
#	LIST(APPEND ${RESOURCES_SRC} GlyphViewer.rc)
	SET(RESOURCES_SRC GlyphViewer.rc)
ENDIF()

ADD_EXECUTABLE(GlyphViewer ${GUI_TYPE} ${SRC_ALL} ${RESOURCES_SRC})
TARGET_LINK_LIBRARIES(GlyphViewer ${Boost_LIBRARIES} 
									Qt5::Core 
									Qt5::Gui 
									Qt5::OpenGL 
									Qt5::Widgets 
									Qt5::Sql
									ANTzCore 
									ANTzBridge 
									ANTzGUI 
									SGXUtility 
									SGXFoundation 
									SGXDataMapping 
									SGXImage 
									SGXGUI 
									SGXGlyphGUI
									SGXDataTransformGUI
									SGXGeospatial
									MapDownloading
									DataEngine)

IF (${BREAKPAD_FOUND})
	TARGET_LINK_LIBRARIES(GlyphViewer ${BREAKPAD_LIBRARIES})
ENDIF()

IF (${ZSPACE_FOUND})
	TARGET_LINK_LIBRARIES(GlyphViewer SGXzSpaceGUI)
ENDIF()	

IF (${LICENSING_FOUND})
	TARGET_LINK_LIBRARIES(GlyphViewer rlmez)
ENDIF()

Set_Windows_Properties(GlyphViewer)

