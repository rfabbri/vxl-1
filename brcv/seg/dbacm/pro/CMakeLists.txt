# This is brcv/seg/dbacm/pro/CMakeLists.txt
# Brown Active Contour Models Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbacm_pro_sources
  dbacm_geodesic_active_contour_process.h dbacm_geodesic_active_contour_process.cxx
  dbacm_knee_cartilage_seg_process.h      dbacm_knee_cartilage_seg_process.cxx
  #dbacm_group_edges_using_gac_process.h   dbacm_group_edges_using_gac_process.cxx
)

#AUX_SOURCE_DIRECTORY(Templates dbacm3d_pro_sources)

ADD_LIBRARY(dbacm_pro ${dbacm_pro_sources})

TARGET_LINK_LIBRARIES(dbacm_pro dbacm vidpro1)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

