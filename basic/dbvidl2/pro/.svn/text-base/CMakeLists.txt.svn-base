# basic/dbvidl2/pro/CMakeLists.txt
# Development Brown Video Processing Extention Library


SET( dbvidl2_pro_sources
   dbvidl2_source.cxx              dbvidl2_source.h
   dbvidl2_multi_source.cxx        dbvidl2_multi_source.h
   dbvidl2_sink.cxx                dbvidl2_sink.h
   dbvidl2_frame_to_resource.cxx   dbvidl2_frame_to_resource.h
   dbvidl2_resource_to_frame.cxx   dbvidl2_resource_to_frame.h
)

ADD_LIBRARY(dbvidl2_pro  ${dbvidl2_pro_sources})
TARGET_LINK_LIBRARIES(dbvidl2_pro dbpro vidl)

