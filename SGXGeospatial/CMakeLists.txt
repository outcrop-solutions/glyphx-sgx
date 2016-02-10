CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS})

SET(SRC_ALL 	geographicboundingbox.cpp
				georeference.cpp
				ogrconvert.cpp)

ADD_LIBRARY(SGXGeospatial SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXGeospatial ${Boost_LIBRARIES})

Set_Windows_Properties(SGXGeospatial)