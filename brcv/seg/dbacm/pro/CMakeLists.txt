# This is brcv/seg/dbacm/pro/CMakeLists.txt
# Brown Active Contour Models Processing library

include_directories( ${lems_SOURCE_DIR}/breye1 )

set(dbacm_pro_sources
  dbacm_geodesic_active_contour_process.h dbacm_geodesic_active_contour_process.cxx
  dbacm_knee_cartilage_seg_process.h      dbacm_knee_cartilage_seg_process.cxx
  #dbacm_group_edges_using_gac_process.h   dbacm_group_edges_using_gac_process.cxx
)

#aux_source_directory(Templates dbacm3d_pro_sources)

add_library(dbacm_pro ${dbacm_pro_sources})

target_link_libraries(dbacm_pro dbacm vidpro1)


if(BUILD_TESTING)
#  subdirs(tests)
endif(BUILD_TESTING)

