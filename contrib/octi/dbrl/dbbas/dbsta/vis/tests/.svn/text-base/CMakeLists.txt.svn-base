# contrib/brl/bbas/bsta/vis/tests/CMakeLists.txt

ADD_EXECUTABLE( dbsta_vis_test_all
  test_driver.cxx
  test_vis_svg.cxx
)

TARGET_LINK_LIBRARIES( dbsta_vis_test_all dbsta_vis testlib )

ADD_TEST( dbsta_vis_test_vis_svg  dbsta_vis_test_all test_vis_svg )

ADD_EXECUTABLE( dbsta_vis_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbsta_vis_test_include bsta_vis )
