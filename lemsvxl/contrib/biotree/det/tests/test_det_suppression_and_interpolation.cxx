//: 
// \file     test_det_suppression_and_interpolation
// \brief    testing for suppression and interpolation
// \author   P. Klein (modified from K. Kang's test for another program)
// \date     2005-02-20
// 

#include <biob/biob_worldpt_field.h>
#include <testlib/testlib_test.h>
#include <biob/biob_worldpt_box.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <vgl/vgl_vector_3d.h>
#include <det/det_suppression_and_interpolation.h>
#include <vcl_iostream.h>

static void test_det_suppression_and_interpolation(){

  const unsigned dim = 5;
  int center = dim/2;
  biob_worldpt_box box(0, 0, 0, dim-1, dim-1, dim-1);
  biob_grid_worldpt_roster * grid = new biob_grid_worldpt_roster(box, 1.);
  biob_worldpt_field<vgl_vector_3d<double> >  field(grid);
  vgl_vector_3d<double> vertical(0,0,1);
  //shouldn't be necessary  field.values().resize(grid.num_points());
  for(int i=0; i<dim; i++) {
    for(int j=0; j<dim; j++) {
      for(int k=0; k<dim; k++) {
        double strength = 2*center*center - (i - center - 0.3)*(i -center - 0.3)
          -(j - center - 0.2)*(j - center - 0.2);
        //maximized at points (2.3, 2.2, z)
        vcl_cout << "(" << i << ", " << j << ", " << k << "): " << strength << "\n";
        field.values()[(grid->index_3d_2_1d(i,j,k)).index()] = strength * vertical;
      }
    }
  }
  det_suppression_and_interpolation sup;
  biob_worldpt_field<vgl_vector_3d<double> >  new_field = sup.apply(field, 1.5, 1.5, 2.*3.14*40./360, .4);
  //should be maximal at (2.3, 2.2, 0), (2.3, 2.2, 1), ..., (2.3, 2.2, 4)
  TEST("number of maximal points", new_field.roster()->num_points(), 5);
  for (int i = 0; i < new_field.roster()->num_points(); ++i){
    TEST_NEAR("x difference", new_field.roster()->point(biob_worldpt_index(i)).x(), 2.3, .02);
    TEST_NEAR("y difference", new_field.roster()->point(biob_worldpt_index(i)).y(), 2.2, .02);
  }
}

TESTMAIN(test_det_suppression_and_interpolation);
