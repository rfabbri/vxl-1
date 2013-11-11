# This is contrib/dec/breg3d_old/exe/CMakeLists.txt
# 3D Registration executables
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib/dec )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

ADD_EXECUTABLE( breg3d_build_world breg3d_build_world.cxx )
ADD_EXECUTABLE( breg3d_cost_neighborhood breg3d_cost_neighborhood.cxx )
ADD_EXECUTABLE( breg3d_main breg3d_main.cxx )

TARGET_LINK_LIBRARIES( breg3d_main breg3d vil vil_io vpgl vnl brip vul dbvrl)
TARGET_LINK_LIBRARIES( breg3d_cost_neighborhood breg3d vil vil_io vpgl vnl brip vul)
TARGET_LINK_LIBRARIES( breg3d_build_world breg3d vil vil_io vpgl vnl brip vul)
