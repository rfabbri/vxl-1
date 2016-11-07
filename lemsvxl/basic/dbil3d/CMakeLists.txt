# /basic/dbil3d/CMakeLists.txt
# Development Brown Image3d Library


add_subdirectory( algo )

add_subdirectory( pro )

#add_subdirectory( vis )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
