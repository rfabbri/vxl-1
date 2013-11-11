# contrib/vehicleseg/test/CMakeLists.txt



INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

SET( seg_exe_sources
aerial_vehicle_segmentation.h aerial_vehicle_segmentation.cxx
)
ADD_LIBRARY( seg_exe SHARED ${seg_exe_sources})
TARGET_LINK_LIBRARIES( seg_exe  vehicleseg_pro vidpro1 )

SET( observation_sources
aerial_vehicle_observation.h aerial_vehicle_observation.cxx
)
ADD_LIBRARY( observation_exe SHARED ${observation_sources})

TARGET_LINK_LIBRARIES( observation_exe  dbbgm_pro vidpro1 vsol vil dbinfo dbru)

SET( save_boulder_images_exe_sources
        save_boulder_video_to_images.cxx
)

ADD_EXECUTABLE( save_boulder_images_exe  ${save_boulder_images_exe_sources})
TARGET_LINK_LIBRARIES(save_boulder_images_exe  dbvidl brip vul)

#ADD_EXECUTABLE( load_boulder_images_exe  ANreader.cxx)
#TARGET_LINK_LIBRARIES(load_boulder_images_exe  dbvidl brip vul vil vgui)

SET( fg_exe_sources
aerial_background_foreground_detection.h aerial_background_foreground_detection.cxx
)
ADD_LIBRARY( fg_exe SHARED ${fg_exe_sources})
TARGET_LINK_LIBRARIES( fg_exe  dbdet_pro vidpro1 vil bpro1 dbbgm_pro dbsta_algo dbsta dbbgm bil)
