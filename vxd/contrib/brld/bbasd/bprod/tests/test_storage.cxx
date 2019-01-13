#include <testlib/testlib_test.h>
#include <bprod/bprod_storage.h>
#include <iostream>


MAIN( test_storage )
{
  START ("storage");

  bprod_storage_sptr ds_int = new bprod_storage_type<int>(10);
  int test = ds_int->data<int>();


  TEST("get data (int)", test, 10);

  SUMMARY();
}
