# Brown 3D Active Contour Models library
# (supposedly ) contains various active contour models, e.g. snakes, curve evolution.
# brcv/seg/dbacm3d/CMakeLists.txt

# include directories into here if needed
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

SET(dbacm3d_sources

# Levelset functions
   dbacm3d_levelset_func.cxx        dbacm3d_levelset_func.h
                                  dbacm3d_levelset_func_sptr.h
   dbacm3d_standard_levelset_func.cxx    dbacm3d_standard_levelset_func.h
   
# Levelset drivers
   dbacm3d_levelset_driver.cxx      dbacm3d_levelset_driver.h
   dbacm3d_geodesic_active_contour.cxx  dbacm3d_geodesic_active_contour.h
   
# Math functions used in levelset methods
  dbacm3d_levelset_math.h
  dbacm3d_image_filters.h
   
  dbacm3d_edge_trough_contour.cxx dbacm3d_edge_trough_contour.h
  dbacm3d_edge_trough_levelset_func.cxx dbacm3d_edge_trough_levelset_func.h

  dbacm3d_edge_lowmem_contour.cxx dbacm3d_edge_lowmem_contour.h
  dbacm3d_edge_lowmem_levelset_func.cxx dbacm3d_edge_lowmem_levelset_func.h
)

AUX_SOURCE_DIRECTORY(Templates dbacm3d_sources)

ADD_LIBRARY(dbacm3d  ${dbacm3d_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbacm3d vil3d vbl dbnl_algo dbil3d_algo )

#SUBDIRS(pro)

SUBDIRS(xio)

IF (VXL_VGUI_FOUND)
#  SUBDIRS(vis)
ENDIF (VXL_VGUI_FOUND)

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
