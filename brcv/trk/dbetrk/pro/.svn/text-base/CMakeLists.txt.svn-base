# This is brcv/trk/dbetrk/pro/CMakeLists.txt
# Brown edgetracking Processing library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(dbetrk_pro_sources
   dbetrk_process.cxx          dbetrk_process.h
   dbetrk_storage.cxx         dbetrk_storage.h        dbetrk_storage_sptr.h
)


ADD_LIBRARY(dbetrk_pro ${dbetrk_pro_sources})

TARGET_LINK_LIBRARIES(dbetrk_pro dbetrk vidpro1)



