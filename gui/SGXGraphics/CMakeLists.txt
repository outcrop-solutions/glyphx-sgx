CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${GLM_INCLUDE_DIR}
							${GLEW_INCLUDE_DIR}
							${ASSIMP_INCLUDE_DIR}
							${STB_IMAGE_INCLUDE_DIR}
							${FREETYPE_INCLUDE_DIR}
							../../core/SGXUtility
							../../core/SGXFoundation
							../../core/SGXDataMapping
							../../core/SGXImage
							../../gui/SGXGUI)


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

# Enable high warning level and warnings-as-errors.
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

set(SGXGRAPHICS_LIBS SGXGraphics GLEW freetype)
if(${WIN32})
	LIST(APPEND SGXGRAPHICS_LIBS assimp-vc120-mt)		#diff naming convention for assimp on pc
else()
	LIST(APPEND SGXGRAPHICS_LIBS z assimp)		#need zlib on apple
endif()

ADD_LIBRARY(SGXGraphics SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES( ${SGXGRAPHICS_LIBS} ${OPENGL_LIBRARIES})

Set_Windows_Properties(SGXGraphics)
