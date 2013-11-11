// This is bcdg_global_option.cxx
//:
// \file

#include <vcl_algorithm.h>
#include <vcl_limits.h>
#include <vgl/vgl_distance.h>
#include "bcdg_finger.h"
#include "bcdg_global_option.h"
#include "bcdg_local_option.h"
#include "bcdg_costs.h"

#include "bcdg_3d_reconstructor.h"

//: Constructor
bcdg_global_option::bcdg_global_option(const vcl_vector<bcdg_local_option_sptr>& o, bcdg_algo0::params* s) :
  _points(o.size()),
  _local_options(o),
  _params(s)
{
  interpolate_min_step(o);
  compute_cost();
  compute_median_projection();
}


//: Get the minimum step for the given local options
void bcdg_global_option::interpolate_min_step(const vcl_vector<bcdg_local_option_sptr>& loptions) {
  // First, find the local option with the minimum "step" distance
  // (measured by theta)
  _theta = vcl_numeric_limits<double>::max();
 
  for(int i = 0; i < loptions.size(); i++) {
    if(loptions[i] != NULL) {
      double thisAng = loptions[i]->theta();
      if(thisAng < _theta) {
        _theta = thisAng;
      }
    }
  }

  // Then, initialize our points to move the same distance (in theta)
  int non_null = 0;
  for(int i = 0; i < _points.size(); i++) {
    if(loptions[i] != NULL) {
      _points[i] = loptions[i]->at_theta(_theta);
      non_null++;
    } else {
      _points[i] = vgl_point_2d<double>(0,0);
    }
  }

  if(non_null > 2 && non_null < loptions.size()) {
    // Choose two points randomly
    int seeda, seedb;
    do {
      seeda = rand() % loptions.size();
    } while(loptions[seeda] == NULL);
    do {
      seedb = rand() % loptions.size();
    } while(loptions[seedb] == NULL || seedb == seeda);
    
    // Make a projection
    bcdg_point_projection proj(loptions[seeda]->at_theta(_theta), seeda, 
                               loptions[seedb]->at_theta(_theta), seedb,
                               _params);
    for(int i = 0; i < _points.size(); i++) {
      if(loptions[i] == NULL) {
        _points[i] = proj.point(i);
      }
    }
  }
}

//: Get the cost
double bcdg_global_option::cost() const {
  return _cost;
}

//: Get the median projection
bcdg_point_projection bcdg_global_option::median_projection() const {
  return _median_proj;
}

//: Compute the cost
void bcdg_global_option::compute_cost()  {
  _cost = 0.0;
  bcdg_point_projection p = median_projection();
  for(int i = 0; i < _points.size(); i++) {
    _cost += specific_cost(i,p);
  }
}

//: A particular cost implementation
double bcdg_global_option::specific_cost(int i, const bcdg_point_projection& proj) const {
  if( _local_options[i] != NULL ) {
    return vgl_distance( _points[i], proj.point(i) );
  } else {
    return GAP_COST;
  }
}


//: Get the piece of the hypothesis to which this corresponds.
bcdg_hypothel_sptr bcdg_global_option::hypothel()  const {
  bcdg_3d_reconstructor rec(this,_params);
  vcl_vector<dbecl_episeg_sptr> tr(_points.size());
  for(int i = 0; i < tr.size(); i++) {
    if(_local_options[i] != NULL) {
      tr[i] = _local_options[i]->curve();
    }
  }
  return bcdg_hypothel_sptr( new bcdg_hypothel(rec.point_3d(), tr, cost()) );
}

//: Update the fingers
vcl_vector< bcdg_finger_sptr > bcdg_global_option::next_fingers (const vcl_vector<bcdg_finger_sptr>& oldf) const {
  vcl_vector<bcdg_finger_sptr> toReturn(oldf.size());
  for(int i = 0; i < oldf.size(); i++) {
    if(_local_options[i] != NULL) {
      toReturn[i] = oldf[i]->next_finger(_theta,_local_options[i]);
    } else {
      // FIXME
      //toReturn[i] = bcdg_finger_sptr(new bcdg_finger());
      toReturn[i] = NULL;
    }
  }
  return toReturn;
}

//: What is the median projection of this point?
void bcdg_global_option::compute_median_projection() {
  vcl_vector<bcdg_point_projection> p;
  
  for(int i = 0; i < _points.size(); i++) {
    for(int j = i + 1; j < _points.size(); j++) {
      p.push_back(bcdg_point_projection(_points[i],i,_points[j],j,_params));
    }
  }

  vcl_sort(p.begin(), p.end());
  _median_proj = p[p.size() / 2];
}


//: Get the point at the given frame
vgl_point_2d<double> bcdg_global_option::point_in_frame(int f) const {
  return _points[f];
}

//: Return the ratio of real points (not-NULL) to total
double bcdg_global_option::magnitude() const {
  int real = 0;
  for(int i = 0; i < _local_options.size(); i++) {
    if(_local_options[i] != NULL)  ++real;
  }
  return (double) real / _local_options.size();
}
