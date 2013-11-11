# This is contrib/mleotta/pro/CMakeLists.txt
# Matt Leotta's Process Library


# include directories into here if needed
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg)


SET(mleotta_pro_sources
   mleotta_video_slice_process.cxx    mleotta_video_slice_process.h
   mleotta_save_keypoints_process.cxx mleotta_save_keypoints_process.h
)

#AUX_SOURCE_DIRECTORY(Templates mleotta_pro_sources)

ADD_LIBRARY(mleotta_pro ${mleotta_pro_sources})

TARGET_LINK_LIBRARIES(mleotta_pro vidpro1 dbdet)



