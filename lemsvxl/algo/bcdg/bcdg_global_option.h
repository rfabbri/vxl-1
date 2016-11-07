// This is algo/bcdg/bcdg_global_option.h
#ifndef algo_bcdg_bcdg_global_option_h_
#define algo_bcdg_bcdg_global_option_h_

//:
// \file
// \brief   A collection of local options, one for each finger.
// \author  scates and dapachec
// \date    7/14/2004

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_vector.h>
#include <dbecl/dbecl_epipole.h>
#include "bcdg_local_option_sptr.h"
#include "bcdg_point_projection.h"
#include "bcdg_hypothel.h"
#include "bcdg_finger_sptr.h"

class bcdg_finger;

//: A collection of local options, one for each finger.
class bcdg_global_option : public vbl_ref_count {
public:
  //: Constructor description
  bcdg_global_option( const vcl_vector<bcdg_local_option_sptr>&, 
                      const bcdg_algo_params p);
  //: Get the cost of using global option
  double cost() const;
  //: Get the hypothesis addition
  bcdg_hypothel_sptr hypothel() const;
  //: Update a bunch of fingers 
  vcl_vector<bcdg_finger_sptr> 
    next_fingers(const vcl_vector<bcdg_finger_sptr>&) const;
  //: Get the point at the given frame
  vgl_point_2d<double> point_in_frame(int) const;
  //: Get the magnitude (a measure of the number of non-null points to total ones)
  double magnitude() const;

private:
  //: Compute the projection that generates the median depth
  void compute_median_projection();
  //: Compute the cost
  void compute_cost();
  //: Used in initialization; step the same amount in each frame
  void interpolate_min_step(const vcl_vector<bcdg_local_option_sptr> &);
  //: Access the median projection
  bcdg_point_projection median_projection() const;
  //: One particular specific cost implementation
  double specific_cost(int, const bcdg_point_projection&) const;
  
  //: Stores the median projection
  bcdg_point_projection _median_proj;
  //: Stores the cost
  double _cost;
  //: Stores the theta
  double _theta;
  //: Stores the points
  vcl_vector< vgl_point_2d<double> > _points;
  //: Stores the local options
  vcl_vector< bcdg_local_option_sptr > _local_options;
  //: Stores the algo params
  bcdg_algo_params _params;
};

#endif // algo_bcdg_bcdg_global_option_h_
