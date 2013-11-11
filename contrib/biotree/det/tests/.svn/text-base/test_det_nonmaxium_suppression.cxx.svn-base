//: 
// \file     test_det_nonmaxium_suppression
// \brief    testing for nonmaxium_suppression class
// \author   K. Kang
// \date     2005-09-26
// 

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <det/det_cylinder_map.h>
#include <det/det_nonmaxium_suppression.h>
#include <vcl_cstdlib.h>
#include <vcl_ctime.h>

static void test_det_nonmaxium_suppression()
{
  vcl_cout << "---test nonmaxium_suppression ---" << vcl_endl;
  const unsigned dim = 5;
  int center = dim/2;
    
  det_cylinder_map cm(dim, dim, dim);

  //: set up synthetic data
  det_map_entry entry;

  for(int k=0; k<dim; k++)
    for(int i=0; i<dim; i++)
      for(int j=0; j<dim; j++)
      {
        entry.dir_ = vgl_vector_3d<double> (0, 0, 1);
        entry.strength_ = 2*center*center - (i - center - 0.3)*(i -center - 0.3)
          -(j - center - 0.2)*(j - center - 0.2);

        cm[i][j][k] = entry;
     }


  det_nonmaxium_suppression sprs(3);

  det_cylinder_map result_cm = sprs.apply(cm, 0);

  for(int k=0; k<dim; k++)
  {
    for(int i=0; i<dim; i++)
    {
      for(int j=0; j<dim; j++)
      {
        if(i==2 && j==2 && k >0 && k<dim-1){
          TEST_NEAR("x difference", result_cm[i][j][k].location_.x(), 
              0.3, 0.1);
          TEST_NEAR("y difference", result_cm[i][j][k].location_.y(), 
              0.2, 0.1);
        }
        
      }
    }
  }
  
}

TESTMAIN(test_det_nonmaxium_suppression);
