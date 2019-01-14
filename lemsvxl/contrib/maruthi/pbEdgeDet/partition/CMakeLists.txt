include_directories(${LEMS_SOURCE_DIR}/contrib/maruthi/pbEdgeDet/util)
set(partition_sources dmatrix.cc smatrix.cc )
 

add_library(pbPartition ${partition_sources})
