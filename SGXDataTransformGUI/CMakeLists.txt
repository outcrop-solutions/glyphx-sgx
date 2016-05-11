CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							../../GUI/SGXGUI
							../../GUI/SGXGlyphGUI
							../../Core/SGXImage
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXDataTransform
							../../Core/SGXDataMapping
							../../DataEngine/DataEngine
							${JAVA_INCLUDE_DIRS})

SET(SRC_ALL 	changedatasourcefiledialog.cpp
				changeimagefiledialog.cpp
				datamappingmodel.cpp
				datasourceinfowidget.cpp
				databaseserverinfowidget.cpp
				singlenumericrangefilterwidget.cpp
				portablevisualizationexport.cpp)

LINK_DIRECTORIES(${JAVA_LINK_DIRS})

ADD_LIBRARY(SGXDataTransformGUI SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXDataTransformGUI ${Boost_LIBRARIES} 
							  Qt5::Core
							  Qt5::Gui
							  Qt5::Network
							  Qt5::Widgets 
							  Qt5::Sql
							  DataEngine
							  SGXImage 
							  SGXUtility 
							  SGXGUI 
							  SGXGlyphGUI
							  SGXFoundation
							  SGXDataMapping)
							 
Set_Windows_Properties(SGXDataTransformGUI)