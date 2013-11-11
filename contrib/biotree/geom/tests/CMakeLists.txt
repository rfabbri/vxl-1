SET (geom_test_src
     test_driver.cxx
     test_index_structure.cxx
#     dummy_probe_volume1.cxx
#     dummy_probe_volume2.cxx
)

ADD_EXECUTABLE(geom_test_all ${geom_test_src})

TARGET_LINK_LIBRARIES(geom_test_all biob vnl testlib vgl vgl_algo vbl geom)

ADD_TEST( geom_test geom_test_all)
