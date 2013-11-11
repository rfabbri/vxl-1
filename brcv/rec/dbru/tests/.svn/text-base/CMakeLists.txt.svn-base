# brl/bseg/blem/brcv/rec/dbru/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

ADD_EXECUTABLE( dbru_test_all
  test_driver.cxx
  test_osl.cxx
)

#TARGET_LINK_LIBRARIES(dbru_test_all dbru_pro dbru bvis1 brip btol vgl_algo vil1 vnl_algo vsl vpl vbl vul testlib)
TARGET_LINK_LIBRARIES(dbru_test_all dbru_pro dbru bvis1 vgl_algo vil1 vnl_algo vul testlib)

ADD_TEST(dbru_test_osl dbru_test_all test_osl)

ADD_EXECUTABLE( dbru_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbru_test_include dbru dbskr )
