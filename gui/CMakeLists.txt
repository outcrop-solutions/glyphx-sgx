CMAKE_MINIMUM_REQUIRED(VERSION 3.1.3)
PROJECT (GlyphBuilder)

#message("CMAKE_VS_PLATFORM_NAME = ${CMAKE_VS_PLATFORM_NAME}")
#message("CMAKE_CONFIGURATION_TYPES = ${CMAKE_CONFIGURATION_TYPES}")

function (Set_Windows_Properties target)
	IF (${WIN32}) 
		set_target_properties(${target} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
		set_target_properties(${target} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	ENDIF()
endfunction()

SET(CMAKE_CXX_STANDARD 14)
SET(BUILD_SHARED_LIBS ON) 

#Setup general platform specific values 
IF (${WIN32})
	SET(ARCH_DIR Win32)
	#For 32 bit windows apps & libraries to be able to access 4GB of memory
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LARGEADDRESSAWARE")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /Gm-")	# Enable multiprocess builds (and disable minimal rebuild since it's incompatible with /MP)

	add_definitions( "/DUNICODE" )

	#disable SSE in GLM since the 16-byte alignment requirements prevent us from passing GLM types by value and allocating
	#them on the heap (which are very useful things to do...) and we probably don't do enough number crunching to benefit
	#from it (todo: probably should test that assumption...)
	add_definitions( "/DGLM_FORCE_PURE" )

	#Suppress C4251 'needs to have dll-interface' warnings
	#Suppress C4503 'decorated name lengths exceeded' because it doesn't tell us anything useful and boost::bimap emits a lot of it
	#Suppress C4201 'nonstandard extension used : nameless struct/union' because GLM emits a lot of it
	#Suppress C4996 'This function or variable may be unsafe.'
	add_definitions( "/wd4251 /wd4503 /wd4201 /wd4996" )
	add_definitions( "/EHsc" )		# Enable unwind semantics (for exception handling)
	SET(GUI_TYPE WIN32)
ELSEIF (${APPLE})
	SET(ARCH_DIR OSX64)
	SET(GUI_TYPE MACOSX_BUNDLE)
	SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.11")
	message("Targeting OSX version " ${CMAKE_OSX_DEPLOYMENT_TARGET})
	set(MACOSX_BUNDLE_ICON_FILE synglyphx_x)
	set(sgx_ICON ${CMAKE_CURRENT_SOURCE_DIR}/../Misc/osx_resources/synglyphx_x.icns)
	set_source_files_properties(${sgx_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ELSE()
	SET(ARCH_DIR Linux64)
ENDIF()

get_filename_component(DEVELOPMENT_TOP_DIR ${CMAKE_SOURCE_DIR}/.. ABSOLUTE)
SET(THIRDPARTYDIR ${DEVELOPMENT_TOP_DIR}/3rdParty)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DEVELOPMENT_TOP_DIR}/lib/${ARCH_DIR})
IF (${WIN32})
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DEVELOPMENT_TOP_DIR}/lib/${ARCH_DIR})
ELSE()
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DEVELOPMENT_TOP_DIR}/bin/${ARCH_DIR})
ENDIF()
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DEVELOPMENT_TOP_DIR}/bin/${ARCH_DIR})

LINK_DIRECTORIES(${THIRDPARTYDIR}/lib/${ARCH_DIR} ${DEVELOPMENT_TOP_DIR}/lib/${ARCH_DIR})

FIND_PACKAGE(OpenGL)
if (OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIR}/Headers)
endif()

#Boost
IF (${WIN32})
	#set(Boost_NO_SYSTEM_PATHS    ON)
	SET(Boost_INCLUDE_DIRS ${THIRDPARTYDIR}/include/boost_1_55_0)
	SET(Boost_LIBRARIES debug boost_filesystem-vc120-mt-gd-1_55 debug boost_system-vc120-mt-gd-1_55 optimized boost_filesystem-vc120-mt-1_55 optimized boost_system-vc120-mt-1_55)
		SET(Boost_FOUND ON)
ELSE()
	SET(Boost_INCLUDE_DIRS ${THIRDPARTYDIR}/include/boost_1_60_0)
IF(${APPLE})
	SET(Boost_LIBRARIES libboost_filesystem.a libboost_system.a)
	SET(Boost_FOUND ON)
ELSE()
	SET(Boost_LIBRARIES libboost_filesystem.so libboost_system.so)
	SET(Boost_FOUND ON)
ENDIF()
	#set(Boost_USE_STATIC_LIBS       OFF) 
	#set(Boost_USE_MULTITHREADED      ON)
	#set(Boost_USE_STATIC_RUNTIME    OFF)
	#FIND_PACKAGE(Boost 1.55 COMPONENTS filesystem system)
ENDIF()

#Qt5
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
IF (${WIN32})
	set(CMAKE_PREFIX_PATH $ENV{QTDIR})
	set(CMAKE_LIBRARY_PATH "C:\\Program Files (x86)\\Windows Kits\\8.0\\Lib\\win8\\um\\x86")
ELSEIF(APPLE)
	set(CMAKE_PREFIX_PATH "/Users/Shared/Qt5.6.0/5.6/clang_64")
ELSE()
	set(CMAKE_PREFIX_PATH "${THIRDPARTYDIR}/Qt5.6.0/5.6/gcc_64")
ENDIF()
FIND_PACKAGE(Qt5Core)
FIND_PACKAGE(Qt5Gui)
FIND_PACKAGE(Qt5Widgets)
FIND_PACKAGE(Qt5OpenGL)
FIND_PACKAGE(Qt5Network)
FIND_PACKAGE(Qt5Sql)
FIND_PACKAGE(Qt5WebEngine)
FIND_PACKAGE(Qt5WebEngineWidgets)
FIND_PACKAGE(Qt5WebChannel)
FIND_PACKAGE(Qt5Quick)
FIND_PACKAGE(Qt5Qml)

set(STLPLUS_INCLUDE_DIR ${THIRDPARTYDIR}/include/stlplus_3_11)
set(GLM_INCLUDE_DIR ${THIRDPARTYDIR}/include/glm_0_9_6_3)
set(ASSIMP_INCLUDE_DIR ${THIRDPARTYDIR}/include/assimp-3.1.1)
set(STB_IMAGE_INCLUDE_DIR ${THIRDPARTYDIR}/include/stb_image)
set(GLEW_INCLUDE_DIR ${THIRDPARTYDIR}/include/glew-2.0.0)
set(FREETYPE_INCLUDE_DIR ${THIRDPARTYDIR}/include/freetype-2.7)

SET(GFXLIBS_LINK_DIR ${THIRDPARTYDIR}/tools/graphics/${ARCH_DIR})
LINK_DIRECTORIES(${GFXLIBS_LINK_DIR})

#Java
# Note we don't actually need to link against Java since we're loading it dynamically.
# (In fact on OSX linking against it can lead to JVM crashes if the OS has a different JVM
# installed from the one we want to use.)
IF (${WIN32})
	SET(JAVA_INCLUDE_DIRS ${DEVELOPMENT_TOP_DIR}/DataEngine/jdk/include)
	LIST(APPEND JAVA_INCLUDE_DIRS ${DEVELOPMENT_TOP_DIR}/DataEngine/jdk/include/win32)
ELSEIF(${APPLE})
	LIST(APPEND JAVA_INCLUDE_DIRS /Library/Java/JavaVirtualMachines/jdk1.8.0_92.jdk/Contents/Home/include)
	LIST(APPEND JAVA_INCLUDE_DIRS /Library/Java/JavaVirtualMachines/jdk1.8.0_92.jdk/Contents/Home/include/darwin)
ELSE()
	SET(JAVA_INCLUDE_DIRS ${DEVELOPMENT_TOP_DIR}/DataEngine/jdk/include)
	LIST(APPEND JAVA_INCLUDE_DIRS ${DEVELOPMENT_TOP_DIR}/DataEngine/jdk/include/linux)
ENDIF()

#Setup zSpace, licensing, and breakpad
#IF (${WIN32})
#	SET(ZSPACE_FOUND TRUE)
#ELSE()
	SET(ZSPACE_FOUND FALSE)
#ENDIF()

SET(LICENSING_FOUND TRUE)
SET(LICENSE_INCLUDE_DIR ${THIRDPARTYDIR}/include/rlm_ez_12_0)
SET(LICENSE_LINK_DIR ${THIRDPARTYDIR}/tools/rlm_ez_12_0/${ARCH_DIR})

IF (${LICENSING_FOUND})
	LINK_DIRECTORIES(${LICENSE_LINK_DIR})
ENDIF()

#IF (${WIN32})
#	SET(BREAKPAD_FOUND TRUE)
#ELSE()
	SET(BREAKPAD_FOUND FALSE)
#ENDIF()

#Libraries
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/ANTzPlus/ANTzBridge "${CMAKE_CURRENT_BINARY_DIR}/ANTzBridge")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/ANTzPlus/ANTzCore "${CMAKE_CURRENT_BINARY_DIR}/ANTzCore")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/ANTzPlus/ANTzSingleGlyphGUI "${CMAKE_CURRENT_BINARY_DIR}/ANTzSingleGlyphGUI")

ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/Core/SGXDataMapping "${CMAKE_CURRENT_BINARY_DIR}/SGXDataMapping")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/Core/SGXFoundation "${CMAKE_CURRENT_BINARY_DIR}/SGXFoundation")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/Core/SGXGeospatial "${CMAKE_CURRENT_BINARY_DIR}/SGXGeospatial")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/Core/SGXImage "${CMAKE_CURRENT_BINARY_DIR}/SGXImage")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/Core/SGXUtility "${CMAKE_CURRENT_BINARY_DIR}/SGXUtility")

ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/GUI/MapDownloading "${CMAKE_CURRENT_BINARY_DIR}/MapDownloading")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/GUI/SGXDataTransformGUI "${CMAKE_CURRENT_BINARY_DIR}/SGXDataTransformGUI")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/GUI/SGXGlyphGUI "${CMAKE_CURRENT_BINARY_DIR}/SGXGlyphGUI")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/GUI/SGXGUI "${CMAKE_CURRENT_BINARY_DIR}/SGXGUI")

ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/GUI/SGXGraphics "${CMAKE_CURRENT_BINARY_DIR}/SGXGraphics")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/GUI/SGXSceneViewer "${CMAKE_CURRENT_BINARY_DIR}/SGXSceneViewer")

ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/DataEngine/DataEngine "${CMAKE_CURRENT_BINARY_DIR}/DataEngine")
IF(${UNIX})
	IF(!APPLE)
		ADD_SUBDIRECTORY("${DEVELOPMENT_TOP_DIR}/DataEngine/Java DataEngine/converthash/HashInput" "${CMAKE_CURRENT_BINARY_DIR}/HashInput")
	ENDIF()
ENDIF()

IF (${ZSPACE_FOUND})
	ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/GUI/SGXzSpaceGUI "${CMAKE_CURRENT_BINARY_DIR}/SGXzSpaceGUI")
ENDIF()

#Executables
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/GUI/DataMapper "${CMAKE_CURRENT_BINARY_DIR}/DataMapper")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/GUI/GlyphDesigner "${CMAKE_CURRENT_BINARY_DIR}/GlyphDesigner")
ADD_SUBDIRECTORY(${DEVELOPMENT_TOP_DIR}/GUI/GlyphViewer "${CMAKE_CURRENT_BINARY_DIR}/GlyphViewer")
