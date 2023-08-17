# This is bsegd/sdetd/io/CMakeLists.txt

find_package( Boost COMPONENTS iostreams)

configure_file( sdetd_io_config.h.in sdetd_io_config.h)

set(sdetd_io_sources
  sdetd_load_edg.cxx sdetd_load_edg.h
  sdetd_cvlet_map_io.cxx sdetd_cvlet_map_io.h
)

# aux_source_directory(Templates sdetd_io_sources)

add_library(sdetd_io ${sdetd_io_sources})
if(Boost_FOUND)
  #set(HAS_BOOST ON)
  target_link_libraries(sdetd_io sdet ${Boost_LIBRARIES})
  include_directories(${Boost_LIBRARIES})
else()
  target_link_libraries(sdetd_io sdet)
endif()

if( BUILD_TESTING )
  # add_subdirectory(tests)
endif( BUILD_TESTING )
