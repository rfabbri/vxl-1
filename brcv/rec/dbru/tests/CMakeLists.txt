# brl/bseg/blem/brcv/rec/dbru/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/breye1 )

add_executable( dbru_test_all
  test_driver.cxx
  test_osl.cxx
)

#target_link_libraries(dbru_test_all dbru_pro dbru bvis1 brip btol vgl_algo vil1 vnl_algo vsl vpl vbl vul testlib)
target_link_libraries(dbru_test_all dbru_pro dbru bvis1 vgl_algo vil1 vnl_algo vul testlib)

add_test(dbru_test_osl dbru_test_all test_osl)

add_executable( dbru_test_include test_include.cxx )
target_link_libraries( dbru_test_include dbru dbskr )
