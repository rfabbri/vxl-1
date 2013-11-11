//: 
// \file     test_det_cylinder_map.cxx
// \brief    testing for cylinder map class
// \author   K. Kang
// \date     2005-09-11
// 

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <det/det_cylinder_map.h>
#include <vcl_cstdlib.h>
#include <vcl_ctime.h>

static void test_det_cylinder_map()
{
  vcl_cout << "---test set and read cylinder map---" << vcl_endl;

  det_cylinder_map cm(5, 5, 5);

  det_map_entry entry;
  for(int i =0; i<5; i++)
    for(int j=0; j<5; j++)
      for(int k=0; k<5; k++)
      {
        entry.dir_ = vgl_vector_3d<double> (i, j, k);
        entry.strength_ = i+j+k; 
        
        cm[i][j][k] = entry;
      }

  
  for(int i =0; i<5; i++)
    for(int j=0; j<5; j++)
      for(int k=0; k<5; k++)
      {
        entry.dir_ = vgl_vector_3d<double> (i, j, k);
        entry.strength_ = i+j+k;
        TEST("cylinder_map test", cm[i][j][k], entry);
      }

}

TESTMAIN(test_det_cylinder_map);
