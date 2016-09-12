# /basic/dbil3d/CMakeLists.txt
# Development Brown Image3d Library


subdirs( algo )

subdirs( pro )

#subdirs( vis )

if( BUILD_TESTING )
  subdirs(tests)
endif( BUILD_TESTING )
