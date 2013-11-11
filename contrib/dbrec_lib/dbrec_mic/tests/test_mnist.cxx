// This is dbrec/tests/test_visitors.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   May 01, 2009
//
#include <testlib/testlib_test.h>

#include <dbrec_mic/dbrec_mnist_utils.h>

static void test_mnist()
{
/*  vcl_string path = "C:\\projects\\mnist_digits\\test_imgs\\";
  vcl_string label_filename = "C:\\projects\\mnist_digits\\test_imgs\\labels.txt";
  vcl_string out_path = "C:\\projects\\mnist_digits\\test_imgs_flipped\\";
  convert_mnist_files(path, 10000, label_filename, out_path);
*/
  vcl_string path = "C:\\projects\\roi_1\\sewage_rot_inv\\output_learning_model_params_using_gt\\";
  vcl_string file = path + "stat_file_class_0_1_2_non_class_stats.txt";
  read_stat_file_gamma(file, 90, 180, path+"non_class");
  read_stat_file_gamma(file, 45, 180, path+"non_class");
  read_stat_file_d_rho(file, -23,23,180, 1, 5, 90, 180, path+"non_class");
  read_stat_file_d_rho(file, -45,45,180, 1, 5, 90, 180, path+"non_class");
}

TESTMAIN( test_mnist );
