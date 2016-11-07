set (geom_test_src
     test_driver.cxx
     test_index_structure.cxx
#     dummy_probe_volume1.cxx
#     dummy_probe_volume2.cxx
)

add_executable(geom_test_all ${geom_test_src})

target_link_libraries(geom_test_all biob vnl testlib vgl vgl_algo vbl geom)

add_test( geom_test geom_test_all)
