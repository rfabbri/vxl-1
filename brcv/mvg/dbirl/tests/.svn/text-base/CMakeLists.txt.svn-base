# brl/bseg/blem/brcv/mvg/dbirl/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

ADD_EXECUTABLE( dbirl_test_all
  test_driver.cxx
  test_h_computation.cxx
)

TARGET_LINK_LIBRARIES(dbirl_test_all dbirl dbinfo bpro1 brip btol vtol vsol vgl_algo vgl vil1 vnl_algo vnl vsl vpl vbl vul testlib)

ADD_TEST(dbirl_test_h_computation dbirl_test_all test_h_computation)
