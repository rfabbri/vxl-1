#include <vnl/vnl_matrix.h>
#include <vil/vil_image_view.h>
#include <vector>
#include <string>

#ifndef _DBDET_FILTER_BANK_H_
#define _DBDET_FILTER_BANK_H_

class dbdet_filter_2d {
public:
  vnl_matrix<double> m;

  double flipped(int i, int j);


  dbdet_filter_2d(const char * fname);

  bool isEmpty();

  int size();

  vil_image_view<double> applyPadded(vil_image_view<vxl_byte> image, int border);

  vil_image_view<double> applyPadded13(vil_image_view<vxl_byte> image, int border);

  vil_image_view<double> applyPadded19(vil_image_view<vxl_byte> image, int border);


};

class dbdet_filter_bank {

  std::vector<dbdet_filter_2d> filters;
  int filtersMaxSize;
public:

  dbdet_filter_bank(std::string baseDir);

  int numFilters();

  std::vector<vil_image_view<double> > decompose(vil_image_view<vil_rgb<vxl_byte> > image);
};

#endif
