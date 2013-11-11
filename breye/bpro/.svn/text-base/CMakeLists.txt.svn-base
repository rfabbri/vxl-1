# breye/bpro/CMakeLists.txt


SET(bpro_sources
   bpro_parameters.cxx                  bpro_parameters.h                  bpro_parameters_sptr.h
   bpro_parameters.txx
   bpro_process_manager.txx             bpro_process_manager.h 
   bpro_mview_process_manager.cxx       bpro_mview_process_manager.h
   bpro_process.cxx                     bpro_process.h                     bpro_process_sptr.h
   bpro_storage.cxx                     bpro_storage.h                     bpro_storage_sptr.h

   bpro_storage_registry.cxx            bpro_storage_registry.h              
   bpro_mview_dbutils.cxx               bpro_mview_dbutils.h
)


AUX_SOURCE_DIRECTORY(Templates bpro_sources)

ADD_LIBRARY(bpro ${bpro_sources})

TARGET_LINK_LIBRARIES(bpro vbl)

