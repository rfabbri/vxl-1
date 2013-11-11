# lemsvxlsrc/basic/dbsol/CMakeLists.txt
# The Development Brown Spatial Object Library
PROJECT(dbsol)

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic)

CONFIGURE_FILE( ${dbsol_SOURCE_DIR}/dbsol_config.h.in
                ${dbsol_BINARY_DIR}/dbsol_config.h )

SET( dbsol_sources
  dbsol_interp_curve_2d.cxx dbsol_interp_curve_2d.h dbsol_interp_curve_2d_sptr.h  
  dbsol_geno_curve_2d.cxx dbsol_geno_curve_2d.h dbsol_geno_curve_2d_sptr.h
  dbsol_gshock.cxx dbsol_gshock.h dbsol_gshock_sptr.h

  dbsol_gshock_curve_2d.cxx dbsol_gshock_curve_2d.h dbsol_gshock_curve_2d_sptr.h
  dbsol_geno_measures.cxx  dbsol_geno_measures.h

  dbsol_circ_arc_2d.cxx   dbsol_circ_arc_2d.h
  dbsol_circ_arc_2d_sptr.h
  dbsol_file_io.cxx        dbsol_file_io.h
  dbsol_algos.cxx          dbsol_algos.h

  dbsol_interp_curve_3d.cxx dbsol_interp_curve_3d.h dbsol_interp_curve_3d_sptr.h  
)

AUX_SOURCE_DIRECTORY(Templates dbsol_sources)

ADD_LIBRARY(dbsol ${dbsol_sources})

TARGET_LINK_LIBRARIES( dbsol dbgl dbgl_algo vgl vsol vil )

IF( Boost_FOUND )
  TARGET_LINK_LIBRARIES( dbsol ${Boost_LIBRARIES})
ENDIF( Boost_FOUND )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS( algo )

SUBDIRS( pro )

SUBDIRS( vis )


#IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
#ENDIF( BUILD_EXAMPLES )
