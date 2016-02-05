CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS})

SET(SRC_ALL 	csvfilehandler.cpp
				csvfilereader.cpp
				csvfilewriter.cpp
				csvtfilereaderwriter.cpp
				filesystem.cpp
				range.cpp
				uuid.cpp
				xmlpropertytreefile.cpp)

ADD_LIBRARY(SGXUtility SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXUtility ${Boost_LIBRARIES})