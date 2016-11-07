set(util_sources 
  configFileLexer.cc configure.cc
  exception.cc       image.cc
  kmeans.cc          message.cc
  random.cc          regex.cc
  segmentation.cc    string.cc
  timer.cc           util.cc
)

add_library(pbUtil ${util_sources})
