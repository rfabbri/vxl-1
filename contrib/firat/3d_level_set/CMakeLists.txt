include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )

set(dbacm3d_test_firat_sources
       driver.cxx
    )
add_executable(dbacm3d_test_firat  ${dbacm3d_test_firat_sources} )
target_link_libraries (dbacm3d_test_firat vtkIO vil3d vil)