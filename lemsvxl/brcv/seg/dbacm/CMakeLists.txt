# This is brcv/seg/dbacm3d/CMakeLists.txt
# Brown Active Contour Models library
# (supposedly ) contains various active contour models, e.g. snakes, curve evolution.


# include directories into here if needed
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )

set(dbacm_sources

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

aux_source_directory(Templates dbacm_sources)

add_library(dbacm  ${dbacm_sources})

# add the required libraries into this list
target_link_libraries( dbacm vil vbl dbnl_algo bil_algo )

add_subdirectory(pro)

if (VXL_VGUI_FOUND)
  add_subdirectory(vis)
endif (VXL_VGUI_FOUND)

if( BUILD_EXAMPLES )
#  add_subdirectory( examples )
endif( BUILD_EXAMPLES )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
