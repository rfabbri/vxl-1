# This is brcv/trk/dbctrk/pro/CMakeLists.txt
# Brown curvetracking Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

SET(dbctrk_pro_sources
   dbctrk_process.cxx          dbctrk_process.h
   #dbctrk_process2.cxx          dbctrk_process2.h
   #dbctrk_pairing_curves_process.cxx          dbctrk_pairing_curves_process.h
   dbctrk_storage.cxx         dbctrk_storage.h        dbctrk_storage_sptr.h
   #dbctrk_curve_detector_process.h dbctrk_curve_detector_process.cxx
)


ADD_LIBRARY(dbctrk_pro ${dbctrk_pro_sources})

TARGET_LINK_LIBRARIES(dbctrk_pro dbctrk  vidpro1 dbdet_pro)



