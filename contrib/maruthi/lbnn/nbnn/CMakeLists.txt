# SIFT library
# brcv/shp/dbsk2d/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/contrib/maruthi )
include_directories( ${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16 )
include_directories( ${lems_SOURCE_DIR}/contrib/maruthi/lbnn )
include_directories( ${lems_SOURCE_DIR}/contrib/maruthi/flann-1.8.4-src/src/cpp )

link_directories(${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16/bin/glnxa64 )
link_directories(/usr/local/boost_1_42_0/stage/lib)

project ( nbnn )

add_executable (lbnn_exec experiment_3.cc )
target_link_libraries ( lbnn_exec protobuf sift_lib gflags glog boost_filesystem boost_system )


# # SIFT library
# # brcv/shp/dbsk2d/CMakeLists.txt

# include_directories( ${lems_SOURCE_DIR}/contrib/maruthi )
# include_directories( ${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16 )

# link_directories(${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16/bin/glnxa64 )
# link_directories(/usr/local/boost_1_42_0/stage/lib)

# project ( sift )

# set( sift_sources

  
#   extractor.h
  
#   sift_descriptors.pb.h sift_descriptors.pb.cc
#                         get_descriptor_info_cli.cc

#   util.h
#   sift_util.h sift_util.cc
  
#   vlfeat_extractor.h vlfeat_extractor.cc
  
  
  
  
 
# )

# add_library(sift_lib ${sift_sources} )
# #target_link_libraries( sift_lib boost_filesystem-mt)

# add_executable (extract_descriptors_cli extract_descriptors_cli.cc )
# target_link_libraries ( extract_descriptors_cli sift_lib boost_system boost_filesystem opencv_imgproc opencv_core opencv_highgui gflags glog  protobuf vl)


