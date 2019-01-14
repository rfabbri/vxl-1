# SIFT library
# brcv/shp/dbsk2d/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/contrib/maruthi )
include_directories( ${LEMS_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16 )

link_directories(${LEMS_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16/bin/glnxa64 )
link_directories(/usr/local/boost_1_42_0/stage/lib)

project ( sift )

set( sift_sources

  
  extractor.h
  
  sift_descriptors.pb.h sift_descriptors.pb.cc
                        get_descriptor_info_cli.cc

  util.h
  sift_util.h sift_util.cc
  
  vlfeat_extractor.h vlfeat_extractor.cc
  
  
  
  
 
)

add_library(sift_lib ${sift_sources} )
#target_link_libraries( sift_lib boost_filesystem-mt)

add_executable (extract_descriptors_cli extract_descriptors_cli.cc )
target_link_libraries ( extract_descriptors_cli sift_lib boost_system boost_filesystem opencv_imgproc opencv_core opencv_highgui gflags glog  protobuf vl)


