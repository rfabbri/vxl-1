# Brown 3D Active Contour Models library
# (supposedly ) contains various active contour models, e.g. snakes, curve evolution.
# brcv/seg/dbacm3d/CMakeLists.txt

# include directories into here if needed
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )

set(dbacm3d_sources

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

aux_source_directory(Templates dbacm3d_sources)

add_library(dbacm3d  ${dbacm3d_sources})

# add the required libraries into this list
target_link_libraries( dbacm3d vil3d vbl dbnl_algo dbil3d_algo )

#add_subdirectory(pro)

add_subdirectory(xio)

if (VXL_VGUI_FOUND)
#  add_subdirectory(vis)
endif (VXL_VGUI_FOUND)

if( BUILD_EXAMPLES )
#  add_subdirectory( examples )
endif( BUILD_EXAMPLES )

if(BUILD_TESTING)
#  add_subdirectory(tests)
endif(BUILD_TESTING)
