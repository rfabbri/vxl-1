# brl/bseg/blem/brcv/mvg/dbirl/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/breye1 )

add_executable( dbirl_test_all
  test_driver.cxx
  test_h_computation.cxx
)

target_link_libraries(dbirl_test_all dbirl dbinfo bpro1 brip btol vtol vsol vgl_algo vgl vil1 vnl_algo vnl vsl vpl vbl vul testlib)

add_test(dbirl_test_h_computation dbirl_test_all test_h_computation)
