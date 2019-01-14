# This is brcv/trk/dbctrk/pro/CMakeLists.txt
# Brown curvetracking Processing library

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )

set(dbctrk_pro_sources
   dbctrk_process.cxx          dbctrk_process.h
   #dbctrk_process2.cxx          dbctrk_process2.h
   #dbctrk_pairing_curves_process.cxx          dbctrk_pairing_curves_process.h
   dbctrk_storage.cxx         dbctrk_storage.h        dbctrk_storage_sptr.h
   #dbctrk_curve_detector_process.h dbctrk_curve_detector_process.cxx
)


add_library(dbctrk_pro ${dbctrk_pro_sources})

target_link_libraries(dbctrk_pro dbctrk  vidpro1 dbdet_pro)



