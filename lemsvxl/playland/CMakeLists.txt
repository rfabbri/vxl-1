# playland/CMakeLists.txt
# Brown personal testing area.


include_directories( ${LEMS_SOURCE_DIR}/algo )
include_directories( ${LEMS_SOURCE_DIR}/breye )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_GEL_INCLUDE_DIR}/vdgl )


#########################################
# Users
#########################################


# Playland for han
option(BUILD_PLAYLAND_han "Build playland for han" YES)

option(BUILD_PLAYLAND_sadal "Build playland for sadal" NO)

#if(BUILD_PLAYLAND_han)
#    add_subdirectory( han )
#endif(BUILD_PLAYLAND_han)

if(BUILD_PLAYLAND_sadal)
    add_subdirectory( sadal )
endif(BUILD_PLAYLAND_sadal)



# Playland for thom
option(BUILD_PLAYLAND_thom "Build playland for thom" NO)
if(BUILD_PLAYLAND_thom)
    add_subdirectory( thom )
endif(BUILD_PLAYLAND_thom)

