#
# A brief file to find vl feat library
#
# Author: Brandon A. Mayer 
# Date: 2/28/2011
#
# NOTE: FOR WINDOWS YOU HAVE TO ADD THE ROOT DIRECTORY OF VL FEAT TO THE PATH SO vl.dll CAN BE 
# FOUND BY EXECUTABLES OTHERWISE YOUR CODE WILL COMPILE BUT YOU WILL GET A RUNTIME ERROR.

set( VL_FEAT_FOUND "NO" )

if( WIN32 )
  find_path( VL_FEAT_INCLUDE_PATH vl/dsift.h )
  if( VL_FEAT_INCLUDE_PATH )
    if( CMAKE_CL_64 )
      find_library( VL_FEAT_LIBRARY NAMES vl.lib PATHS ${VL_FEAT_INCLUDE_PATH}/bin/w32 )
    elseif( CMAKE_CL_64 )
      find_library( VL_FEAT_LIBRARY NAMES vl.lib PATHS ${VL_FEAT_INCLUDE_PATH}/bin/w64 )
    endif( CMAKE_CL_64 )
    
    if( VL_FEAT_LIBRARY )
      set( VL_FEAT_FOUND "YES" )
    endif( VL_FEAT_LIBRARY )
  endif( VL_FEAT_INCLUDE_PATH )
endif( WIN32 )

if( VL_FEAT_FOUND )
# message("VL_FEAT_LIBRARY = " ${VL_FEAT_LIBRARY} )
# message("VL_FEAT_INCLUDE_PATH = " ${VL_FEAT_INCLUDE_PATH})

  project( vlfeat )

  include_directories(${VXL_CORE_INCLUDE_DIR})
  include_directories(${VL_FEAT_INCLUDE_PATH})
  

  set( VL_FEAT_VXL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
  
  set( VLFEAT_SOURCES
       vlfeat.h                   vlfeat.cxx
       ${VL_FEAT_INCLUDE_PATH}/vl/dsift.h )
  
  #aux_source_directory( ${VL_FEAT_INCLUDE_PATH}/vl VLFEAT_SOURCES )      
     
  add_library( vlfeat ${VLFEAT_SOURCES} )     
  target_link_libraries(vlfeat ${VL_FEAT_LIBRARY} vil vgl)

  add_subdirectory( exec )
  add_subdirectory( pro )
  add_subdirectory( tests )

endif( VL_FEAT_FOUND )