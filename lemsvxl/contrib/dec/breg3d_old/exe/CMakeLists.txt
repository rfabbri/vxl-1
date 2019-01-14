# This is contrib/dec/breg3d_old/exe/CMakeLists.txt
# 3D Registration executables
include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories(${LEMS_SOURCE_DIR}/contrib/dec )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )

add_executable( breg3d_build_world breg3d_build_world.cxx )
add_executable( breg3d_cost_neighborhood breg3d_cost_neighborhood.cxx )
add_executable( breg3d_main breg3d_main.cxx )

target_link_libraries( breg3d_main breg3d vil vil_io vpgl vnl brip vul dbvrl)
target_link_libraries( breg3d_cost_neighborhood breg3d vil vil_io vpgl vnl brip vul)
target_link_libraries( breg3d_build_world breg3d vil vil_io vpgl vnl brip vul)
