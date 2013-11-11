# The Development Brown Numerical Library - Specific Algorithms
INCLUDE_DIRECTORIES( ${vxl_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_NETLIB_INCLUDE_DIR} )

SET( dbnl_algo_sources
   dbnl_eno.cxx               dbnl_eno.h
   dbnl_eno_interp.cxx
   dbnl_eno_poly.cxx
   dbnl_eno_measures.cxx      dbnl_eno_measures.h
   dbnl_eno_zerox.cxx         dbnl_eno_zerox.h
   dbnl_eno_zerox_label.cxx   dbnl_eno_zerox_label.h
   dbnl_eno_shock.cxx         dbnl_eno_shock.h
   dbnl_eno_image.cxx         dbnl_eno_image.h
   dbnl_eno_zerox_image.cxx   dbnl_eno_zerox_image.h
   dbnl_bbf_tree.h            dbnl_bbf_tree.txx
   dbnl_matrix_multiply.cxx   dbnl_matrix_multiply.h
   dbnl_eno_third_order.cxx   dbnl_eno_third_order.h
   dbnl_brent_root.cxx        dbnl_brent_root.h

)

AUX_SOURCE_DIRECTORY(Templates dbnl_algo_sources)

ADD_LIBRARY( dbnl_algo ${dbnl_algo_sources} )

TARGET_LINK_LIBRARIES( dbnl_algo dbnl vnl_algo vil brent)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

SUBDIRS(pro)


