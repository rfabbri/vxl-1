//: 
// \file  test_driver.cxx
// \brief  testing the library vol3d
// \author    Kongbin Kang
// \date        2006-05-29
// 


#include <testlib/testlib_register.h>

// DECLARE();
DECLARE( test_vol3d_algo_radius_detection );

void register_tests()
{
  REGISTER( test_vol3d_algo_radius_detection);
}

DEFINE_MAIN;
