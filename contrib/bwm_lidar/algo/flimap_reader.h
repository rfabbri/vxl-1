// This is lemsvxlsrc/contrib/bwm_lidar/algo/flimap_reader.h

//:
// \file
// \brief I/O interface for the flimap data
//
// \author Ibrahim Eden, (ieden@lems.brown.edu)
// \date 12/16/2007
//      
// \endverbatim

#ifndef flimap_reader_h
#define flimap_reader_h

///#include "all_includes.h"
#include <vcl_string.h>
#include <vnl/vnl_matrix.h>

class flimap_reader
{
public:
  static void read_flimap_file(vcl_string flimap_file_path, vnl_matrix<double>& points);
};
#endif
