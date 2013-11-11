# This is brcv/mvg/dbmrf/pro/CMakeLists.txt
# Brown Markov Random Field Track Processing Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbmrf_pro_sources
   dbmrf_network_builder_process.cxx    dbmrf_network_builder_process.h
   dbmrf_curve_3d_builder_process.cxx   dbmrf_curve_3d_builder_process.h
   dbmrf_curve_projector_process.cxx    dbmrf_curve_projector_process.h
   dbmrf_predict_process.cxx            dbmrf_predict_process.h
   dbmrf_evaluate_process.cxx           dbmrf_evaluate_process.h
   dbmrf_backproject_process.cxx        dbmrf_backproject_process.h
   dbmrf_rocdata_process.cxx            dbmrf_rocdata_process.h
   
   dbmrf_bmrf_storage.cxx          dbmrf_bmrf_storage.h          dbmrf_bmrf_storage_sptr.h
   dbmrf_curvel_3d_storage.cxx     dbmrf_curvel_3d_storage.h     dbmrf_curvel_3d_storage_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates dbmrf_pro_sources)

ADD_LIBRARY(dbmrf_pro ${dbmrf_pro_sources})

TARGET_LINK_LIBRARIES(dbmrf_pro bmrf vidpro1)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

