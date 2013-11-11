# contrib/Manifold_extraction/cmd/CMakeLists.txt
# Developement for : pradeep

IF(VXL_VGUI_FOUND)
  INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
ENDIF (VXL_VGUI_FOUND)


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/biotree)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Manifold_extraction)
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )



# SET(Lie_group_proj_sources
# box_model_proj_example.cxx
# )
# ADD_EXECUTABLE(Lie_group_proj_example ${Lie_group_proj_sources})
# TARGET_LINK_LIBRARIES(Lie_group_proj_example  vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui vsol # Manifold_extraction)




# SET(vehicle_classifier_example_sources
# vehicle_classifier_example.cxx
# )
# ADD_EXECUTABLE(vehicle_classifier_example ${vehicle_classifier_example_sources})
# TARGET_LINK_LIBRARIES(vehicle_classifier_example vil osl vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui # Manifold_extraction)

SET(intrinsic_mean_example_sources
intrinsic_mean_example.cxx

)
ADD_EXECUTABLE(intrinsic_mean_example ${intrinsic_mean_example_sources})

TARGET_LINK_LIBRARIES(intrinsic_mean_example vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl Manifold_extraction)

# SET(optimal_geodesic_example_sources
# optimal_geodesic_example.cxx
# )
# ADD_EXECUTABLE(optimal_geodesic_example ${optimal_geodesic_example_sources})
# TARGET_LINK_LIBRARIES(optimal_geodesic_example vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl # Manifold_extraction)


SET(pga_vehicle_model_sources
pga_vehicle_model.cxx

)
ADD_EXECUTABLE(pga_vehicle_model ${pga_vehicle_model_sources})

TARGET_LINK_LIBRARIES(pga_vehicle_model vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl Manifold_extraction)

SET(pga_lidar_data_sources
pga_lidar_data.cxx

)
ADD_EXECUTABLE(pga_lidar_data ${pga_lidar_data_sources})

TARGET_LINK_LIBRARIES(pga_lidar_data vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl Manifold_extraction)


# SET(non_class_models_sources
# non_class_models.cxx
# )
# ADD_EXECUTABLE(non_class_models ${non_class_models_sources})
# TARGET_LINK_LIBRARIES(non_class_models vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl Manifold_extraction)

SET(outline_matching_sources
outline_matching.cxx

)
ADD_EXECUTABLE(outline_matching ${outline_matching_sources})

TARGET_LINK_LIBRARIES(outline_matching vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl dbcvr dbdp dbsol dbsol_algo  Manifold_extraction)

SET(outline_batch_example_sources
outline_batch_example.cxx

)

ADD_EXECUTABLE(outline_batch_example ${outline_batch_example_sources})

TARGET_LINK_LIBRARIES(outline_batch_example vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl dbsol dbsol_algo  Manifold_extraction)

