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

# Copied over from VXL:
if( "${CMAKE_CXX_STANDARD}" MATCHES "(11|14|17|20)")
   # If building for modern C++ language standards,
   # require use of newer cmake version
   cmake_minimum_required(VERSION 3.9.5 FATAL_ERROR)
else()
   cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)
endif()
# Set policies consistent with newer versions of cmake
# to ease integration with projects that require newer
# cmake versions.

# Copied over from VXL:
foreach(p
    ## Only policies introduced after the cmake_minimum_required
    ## version need to explicitly be set to NEW.

    ##----- Policies Introduced by CMake 3.12
    CMP0075  #: Include file check macros honor CMAKE_REQUIRED_LIBRARIES.
    ##----- Policies Introduced by CMake 3.10
    CMP0071  #: Let AUTOMOC and AUTOUIC process GENERATED files.
    CMP0070  #: Define file(GENERATE) behavior for relative paths.
    ##----- Policies Introduced by CMake 3.9
    CMP0069  #: INTERPROCEDURAL_OPTIMIZATION is enforced when enabled.
    CMP0068  #: RPATH settings on macOS do not affect install_name.
    ##----- Policies Introduced by CMake 3.8
    CMP0067  #: Honor language standard in try_compile() source-file signature.
    ##----- Policies Introduced by CMake 3.7
    CMP0066  #: Honor per-config flags in try_compile() source-file signature.
    ##----- Policies Introduced by CMake 3.4
    CMP0065  #: Do not add flags to export symbols from executables without the ENABLE_EXPORTS target property.
    CMP0064  #: Support new TEST if() operator.
    )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()


project(lems)

#-----------------------------------------------------------------------------
if(NOT COMMAND SETIFEMPTY)
  macro(SETIFEMPTY)
    set(KEY ${ARGV0})
    set(VALUE ${ARGV1})
    if(NOT ${KEY})
      set(${ARGV})
    endif()
  endmacro()
endif()

#-----------------------------------------------------------------------------
SETIFEMPTY(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
SETIFEMPTY(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
SETIFEMPTY(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

#-----------------------------------------------------------------------------
SETIFEMPTY(CMAKE_INSTALL_LIBRARY_DESTINATION lib)
SETIFEMPTY(CMAKE_INSTALL_ARCHIVE_DESTINATION lib)
SETIFEMPTY(CMAKE_INSTALL_RUNTIME_DESTINATION bin)

# Allow external project to override the export target
if(NOT LEMS_NO_EXPORT)
  SETIFEMPTY(LEMS_INSTALL_EXPORT_NAME LEMSTargets)
endif()

SETIFEMPTY(LEMS_INSTALL_RUNTIME_DIR bin)
SETIFEMPTY(LEMS_INSTALL_LIBRARY_DIR lib)
SETIFEMPTY(LEMS_INSTALL_ARCHIVE_DIR lib)
SETIFEMPTY(LEMS_INSTALL_INCLUDE_DIR include/LEMS)
if(NOT LEMS_LIB_PREFIX)
  set( LEMS_LIB_PREFIX "") # This is typically empty
endif()

# CMake support directory.
set(LEMS_ROOT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(LEMS_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/config/cmake/Modules)

include(${LEMS_CMAKE_DIR}/LEMSStandardOptions.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/config/cmake/doxygen/doxygen.cmake)
include( ${CMAKE_CURRENT_LIST_DIR}/config/cmake/config/vxl_utils.cmake )
# Only include these if we have vxd-specific cmake utilities. We currently use VXL's:
#include(${CMAKE_CURRENT_LIST_DIR}/config/cmake/config/vxd_utils.cmake)

# Location of VXD's FindXXX.cmake CMake modules.
# This is identical to VXD_CMAKE_DIR.  Perhaps we should eliminate MODULE_PATH?
set( MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/config/cmake/Modules CACHE STATIC "VXD module path" )
set (LIBRARY_OUTPUT_PATH ${lems_BINARY_DIR}/lib CACHE PATH "Single output directory for building all libraries.")

find_package(VXL PATHS "../vxl-bin")
if(VXL_FOUND)
  # include the standard stuff, such as Dart test
  set(VXL_PROVIDE_STANDARD_OPTIONS 1)
  include(${VXL_CMAKE_DIR}/UseVXL.cmake)
else(VXL_FOUND)
  message( FATAL_ERROR "VXL not found, but required" )
endif(VXL_FOUND)

find_package(VXD PATHS "../vxd-bin")
if(VXD_FOUND)
  # include the standard stuff, such as Dart test
  set(VXD_PROVIDE_STANDARD_OPTIONS 1)
  include(${VXD_CMAKE_DIR}/UseVXD.cmake)
  set( BUILD_LEMS "YES" )
else(VXD_FOUND)
  message( FATAL_ERROR "VXD not found, but required" )
endif(VXD_FOUND)

# Brown 3rd Party Libraries
add_subdirectory( db3p )

# Brown Vision Algorithms
option(BUILD_ALGO "Build Depreciated Algorithms" NO)
if(BUILD_ALGO)
  add_subdirectory( algo )
endif(BUILD_ALGO)

# Basic Utility Libraries
add_subdirectory( basic ) 
 
# Brown Computer Vision Libraries
add_subdirectory( brcv )

# Brown Eyes Code
option(BUILD_BREYE "Build Brown Eyes" NO)
if(BUILD_BREYE)
  add_subdirectory( breye )
endif(BUILD_BREYE)

# Brown Eyes Code
option(BUILD_BREYE1 "Build Old Brown Eyes" YES)
if(BUILD_BREYE1)
  add_subdirectory( breye1 )
endif(BUILD_BREYE1)

#Apps
option(BUILD_APPS "Build Apps" NO)
if(BUILD_APPS)
  add_subdirectory( apps )
endif(BUILD_APPS)

option(BUILD_PLAYLAND "Build the personal testing area" NO)
if(BUILD_PLAYLAND)
  add_subdirectory( playland )
endif(BUILD_PLAYLAND)

if( DART_ROOT )
  option(BUILD_CONTRIB "Build the contrib development area" YES)
else( DART_ROOT )
  option(BUILD_CONTRIB "Build the contrib development area" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB)
  add_subdirectory( contrib )
endif(BUILD_CONTRIB)

# This is required on some platforms because various libraries
# include OpenGL indirectly even when it is not used
include_directories( ${VXL_VGUI_INCLUDE_DIR} )

#update vxl/config/Modules/NewCMakeModules folder to get the following
include( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )

if(DIRECTSHOW_FOUND)
  include_directories(${DIRECTSHOW_INCLUDE_DIR})
  add_definitions(-DHAS_DSHOW)
  # For Euresys Multicam Driver's DirectShow source filter.
  # - should I put this in a Module of it's own?
  #include( ${MODULE_PATH}/NewCMake/FindDirectShow_ESF.cmake )
  find_path(DIRECTSHOW_ESF_INCLUDE_DIR ESFilter.h
    "C:/Program Files/Euresys/MultiCam/Include"
    DOC "What is the path where the file ESFilter.h can be found"
  )
  if(DIRECTSHOW_ESF_INCLUDE_DIR)
    set(DIRECTSHOW_ESF_FOUND "YES")
  endif(DIRECTSHOW_ESF_INCLUDE_DIR)
  if(DIRECTSHOW_ESF_FOUND)
    include_directories(${DIRECTSHOW_ESF_INCLUDE_DIR})
    add_definitions(-DHAS_DSHOW_ESF)
  endif(DIRECTSHOW_ESF_FOUND)
endif(DIRECTSHOW_FOUND)

option(USE_Boost "Use Boost libraries" NO)
if ( USE_Boost )
  set(Boost_USE_STATIC_LIBS   OFF)
  find_package( Boost 1.35.0 COMPONENTS iostreams)
else ( USE_Boost )
  set( Boost_FOUND FALSE )
endif( USE_Boost )

#required by dbul library
include( ${LEMS_CMAKE_DIR}/UseOctave.cmake )

# adds a test for Endianness and a global variable that should be useful for file format programming -MM
include(TestBigEndian)
TEST_BIG_ENDIAN(MY_BIG_ENDIAN)

# Copy the UseLEMS.cmake file to the binary directory so that client
# projects don't need to find the source directory first. They can run
# the UseLEMS.cmake from the lemsvxl binary directory, and determine the
# lemsvxl source directory by loading the cache.
configure_file( ${LEMS_CMAKE_DIR}/UseLEMS.cmake
                ${lems_BINARY_DIR}/UseLEMS.cmake COPYONLY )
# For use in client projects that call UseLEMS.cmake
set(LEMS_LIBRARY_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} CACHE STATIC "Where all the lemsvxl libraries are, for clients to use." )

# Copy CTestCustom.cmake to top of build tree
#configure_file( ${LEMS_CMAKE_DIR}/CTestCustom.cmake
#  ${lems_BINARY_DIR}/CTestCustom.cmake COPYONLY )

# This command must be the last command in this file
if(NOT LEMS_NO_EXPORT)
  include(${CMAKE_CURRENT_LIST_DIR}/config/cmake/export/LEMSCreateProject.cmake)
endif()
