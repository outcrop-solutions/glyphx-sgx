CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							../../Core/SGXUtility)

#Enable licensing if it is available						
IF (${LICENSING_FOUND})
	INCLUDE_DIRECTORIES(BEFORE ${LICENSE_INCLUDE_DIR})
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
				fixedaspectratioimagelabel.cpp
				gridwidget.cpp
				groupboxsinglewidget.cpp
				horizontalformwidget.cpp
				intsizewidget.cpp
				labelline.cpp
				licensingdialog.cpp
				mainwindow.cpp
				rangeslider.cpp
				notemptyvalidator.cpp
				radiobuttongroupwidget.cpp
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
				passwordlineedit.cpp
				prefixsuffixvalidator.cpp
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
	IF (${WIN32}) 
		#Check if /SAFESEH:NO is needed with the newer 12.0 VS 2015 version of RLM-EZ (32 bit only)
		set_target_properties(SGXGUI PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:libcmt.lib /SAFESEH:NO /SUBSYSTEM:WINDOWS")
		set_target_properties(SGXGUI PROPERTIES LINK_FLAGS_RELEASE "/NODEFAULTLIB:libcmt.lib /SAFESEH:NO /SUBSYSTEM:WINDOWS")
		TARGET_LINK_LIBRARIES(SGXGUI ws2_32)
	ELSE()
		#TARGET_LINK_LIBRARIES(SGXGUI hasp)
	ENDIF()
ENDIF()

#Set_Windows_Properties(SGXGUI)
