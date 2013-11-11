# This is brcv/rec/dbru/pro/CMakeLists.txt
# Brown Recognition Tools to aid Testing

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SET(dbru_pro_sources 

#storage classes
dbru_rcor_storage_sptr.h
dbru_rcor_storage.h               dbru_rcor_storage.cxx
dbru_osl_storage_sptr.h
dbru_osl_storage.h               dbru_osl_storage.cxx


#processes
#dbru_polygon_process.h           dbru_polygon_process.cxx
dbru_dbinfo_process.h             dbru_dbinfo_process.cxx
dbru_mutual_info_process.h        dbru_mutual_info_process.cxx
dbru_compute_rcor_process.h       dbru_compute_rcor_process.cxx
dbru_print_rcor_process.h         dbru_print_rcor_process.cxx
dbru_load_osl_process.h           dbru_load_osl_process.cxx
dbru_load_polygons_process.h      dbru_load_polygons_process.cxx
dbru_save_osl_process.h           dbru_save_osl_process.cxx
dbru_create_empty_osl_process.h   dbru_create_empty_osl_process.cxx
#dbru_run_osl_process.h            dbru_run_osl_process.cxx
)

ADD_LIBRARY(dbru_pro ${dbru_pro_sources})

TARGET_LINK_LIBRARIES(dbru_pro bvis1 dbdet dbru_algo dbskr dbsk2d_algo)

