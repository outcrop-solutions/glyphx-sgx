CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)

INCLUDE_DIRECTORIES(BEFORE  ${OPENGL_INCLUDE_DIR}
							../../3rdParty/include/SOIL_20080707)

SET(SRC_COMMON 	npctrl.c
				npdata.c
				ctrl/npengine.c
				data/npmap.c
				data/npmapfile.c
				data/npmath.c
				data/npnode.c
				data/npstr.c
				io/file/npcsv.c
				io/gl/npcolor.c
				io/gl/npgeometry.c
				io/gl/nptags.c
				io/gl/npgldraw.c
				io/gl/nptexmap.c
				io/npch.c
				io/npconsole.c
				io/npfile.c
				io/npgl.c
				npui.c)
				
IF (${WIN32})
	SET(SRC_ALL os/msw/npos.c 
				stdafx.cpp
				dllmain.cpp
				${SRC_COMMON})
ELSEIF (${APPLE})
	SET(SRC_ALL os/osx/nposx.c 
				${SRC_COMMON})
ELSE()
	SET(SRC_ALL os/posix/npos_posix.c 
				${SRC_COMMON})
ENDIF()

ADD_LIBRARY(ANTzCore SHARED ${SRC_ALL})
TARGET_LINK_LIBRARIES(ANTzCore ${OPENGL_LIBRARIES} SOIL)