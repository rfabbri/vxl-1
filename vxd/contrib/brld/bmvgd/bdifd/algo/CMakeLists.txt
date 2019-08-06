# This is bmvgd/bdifd/algo/CMakeLists.txt
# Brown Multiview Differential Geometry Algorithms Library

project(bdifd_algo)

set(bdifd_algo_sources
  bdifd_data.cxx bdifd_data.h
  bdifd_transfer.cxx bdifd_transfer.h
)

add_library(bdifd_algo ${bdifd_algo_sources})

if ( USE_Boost )
  set(Boost_USE_STATIC_LIBS   OFF)
  find_package( Boost 1.35.0 COMPONENTS iostreams)
endif( USE_Boost )

if ( Boost_FOUND )
  target_link_libraries( bdifd_algo bdifd ${Boost_LIBRARIES} )
  include_directories( ${Boost_INCLUDE_DIRS} )
  #  message("FOUND YEAH" "${Boost_LIBRARIES}" "${Boost_INCLUDE_DIRS}" )
else ( Boost_FOUND )
  #message("FOUND NO")
  target_link_libraries(bdifd_algo bdifd)
endif( Boost_FOUND )

configure_file( ${bdifd_algo_SOURCE_DIR}/bdifd_algo_config.h.in
                ${bdifd_algo_BINARY_DIR}/bdifd_algo_config.h )


if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
