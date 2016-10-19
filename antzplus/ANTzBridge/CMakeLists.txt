CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							${GLM_INCLUDE_DIR}
							../../Core/SGXUtility
							../ANTzCore
							../../Core/SGXFoundation)


file(GLOB SRC_COMMON
	"*.h"
	"*.cpp"
)
				
IF (${WIN32})
	SET(SRC_ALL stdafx.cpp
				dllmain.cpp
				${SRC_COMMON})
ELSEIF (${APPLE})
	SET(SRC_ALL ${SRC_COMMON})
ELSE()
	SET(SRC_ALL ${SRC_COMMON})
ENDIF()

ADD_LIBRARY(ANTzBridge SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(ANTzBridge ${Boost_LIBRARIES} SGXUtility ANTzCore SGXFoundation)

Set_Windows_Properties(ANTzBridge)