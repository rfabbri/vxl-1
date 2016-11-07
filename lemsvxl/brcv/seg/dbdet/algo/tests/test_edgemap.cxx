// This is brcv/seg/dbdet/tests/test_edgemap.cxx

#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/algo/dbdet_third_order_edge_det.h>
#include <sdet/sdet_third_order_edge_det.h>
#include <sdet/sdet_third_order_edge_det_params.h>
#include <dbdet/sel/dbdet_edgel.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>

#include <vdgl/vdgl_edgel.h>

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_vector.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>

MAIN( test_edgemap )
{ 
  double tolerance = 1e-3;
  bool test_passed = true;

  //*******************************************************
  START (" Compare VXL third-order output edgemap with dbdet");

  vcl_string root = testlib_root_dir();
  vcl_string image_path = root + "/contrib/brl/bseg/sdet/tests/jar-closeup.tif";
  vcl_cout << "Loading Image " << image_path << '\n';
  vil_image_view<vxl_byte> img_grey = vil_convert_stretch_range (vxl_byte(), vil_load(image_path.c_str()));

  /////// Default Params ////////
  unsigned int grad_op = 0;
  unsigned int conv_algo = 0;
  double sigma = 1.0;
  double thresh = 2.0;
  int N = 1;
  unsigned int parabola_fit = 0;
  bool adapt_thresh = false;
  bool grid = false;
  bool tokens = false;

  ////lems DBDET third order
  dbdet_edgemap_sptr lems_map =  dbdet_detect_third_order_edges(img_grey, sigma, thresh, 
                                                  N, parabola_fit, grad_op, conv_algo, 
                                                  adapt_thresh, grid, tokens);
  ////Vxl SDET third order
   sdet_third_order_edge_det_params my_params(sigma, thresh, N, parabola_fit, grad_op, conv_algo, adapt_thresh);
   sdet_third_order_edge_det my_det(my_params);
   my_det.apply(img_grey);
   vcl_vector<vdgl_edgel>& my_edgels = my_det.edgels();

   //Convert the edge vector to compatible edge map
   dbdet_edgemap_sptr vxl_map;
   vxl_map = new dbdet_edgemap(img_grey.ni(), img_grey.nj());
  
   for (unsigned i=0; i<my_edgels.size(); i++)
   {
       dbdet_edgel* new_edgel = new dbdet_edgel(my_edgels[i].get_pt(), my_edgels[i].get_theta(), my_edgels[i].get_grad()); 
       vxl_map->insert(new_edgel);
   }

  TEST("Num edges", lems_map->num_edgels() == vxl_map->num_edgels(), true);
  vcl_cout << "  Num edges: (lems) = " << lems_map->num_edgels() << " , (vxl) = " << vxl_map->num_edgels() << "\n\n";

  TEST("Size(W)", lems_map->width() == vxl_map->width(), true);
  TEST("Size(H)", lems_map->height() == vxl_map->height(), true);
  vcl_cout << "  Size(W,H): (lems) = (" << lems_map->width() << "," << lems_map->height() <<
                                   ") , (vxl) = (" << vxl_map->width() << "," <<vxl_map->height() << ") \n\n";
  TEST("lems-Map Equal vxl-Map \n  edgel tangent & strength \t\t", (*lems_map).AlmostEqual(*vxl_map , 0.0, 0.0), true);

  SUMMARY();
}
