# This is lemsvxlsrc/contrib/dec/psm/algo/pro/CMakeLists.txt
# psm_algo's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily

if(VXL_EXPAT_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bpro )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${lems_SOURCE_DIR}/contrib/dec )

set(psm_algo_pro_sources
    psm_algo_processes.h
    psm_algo_register.h  psm_algo_register.cxx   
)

aux_source_directory(processes psm_algo_pro_sources)

add_library(psm_algo_pro ${psm_algo_pro_sources})

target_link_libraries(psm_algo_pro psm psm_io psm_algo bprb vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vil vil_io vil_pro vnl vnl_io bgeo vgl brdb)
if(PSM_USE_OPENGL2_1)
  message("glew libaray = " ${GLEW_LIBRARY})
  target_link_libraries(psm_algo_pro ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARY})
endif(PSM_USE_OPENGL2_1)

#if( BUILD_TESTING )
#  add_subdirectory(tests)
#endif( BUILD_TESTING )
endif(VXL_EXPAT_FOUND)
