# This is contrib/medial_livewire/CMakeLists.txt
# Developement for : medial_livewire

include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR})
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/db3p/expatpp/expat/lib
 )

# my bvis1 based GUI

add_subdirectory( gui )

add_executable(example_compute_fish_sdt example_compute_fish_sdt.cxx)
target_link_libraries(example_compute_fish_sdt vil dbil dbil_algo dbsol )

# my command line executables
# add_subdirectory( cmd )
