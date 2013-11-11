# contrib/Manifold_extraction/vis/CMakeLists.txt
# Developement for : pradeep

IF(VXL_VGUI_FOUND)
  INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
ENDIF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/biotree)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Manifold_extraction)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )


SET(manifold_extraction_vis_sources

Lie_contour_match_displayer.cxx                Lie_contour_match_displayer.h
Lie_contour_match_tableau.cxx                  Lie_contour_match_tableau.h
                                               Lie_contour_match_tableau_sptr.h
)


ADD_LIBRARY(manifold_extraction_vis ${manifold_extraction_vis_sources})


TARGET_LINK_LIBRARIES(manifold_extraction_vis manifold_extraction_pro sdet bsta brip bugl bsol btol bdgl vsol vil vgl_algo vnl vsl vbl_io vbl vul)


SET(vehicle_model_vis_sources 
vehicle_model_vis_manager.cxx       vehicle_model_vis_manager.h)

ADD_LIBRARY(vehicle_model_vis ${vehicle_model_vis_sources})

TARGET_LINK_LIBRARIES( vehicle_model_vis Manifold_extraction bgui vgui vil vbl vsol)

SET(Lidar_vis_three_box_model_sources 
Lidar_vis_three_box_model.cxx  )

ADD_EXECUTABLE(Lidar_vis_three_box_model ${Lidar_vis_three_box_model_sources})

TARGET_LINK_LIBRARIES( Lidar_vis_three_box_model Manifold_extraction bgui bgui3d vgui vil vbl vsol)

SET(Lidar_vehicle_segmentation_sources 
Lidar_vehicle_segmentation.cxx  )

ADD_EXECUTABLE(Lidar_vehicle_segmentation ${Lidar_vehicle_segmentation_sources})

TARGET_LINK_LIBRARIES( Lidar_vehicle_segmentation Manifold_extraction bgui bgui3d vgui vil vbl vsol)


# SET(stereographic_proj_sources
# stereographic_proj_example.cxx
# )
# ADD_EXECUTABLE(stereographic_proj ${stereographic_proj_sources})
# TARGET_LINK_LIBRARIES(stereographic_proj  biov bgui3d vil vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui)


# SET(stereographic_proj_vrml_example_sources
# stereographic_proj_vrml_example.cxx
# )
# ADD_EXECUTABLE(stereographic_proj_vrml_example ${stereographic_proj_vrml_example_sources})
# TARGET_LINK_LIBRARIES(stereographic_proj_vrml_example  vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui)

# SET(pickup_truck_vrml_plot_sources
# pickup_truck_vrml_plot.cxx
# )
# ADD_EXECUTABLE(pickup_truck_vrml_plot ${pickup_truck_vrml_plot_sources})
# TARGET_LINK_LIBRARIES(pickup_truck_vrml_plot vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui vsol # manifold_extraction bgui3d)

 SET(Lidar_three_box_models_vrml_plot_sources
 Lidar_three_box_models_vrml_plot.cxx
 )
 ADD_EXECUTABLE(Lidar_three_box_models_vrml_plot ${Lidar_three_box_models_vrml_plot_sources})
 TARGET_LINK_LIBRARIES(Lidar_three_box_models_vrml_plot vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui     vsol manifold_extraction)


# SET(geodesics_vrml_plot_sources
# geodesics_vrml_plot.cxx
# )
# ADD_EXECUTABLE(geodesics_vrml_plot ${geodesics_vrml_plot_sources})
# TARGET_LINK_LIBRARIES(geodesics_vrml_plot vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui vsol # manifold_extraction # bgui3d)

# SET(Lidar_geodesics_vrml_plot_sources
# Lidar_geodesics_vrml_plot.cxx
# )
# ADD_EXECUTABLE(Lidar_geodesics_vrml_plot ${Lidar_geodesics_vrml_plot_sources})
# TARGET_LINK_LIBRARIES(Lidar_geodesics_vrml_plot vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui vsol  
# manifold_extraction bgui3d)


# SET(geodesics_movie_sources
# geodesics_movie.cxx
# )
# ADD_EXECUTABLE(geodesics_movie ${geodesics_movie_sources})
# TARGET_LINK_LIBRARIES(geodesics_movie vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui manifold_extraction)

# SET(Lidar_geodesics_movie_sources
# Lidar_geodesics_movie.cxx
# )
# ADD_EXECUTABLE(Lidar_geodesics_movie ${Lidar_geodesics_movie_sources})
# TARGET_LINK_LIBRARIES(Lidar_geodesics_movie vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui manifold_extraction)


SET(vehicle_model_visualizer_sources 
vehicle_model_visualizer.cxx 
vehicle_model_menus.cxx       vehicle_model_menus.h)

ADD_EXECUTABLE (vehicle_model_visualizer ${vehicle_model_visualizer_sources})

TARGET_LINK_LIBRARIES(vehicle_model_visualizer vehicle_model_vis vgui)



SET(outline_matching_visualizer_sources 
outline_matching_visualizer.cxx 
outline_matching_menu.cxx                outline_matching_menu.h

)

ADD_EXECUTABLE (outline_matching_visualizer ${outline_matching_visualizer_sources})

TARGET_LINK_LIBRARIES(outline_matching_visualizer bvis1 dbcvr manifold_extraction manifold_extraction_pro manifold_extraction_vis dbcvr_vis vgui)





