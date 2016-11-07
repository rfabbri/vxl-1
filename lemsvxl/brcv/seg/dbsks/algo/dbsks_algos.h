// This is dbsks/algo/dbsks_algos.h
#ifndef dbsks_algos_h_
#define dbsks_algos_h_

//:
// \file
// \brief Place holder for common utilities
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date August 23, 2009
//
// \verbatim
//  Modifications
// \endverbatim


#include <vgl/vgl_box_2d.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Place holder for common utilities
class dbsks_algos
{
public:
  //: Compute a list of overlapping windows to cover an image
  static void compute_detection_windows(int max_window_width, 
    int max_window_height,
    int image_width, int image_height,
    vcl_vector<vgl_box_2d<int > >& windows);



private:
  dbsks_algos(){};
  ~dbsks_algos(){};
};




#endif
