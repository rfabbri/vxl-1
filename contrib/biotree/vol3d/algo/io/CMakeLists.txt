set(vol3d_io_src
  vol3d_reader.h     vol3d_reader.cxx

)


add_library(vol3d_io ${vol3d_io_src})

target_link_libraries(vol3d_io vol3d_algo vol3d)
   
