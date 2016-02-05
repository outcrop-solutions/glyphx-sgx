CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							../../Core/SGXUtility)

SET(SRC_ALL 	glyph.cpp
				glyphcolor.cpp
				glyphgeometryinfo.cpp
				glyphgraph.cpp
				nonmappablegeometryproperties.cpp
				virtualtopologyinfo.cpp)

ADD_LIBRARY(SGXFoundation SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXFoundation ${Boost_LIBRARIES} SGXUtility)