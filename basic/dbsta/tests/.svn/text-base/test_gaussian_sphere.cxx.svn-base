#include <testlib/testlib_test.h>
#include <dbsta/dbsta_gaussian_sphere.h>
#include <vcl_string.h>
#include <vcl_iostream.h>


template <class T>
void test_gaussian_sphere_type(T dummy, const vcl_string& type_name)
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


