#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>
#include <vcl_string.h>

#ifndef _DBDET_FILTER_BANK_H_
#define _DBDET_FILTER_BANK_H_

class dbdet_filter_2d {

  vnl_matrix<double> m;

        double flipped(int i, int j);
public:

  dbdet_filter_2d(const char * fname);

  bool isEmpty();

  int size();

  vnl_matrix<double> applyPadded(vnl_matrix<double> image, int border);

  vnl_matrix<double> applyPadded13(vnl_matrix<double> image, int border);

  vnl_matrix<double> applyPadded19(vnl_matrix<double> image, int border);


};

class dbdet_filter_bank {

  vcl_vector<dbdet_filter_2d> filters;
  int filtersMaxSize;
public:

  dbdet_filter_bank(vcl_string baseDir);

  int numFilters();

  vcl_vector<vnl_matrix<double> > decompose(vnl_matrix<double> image);
};

#endif
