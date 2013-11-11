# This is brcv/rec/dbru/pro/CMakeLists.txt
# Brown Recognition Tools to aid Testing

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SET(face_pro_sources 

#storage classes
dbru_facedb_storage_sptr.h
dbru_facedb_storage.h               dbru_facedb_storage.cxx


#processes
dbru_load_facedb_process.h           dbru_load_facedb_process.cxx
dbru_save_facedb_process.h           dbru_save_facedb_process.cxx
dbru_create_empty_facedb_process.h   dbru_create_empty_facedb_process.cxx
)

ADD_LIBRARY(face_pro ${face_pro_sources})

TARGET_LINK_LIBRARIES(face_pro bvis1 dbdet dbru_algo dbskr dbsk2d_algo)

