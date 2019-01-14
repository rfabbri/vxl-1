//This is brcv/seg/dbacm/tests/test_bnd_vertex.cxx

//: \author Nhon Trinh
//: \date 12/03/2005


#include <testlib/testlib_test.h>
#include <iostream>

#include <dbacm/dbacm_image_filters.h>

#include <vnl/vnl_math.h>

void test_malladi_image_force()
{
  std::cout << "This is test_malladi_image_force()" << std::endl;
}


//------------------------------------------------------------------
MAIN( test_image_filters )
{
  START( "test_image_filters" );

  test_malladi_image_force();
  SUMMARY();
}
