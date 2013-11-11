// This is algo/bcdg/bcdg_frame.h
#ifndef bcdg_frame_h_
#define bcdg_frame_h_
//:
// \file
// \brief Represents a frame of video (stores image AND edge data).
// \author scates and dapachec
// \date 7/9/04
//

#include <vcl_vector.h>
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include <dbecl/dbecl_episeg_point_sptr.h>
#include "bcdg_frame_sptr.h"
#include "bcdg_algo0.h"

//: A frame of video
class bcdg_frame : public vbl_ref_count {
public:
  //: Constructor
  bcdg_frame(const vcl_vector<vsol_digital_curve_2d_sptr>&, int, double);
  //: Returns all (2D) edges in this frame.
  //vcl_vector<vsol_digital_curve_2d_sptr> curves() const;
  //: Returns the time associated with this frame.
  int time() const;
  //: Returns the neighbors of the specified point
  vcl_vector<dbecl_episeg_point> neighbors(vgl_point_2d<double>) const;

  //: Convert the digital curves to episegments
  void convert(bcdg_algo_params);

  //: Iterate over all points on curves in this frame
  struct point_iterator {
      //: Construct a point_iterator for the given frame. 
      // \note Bool represents whether it starts at the beginning or the end.
      point_iterator(const bcdg_frame&, bool = true);
      //: Increment an iterator (move to the next point)
      point_iterator& operator++();
      //: Is this iterator equal to that one?
      bool operator==(const point_iterator&) const;
      //: Isn't this iterator equal to that one?
      bool operator!=(const point_iterator&) const;
      //: What is the point at this iterator?
      vgl_point_2d<double> operator*() const;
      //: Are there any more points?
      bool is_done() const;

      // Indirect accessors
      //: What curve are we on?
      dbecl_episeg_sptr curve() const;
      //: What point are we at?
      int index() const;
    protected:
      //: What frame are we looking at
      bcdg_frame_sptr _frame;
      //: which curve
      int _curve_idx;
      //: which point
      int _point_idx;
      //: neighbor radius
      double _neighbor_radius;
  };

  //: Iterate over a random sample of all points on all curves in this frame.
  struct random_point_iterator : public point_iterator {
    random_point_iterator(int, const bcdg_frame&, bool = true);
    point_iterator& operator++();
    bool is_done() const;
  protected:
    //: How many samples should we take?
    int _samples;
  };

  friend struct random_point_iterator;
  friend struct point_iterator;

  //: Return a random-sampling iterator
  random_point_iterator begin_random(int) const;
  //: Create a linear iterator (all points)
  point_iterator begin_linear() const;

  
private:
  //: Convert one curve
  vcl_vector<dbecl_episeg_sptr> convert_curve(vsol_digital_curve_2d_sptr, bcdg_algo_params) const;
  //: Compute point discretization
  void discretize();
  //: The list of edges
  vcl_vector<vsol_digital_curve_2d_sptr> _curves;
  //: The list of curves
  vcl_vector<dbecl_episeg_sptr> _epi_segs;
  //: The time
  int _time;
  //: Discretization of the points
  vcl_vector< vcl_vector< vcl_vector<dbecl_episeg_point > > > _point_buckets;
  //: Neighbor radius
  double _neighbor_radius;
};

typedef vbl_smart_ptr< bcdg_frame > bcdg_frame_sptr;

#endif
