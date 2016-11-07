#include <testlib/testlib_test.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <splr/splr_basic_pizza_slice_splat_collection.h>
#include <splr/tests/dummy_filter_2d.h>
#include <splr/tests/dummy_filter_3d.h>
#include <xmvg/xmvg_perspective_camera.h>
#include <xscan/xscan_dummy_scan.h>

static void test_basic_pizza_slice_splat_collection(){
  //dummy filter 3d
  dummy_filter_3d filter_3d;
  //dummy scan
  unsigned int num_views = 3;
  //create a dummy scan
  double pi = vnl_math::pi;
  xscan_dummy_scan scan(10.0, 40., 160., vnl_int_2(256, 256),vnl_double_2(128.0, 128.0),
                  vgl_point_3d<double>(40.0,0.0,0.0), vnl_double_3(0.0,0.0,1.0),
                  2*pi/num_views, num_views);
   biob_worldpt_box box(worldpt(0,0,0), worldpt(1,2,4));


   double spacing = .49;
   splr_basic_pizza_slice_splat_collection<double, int, 
     xscan_dummy_scan, dummy_filter_2d, dummy_filter_3d, xmvg_perspective_camera<double> > splat_collection(scan, &filter_3d, box, spacing);
  biob_worldpt_index_enumerator_sptr iter = splat_collection.collection()->enumerator();
  while (iter->has_next()){
    //get next point index; if necessary, compute splat
    biob_worldpt_index pti = iter->next();
    for(orbit_index t = 0; t < scan.scan_size(); t++){
      const dummy_filter_2d & filter = splat_collection.collection()->splat(t, pti);
    }
  }
}


TESTMAIN(test_basic_pizza_slice_splat_collection);

