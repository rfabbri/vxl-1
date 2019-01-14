include_directories(${BIOTREE_SOURCE_DIR}, ${LEMS_SOURCE_DIR}/brcv/mvg)
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)

set( xscan_src
     xscan_scan.cxx                  xscan_scan.h
                                     xscan_orbit_base.h         xscan_orbit_base_sptr.h
     xscan_uniform_orbit.cxx         xscan_uniform_orbit.h  
     xscan_dummy_scan.cxx            xscan_dummy_scan.h
   )

aux_source_directory(Templates xscan_src)

add_library( xscan ${xscan_src} )

target_link_libraries( xscan vcl vbl vnl vnl_xio vgl_algo xmvg vsl vpgl_xio)

# algorithms related to 3D imaging processing
#add_subdirectory( algo )

# calibration artifact classes
#add_subdirectory( old_cali )
add_subdirectory( cali )

if(BUILD_TESTING)
  add_subdirectory( tests )
endif(BUILD_TESTING)
