# This is contrib/mleotta/pro/CMakeLists.txt
# Matt Leotta's Process Library


# include directories into here if needed
include_directories( ${lems_SOURCE_DIR}/breye1)
include_directories( ${lems_SOURCE_DIR}/brcv/seg)


set(mleotta_pro_sources
   mleotta_video_slice_process.cxx    mleotta_video_slice_process.h
   mleotta_save_keypoints_process.cxx mleotta_save_keypoints_process.h
)

#aux_source_directory(Templates mleotta_pro_sources)

add_library(mleotta_pro ${mleotta_pro_sources})

target_link_libraries(mleotta_pro vidpro1 dbdet)



