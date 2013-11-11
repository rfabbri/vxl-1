# contrib/biotree/biob/tests/CMakeLists.txt

SET( biob_test_src
     test_driver.cxx
     test_worldpt_transformation.cxx
     test_worldpt_index.cxx
#     test_grid_worldpt_roster.cxx
     test_explicit_neighborhood_structure.cxx
     test_worldpt_field.cxx
    )

ADD_EXECUTABLE(biob_test_all ${biob_test_src})

TARGET_LINK_LIBRARIES(biob_test_all biob vnl testlib vgl vgl_algo
vgl_xio vbl)

ADD_TEST( biob_test biob_test_all)

