# breye/bpro/CMakeLists.txt


set(bpro_sources
   bpro_parameters.cxx                  bpro_parameters.h                  bpro_parameters_sptr.h
   bpro_parameters.hxx
   bpro_process_manager.hxx             bpro_process_manager.h 
   bpro_mview_process_manager.cxx       bpro_mview_process_manager.h
   bpro_process.cxx                     bpro_process.h                     bpro_process_sptr.h
   bpro_storage.cxx                     bpro_storage.h                     bpro_storage_sptr.h

   bpro_storage_registry.cxx            bpro_storage_registry.h              
   bpro_mview_dbutils.cxx               bpro_mview_dbutils.h
)


aux_source_directory(Templates bpro_sources)

add_library(bpro ${bpro_sources})

target_link_libraries(bpro vbl)

