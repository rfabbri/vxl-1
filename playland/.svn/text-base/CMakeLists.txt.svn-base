# playland/CMakeLists.txt
# Brown personal testing area.


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/vdgl )


#########################################
# Users
#########################################


# Playland for han
OPTION(BUILD_PLAYLAND_han "Build playland for han" YES)

OPTION(BUILD_PLAYLAND_sadal "Build playland for sadal" NO)

#IF(BUILD_PLAYLAND_han)
#    SUBDIRS( han )
#ENDIF(BUILD_PLAYLAND_han)

IF(BUILD_PLAYLAND_sadal)
    SUBDIRS( sadal )
ENDIF(BUILD_PLAYLAND_sadal)



# Playland for thom
OPTION(BUILD_PLAYLAND_thom "Build playland for thom" NO)
IF(BUILD_PLAYLAND_thom)
    SUBDIRS( thom )
ENDIF(BUILD_PLAYLAND_thom)

