# This is brcv/seg/dbacm3d/CMakeLists.txt
# Brown Active Contour Models library
# (supposedly ) contains various active contour models, e.g. snakes, curve evolution.


# include directories into here if needed
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

SET(dbacm_sources

# Levelset functions
   dbacm_levelset_func.cxx               dbacm_levelset_func.h
                                         dbacm_levelset_func_sptr.h
   dbacm_standard_levelset_func.cxx      dbacm_standard_levelset_func.h
   
# Levelset drivers
   dbacm_levelset_driver.cxx             dbacm_levelset_driver.h
   dbacm_geodesic_active_contour.cxx     dbacm_geodesic_active_contour.h
   
# Math functions used in levelset methods
  dbacm_levelset_math.h
)

AUX_SOURCE_DIRECTORY(Templates dbacm_sources)

ADD_LIBRARY(dbacm  ${dbacm_sources})

# add the required libraries into this list
TARGET_LINK_LIBRARIES( dbacm vil vbl dbnl_algo bil_algo )

SUBDIRS(pro)

IF (VXL_VGUI_FOUND)
  SUBDIRS(vis)
ENDIF (VXL_VGUI_FOUND)

IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
