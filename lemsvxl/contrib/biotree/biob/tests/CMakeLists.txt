# contrib/biotree/biob/tests/CMakeLists.txt

set( biob_test_src
     test_driver.cxx
     test_worldpt_transformation.cxx
     test_worldpt_index.cxx
#     test_grid_worldpt_roster.cxx
     test_explicit_neighborhood_structure.cxx
     test_worldpt_field.cxx
    )

add_executable(biob_test_all ${biob_test_src})

target_link_libraries(biob_test_all biob vnl testlib vgl vgl_algo
vgl_xio vbl)

add_test( NAME  biob_test COMMAND $<TARGET_FILE:biob_test_all>)

