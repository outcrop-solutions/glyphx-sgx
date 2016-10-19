CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							../../Core/SGXUtility)


file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)

ADD_LIBRARY(SGXFoundation SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXFoundation ${Boost_LIBRARIES} SGXUtility)

Set_Windows_Properties(SGXFoundation)