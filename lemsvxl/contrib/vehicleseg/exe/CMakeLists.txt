# contrib/vehicleseg/test/CMakeLists.txt



include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )


include_directories( ${lems_SOURCE_DIR}/contrib)
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/basic )

set( seg_exe_sources
aerial_vehicle_segmentation.h aerial_vehicle_segmentation.cxx
)
add_library( seg_exe SHARED ${seg_exe_sources})
target_link_libraries( seg_exe  vehicleseg_pro vidpro1 )

set( observation_sources
aerial_vehicle_observation.h aerial_vehicle_observation.cxx
)
add_library( observation_exe SHARED ${observation_sources})

target_link_libraries( observation_exe  dbbgm_pro vidpro1 vsol vil dbinfo dbru)

set( save_boulder_images_exe_sources
        save_boulder_video_to_images.cxx
)

add_executable( save_boulder_images_exe  ${save_boulder_images_exe_sources})
target_link_libraries(save_boulder_images_exe  dbvidl brip vul)

#add_executable( load_boulder_images_exe  ANreader.cxx)
#target_link_libraries(load_boulder_images_exe  dbvidl brip vul vil vgui)

set( fg_exe_sources
aerial_background_foreground_detection.h aerial_background_foreground_detection.cxx
)
add_library( fg_exe SHARED ${fg_exe_sources})
target_link_libraries( fg_exe  dbdet_pro vidpro1 vil bpro1 dbbgm_pro dbsta_algo dbsta dbbgm bil)
