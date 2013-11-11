# This is brcv/mvg/dbdif/algo/CMakeLists.txt
# Brown Multiview Differential Geometry Algorithms Library

SET(dbdif_algo_sources
  dbdif_data.cxx dbdif_data.h
  dbdif_transfer.cxx dbdif_transfer.h
)

ADD_LIBRARY(dbdif_algo ${dbdif_algo_sources})

TARGET_LINK_LIBRARIES(dbdif_algo dbdif)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
