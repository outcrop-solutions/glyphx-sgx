CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							../../Core/SGXUtility)

#Enable licensing if it is available						
IF (${LICENSING_FOUND})
	INCLUDE_DIRECTORIES(BEFORE ${LICENSE_INCLUDE_DIR})
	add_definitions(-DUSE_LICENSING)
ENDIF()

file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)
				
SET(RESOURCES_SRC sgxgui.qrc)

ADD_LIBRARY(SGXGUI SHARED ${SRC_ALL} ${RESOURCES_SRC})
TARGET_LINK_LIBRARIES(SGXGUI ${Boost_LIBRARIES} 
							 SGXUtility
							 Qt5::Core
							 Qt5::Gui
							 Qt5::Widgets
							 Qt5::WebEngine
							 Qt5::WebEngineWidgets
)

#Enable licensing if it is available
IF (${LICENSING_FOUND})
	TARGET_LINK_LIBRARIES(SGXGUI rlmez)
	IF (${WIN32}) 
		#Check if /SAFESEH:NO is needed with the newer 12.0 VS 2015 version of RLM-EZ (32 bit only)
		set_target_properties(SGXGUI PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:libcmt.lib /SAFESEH:NO /SUBSYSTEM:WINDOWS")
		set_target_properties(SGXGUI PROPERTIES LINK_FLAGS_RELEASE "/NODEFAULTLIB:libcmt.lib /SAFESEH:NO /SUBSYSTEM:WINDOWS")
		TARGET_LINK_LIBRARIES(SGXGUI ws2_32)
		TARGET_LINK_LIBRARIES(SGXGUI rlmid1)
	ELSE()
		#TARGET_LINK_LIBRARIES(SGXGUI hasp)
	ENDIF()
ENDIF()

#Set_Windows_Properties(SGXGUI)
