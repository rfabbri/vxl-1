# brcv/mvg/dvpgl/algo/cmd/CMakeLists.txt
# Photogrametry Algorithms Command Line Tools.


INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/mvg/bmvl )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )

ADD_EXECUTABLE(dvpgl_optimize_cmd dvpgl_optimize_cmd.cxx)
TARGET_LINK_LIBRARIES(dvpgl_optimize_cmd vpgl_algo brct)
