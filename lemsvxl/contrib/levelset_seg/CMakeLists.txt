# contrib/lems_levelset_seg/CMakeLists.txt
# Developement for : lems_levelset_seg

include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/contrib/levelset_seg )

# my bvis1 based GUI
add_subdirectory( gui )

# my command line executables
add_subdirectory( cmd )

# Active contour models
# add_subdirectory( dbacm )
