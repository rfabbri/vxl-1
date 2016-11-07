# brl/bseg/boxm/algo/CMakeLists.txt

set(boxm_algo_sources
    boxm_split_sample.h
    dummy.cxx
   )

#aux_source_directory(Templates boxm_opt_sources)

add_library(boxm_algo ${boxm_algo_sources})

target_link_libraries(boxm_algo boxm)

#processes
add_subdirectory(pro)

