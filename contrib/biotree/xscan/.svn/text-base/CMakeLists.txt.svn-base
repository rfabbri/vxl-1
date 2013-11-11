INCLUDE_DIRECTORIES(${BIOTREE_SOURCE_DIR}, ${lems_SOURCE_DIR}/brcv/mvg)
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)

SET( xscan_src
     xscan_scan.cxx                  xscan_scan.h
                                     xscan_orbit_base.h         xscan_orbit_base_sptr.h
     xscan_uniform_orbit.cxx         xscan_uniform_orbit.h  
     xscan_dummy_scan.cxx            xscan_dummy_scan.h
   )

AUX_SOURCE_DIRECTORY(Templates xscan_src)

ADD_LIBRARY( xscan ${xscan_src} )

TARGET_LINK_LIBRARIES( xscan vcl vbl vnl vnl_xio vgl_algo xmvg vsl vpgl_xio)

# algorithms related to 3D imaging processing
#SUBDIRS( algo )

# calibration artifact classes
#SUBDIRS( old_cali )
SUBDIRS( cali )

IF(BUILD_TESTING)
  SUBDIRS( tests )
ENDIF(BUILD_TESTING)
