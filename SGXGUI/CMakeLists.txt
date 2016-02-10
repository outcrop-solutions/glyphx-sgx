CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							../../Core/SGXUtility)

#Enable licensing if it is available						
IF (${LICENSING_FOUND})
	INCLUDE_DIRECTORIES(AFTER ../../3rdParty/include/rlm_ez_11_0)
	add_definitions(-DUSE_LICENSING)
ENDIF()

SET(SRC_ALL 	application.cpp
				browselineedit.cpp
				checkboxheaderview.cpp
				coloralphawidget.cpp
				colorbutton.cpp
				doublesizewidget.cpp
				elasticlistmodel.cpp
				elasticlistwidget.cpp
				gridwidget.cpp
				groupboxsinglewidget.cpp
				horizontalformwidget.cpp
				intsizewidget.cpp
				labelline.cpp
				licensingdialog.cpp
				mainwindow.cpp
				notemptyvalidator.cpp
				radiobuttongroupwidget.cpp
				rangewidget.cpp
				replacefilenamedialog.cpp
				richtexteditor.cpp
				roledatafilterproxymodel.cpp
				sharedactionlist.cpp
				singlewidgetdialog.cpp
				sizewidget.cpp
				stretchsurroundedwidget.cpp
				tablesubsetproxymodel.cpp
				treeview.cpp
				verticaltabordertableview.cpp
				verticalscrollarea.cpp
				xyzwidget.cpp)
				
SET(RESOURCES_SRC sgxgui.qrc)

ADD_LIBRARY(SGXGUI SHARED ${SRC_ALL} ${RESOURCES_SRC})
TARGET_LINK_LIBRARIES(SGXGUI ${Boost_LIBRARIES} 
							 SGXUtility
							 Qt5::Core
							 Qt5::Gui
							 Qt5::Widgets)

#Enable licensing if it is available
IF (${LICENSING_FOUND})
	TARGET_LINK_LIBRARIES(SGXGUI rlmez rlmid1)
ENDIF()

Set_Windows_Properties(SGXGUI)