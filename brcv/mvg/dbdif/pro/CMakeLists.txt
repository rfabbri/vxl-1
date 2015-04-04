# This is brcv/mvg/dbdif/pro/CMakeLists.txt
# Brown Multiview Differential Geometry Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbdif_pro_sources
dbdif_3rd_order_geometry_storage.cxx dbdif_3rd_order_geometry_storage.h dbdif_3rd_order_geometry_storage_sptr.h
dbdif_edgel_data_process.cxx dbdif_edgel_data_process.h
)


ADD_LIBRARY(dbdif_pro ${dbdif_pro_sources})

TARGET_LINK_LIBRARIES(dbdif_pro dbdif_algo vidpro1 bpro1)
