CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${GLM_INCLUDE_DIR}
							${GLEW_INCLUDE_DIR}
							${ASSIMP_INCLUDE_DIR}
							${STB_IMAGE_INCLUDE_DIR}
							${FREETYPE_INCLUDE_DIR}
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXDataMapping
							../../Core/SGXImage
							../../GUI/SGXGUI)


file(GLOB SRC_ALL
	"hal/*.h"
	"hal/*.cpp"
	"hal/internal/*.h"
	"hal/internal/*.cpp"
	"render/*.h"
	"render/*.cpp"
	"*.h"
	"*.cpp"
)

# Enable -Wall and warnings-as-errors for this library.
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
	message(STATUS "SGXGraphics CXX flags: " ${CMAKE_CXX_FLAGS})
endif()

ADD_LIBRARY(SGXGraphics SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXGraphics GLEW assimp freetype z ${OPENGL_LIBRARIES})

Set_Windows_Properties(SGXGraphics)
