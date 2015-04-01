# This is brcv/mvg/dvpgl/pro/algo/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET( dvpgl_algo_pro_sources
   dvpgl_vsol_lens_warp_process.cxx    dvpgl_vsol_lens_warp_process.h
   dvpgl_image_lens_warp_process.cxx   dvpgl_image_lens_warp_process.h
)


ADD_LIBRARY(dvpgl_algo_pro ${dvpgl_algo_pro_sources})

TARGET_LINK_LIBRARIES(dvpgl_algo_pro vpgl_algo bpgl_algo vidpro1 )


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

