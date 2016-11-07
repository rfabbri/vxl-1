// This is algo/bcdg/bcdg_algo0.cpp
//:
// \file
// \brief Main algorithm class (implementation 0)
// \author scates and dapachec
// \date 7/9/04
//

#include <vcl_vector.h>
//#include <vcl_set.h>
#include <vcl_list.h>

#include <vul/vul_timer.h>

#include "bcdg_algo0.h"
#include "bcdg_finger.h"
#include "bcdg_hypothesis.h"
#include "bcdg_local_option.h"
#include "bcdg_global_option.h"
#include "bcdg_frame.h"

#include "TextProgressBar.h"

//: Construct the algorithm implementation
bcdg_algo0::bcdg_algo0(vcl_vector<bcdg_frame_sptr>& frames, 
    double speed, 
    dbecl_epipole_sptr epi,
    double mag,
    int seeds
    ) :
  _params (new bcdg_algo0::params(epi, speed, frames.size(), mag, seeds, this)),
  _frames( frames )
{
  TextProgressBar boo("Converting frames",_frames.size());
  for(int i = 0; i < _frames.size(); i++){
    _frames[i]->convert(_params);
    boo.step();
    boo.print();
  }
  boo.print();
}

//: Destructor
bcdg_algo0::~bcdg_algo0()
{
  delete _params;
}

//: Run the algorithm
bool bcdg_algo0::run() {
  fill_bag();
  choose_from_bag();
  return true;
}

//: Return the result
vcl_vector<bcdg_hypothesis_sptr> bcdg_algo0::result() const {
  return _hypos;
}

//: Build up a list of possible hypotheses
void bcdg_algo0::fill_bag() {
  TextProgressBar bar("Choosing seed points", _params->num_frames() * _params->num_frames() * _params->seed_points() * _params->seed_points());
  // PICK STARTING POINTS
  // Foreach frame
  vcl_cerr << "There are " << _frames.size() << " frames!\n";
  for(int frame1cnt = 0; frame1cnt < _frames.size(); frame1cnt++) {
    // Foreach point on a curve in the frame
//    vul_timer* t = new vul_timer();
    for(bcdg_frame::random_point_iterator pit1 = _frames[frame1cnt]->begin_random(_params->seed_points());
        !pit1.is_done(); ++pit1) {
      bar.print();
      // Foreach other frame
      for(int frame2cnt = 0; frame2cnt < _frames.size(); frame2cnt++) {
        // foreach point on a curve in the other frame
        for(bcdg_frame::random_point_iterator pit2 = _frames[frame2cnt]->begin_random(_params->seed_points());
            !pit2.is_done(); ++pit2) {
          bar.step();

          // TRACE FROM HERE
           // Build up a vector of possible starting positions, where
           // a starting position is a collection of points, one in each frame.
           vcl_vector< vcl_vector< bcdg_finger_sptr > > fings;
           build_starting_fingers(fings, *pit1, frame1cnt, *pit2, frame2cnt);
           // Trace all possible starting fingers
           for(int i = 0; i < fings.size(); i++) {
             trace( fings[i], NULL, 
                    bcdg_hypothesis_sptr(new bcdg_hypothesis()));
           }
        }
      }
    }
    bar.print();
  }
  vcl_clog << "\n";
}


//: Build up the initial fingers

void bcdg_algo0::build_starting_fingers(
    vcl_vector< vcl_vector< bcdg_finger_sptr > >& fings,
    vgl_point_2d<double> p1, int t1, vgl_point_2d<double> p2, int t2) {
  bcdg_point_projection proj(p1,t1,p2,t2,_params);
  vcl_vector< dbecl_episeg_point > pts(_frames.size());
  build_starting_fingers_helper(fings,pts,proj);
}

//: Helper procedure to build the starting fingers.
void bcdg_algo0::build_starting_fingers_helper(
    vcl_vector< vcl_vector< bcdg_finger_sptr > >& fings,
    vcl_vector< dbecl_episeg_point >& pts, bcdg_point_projection& proj) {
  int k = pts.size();
  if(k == _frames.size()) {
    // We've finished
    vcl_vector<bcdg_finger_sptr> fingers(_frames.size());
    for(int i = 0; i < fingers.size(); i++) {
      fingers[i] = bcdg_finger_sptr(
          new bcdg_finger(pts[i].episeg(), pts[i].index(), UP, k - 1));
    }
    fings.push_back(fingers);
  } else {
    // Get near neighbors, and recursively build the rest of the list
    vcl_vector< dbecl_episeg_point > neighb = _params->neighbors(proj.point(k),k);
    for(int i = 0; i < neighb.size(); i++) {
      vcl_vector< dbecl_episeg_point > pts2 = pts;
      pts2.push_back(neighb[i]);
      build_starting_fingers_helper(fings,pts2,proj);
    }
  }
}


//: Trace the curve
void bcdg_algo0::trace(const vcl_vector<bcdg_finger_sptr>& fingers,
     const bcdg_global_option_sptr& glob, const bcdg_hypothesis_sptr& hypo) {
  if(glob != NULL && hypo != NULL) {
    // Update the hypothesis with the given global option
    hypo->prepend_hypothesis(glob->hypothel());
    // Update the correct finger
    vcl_vector<bcdg_finger_sptr> fingers = glob->next_fingers(fingers);
  }
  
  // Get all the local options in each frame
  _loptions = vcl_vector< vcl_vector< bcdg_local_option_sptr > >(fingers.size());
  vcl_vector<bcdg_finger_sptr>::const_iterator i;
  for(i = fingers.begin(); i != fingers.end(); i++) {
    _loptions.push_back((*i)->local_options(_params));
  }

  // Get corresponding acceptable global options
  vcl_list<bcdg_global_option_sptr> goptions = figure_out_options();

  // Do something special
  vcl_list<bcdg_global_option_sptr>::iterator git = goptions.begin();
  if(git == goptions.end()) {
    // No options - Base case: we're done
    _hypos.push_back(hypo);
  } else {
    // Recursively try all options
    for(; git != goptions.end(); ++git) {
      vcl_cerr << "About to trace recursively...\n";
      trace(fingers,*git,hypo->clone());
    }
  }
}

//: Choose the best subset of them
vcl_vector<bcdg_hypothesis> bcdg_algo0::choose_from_bag() {
  return vcl_vector<bcdg_hypothesis>();
}

vcl_list<bcdg_global_option_sptr>
bcdg_algo0::figure_out_options() const {
  vcl_list<bcdg_global_option_sptr> g(0);
  // Build options
  figure_out_options_helper(0, vcl_vector<bcdg_local_option_sptr>(), g);
  
  // Filter options
  for(vcl_list<bcdg_global_option_sptr>::iterator i = g.begin();
      i != g.end(); ++i) {
    if((*i)->magnitude() < _params->min_magnitude()) {
      vcl_list<bcdg_global_option_sptr>::iterator j = i;
      ++i;
      g.erase(j);
    }
  }
  return g;
}

void bcdg_algo0::figure_out_options_helper(int i, vcl_vector<bcdg_local_option_sptr> building_options, vcl_list<bcdg_global_option_sptr>& goptions) const {

  if(i == _loptions.size()) {
    bcdg_global_option* p = new bcdg_global_option(building_options, _params);
    goptions.push_back(bcdg_global_option_sptr(p));
  } else {

    // Check void case
    vcl_vector< bcdg_local_option_sptr > void_building_option = building_options;
    void_building_option.push_back(NULL);
    figure_out_options_helper(i+1, void_building_option, goptions);

    // Check all other cases
    for(int j = 0; j < _loptions[i].size(); j++) {
      vcl_vector<bcdg_local_option_sptr> this_option = building_options;
      this_option.push_back(_loptions[i][j]);
      figure_out_options_helper(i+1,this_option, goptions);
    }
  }
}


// PARAMS CLASS
bcdg_algo0::params::params(dbecl_epipole_sptr e, double d, int f, double m, int s, bcdg_algo0* a) :
  _epipole(e),
  _speed(d),
  _frames(f),
  _min_magnitude(m),
  _num_seeds(s),
  _algo(a)
{}

dbecl_epipole_sptr bcdg_algo0::params::epipole() const { return _epipole; }
int bcdg_algo0::params::num_frames() const            { return _frames; }
double bcdg_algo0::params::speed() const              { return _speed; }
double bcdg_algo0::params::min_magnitude() const      { return _min_magnitude; }
int bcdg_algo0::params::seed_points() const           { return _num_seeds; }
double bcdg_algo0::params::neighbor_radius() const { return _neighbor_radius; }

vcl_vector< dbecl_episeg_point > 
bcdg_algo0::params::neighbors(vgl_point_2d<double> p, int i) const {
  return _algo->_frames.at(i)->neighbors(p);
}
