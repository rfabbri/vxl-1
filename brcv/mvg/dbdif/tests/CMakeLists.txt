# This is brcv/mvg/dbdif/tests/CMakeLists.txt

ADD_EXECUTABLE ( dbdif_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_camera.cxx
)

TARGET_LINK_LIBRARIES( dbdif_test_all dbdif testlib )

ADD_TEST( dbdif_test_camera dbdif_test_all test_camera )

ADD_EXECUTABLE( dbdif_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( dbdif_test_include dbdif )
