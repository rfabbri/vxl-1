# contrib/Manifold_extraction/cmd/CMakeLists.txt
# Developement for : pradeep

if(VXL_VGUI_FOUND)
  include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
endif (VXL_VGUI_FOUND)


include_directories( ${lems_SOURCE_DIR}/breye)
include_directories( ${lems_SOURCE_DIR}/basic)
include_directories( ${lems_SOURCE_DIR}/contrib/biotree)
include_directories( ${lems_SOURCE_DIR}/brcv/rec)
include_directories( ${lems_SOURCE_DIR}/contrib/Manifold_extraction)
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )



# set(Lie_group_proj_sources
# box_model_proj_example.cxx
# )
# add_executable(Lie_group_proj_example ${Lie_group_proj_sources})
# target_link_libraries(Lie_group_proj_example  vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui vsol # Manifold_extraction)




# set(vehicle_classifier_example_sources
# vehicle_classifier_example.cxx
# )
# add_executable(vehicle_classifier_example ${vehicle_classifier_example_sources})
# target_link_libraries(vehicle_classifier_example vil osl vgl_algo vnl vsl vbl_io vbl vul  vbl vgl vcl vgui # Manifold_extraction)

set(intrinsic_mean_example_sources
intrinsic_mean_example.cxx

)
add_executable(intrinsic_mean_example ${intrinsic_mean_example_sources})

target_link_libraries(intrinsic_mean_example vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl Manifold_extraction)

# set(optimal_geodesic_example_sources
# optimal_geodesic_example.cxx
# )
# add_executable(optimal_geodesic_example ${optimal_geodesic_example_sources})
# target_link_libraries(optimal_geodesic_example vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl # Manifold_extraction)


set(pga_vehicle_model_sources
pga_vehicle_model.cxx

)
add_executable(pga_vehicle_model ${pga_vehicle_model_sources})

target_link_libraries(pga_vehicle_model vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl Manifold_extraction)

set(pga_lidar_data_sources
pga_lidar_data.cxx

)
add_executable(pga_lidar_data ${pga_lidar_data_sources})

target_link_libraries(pga_lidar_data vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl Manifold_extraction)


# set(non_class_models_sources
# non_class_models.cxx
# )
# add_executable(non_class_models ${non_class_models_sources})
# target_link_libraries(non_class_models vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl Manifold_extraction)

set(outline_matching_sources
outline_matching.cxx

)
add_executable(outline_matching ${outline_matching_sources})

target_link_libraries(outline_matching vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl dbcvr dbdp dbsol dbsol_algo  Manifold_extraction)

set(outline_batch_example_sources
outline_batch_example.cxx

)

add_executable(outline_batch_example ${outline_batch_example_sources})

target_link_libraries(outline_batch_example vgl_algo vnl vsl vsol vbl_io vbl vul vgl vcl dbsol dbsol_algo  Manifold_extraction)

