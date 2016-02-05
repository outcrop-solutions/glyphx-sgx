CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							${GLM_INCLUDE_DIR}
							${QT_INCLUDE_DIR}
							../../3rdParty/include/SOIL_20080707
							../../GUI/SGXGlyphGUI
							../../GUI/SGXGUI
							../../ANTzPlus/ANTzBridge
							../../ANTzPlus/ANTzCore
							../../Core/SGXDataTransform
							../../Core/SGXUtility
							../../Core/SGXFoundation)

#zSpace is only available on windows							
IF (${ZSPACE_FOUND})
	INCLUDE_DIRECTORIES(AFTER ${ZSPACE_INCLUDE_DIR} ../../GUI/SGXzSpaceGUI)
	add_definitions(-DUSE_ZSPACE)
ENDIF()

SET(SRC_ALL 	antzforestwidget.cpp
				glyphforestmodel.cpp)

ADD_LIBRARY(ANTzGUI SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(ANTzGUI ${Boost_LIBRARIES} ${OPENGL_LIBRARIES} ${QT_LIBRARIES} SOIL SGXImage SGXUtility SGXGlyphGUI SGXGUI ANTzCore SGXFoundation SGXDataTransform ANTzBridge)

#zSpace is only available on windows
IF (${ZSPACE_FOUND})
	TARGET_LINK_LIBRARIES(ANTzGUI ${ZSPACE_LIBRARIES} SGXzSpaceGUI)
ENDIF()