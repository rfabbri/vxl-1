# contrib/brl/bbas/bsta/vis/tests/CMakeLists.txt

add_executable( dbsta_vis_test_all
  test_driver.cxx
  test_vis_svg.cxx
)

target_link_libraries( dbsta_vis_test_all dbsta_vis testlib )

add_test( dbsta_vis_test_vis_svg  dbsta_vis_test_all test_vis_svg )

add_executable( dbsta_vis_test_include test_include.cxx )
target_link_libraries( dbsta_vis_test_include bsta_vis )
