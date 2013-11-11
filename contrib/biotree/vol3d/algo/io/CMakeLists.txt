SET(vol3d_io_src
  vol3d_reader.h     vol3d_reader.cxx

)


ADD_LIBRARY(vol3d_io ${vol3d_io_src})

TARGET_LINK_LIBRARIES(vol3d_io vol3d_algo vol3d)
   
