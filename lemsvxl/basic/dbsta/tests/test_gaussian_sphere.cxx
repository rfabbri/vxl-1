#include <testlib/testlib_test.h>
#include <dbsta/dbsta_gaussian_sphere.h>
#include <string>
#include <iostream>


template <class T>
void test_gaussian_sphere_type(T dummy, const std::string& type_name)
{
  // test stuff here
  TEST(("Dummy test <"+type_name+">").c_str(), true, true);
}



MAIN( test_gaussian_sphere )
{
  START ("gaussian_sphere");
  test_gaussian_sphere_type(float(),"float");
  test_gaussian_sphere_type(double(),"double");
  SUMMARY();
}


