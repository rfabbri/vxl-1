# This is contrib/world_model/exe/CMakeLists.txt
# World Modeling GUI executables
include_directories(${LEMS_SOURCE_DIR}/contrib )

add_executable( world_modeler world_modeler.cxx )
target_link_libraries( world_modeler world_model vgui vil )

add_executable( world_lidar_modeler world_lidar_modeler.cxx )
target_link_libraries( world_lidar_modeler world_model vgui  vil )

add_executable( world_hybrid_modeler world_hybrid_modeler.cxx )
target_link_libraries( world_hybrid_modeler world_model vgui vil )

add_executable( world_coin3d_modeler world_coin3d_modeler.cxx )
target_link_libraries( world_coin3d_modeler world_model vgui vil )

