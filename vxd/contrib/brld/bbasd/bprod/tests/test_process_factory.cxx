#include <testlib/testlib_test.h>
#include "bprod_sample_processes.h"
#include <iostream>
#include <bprod/bprod_process_factory.h>




MAIN( test_process_factory )
{
  START ("process_factory");


  //TEST("number of iterations", count, data.size());

  typedef bprod_process_factory::Reg_Type Reg_Type;
  const Reg_Type& reg = bprod_process_factory::registry();
  std::cout << "Registered processes" << std::endl;
  for (Reg_Type::const_iterator i = reg.begin(); i != reg.end(); ++i)
    std::cout << i->first << std::endl;


  SUMMARY();
}
