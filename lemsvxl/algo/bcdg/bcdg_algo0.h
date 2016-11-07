// This is algo/bcdg/bcdg_algo0.h
#ifndef bcdg_algo0_h_
#define bcdg_algo0_h_
//:
// \file
// \brief Main algorithm class (implementation 0)
// \author scates and dapachec
// \date 7/9/04
//

#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_utility.h>
#include <vgl/vgl_point_2d.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include <dbecl/dbecl_epipole_sptr.h>
#include <dbecl/dbecl_episeg_point.h>
#include "bcdg_frame_sptr.h"
#include "bcdg_hypothesis_sptr.h"
#include "bcdg_finger_sptr.h"
#include "bcdg_local_option_sptr.h"
#include "bcdg_global_option_sptr.h"
//#include "bcdg_point_projection.h"
class bcdg_point_projection;

//: The algorithm
class bcdg_algo0 {
public:
  //: Store parameters
  class params;
  friend class params;

  //: Run the algorithm (Returns true if success, false otherwise.)
  bool run();

  //: Constructor
  bcdg_algo0(vcl_vector<bcdg_frame_sptr>&, double = 0, dbecl_epipole_sptr = NULL, double = 0.5, int = 3000);
  //: Destructor
  ~bcdg_algo0();

  //: Get the result
  vcl_vector<bcdg_hypothesis_sptr> result() const;
  
private:
  //: Store all frames in the video
  vcl_vector<bcdg_frame_sptr> _frames;
  //: Store the bag of hypotheses
  vcl_vector<bcdg_hypothesis_sptr> _hypos;

  //: Initialize fingers
  void build_starting_fingers(
    vcl_vector< vcl_vector< bcdg_finger_sptr > >& fings,
    vgl_point_2d<double> p1, int t1, vgl_point_2d<double> p2, int t2);

  //: Helper for initializer
  void build_starting_fingers_helper(vcl_vector< vcl_vector< bcdg_finger_sptr > >&, vcl_vector< dbecl_episeg_point >&, bcdg_point_projection&);
  
  //: The recursive crux of the algorithm.
  void trace( const vcl_vector<bcdg_finger_sptr>&, 
              const bcdg_global_option_sptr&, 
              const bcdg_hypothesis_sptr&);

  //: Figure out global options from a list of local ones in each frame.
  vcl_list<bcdg_global_option_sptr> figure_out_options() const;
  void figure_out_options_helper(int i, 
              vcl_vector<bcdg_local_option_sptr> building_options, 
              vcl_list<bcdg_global_option_sptr>& goptions) const;

  //: Store the parameters
  bcdg_algo0::params* _params;

  //: Build up a list of possible hypotheses
  void fill_bag();
  //: Choose the best subset of them
  vcl_vector<bcdg_hypothesis> choose_from_bag();
  //: shared temp variable
  mutable vcl_vector< vcl_vector< bcdg_local_option_sptr > > _loptions;
};

//: Parameters
class bcdg_algo0::params  {
    dbecl_epipole_sptr _epipole;
    bcdg_algo0* _algo;
    double _speed;
    int _frames;
    double _min_magnitude;
    int _num_seeds;
    double _neighbor_radius;
  public:
    params(dbecl_epipole_sptr, double, int, double, int, bcdg_algo0*);
    dbecl_epipole_sptr epipole() const;
    double speed() const;
    vcl_vector< dbecl_episeg_point > neighbors(vgl_point_2d<double>, int) const;
    int num_frames() const;
    double min_magnitude() const;
    int seed_points() const;
    double neighbor_radius() const;
};

typedef bcdg_algo0::params* bcdg_algo_params;

#endif
