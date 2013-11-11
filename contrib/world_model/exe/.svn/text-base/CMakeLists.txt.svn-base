# This is contrib/world_model/exe/CMakeLists.txt
# World Modeling GUI executables
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib )

ADD_EXECUTABLE( world_modeler world_modeler.cxx )
TARGET_LINK_LIBRARIES( world_modeler world_model vgui vil )

ADD_EXECUTABLE( world_lidar_modeler world_lidar_modeler.cxx )
TARGET_LINK_LIBRARIES( world_lidar_modeler world_model vgui  vil )

ADD_EXECUTABLE( world_hybrid_modeler world_hybrid_modeler.cxx )
TARGET_LINK_LIBRARIES( world_hybrid_modeler world_model vgui vil )

ADD_EXECUTABLE( world_coin3d_modeler world_coin3d_modeler.cxx )
TARGET_LINK_LIBRARIES( world_coin3d_modeler world_model vgui vil )

