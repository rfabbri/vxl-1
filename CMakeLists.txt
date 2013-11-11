#
#Some will want to construct a VXL programming environment where Brown Eyes
#is directly in the file heirarchy of VXL rather than a separate source tree.
#This approach has a distinct advantage when using Visual Studio since both 
#VXL libraries and Brown Eyes libraries can be debugged and linked in the same
#studio project.  
#
#To construct an interleaved source tree, CVS checkout Brown Eyes under the
#directory:   vxl/contrib/brl/blem/   which is the local CVS copy of the root 
#directory in lems: /vision/projects/cvsroot/lemsvxlsrc/
#
#If this directory is present in the user's VXL source tree, then a 
#build switch is automatically inserted in the CMake GUI to ask the user 
#if Brown Eyes should be built.
# JLM Jan 31, 2004
#

PROJECT(lems)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.5)

# Support for CMake 2.6
IF( COMMAND CMAKE_POLICY )
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF( COMMAND CMAKE_POLICY )

SET( BUILD_LEMS "NO" )


# We are building Brown Eyes interleaved with VXL
IF (EXISTS ${vxl_SOURCE_DIR}/contrib/brl/blem/CMakeLists.txt)
  #Import cmake variable used by external projects
  INCLUDE(${vxl_BINARY_DIR}/VXLConfig.cmake)
  SET( BUILD_BROWN_INTERLEAVED "YES" )
  SET( BUILD_LEMS "YES" )


# Else, we are building Brown Eyes as a stand alone project 
ELSE (EXISTS ${vxl_SOURCE_DIR}/contrib/brl/blem/CMakeLists.txt)

  SET (LIBRARY_OUTPUT_PATH ${lems_BINARY_DIR}/lib CACHE PATH "Single output directory for building all libraries.")

  FIND_PACKAGE(VXL)
  IF(VXL_FOUND)
    # include the standard stuff, such as Dart test
    SET(VXL_PROVIDE_STANDARD_OPTIONS 1)
    INCLUDE(${VXL_CMAKE_DIR}/UseVXL.cmake)

    SET( BUILD_LEMS "YES" )

  ELSE(VXL_FOUND)
    IF( VXL_BINARY_PATH )
      SET( VXL_DIR ${VXL_BINARY_PATH} )
      MESSAGE( "Use preferred VXL_DIR in place of VXL_BINARY_PATH" )
    ENDIF( VXL_BINARY_PATH )
  ENDIF(VXL_FOUND)

ENDIF (EXISTS ${vxl_SOURCE_DIR}/contrib/brl/blem/CMakeLists.txt)



# If LEMS code is built (either stand-alone or interleaved)
IF(BUILD_LEMS) 
  # CMake support directory.
  SET(LEMS_CMAKE_DIR ${lems_SOURCE_DIR}/config/cmake)

  # Brown 3rd Party Libraries
  SUBDIRS( db3p )

  # Brown Vision Algorithms
  OPTION(BUILD_ALGO "Build Depreciated Algorithms" NO)
  IF(BUILD_ALGO)
    SUBDIRS( algo )
  ENDIF(BUILD_ALGO)
  
  # Basic Utility Libraries
  SUBDIRS( basic ) 
   
  # Brown Computer Vision Libraries
  SUBDIRS( brcv )
  
  # Brown Eyes Code
  OPTION(BUILD_BREYE "Build Brown Eyes" NO)
  IF(BUILD_BREYE)
    SUBDIRS( breye )
  ENDIF(BUILD_BREYE)

  # Brown Eyes Code
  OPTION(BUILD_BREYE1 "Build Old Brown Eyes" YES)
  IF(BUILD_BREYE1)
    SUBDIRS( breye1 )
  ENDIF(BUILD_BREYE1)

  #Apps
  OPTION(BUILD_APPS "Build Apps" NO)
  IF(BUILD_APPS)
    SUBDIRS( apps )
  ENDIF(BUILD_APPS)

  OPTION(BUILD_PLAYLAND "Build the personal testing area" NO)
  IF(BUILD_PLAYLAND)
    SUBDIRS( playland )
  ENDIF(BUILD_PLAYLAND)

  IF( DART_ROOT )
    OPTION(BUILD_CONTRIB "Build the contrib development area" YES)
  ELSE( DART_ROOT )
    OPTION(BUILD_CONTRIB "Build the contrib development area" NO)
  ENDIF( DART_ROOT )
  IF(BUILD_CONTRIB)
    SUBDIRS( contrib )
  ENDIF(BUILD_CONTRIB)

ENDIF(BUILD_LEMS)

IF(VXL_FOUND)

  # This is required on some platforms because various libraries
  # include OpenGL indirectly even when it is not used
  INCLUDE_DIRECTORIES( ${VXL_VGUI_INCLUDE_DIR} )

  #update vxl/config/Modules/NewCMakeModules folder to get the following
  INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )

  IF(DIRECTSHOW_FOUND)
    INCLUDE_DIRECTORIES(${DIRECTSHOW_INCLUDE_DIR})
    ADD_DEFINITIONS(-DHAS_DSHOW)
    # For Euresys Multicam Driver's DirectShow source filter.
    # - should I put this in a Module of it's own?
    #INCLUDE( ${MODULE_PATH}/NewCMake/FindDirectShow_ESF.cmake )
    FIND_PATH(DIRECTSHOW_ESF_INCLUDE_DIR ESFilter.h
      "C:/Program Files/Euresys/MultiCam/Include"
      DOC "What is the path where the file ESFilter.h can be found"
    )
    IF(DIRECTSHOW_ESF_INCLUDE_DIR)
      SET(DIRECTSHOW_ESF_FOUND "YES")
    ENDIF(DIRECTSHOW_ESF_INCLUDE_DIR)
    IF(DIRECTSHOW_ESF_FOUND)
      INCLUDE_DIRECTORIES(${DIRECTSHOW_ESF_INCLUDE_DIR})
      ADD_DEFINITIONS(-DHAS_DSHOW_ESF)
    ENDIF(DIRECTSHOW_ESF_FOUND)
  ENDIF(DIRECTSHOW_FOUND)

  OPTION(USE_Boost "Use Boost libraries" NO)
  IF ( USE_Boost )
    set(Boost_USE_STATIC_LIBS   OFF)
    find_package( Boost 1.35.0 COMPONENTS iostreams)
  ELSE ( USE_Boost )
    set( Boost_FOUND FALSE )
  ENDIF( USE_Boost )
  
  #required by dbul library
  INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseOctave.cmake )

ENDIF(VXL_FOUND)


IF(NOT BUILD_BROWN_INTERLEAVED)
  # This SUBDIRS command must be the last SUBDIRS command in this file
  SUBDIRS( config/cmake/export )
ENDIF(NOT BUILD_BROWN_INTERLEAVED)


# adds a test for Endianness and a global variable that should be useful for file format programming -MM
INCLUDE(TestBigEndian)
TEST_BIG_ENDIAN(MY_BIG_ENDIAN)
