#ifndef curve_utils_h_
#define curve_utils_h_
//:
// \file
// \brief Some algorithms to compute spatial relations between curve fragments
//        Most of the algorithms in this file caters towards Vishal's needs in
//        tracking curves in a video sequence.
//
// \author Amir Tamrakar
// \date 02/22/05
// 
// \verbatim
//  Modifications
//
// \endverbatim

#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vgl/vgl_point_2d.h>

//: list of {(start/end, curve1_id), (start/end, curve2_id)} represent all possible completions
typedef vcl_pair<bool, int> curve_endpoint_id;
typedef vcl_pair<curve_endpoint_id, curve_endpoint_id> curve_pair;
typedef vcl_vector<curve_pair> potential_curve_completions_list;

typedef vcl_pair<curve_endpoint_id, int> endpt_curve_pair;
typedef vcl_vector<endpt_curve_pair> potential_junctions_list;

potential_curve_completions_list 
compute_potential_curve_completions(vcl_vector<vcl_vector<vgl_point_2d<double> > > curve_set);

potential_junctions_list 
compute_potential_junctions(vcl_vector<vcl_vector<vgl_point_2d<double> > > curve_set);

#endif //curve_utils_h_
