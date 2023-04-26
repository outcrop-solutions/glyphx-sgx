CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${QT_INCLUDE_DIR}
							${STLPLUS_INCLUDE_DIR}
							../../core/SGXUtility
							../../core/SGXFoundation
							../../core/SGXDataMapping
							../../core/SGXImage
							../../gui/SGXGUI)


file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)

ADD_LIBRARY(SGXGlyphGUI SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXGlyphGUI ${Boost_LIBRARIES} ${QT_LIBRARIES} SGXUtility SGXFoundation SGXDataMapping SGXImage SGXGUI)

Set_Windows_Properties(SGXGlyphGUI)
