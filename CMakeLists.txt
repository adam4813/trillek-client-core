# Modular CMake build stuff with git somudles, based on Franciscotufro's Simplex-Engine
cmake_minimum_required(VERSION 2.8)

# Set the directory of Find<Library>.cmake modules
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules")

# change output directory of executable so that relative paths in the code are correct
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)

#------------------------------------------------------------------------------
## Check that the compiler is compatible and new enough to build everything.

# check for gcc version to set c++11 or c++0x.
# thanks to http://stackoverflow.com/questions/10984442/how-to-detect-c11-support-of-a-compiler-with-cmake .
IF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	IF (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
		# c++11 std only appears in gcc 4.7 or higher. c++0x will work for lesser versions.
		MESSAGE("gcc ${GCC_VERSION} does not support c++11, only c++0x. This may cause some problems in the future. We recommend upgrading to gcc-4.7 or greater.")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	ELSE (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
		# compiler is gcc 4.7 or higher. Force c++11.
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	ENDIF (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	# Clang 3.3 and up support c++11.  
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE CLANG_VERSION)
	IF (NOT (CLANG_VERSION VERSION_GREATER 3.3 OR CLANG_VERSION VERSION_EQUAL 3.3))
		# c++11 std only appears in clang 3.3 or higher. c++0x will work for lesser versions.
		MESSAGE("clang ${CLANG_VERSION} does not support c++11, only c++0x. This may cause some problems in the future. We recommend upgrading to clang-3.3 or greater.")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	ELSE (NOT (CLANG_VERSION VERSION_GREATER 3.3 OR CLANG_VERSION VERSION_EQUAL 3.3))
		# compiler is clang 3.3 or higher. Force c++11 and use libc++.
		IF (XCODE_VERSION)
			SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD c++11)
			SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY libc++)
		ELSE (XCODE_VERSION)
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
		ENDIF (XCODE_VERSION)
	ENDIF (NOT (CLANG_VERSION VERSION_GREATER 3.3 OR CLANG_VERSION VERSION_EQUAL 3.3))
ELSEIF (MSVC AND (MSVC_VERSION GREATER 1599))
	MESSAGE("Supported Visual Studio!")
ELSE ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	MESSAGE(FATAL_ERROR "Your C++ compiler does not support C++11.")
ENDIF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")

#------------------------------------------------------------------------------
# VS201x stuff

# If we are on windows add in the local search directories as well.
IF (WIN32 AND NOT MINGW) # Windows
	SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/include/libraries)
	INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/include/libraries")
	IF (CMAKE_CL_64)
		LINK_DIRECTORIES("${CMAKE_SOURCE_DIR}/lib/x64/debug" "${CMAKE_SOURCE_DIR}/lib/x64/release")
		SET(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/lib/x64/debug ${CMAKE_SOURCE_DIR}/lib/x64/release)
	ELSE (CMAKE_CL_64)
		LINK_DIRECTORIES("${CMAKE_SOURCE_DIR}/lib/x86/debug" "${CMAKE_SOURCE_DIR}/lib/x86/release")
		SET(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/lib/x86/debug ${CMAKE_SOURCE_DIR}/lib/x86/release)
	ENDIF (CMAKE_CL_64)
ENDIF (WIN32 AND NOT MINGW)

#------------------------------------------------------------------------------
# Find/request and sets necesary libraries

# Find all the libraries except GLEW and platform specific.
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLFW3 REQUIRED)
FIND_PACKAGE(OggVorbis REQUIRED)
FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(Bullet REQUIRED)
FIND_PACKAGE(SOIL REQUIRED)
FIND_PACKAGE(GLM REQUIRED)
if(BUILD_LINK_CEF)
	FIND_PACKAGE(CEF REQUIRED)
else(BUILD_LINK_CEF)
	add_definitions(-DNO_CEF)
endif(BUILD_LINK_CEF)

# Give these some dummy values and if the platform is LINUX or OSX they will be set accordingly.
SET(X11_LIBRARIES "")
SET(OSX_LIBRARIES "")

IF (NOT APPLE) # X11 and GLEW are not needed on OSX.
	FIND_PACKAGE(X11) # TODO This should be in GNU/Linux and other *nix ??? Windows not have X-Windows

	set(USE_STATIC_GLEW FALSE CACHE BOOL "Build against GLEW static (default no)")

	FIND_PACKAGE(GLEW REQUIRED) # We find GLEW here as OSX doesn't need it.
	IF (NOT GLEW_FOUND)
		MESSAGE(FATAL_ERROR "GLEW not found! Aborting!")
	ENDIF (NOT GLEW_FOUND)
ENDIF (NOT APPLE)

# GNU/Linux and other *nix
IF (UNIX AND NOT APPLE)
	FIND_PACKAGE(GTK2 REQUIRED)
	INCLUDE_DIRECTORIES("${GTK2_INCLUDE_DIRS}")
ENDIF (UNIX AND NOT APPLE)

# Mac OS X
IF (APPLE)
	SET(GLEW_LIBRARY "") # Set a dummy value for GLEW.

	SET(CMAKE_OSX_ARCHITECTURES i386;x86_64)
	SET(CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS "i386 x86_64")
	SET(CMAKE_XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS macosx)

	# Need the 10.7 SDK or later.
	EXECUTE_PROCESS(COMMAND xcodebuild -sdk macosx -version SDKVersion OUTPUT_VARIABLE OSX_SDK_VERSION)
	IF (NOT (OSX_SDK_VERSION VERSION_GREATER 10.7 OR OSX_SDK_VERSION VERSION_EQUAL 10.7))
		MESSAGE(FATAL_ERROR "The installed version of Xcode does not support the 10.7 SDK or later.  Please upgrade Xcode and try again.")
	ENDIF (NOT (OSX_SDK_VERSION VERSION_GREATER 10.7 OR OSX_SDK_VERSION VERSION_EQUAL 10.7))

	# Configure the project to use the correct SDK.
	IF (XCODE_VERSION)
		SET(CMAKE_OSX_SYSROOT macosx)
	ELSE (XCODE_VERSION)
		# Non-Xcode generators need the full path.
		EXECUTE_PROCESS(COMMAND xcodebuild -sdk macosx -version Path | head -n 1 OUTPUT_VARIABLE CMAKE_OSX_SYSROOT)
		STRING(REGEX REPLACE "(\r?\n)+$" "" CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT}")
	ENDIF (XCODE_VERSION)

	# Can deploy back to 10.7, the first OS X to support the GL Core.
	SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.7)

	# Need Core Foundation and libobjc.
	SET(OSX_LIBRARIES "-framework CoreFoundation /usr/lib/libobjc.dylib")
ENDIF (APPLE)

# TODO Should be stuff to do unit testing i neach module... see modules/core/CMake file for example
# TODO Set TEST_LIBRARIES to the apropaite libs needed to build and rund tests (like Google's C++ Testing framework

#------------------------------------------------------------------------------
# Modules configuration stuff

# Sigma
set(BUILD_EXE_Sigma False CACHE BOOL "Build the Sigma test executable")
set(BUILD_STATIC_Sigma True CACHE BOOL "Build Sigma as a static library")
set(BUILD_SHARED_Sigma False CACHE BOOL "Build Sigma as a shared library")

# Virtual Computer
set(BUILD_TOOLS_TR3200 False CACHE BOOL "Build TR3200 VM tools")
set(BUILD_TESTS_TR3200 False CACHE BOOL "Build TR3200 VM tests")
set(BUILD_STATIC_TR3200 True CACHE BOOL "Build TR3200 VM as a static library")



#------------------------------------------------------------------------------
# Every module has a CMakeLists.txt in it's root.
# Include all the modules to be built
file ( GLOB MODULES ${CMAKE_SOURCE_DIR}/modules/* )

#
# Each module has an include directory where all the headers go to.
# To make it easier to find header files, all the headers are available to all modules globally.
# You can achieve this using include_directories in the root CMakeLists.txt
# before loading the modules.
foreach ( MODULE ${MODULES} )
	include_directories ( "${MODULE}/include/" )
endforeach ()


#
# After adding the include paths, the only remaining thing is to
# include each CMakeLists.txt for each module.
# add_subdirectory searches for a CMakeLists.txt file in the given
# directory, so that's what we use to include each module's build script.
foreach ( MODULE ${MODULES} )
	MESSAGE(STATUS "Procesing module ${MODULE}")
	add_subdirectory ( ${MODULE} )
endforeach ()


