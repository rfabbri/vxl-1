//: 
// \file     test_imgr_skyscan_header.cxx
// \brief    testing for skyscan header
// \author   Michael Morgenstern
// \date     2005-07-07

#include <testlib/testlib_test.h>
#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <imgr/file_formats/imgr_skyscan_log_header.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <vcl_cstddef.h>
#include <vcl_cstdio.h>
#include <vcl_iostream.h>


static void test_imgr_skyscan_header(int argc, char* argv[] )
{

   vcl_FILE *fp = vcl_fopen(argv[1],"r");
   assert(fp != NULL);
   imgr_skyscan_log_header skyscan_log_header(fp);

    TEST_NEAR("Camera Pixel Size in um",skyscan_log_header.cam_pixel_size_,11.68,1e-6);
    TEST_NEAR("Camera X/Y Ratio",skyscan_log_header.cam_xy_ratio_,0.9830,1e-6);
    TEST_NEAR("Source to Rotation Axis Distance along Principal Line in mm",skyscan_log_header.object_to_source_dist_,121,1e-6);
    TEST_NEAR("Source to Principal Point Distance in mm",skyscan_log_header.camera_to_source_dist_,161,1e-6);
    TEST_NEAR("Number of Projection Data Files",skyscan_log_header.number_of_files_,580,1e-6);
    TEST_NEAR("Number of Rows of Sensor",skyscan_log_header.number_of_rows_,2096,1e-6);
    TEST_NEAR("Number of Columns of Sensor",skyscan_log_header.number_of_columns_,3872,1e-6);
    TEST_NEAR("Y-coordinate of Principal Point",skyscan_log_header.optical_axis_,980,1e-6);
    TEST_NEAR("Voxel Size of Reconstructed Volume",skyscan_log_header.image_pixel_size_,8.78287,1e-6);
    TEST_NEAR("Rotation Step Angle",skyscan_log_header.rotation_step_,0.340,1e-6);
    TEST_NEAR("Relative Angular Position of Reconstructed Volume and Scanner Coordinate System",skyscan_log_header.cs_static_rotation_,0,1e-6);

    vcl_fclose(fp);

}

TESTMAIN_ARGS(test_imgr_skyscan_header);
