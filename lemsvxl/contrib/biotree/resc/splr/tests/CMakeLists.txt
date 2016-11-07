# contrib/biotree/resc/splr/tests/CMakeLists.txt

set(splr_test_src
    test_driver.cxx
    test_explicit_splat_collection.cxx
    test_pizza_slice_symmetry.cxx
    test_symmetry_worldpt_roster.cxx
    test_symmetry_splat_collection.cxx
    test_basic_pizza_slice_splat_collection.cxx
#   test_splr_per_orbit_index_splat_collection_with_real_filters.cxx
#    test_pizza_slice_symmetry_representatives.cxx
    test_subset_splat_collection.cxx
    dummy_filter_2d.cxx
)

aux_source_directory(Templates splr_test_src)

add_executable(splr_test_all ${splr_test_src})

target_link_libraries(splr_test_all splr xmvg xmvg_io xscan testlib vcl vbl biob vsl)

add_test(splr_test splr_test_all)

