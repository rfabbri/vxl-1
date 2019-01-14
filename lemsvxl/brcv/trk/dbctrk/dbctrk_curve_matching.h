// \file
// \brief to match two sets of curves
// \author vj (vj@lems.brown.edu)
// \date   8/20/2003
// \verbatim
// Modifications
// \endverbatim
// Input : two lists of curves
// Output : returns the best hypothesis for correspondence between the curves.
#ifndef dbctrk_curve_matching_h_
#define dbctrk_curve_matching_h_

#include<dbctrk/dbctrk_tracker_curve.h>
#include<dbctrk/dbctrk_tracker_curve_sptr.h>
#include<dbctrk/dbctrk_curve_description.h>
#include<vector>
#include<algorithm>
#include<utility>
#include<map>
#include<mvl/FMatrix.h>
#include<dbctrk/dbctrk_curveMatch.h>

class dbctrk_curve_matching_params
{
 public:
  // Parameters

  int  motion_in_pixels;
  int  no_of_top_choices;
  bool matching_;
  vgl_point_2d<double> e_;
  int alpha_; // lenght
  int beta_; // curvature
  int gamma_; // epipole
  int delta_; // stereo
  int eta_; //Intenstity
  int zeta_; // Hue
  int theta_; // saturation
   
  bool onesided;
  
  float tauihs_;
  
  bool isintensity_;
  dbctrk_curve_matching_params();
  dbctrk_curve_matching_params(int motion,int top_ranks)
  {
   motion_in_pixels=motion;
   no_of_top_choices=top_ranks;
   e_.set(0,0);
  }

  ~dbctrk_curve_matching_params(){}

  void operator=(dbctrk_curve_matching_params mp);
  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;
};

std::ostream& operator<<(std::ostream& s, const dbctrk_curve_matching_params & params);

class dbctrk_curve_matching
{
 public:

  dbctrk_curve_matching();
  // constructor initializing the estimated motion of object
  dbctrk_curve_matching(dbctrk_curve_matching_params & mp)
    {
        mp_=mp;
    }
  /*dbctrk_curve_matching(dbctrk_curve_matching_params & mp)
    {
      motion_in_pixels_=mp.motion_in_pixels;
      no_of_top_choices_=mp.no_of_top_choices;
      if(mp.e_.x()!=0 && mp.e_.y()!=0)
  {
    epipole_=mp.e_;
  }

      mp_=mp;

    }*/
  ~dbctrk_curve_matching(){}

  double match_DP(dbctrk_curve_description * desc1,
                  dbctrk_curve_description * desc2,
                  match_data_sptr m,
                  vgl_point_2d<double> & e);
  // takes two sets of curves and returns the best corresponding(matching) curves
  void match(std::vector<dbctrk_tracker_curve_sptr >  * new_curves,
             std::vector<dbctrk_tracker_curve_sptr >  * old_curves);

 // coarser version of curve-matching to reduce the number of
 // candidates of curve-matching
  double coarse_match_DP(dbctrk_curve_description * desc1,
                         dbctrk_curve_description * desc2,
             vgl_point_2d<double> epi);
 // curve-matching algorithm
  double match_DP(dbctrk_curve_description  * desc1,
                  dbctrk_curve_description  * desc2,
                  std::map<int,int> & mapping,double &cost,
                  vnl_matrix<double> & R,vnl_matrix<double> & T,
                  vnl_matrix<double> & Tbar,double & scale,
                  std::vector<int> & tail1,std::vector<int> & tail2,
                  vgl_point_2d<double> &e);

  // curve-matching algorithm
  double match_DP(dbctrk_curve_description  * desc1,
                  dbctrk_curve_description  * desc2,
                  std::map<int,int> & mapping,double &cost,
                  vnl_matrix<double> & R,vnl_matrix<double> & T,
                  vnl_matrix<double> & Tbar,double & scale,
                  std::vector<int> & tail1,std::vector<int> & tail2,
                  FMatrix F);

  // compute the euclidean dist of  b using transformation of a
  double compute_euc_dist(dbctrk_tracker_curve_sptr a,
                          dbctrk_tracker_curve_sptr b);
  // compute the best pairwise match
  void best_matches(std::vector<dbctrk_tracker_curve_sptr> * new_curves,
                    std::vector<dbctrk_tracker_curve_sptr> * old_curves);

  void best_matches_tc(std::vector<dbctrk_tracker_curve_sptr> * current_curves,
             std::vector<dbctrk_tracker_curve_sptr> * past_curves,
             std::vector<dbctrk_tracker_curve_sptr> * future_curves);
  void softmax(std::vector<dbctrk_tracker_curve_sptr> * new_curves,
               std::vector<dbctrk_tracker_curve_sptr> * old_curves);
  void write_transformations(std::vector<dbctrk_tracker_curve_sptr> * new_curves);
  void compute_spatial_distance(std::vector<dbctrk_tracker_curve_sptr> curves,
                std::map<std::pair<int,int>, double > & spatiald);
  double spatial_euclidean_dist(dbctrk_tracker_curve_sptr c1,
  dbctrk_tracker_curve_sptr c2);

  bool greedy_and_closure(int n,
                          std::vector<dbctrk_tracker_curve_sptr> * new_curves,
                          std::vector<dbctrk_tracker_curve_sptr> * old_curves);
  static bool best_n_matches(int n,
     std::vector<dbctrk_tracker_curve_sptr> * new_curves,
     std::vector<dbctrk_tracker_curve_sptr> * old_curves,
     std::map<int,std::vector<std::pair<dbctrk_tracker_curve_sptr, dbctrk_tracker_curve_sptr> > > & transitives_);

  static bool best_topn_matches(int n,
     std::vector<dbctrk_tracker_curve_sptr> * new_curves,
     std::vector<dbctrk_tracker_curve_sptr> * old_curves,
     std::map<int,std::vector<std::pair<dbctrk_tracker_curve_sptr, dbctrk_tracker_curve_sptr> > > & transitives_);
  //: sausage intersection
  static bool sausage_intersection(dbctrk_tracker_curve_sptr c1,dbctrk_tracker_curve_sptr c2, double motion);
 protected:
  
    // compute the best match of tail in previous curve
  void match_prev_tail_curve(dbctrk_tracker_curve_sptr parent_curve,
                             dbctrk_tracker_curve_sptr & tail_curve,
                             std::vector<dbctrk_tracker_curve_sptr> * new_curves);
   // compute the best match of tail in next curve
  void match_next_tail_curve(dbctrk_tracker_curve_sptr parent_curve,
                             dbctrk_tracker_curve_sptr & tail_curve,
                             std::vector<dbctrk_tracker_curve_sptr> * old_curves);
  // initialize the cost matrix
  void initialize_matrix(std::vector<dbctrk_tracker_curve>  new_curves,
                         std::vector<dbctrk_tracker_curve > old_curves);

  // function to compute matches using statistical info of curves
  ///double match_stat(dbctrk_curve_description * desc1,
  ///                  dbctrk_curve_description  * desc2);
  // check if the bounding boxes of the curves intersect or not
  bool bounding_box_intersection(vsol_box_2d_sptr box1,vsol_box_2d_sptr box2);


  // merging curves to form a single curve
  void merge_curves(dbctrk_tracker_curve_sptr cs1,
                    dbctrk_tracker_curve_sptr cs2,
                    dbctrk_tracker_curve_sptr &outcs);

  // compute mean
  double compute_mean(std::vector<double> t);
    // compute stalndard deviation
  double compute_std(std::vector<double> t);
  // estimated motion in pixels
  int motion_in_pixels_;
    // after coarse DP Match filter top rank choices
  int no_of_top_choices_;
  vgl_point_2d<double> epipole_;
  dbctrk_curve_matching_params mp_;
 
};

struct less_cost
{
  bool operator()(match_data_sptr x, match_data_sptr y)
  { return x->cost_ < y->cost_; }
};
#endif
