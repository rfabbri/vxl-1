// This is brcv/seg/dbdet/tests/test_edgemap.cxx

#include <testlib/testlib_test.h>

#include <dbdet/filter/dbdet_filter_util.h>
#include <dbdet/filter/dbdet_filter_bank.h>
#include <dbdet/filter/dbdet_texton_classifier.h>
#include <dbtest_root_dir.h>

#include <vector>
#include <iostream>
#include <string>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_file_matrix.h>
#include <vil/vil_load.h>
#include <vil/vil_rgb.h>
#include <vil/vil_convert.h>
#include <vil/vil_crop.h>

MAIN( test_texton_classify )
{ 
  double tolerance = 1e-3;
  bool test_passed = true;

  //*******************************************************
  START ("Texton classifier test");

  std::string root = dbtest_root_dir();
  std::string base_path = root + "/brcv/seg/dbdet/tests/test_data";

  //vxl has no instance of vnl_file_matrix<int/unsigned>
  vnl_matrix<unsigned> reference = vnl_matrix<unsigned>(1,1); 
  bool status = loadFromTabSpaced((base_path + "/out.txt").c_str(), reference);

  vil_image_view<vil_rgb<vxl_byte> > in = vil_convert_to_component_order(vil_convert_to_n_planes(3,
        vil_convert_stretch_range (vxl_byte(), vil_load((base_path + "/in.jpg").c_str()))));

  dbdet_filter_bank fb(base_path);
  dbdet_texton_classifier tex((base_path + "/tex/tex.txt").c_str());
	
  if(status && fb.numFilters() > 0 && tex.numClasses() > 0)
  {

	  std::cout << "IN image size: " << in.ni() << " " << in.nj() << std::endl;
	  std::cout << "REFERENCE matrix size: " << reference.rows() << " " << reference.cols() << std::endl;
	  std::cout << "Filter Bank size: " << fb.numFilters() << std::endl;
	  std::cout << "Texture classes: " << tex.numClasses() << std::endl;

	  

	  std::vector<vil_image_view<double> > decomposed = fb.decompose(in);
	  vnl_matrix<unsigned> classified = tex.classify(decomposed);
int counter = 0;
double dist2 = 0;
for(int i = 0; i < classified.rows(); ++i)
{
  for(int j = 0; j < classified.cols(); ++j)
  {
    if(classified(i,j) != reference(i,j))
    { 
      ++counter;
      double sum1 = 0.0, sum2 = 0.0;
      for(int k=0; k < tex.classes.rows();++k)
      {
        sum1 += (tex.classes(k, reference(i,j)) - decomposed[k](i,j)) * (tex.classes(k, reference(i,j)) - decomposed[k](i,j));
        sum2 += (tex.classes(k, classified(i,j)) - decomposed[k](i,j)) * (tex.classes(k, classified(i,j)) - decomposed[k](i,j));
      }
      dist2 += (std::sqrt(sum1) - std::sqrt(sum2)) * (std::sqrt(sum1) - std::sqrt(sum2));
    }
  }
}

double maxd2 = 0;
for(int i = 0; i < tex.classes.cols(); ++i)
{
  for(int j = 0; j < tex.classes.cols(); ++j)
  {
    double sum1 = 0.0;
    for(int k=0; k < tex.classes.rows();++k)
    {
      sum1 += (tex.classes(k,i) - tex.classes(k,j)) * (tex.classes(k,i) - tex.classes(k,j));
    }
    maxd2 = maxd2 < sum1 ? sum1 : maxd2;  
  }
}

dist2 /= (classified.rows()*classified.cols());
dist2 = 10.0 * std::log10(maxd2*maxd2 / dist2);

    TEST("Test Classification", counter, 0);
    std::cout << "Difference(diff pixels / total pixels): " << (double)counter/(classified.rows()*classified.cols()) * 100 << "%\t" << "PSNR: " << dist2 << "dB" << std::endl;
  }
  else
  {
      TEST("Failed to load data", false, true);
  }
  SUMMARY();
}
