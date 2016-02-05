CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${STLPLUS_INCLUDE_DIR}
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXImage)

SET(SRC_ALL 	datamappingdefaults.cpp
				datamappingfunction.cpp
				datamappingglyph.cpp
				datamappingglyphfile.cpp
				datamappingglyphgeometry.cpp
				datamappingglyphgraph.cpp
				datamappingproperty.cpp
				datamappingvirtualtopology.cpp
				datasource.cpp
				datasourcemaps.cpp
				datasourcetable.cpp
				datatransformmapping.cpp
				filedatasource.cpp
				inputbinding.cpp
				inputcombinationfunction.cpp
				inputfield.cpp
				inputtable.cpp
				interpolationmappingfunction.cpp
				mindiff.cpp
				sceneproperties.cpp
				stringconcatmappingfunction.cpp
				stringinputcombinationfunction.cpp
				valuemappingfile.cpp
				valuemappingfunction.cpp)

ADD_LIBRARY(SGXDataMapping SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXDataMapping ${Boost_LIBRARIES} SGXImage SGXUtility SGXFoundation)