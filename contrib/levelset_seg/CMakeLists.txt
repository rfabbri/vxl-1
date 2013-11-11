# contrib/lems_levelset_seg/CMakeLists.txt
# Developement for : lems_levelset_seg

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/levelset_seg )

# my bvis1 based GUI
SUBDIRS( gui )

# my command line executables
SUBDIRS( cmd )

# Active contour models
# SUBDIRS ( dbacm )
