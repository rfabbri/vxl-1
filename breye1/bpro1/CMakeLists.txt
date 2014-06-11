# breye1/bpro1/CMakeLists.txt


SET(bpro1_sources
   bpro1_parameters.cxx                  bpro1_parameters.h                  bpro1_parameters_sptr.h
   bpro1_parameters.txx
   bpro1_process.cxx                     bpro1_process.h                     bpro1_process_sptr.h
   bpro1_storage.cxx                     bpro1_storage.h                     bpro1_storage_sptr.h
)


AUX_SOURCE_DIRECTORY(Templates bpro1_sources)

ADD_LIBRARY(bpro1  ${bpro1_sources})

TARGET_LINK_LIBRARIES(bpro1 vsl vbl)

