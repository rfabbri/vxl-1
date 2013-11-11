# SIFT library
# brcv/shp/dbsk2d/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16 )

LINK_DIRECTORIES(${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16/bin/glnxa64 )
LINK_DIRECTORIES(/usr/local/boost_1_42_0/stage/lib)

PROJECT ( sift )

SET( sift_sources

  
  extractor.h
  
  sift_descriptors.pb.h sift_descriptors.pb.cc
                        get_descriptor_info_cli.cc

  util.h
  sift_util.h sift_util.cc
  
  vlfeat_extractor.h vlfeat_extractor.cc
  
  
  
  
 
)

ADD_LIBRARY(sift_lib ${sift_sources} )
#TARGET_LINK_LIBRARIES( sift_lib boost_filesystem-mt)

ADD_EXECUTABLE (extract_descriptors_cli extract_descriptors_cli.cc )
TARGET_LINK_LIBRARIES ( extract_descriptors_cli sift_lib boost_system boost_filesystem opencv_imgproc opencv_core opencv_highgui gflags glog  protobuf vl)


