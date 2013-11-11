# contrib/ozge/rec_exe/CMakeLists.txt

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )

SET( rec_exe_sources
aerial_vehicle_recognition.h aerial_vehicle_recognition.cxx
)

ADD_LIBRARY( rec_exe SHARED ${rec_exe_sources})
TARGET_LINK_LIBRARIES(rec_exe vidpro1 vil dbru_algo dbru_pro dbsk2d_algo)

#SET( rec_exe_normal_sources
#recognition_main.cxx
#)

#ADD_EXECUTABLE( rec_exe_normal ${rec_exe_normal_sources})
#TARGET_LINK_LIBRARIES(rec_exe_normal vidpro1 vil dbru_algo dbsk2d_algo)

#SET( rec_exe_vidl_sources
#recognition_main_vidl.cxx
#)
#ADD_EXECUTABLE( rec_exe_vidl ${rec_exe_vidl_sources})
#TARGET_LINK_LIBRARIES(rec_exe_vidl vidpro1 vidl vil dbru_algo dbsk2d_algo)

SET( rec_exe_shock_sources
recognition_main_shock.cxx
)
ADD_EXECUTABLE( rec_exe_shock ${rec_exe_shock_sources})
TARGET_LINK_LIBRARIES(rec_exe_shock vidpro1 vil dbru_algo dbsk2d_algo)

SET( rec_exe_curve_sources
recognition_main_curve.cxx
)
ADD_EXECUTABLE( rec_exe_curve ${rec_exe_curve_sources})
TARGET_LINK_LIBRARIES(rec_exe_curve vidpro1 vil dbru_algo dbsk2d_algo)

SET( rec_exe_rigid_sources
recognition_main_rigid.cxx
)
ADD_EXECUTABLE( rec_exe_rigid ${rec_exe_rigid_sources})
TARGET_LINK_LIBRARIES(rec_exe_rigid vidpro1 vil dbru_algo dbru_pro dbru_vis dbsk2d_algo)



