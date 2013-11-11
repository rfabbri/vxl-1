# world modeling application
# contrib/world_model/CMakeLists.txt

IF(VXL_BGUI3D_FOUND)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_TIFF_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEOTIFF_INCLUDE_DIR} ) 

ADD_DEFINITIONS ( -DCOIN_NOT_DLL )
INCLUDE_DIRECTORIES( ${VXL_COIN3D_INCLUDE_DIR} )

SET(world_model_sources
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


#ADD_EXECUTABLE( world_model ${world_model_sources} )
ADD_LIBRARY(world_model ${world_model_sources})

TARGET_LINK_LIBRARIES( world_model dbmsh3d dbmsh3d_algo dbmsh3d_vis vil vbl vgl vgl_algo vnl vsol vpgl vpgl_algo bgui bgeo)

SUBDIRS(exe)

SUBDIRS(tests)

ENDIF(VXL_BGUI3D_FOUND)
