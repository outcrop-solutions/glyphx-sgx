CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							../../core/SGXFoundation
							../../core/SGXUtility)


file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)

ADD_LIBRARY(SGXImage SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXImage ${Boost_LIBRARIES} SGXUtility SGXFoundation)

Set_Windows_Properties(SGXImage)
