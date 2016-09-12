# world modeling application
# contrib/world_model/CMakeLists.txt

if(VXL_BGUI3D_FOUND)

include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_TIFF_INCLUDE_DIR} )
include_directories( ${VXL_GEOTIFF_INCLUDE_DIR} ) 

add_definitions ( -DCOIN_NOT_DLL )
include_directories( ${VXL_COIN3D_INCLUDE_DIR} )

set(world_model_sources
   poly_menu.cxx            poly_menu.h
   poly_lidar_menu.cxx      poly_lidar_menu.h
   poly_hybrid_menu.cxx     poly_hybrid_menu.h
   poly_coin3d_menu.cxx     poly_coin3d_menu.h
   poly_manager.cxx         poly_manager.h
   poly_lidar_manager.cxx   poly_lidar_manager.h
   poly_hybrid_manager.cxx  poly_hybrid_manager.h
   poly_coin3d_manager.cxx  poly_coin3d_manager.h
   poly_observer.cxx        poly_observer.h 
   poly_cam_observer.cxx    poly_cam_observer.h
   poly_lidar_observer.cxx  poly_lidar_observer.h
   poly_coin3d_observer.cxx poly_coin3d_observer.h
   poly_coin3d_proj_observer.cxx poly_coin3d_proj_observer.h
                            poly_coin3d_proj_simple_observer.h
                            observable.h
   obj_observable.cxx       obj_observable.h
   texture_map_generator.cxx texture_map_generator.h
   poly_utils.cxx           poly_utils.h
   utm_lat_lon.cxx          utm_lat_lon.h
   vpgl_rat_proj_camera.cxx vpgl_rat_proj_camera.h
)


#add_executable( world_model ${world_model_sources} )
add_library(world_model ${world_model_sources})

target_link_libraries( world_model dbmsh3d dbmsh3d_algo dbmsh3d_vis vil vbl vgl vgl_algo vnl vsol vpgl vpgl_algo bgui bgeo)

subdirs(exe)

subdirs(tests)

endif(VXL_BGUI3D_FOUND)
