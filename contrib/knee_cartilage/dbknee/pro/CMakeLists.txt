# This is contrib/knee_cartilage/dbknee/pro/CMakeLists.txt
# Brown Processing Library for the Knee cartilage

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbknee_pro_sources                                               
  # Processes
  dbknee_compute_cylinder_cs_process.h      dbknee_compute_cylinder_cs_process.cxx

  dbknee_separate_inner_outer_surfaces_process.h
  dbknee_separate_inner_outer_surfaces_process.cxx

  dbknee_compute_thickness_process.h
  dbknee_compute_thickness_process.cxx
  
  dbknee_region_thickness_process.h
  dbknee_region_thickness_process.cxx
  
  dbknee_export_thickness_map_process.h
  dbknee_export_thickness_map_process.cxx
  
  dbknee_surface_points_from_contours_process.h
  dbknee_surface_points_from_contours_process.cxx
  
  dbknee_surface_mesh_process.h
  dbknee_surface_mesh_process.cxx
  
  dbknee_plot_cylinder_cs_process.h
  dbknee_plot_cylinder_cs_process.cxx
  
  dbknee_compute_tibia_cs_process.h      dbknee_compute_tibia_cs_process.cxx
  
  dbknee_crop_mesh_process.h             dbknee_crop_mesh_process.cxx
)


ADD_LIBRARY( dbknee_pro  ${dbknee_pro_sources} )

TARGET_LINK_LIBRARIES( dbknee_pro vidpro1 dbknee dbmsh3d_algo dbmsh3d_pro dbdet dbdet_xio)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

