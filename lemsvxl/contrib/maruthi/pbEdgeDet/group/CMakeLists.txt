include_directories(${lems_SOURCE_DIR}/contrib/maruthi/pbEdgeDet/partition)
include_directories(${lems_SOURCE_DIR}/contrib/maruthi/pbEdgeDet/util)

set(group_sources affinity.cc
  color.cc     filterbank.cc
  histogram.cc ic.cc
  nonmax.cc    pb.cc
  region.cc    texture.cc)


add_library(pbGroup ${group_sources})
