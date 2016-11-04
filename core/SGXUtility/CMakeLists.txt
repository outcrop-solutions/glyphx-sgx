CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS})

file(GLOB SRC_ALL
	"*.h"
	"*.cpp"
)


# Enable high warning level and warnings-as-errors.
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
endif()

ADD_LIBRARY(SGXUtility SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXUtility ${Boost_LIBRARIES})

Set_Windows_Properties(SGXUtility)
