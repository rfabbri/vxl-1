// This is brl/brcv/mvg/dbirl/dbirl_h_computation.h
#ifndef dbirl_h_computation_h_
#define dbirl_h_computation_h_
//---------------------------------------------------------------------
//:
// \file
// \brief A class for computing homographies between frames in a video given
// a set of tracks.  It returns a list of homographies mapping from each frame
// to the first frame.
//
// \author
//  Thomas Pollard - Oct 2, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//---------------------------------------------------------------------

#include <dbinfo/dbinfo_track_geometry.h>

#include <vcl_vector.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vsol/vsol_point_2d_sptr.h>


class dbirl_h_computation {

public:

  dbirl_h_computation(){}

  bool 
  compute_affine( 
    vcl_vector< dbinfo_track_geometry_sptr >& tracks, //const 
    vcl_vector< vgl_h_matrix_2d<double> >& homographies,
    int ground_frame );

  bool 
  compute_similarity( 
    vcl_vector< dbinfo_track_geometry_sptr >& tracks, //const 
    vcl_vector< vgl_h_matrix_2d<double> >& homographies,
    int ground_frame );

};

#endif // dbirl_h_computation_h_

