CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							../../Core/SGXFoundation
							../../Core/SGXUtility)

SET(SRC_ALL 	baseimage.cpp
				baseimageproperties.cpp
				defaultbaseimageproperties.cpp
				downloadedmapproperties.cpp
				userdefinedbaseimageproperties.cpp)

ADD_LIBRARY(SGXImage SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXImage ${Boost_LIBRARIES} SGXUtility SGXFoundation)

Set_Windows_Properties(SGXImage)