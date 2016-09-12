# Brown softassign algorithm library
# algo/softassign/CMakeLists.txt

# include directories into here if needed
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
#include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_GEL_INCLUDE_DIR} )

set(softassign_sources
   softassign.cxx           softassign.h
)

aux_source_directory(Templates softassign_sources)

add_library(softassign  ${softassign_sources})

# add the required libraries into this list
target_link_libraries( softassign mbl bdgl vgl mvl vbl vdgl vtol vsol btol vnl )    

if( BUILD_EXAMPLES )
  subdirs( examples )
endif( BUILD_EXAMPLES )


