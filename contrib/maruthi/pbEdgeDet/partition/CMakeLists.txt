INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib/maruthi/pbEdgeDet/util)
SET(partition_sources dmatrix.cc smatrix.cc )
 

ADD_LIBRARY(pbPartition ${partition_sources})
