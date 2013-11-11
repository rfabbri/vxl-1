# contrib/mleotta/modrec/io/CMakeLists.txt
# Binary IO for modrec


SET(modrec_io_sources
   modrec_io_desc_feature_3d.txx   modrec_io_desc_feature_3d.h 
   modrec_io_codeword.txx          modrec_io_codeword.h
)

AUX_SOURCE_DIRECTORY(Templates modrec_io_sources)

ADD_LIBRARY(modrec_io  ${modrec_io_sources})

TARGET_LINK_LIBRARIES(modrec_io modrec vnl_io vgl_io)

IF( BUILD_TESTING )
#  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
