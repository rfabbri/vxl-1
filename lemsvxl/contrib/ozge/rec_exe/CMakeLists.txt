# contrib/ozge/rec_exe/CMakeLists.txt

include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
#include_directories(${VXL_OXL_INCLUDE_DIR} )
#include_directories(${VXL_MUL_INCLUDE_DIR} )

include_directories( ${LEMS_SOURCE_DIR}/contrib)
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
#include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/contrib/Gregistration )

set( rec_exe_sources
aerial_vehicle_recognition.h aerial_vehicle_recognition.cxx
)

add_library( rec_exe SHARED ${rec_exe_sources})
target_link_libraries(rec_exe vidpro1 vil dbru_algo dbru_pro dbsk2d_algo)

#set( rec_exe_normal_sources
#recognition_main.cxx
#)

#add_executable( rec_exe_normal ${rec_exe_normal_sources})
#target_link_libraries(rec_exe_normal vidpro1 vil dbru_algo dbsk2d_algo)

#set( rec_exe_vidl_sources
#recognition_main_vidl.cxx
#)
#add_executable( rec_exe_vidl ${rec_exe_vidl_sources})
#target_link_libraries(rec_exe_vidl vidpro1 vidl vil dbru_algo dbsk2d_algo)

set( rec_exe_shock_sources
recognition_main_shock.cxx
)
add_executable( rec_exe_shock ${rec_exe_shock_sources})
target_link_libraries(rec_exe_shock vidpro1 vil dbru_algo dbsk2d_algo)

set( rec_exe_curve_sources
recognition_main_curve.cxx
)
add_executable( rec_exe_curve ${rec_exe_curve_sources})
target_link_libraries(rec_exe_curve vidpro1 vil dbru_algo dbsk2d_algo)

set( rec_exe_rigid_sources
recognition_main_rigid.cxx
)
add_executable( rec_exe_rigid ${rec_exe_rigid_sources})
target_link_libraries(rec_exe_rigid vidpro1 vil dbru_algo dbru_pro dbru_vis dbsk2d_algo)



