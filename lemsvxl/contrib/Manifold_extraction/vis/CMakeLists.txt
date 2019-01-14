# contrib/Manifold_extraction/vis/CMakeLists.txt
# Developement for : pradeep

if(VXL_VGUI_FOUND)
  include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
endif (VXL_VGUI_FOUND)

include_directories( ${LEMS_SOURCE_DIR}/breye)
include_directories( ${LEMS_SOURCE_DIR}/breye1)
include_directories( ${LEMS_SOURCE_DIR}/basic)
include_directories( ${LEMS_SOURCE_DIR}/contrib/biotree)
include_directories( ${LEMS_SOURCE_DIR}/contrib/Manifold_extraction)
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_CORE_INCLUDE_DIR} )

include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

include_directories( ${LEMS_SOURCE_DIR}/contrib )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )


set(manifold_extraction_vis_sources

Lie_contour_match_displayer.cxx                Lie_contour_match_displayer.h
Lie_contour_match_tableau.cxx                  Lie_contour_match_tableau.h
                                               Lie_contour_match_tableau_sptr.h
)


add_library(manifold_extraction_vis ${manifold_extraction_vis_sources})


target_link_libraries(manifold_extraction_vis manifold_extraction_pro sdet bsta brip bugl bsol btol bdgl vsol vil vgl_algo vnl vsl vbl_io vbl vul)


set(vehicle_model_vis_sources 
vehicle_model_vis_manager.cxx       vehicle_model_vis_manager.h)

add_library(vehicle_model_vis ${vehicle_model_vis_sources})

target_link_libraries( vehicle_model_vis Manifold_extraction bgui vgui vil vbl vsol)

set(Lidar_vis_three_box_model_sources 
Lidar_vis_three_box_model.cxx  )

add_executable(Lidar_vis_three_box_model ${Lidar_vis_three_box_model_sources})

target_link_libraries( Lidar_vis_three_box_model Manifold_extraction bgui bgui3d vgui vil vbl vsol)

set(Lidar_vehicle_segmentation_sources 
Lidar_vehicle_segmentation.cxx  )

add_executable(Lidar_vehicle_segmentation ${Lidar_vehicle_segmentation_sources})

target_link_libraries( Lidar_vehicle_segmentation Manifold_extraction bgui bgui3d vgui vil vbl vsol)


# set(stereographic_proj_sources
# stereographic_proj_example.cxx
# )
# add_executable(stereographic_proj ${stereographic_proj_sources})
# target_link_libraries(stereographic_proj  biov bgui3d vil vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui)


# set(stereographic_proj_vrml_example_sources
# stereographic_proj_vrml_example.cxx
# )
# add_executable(stereographic_proj_vrml_example ${stereographic_proj_vrml_example_sources})
# target_link_libraries(stereographic_proj_vrml_example  vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui)

# set(pickup_truck_vrml_plot_sources
# pickup_truck_vrml_plot.cxx
# )
# add_executable(pickup_truck_vrml_plot ${pickup_truck_vrml_plot_sources})
# target_link_libraries(pickup_truck_vrml_plot vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui vsol # manifold_extraction bgui3d)

 set(Lidar_three_box_models_vrml_plot_sources
 Lidar_three_box_models_vrml_plot.cxx
 )
 add_executable(Lidar_three_box_models_vrml_plot ${Lidar_three_box_models_vrml_plot_sources})
 target_link_libraries(Lidar_three_box_models_vrml_plot vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui     vsol manifold_extraction)


# set(geodesics_vrml_plot_sources
# geodesics_vrml_plot.cxx
# )
# add_executable(geodesics_vrml_plot ${geodesics_vrml_plot_sources})
# target_link_libraries(geodesics_vrml_plot vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui vsol # manifold_extraction # bgui3d)

# set(Lidar_geodesics_vrml_plot_sources
# Lidar_geodesics_vrml_plot.cxx
# )
# add_executable(Lidar_geodesics_vrml_plot ${Lidar_geodesics_vrml_plot_sources})
# target_link_libraries(Lidar_geodesics_vrml_plot vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui vsol  
# manifold_extraction bgui3d)


# set(geodesics_movie_sources
# geodesics_movie.cxx
# )
# add_executable(geodesics_movie ${geodesics_movie_sources})
# target_link_libraries(geodesics_movie vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui manifold_extraction)

# set(Lidar_geodesics_movie_sources
# Lidar_geodesics_movie.cxx
# )
# add_executable(Lidar_geodesics_movie ${Lidar_geodesics_movie_sources})
# target_link_libraries(Lidar_geodesics_movie vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui manifold_extraction)


set(vehicle_model_visualizer_sources 
vehicle_model_visualizer.cxx 
vehicle_model_menus.cxx       vehicle_model_menus.h)

add_executable (vehicle_model_visualizer ${vehicle_model_visualizer_sources})

target_link_libraries(vehicle_model_visualizer vehicle_model_vis vgui)



set(outline_matching_visualizer_sources 
outline_matching_visualizer.cxx 
outline_matching_menu.cxx                outline_matching_menu.h

)

add_executable (outline_matching_visualizer ${outline_matching_visualizer_sources})

target_link_libraries(outline_matching_visualizer bvis1 dbcvr manifold_extraction manifold_extraction_pro manifold_extraction_vis dbcvr_vis vgui)





