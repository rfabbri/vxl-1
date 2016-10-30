// This is brcv/seg/dbdet/tests/test_edgemap.cxx

#include <testlib/testlib_test.h>

#include <dbdet/filter/dbdet_filter_util.h>
#include <dbdet/filter/dbdet_filter_bank.h>
#include <dbdet/filter/dbdet_texton_classifier.h>
#include <dbtest_root_dir.h>

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_file_matrix.h>
#include <vil/vil_load.h>
#include <vil/vil_rgb.h>
#include <vil/vil_convert.h>

MAIN( test_texton_classify )
{ 
  double tolerance = 1e-3;
  bool test_passed = true;

  //*******************************************************
  START ("Texton classifier test");

  vcl_string root = dbtest_root_dir();
  vcl_string base_path = root + "/brcv/seg/dbdet/tests/filter_bank";

  //vxl has no instance of vnl_file_matrix<int/unsigned>
  vnl_matrix<unsigned> reference = vnl_matrix<unsigned>(1,1); 
  bool status = loadFromTabSpaced((base_path + "/out.txt").c_str(), reference);

  vil_image_view<vil_rgb<vxl_byte> > in = vil_convert_to_component_order(vil_convert_to_n_planes(3,
        vil_convert_stretch_range (vxl_byte(), vil_load((base_path + "/in.png").c_str()))));

  dbdet_filter_bank fb(base_path);
  dbdet_texton_classifier tex((base_path + "/tex/tex.txt").c_str());
	
  if(status && fb.numFilters() > 0 && tex.numClasses() > 0)
  {

	  vcl_cout << "IN image size: " << in.ni() << " " << in.nj() << vcl_endl;
	  vcl_cout << "REFERENCE matrix size: " << reference.rows() << " " << reference.cols() << vcl_endl;
	  vcl_cout << "Filter Bank size: " << fb.numFilters() << vcl_endl;
	  vcl_cout << "Texture classes: " << tex.numClasses() << vcl_endl;

	  

	  vcl_vector<vil_image_view<double> > decomposed = fb.decompose(in);
	  
	  //vnl_matrix<unsigned> classified = tex.classify(decomposed);
	  //TEST("Test Classification", classified == reference, true);
  }
  else
  {
      TEST("Failed to load data", false, true);
  }
  SUMMARY();
}
