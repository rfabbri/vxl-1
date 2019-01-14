#include <dbetrk/dbetrk_track.h>
#include <fstream>
#include <string>
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/vnl_vector.h>
#include <sstream>
#include <testlib/testlib_test.h>

int test_dbetrk_track_main(int argc, char *argv[])
{
   testlib_test_begin("dbetrk_track");
   return testlib_test_summary();   
}


