INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

SET(dbacm3d_test_firat_sources
       driver.cxx
    )
ADD_EXECUTABLE(dbacm3d_test_firat  ${dbacm3d_test_firat_sources} )
TARGET_LINK_LIBRARIES (dbacm3d_test_firat vtkIO vil3d vil)