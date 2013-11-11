# Brown softassign algorithm library
# algo/softassign/CMakeLists.txt

# include directories into here if needed
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )

SET(softassign_sources
   softassign.cxx           softassign.h
)

AUX_SOURCE_DIRECTORY(Templates softassign_sources)

ADD_LIBRARY(softassign  ${softassign_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( softassign mbl bdgl vgl mvl vbl vdgl vtol vsol btol vnl )    

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )


