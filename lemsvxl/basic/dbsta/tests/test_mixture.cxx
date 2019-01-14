#include <testlib/testlib_test.h>
#include <dbsta/dbsta_mixture.h>
#include <string>
#include <iostream>


template <class T>
void test_mixture_type(T dummy, const std::string& type_name)
{
  // test stuff here
  TEST(("Dummy test <"+type_name+">").c_str(), true, true);
}



MAIN( test_mixture )
{
  START ("mixture");
  test_mixture_type(float(),"float");
  test_mixture_type(double(),"double");
  SUMMARY();
}


