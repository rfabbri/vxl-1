#include <testlib/testlib_test.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <splr/splr_per_orbit_index_splat_collection.txx>
#include <splr/tests/dummy_filter_2d.h>
#include <splr/tests/dummy_filter_3d.h>
#include <splr/tests/dummy_camera.h>
#include <splr/tests/dummy_scan.h>
#include <vsl/vsl_binary_io.h>

/*
template class splr_per_orbit_index_splat_collection<double, int, dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera>;
template void vsl_b_write(vsl_b_ostream & os, 
          const splr_per_orbit_index_splat_collection<double, int, dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera>
    & splat_collection);
template void vsl_b_read(vsl_b_istream & os, 
           splr_per_orbit_index_splat_collection<double, int, dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera>
    & splat_collection);
*/


void test_splr_io_per_orbit_index_splat_collection(){

  //dummy filter 3d
  dummy_filter_3d filter_3d;
  //dummy scan
  dummy_scan scan;
  //roster
   biob_worldpt_box box(worldpt(0,0,0), worldpt(1,2,4));
   biob_grid_worldpt_roster roster(box, .49);

  splr_per_orbit_index_splat_collection<double, int, 
     dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera> splat_collection(scan, &filter_3d, &roster);

  //populate the splat collection
  splat_collection.populate(0, biob_worldpt_index(0));
  splat_collection.populate(1, biob_worldpt_index(0));

  //create a file output stream
  vsl_b_ofstream outfs("test-output-file");
//write the splat collection
  vsl_b_write(outfs, splat_collection);
  //close the file
  outfs.close();
  //create a new, empty splat collection
  splr_per_orbit_index_splat_collection<double, int, 
     dummy_scan, dummy_filter_2d, dummy_filter_3d, dummy_camera> splat_collection_2(scan, &filter_3d, &roster);
  //open the file for reading
  vsl_b_ifstream infs("test-output-file");
  //read the data into the splat collection
  vsl_b_read(infs, splat_collection_2);

  //get the camera from the scan
  dummy_camera camera = scan(0);
    //see if splat obtained from camera is  the same as that obtained from the splat_collection
    TEST("same filter retrieved", splat_collection_2.splat(0, biob_worldpt_index(0)) == filter_3d.splat(camera, roster.point(biob_worldpt_index(0))), true);
}

TESTMAIN(test_splr_io_per_orbit_index_splat_collection);
