CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXImage)


file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)

ADD_LIBRARY(SGXDataMapping SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXDataMapping ${Boost_LIBRARIES} SGXImage SGXUtility SGXFoundation)

Set_Windows_Properties(SGXDataMapping)