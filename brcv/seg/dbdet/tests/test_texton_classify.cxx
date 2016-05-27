// This is brcv/seg/dbdet/tests/test_edgemap.cxx

#include <testlib/testlib_test.h>

#include <dbdet/filter/dbdet_filter_util.h>
#include <dbdet/filter/dbdet_filter_bank.h>
#include <dbdet/filter/dbdet_texton_classifier.h>
#include <dbtest_root_dir.h>

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

MAIN( test_texton_classify )
{ 
  double tolerance = 1e-3;
  bool test_passed = true;

  //*******************************************************
  START ("Texton classifier test");

  vcl_string root = dbtest_root_dir();
  vcl_string base_path = root + "/brcv/seg/dbdet/tests/filter_bank";

  vnl_matrix<double> in(1,1);
  vnl_matrix<int> reference(1, 1);

  bool loadIn = loadFromTabSpaced((base_path + "/in.txt").c_str(), in);
  bool loadRef = loadFromTabSpaced((base_path + "/out.txt").c_str(), reference);
  FilterBank fb(base_path);
  TextonClassifier tex((base_path + "/tex/tex.txt").c_str());
	
  if(loadIn && loadRef && fb.numFilters() > 0 && tex.numClasses() > 0)
  {

	  vcl_cout << "IN matrix size: " << in.rows() << " " << in.cols() << vcl_endl;
	  vcl_cout << "REFERENCE matrix size: " << reference.rows() << " " << reference.cols() << vcl_endl;
	  vcl_cout << "Filter Bank size: " << fb.numFilters() << vcl_endl;
	  vcl_cout << "Texture classes: " << tex.numClasses() << vcl_endl;

	  

	  vcl_vector<vnl_matrix<double> > decomposed = fb.decompose(in);
	  
	  vnl_matrix<int> classified = tex.classify(decomposed);
	  TEST("Test Classification", classified == reference, true);
  }
  else
  {
      TEST("Failed to load data", false, true);
  }
  SUMMARY();
}
