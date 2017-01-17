# This is brcv/trk/dbetrk/pro/CMakeLists.txt
# Brown edgetracking Processing library

include_directories( ${lems_SOURCE_DIR}/breye1 )

set(dbetrk_pro_sources
   dbetrk_process.cxx          dbetrk_process.h
   dbetrk_storage.cxx         dbetrk_storage.h        dbetrk_storage_sptr.h
)


add_library(dbetrk_pro ${dbetrk_pro_sources})

target_link_libraries(dbetrk_pro dbetrk vidpro1)



