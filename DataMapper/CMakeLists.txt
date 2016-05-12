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
							../../Core/SGXGeospatial
							../../GUI/SGXGUI
							../../GUI/SGXGlyphGUI
							../../Core/SGXDataTransform
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

#IF (${ZSPACE_FOUND})
#	INCLUDE_DIRECTORIES(AFTER ${ZSPACE_INCLUDE_DIR} ../../GUI/SGXzSpaceGUI)
#	add_definitions(-DUSE_ZSPACE)
#ENDIF()

SET(SRC_ALL adddatabaseserverwizard.cpp
			addfiledatasourcewizard.cpp
			baseimagedialog.cpp
			baseobjectlistview.cpp
			bindinglineedit.cpp
			databindingtableswidget.cpp
			datamapperwindow.cpp
			datamapping3dwidget.cpp
			datasourcestatswidget.cpp
			datasourcesview.cpp
			datastatsmodel.cpp
			datatransformmodel.cpp
			fieldgroupmodel.cpp
			fieldgroupwidget.cpp
			glyphoptionswidget.cpp
			glyphrolessubsettablewidget.cpp
			glyphrolestablemodel.cpp
			glyphtagview.cpp
			glyphtreesview.cpp
			inputfieldmimedata.cpp
			interpolationmappingdialog.cpp
			legendlistview.cpp
			main.cpp
			mappingfunctionwidget.cpp
			newmappingdefaultswidget.cpp
			tablechoicewidget.cpp
			valuemappingdialog.cpp)
				
#SET(RESOURCES_SRC datamapperwindow.qrc)
IF (${WIN32})
#	LIST(APPEND ${RESOURCES_SRC} DataMapper.rc)
	SET(RESOURCES_SRC DataMapper.rc)
ENDIF()

ADD_EXECUTABLE(DataMapper ${GUI_TYPE} ${SRC_ALL} ${RESOURCES_SRC})
TARGET_LINK_LIBRARIES(DataMapper ${Boost_LIBRARIES} 
									Qt5::Core 
									Qt5::Gui 
									Qt5::OpenGL 
									Qt5::Widgets 
									Qt5::Sql
									ANTzCore 
									ANTzBridge 
									ANTzSingleGlyphGUI 
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
									
TARGET_LINK_LIBRARIES(DataMapper)

IF (${BREAKPAD_FOUND})
	TARGET_LINK_LIBRARIES(DataMapper ${BREAKPAD_LIBRARIES})
ENDIF()

#IF (${ZSPACE_FOUND})
#	TARGET_LINK_LIBRARIES(DataMapper SGXzSpaceGUI)
#ENDIF()

Set_Windows_Properties(DataMapper)
