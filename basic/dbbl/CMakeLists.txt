# /basic/dbbl/CMakeLists.txt
# Development Brown Basics (templates) Library

# SET(dbbl_sources 
#   dbbl_subsequence.h
#    )
# 
# #AUX_SOURCE_DIRECTORY(Templates dbbl_sources)
# 
# ADD_LIBRARY(dbbl  ${dbbl_sources})
# 
# TARGET_LINK_LIBRARIES(dbbl vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
