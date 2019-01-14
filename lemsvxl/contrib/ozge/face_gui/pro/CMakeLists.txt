# This is brcv/rec/dbru/pro/CMakeLists.txt
# Brown Recognition Tools to aid Testing

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

set(face_pro_sources 

#storage classes
dbru_facedb_storage_sptr.h
dbru_facedb_storage.h               dbru_facedb_storage.cxx


#processes
dbru_load_facedb_process.h           dbru_load_facedb_process.cxx
dbru_save_facedb_process.h           dbru_save_facedb_process.cxx
dbru_create_empty_facedb_process.h   dbru_create_empty_facedb_process.cxx
)

add_library(face_pro ${face_pro_sources})

target_link_libraries(face_pro bvis1 dbdet dbru_algo dbskr dbsk2d_algo)

