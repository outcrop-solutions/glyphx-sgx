CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${Boost_INCLUDE_DIRS}
							${QT_INCLUDE_DIR}
							${STLPLUS_INCLUDE_DIR}
							../../Core/SGXUtility
							../../Core/SGXFoundation
							../../Core/SGXDataMapping
							../../Core/SGXImage
							../../GUI/SGXGUI)

SET(SRC_ALL 	allglyphpropertieswidget.cpp
				colorconverter.cpp
				colorminmaxwidget.cpp
				defaultbaseimagescombobox.cpp
				doubleminmaxwidget.cpp
				glyphbuilderapplication.cpp
				glyphdefaultswidget.cpp
				glyphenumcombobox.cpp
				glyphmimedata.cpp
				glyphstructurewidget.cpp
				intminmaxwidget.cpp
				itemfocusselectionmodel.cpp
				linklessgraphmimedata.cpp
				newglyphtreewizard.cpp
				nonmappablegeometrywidget.cpp
				scenepropertieswidget.cpp
				singleglyphviewoptionswidget.cpp
				surfaceradiobuttonwidget.cpp
				textglyphpropertieswidget.cpp
				treeeditview.cpp
				visualglyphpropertieswidget.cpp)

ADD_LIBRARY(SGXGlyphGUI SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(SGXGlyphGUI ${Boost_LIBRARIES} ${QT_LIBRARIES} SGXUtility SGXFoundation SGXDataMapping SGXImage SGXGUI)

Set_Windows_Properties(SGXGlyphGUI)