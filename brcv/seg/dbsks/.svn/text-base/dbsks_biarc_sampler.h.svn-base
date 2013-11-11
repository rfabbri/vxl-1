// This is seg/dbsks/dbsks_biarc_sampler.h
#ifndef dbsks_biarc_sampler_h_
#define dbsks_biarc_sampler_h_

//:
// \file
// \brief A class to generate sample points of an biarc - fast
//       
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 28, 2008
//
// \verbatim
//  Modifications
// \endverbatim



#include <vnl/vnl_vector.h>
#include <dbgl/algo/dbgl_biarc.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_ref_count.h>



// =============================================================================
// dbsks_biarc_sampler_params
// =============================================================================
//: one type of parameter set to construct a xnode grid
struct dbsks_biarc_sampler_params
{
  double min_dx;
  double step_dx;
  int num_dx;

  double min_dy;
  double step_dy;
  int num_dy;

  double min_alpha0;
  double step_alpha0;
  int num_alpha0;

  double min_alpha2;
  double step_alpha2;
  int num_alpha2;

  void set_to_default_values();
};
  



// ============================================================================
// dbsks_biarc_sampler
// ============================================================================

//: A class to generate samples of a biarc fast
class dbsks_biarc_sampler: public vbl_ref_count
{
public:
  // Constructors / Destructors-------------------------------------------------
  //: Constructor 
	dbsks_biarc_sampler(){}; // this->clear(); };

  //: Destructor
  ~dbsks_biarc_sampler(){};

  //: Clear all information
  void clear();

  // Return instance of the biarc_sampler with default parameter settings
  static dbsks_biarc_sampler& default_instance();

  // Access member variables----------------------------------------------------

  //: Compute the grid from a parameter set
  bool set_grid(const dbsks_biarc_sampler_params& p);

  //: Set sampling parameters and discretization resolution of target space
  void set_sampling_params(int num_bins_0to2pi, double ds, 
                           int max_num_pts_per_biarc = 51)
  { 
    this->num_bins_0to2pi_ = num_bins_0to2pi; 
    this->ds_ = ds; 
    this->max_num_pts_per_biarc_ = max_num_pts_per_biarc;
  }

  //: Number of discretized angle bins
  int nbins_0to2pi() const { return this->num_bins_0to2pi_; }

  //: Number of radians per angle bin
  double radians_per_bin() const;

  //: Sampling rate
  double ds() const {return ds_; }

  //: Return true if this sampler has computed cache biarc sample points
  bool has_cache_samples() const { return this->has_cache_samples_; }

  //: Return true if this sampler has cache values for normalized curvature difference
  bool has_cache_nkdiff() const { return this->has_cache_nkdiff_; }


  
  // Utilities------------------------------------------------------------------

  //: Compute cache sample points for all biarcs defined by the grid, assuming a fixed starting point
  // "num_angle_bins" covers [0, 2pi]
  bool compute_cache_sample_points();

  //: Compute cache "normalized curvature difference" for all biarcs
  bool compute_cache_nkdiff();


  //: Compute indices of a biarc, given a pair of oriented points
  // "i_profile" encodes both i_alpha0 and i_alpha2
  // Return false if the given biarc is out of range
  bool biarc_to_grid(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
    const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
    int& i_dx, int& i_dy, int& i_profile) const;

  //: Compute samples - floating-point precision
  bool compute_samples(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
    const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
    vcl_vector<double >& x_vec, vcl_vector<double >& y_vec, 
    vcl_vector<double >& tx_vec, vcl_vector<double >& ty_vec);

  //: Compute samples - floating-point precision
  bool compute_samples(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
                       const vgl_point_2d<double >& end,   const vgl_vector_2d<double >& end_tangent,
                       vcl_vector<vgl_point_2d<double > >&  pts,  
                       vcl_vector<vgl_vector_2d<double > >& tangents); 

  //: Compute samples of a biarc - integral precision
  bool compute_samples(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
    const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
    vcl_vector<int >& xs, vcl_vector<int >& ys, vcl_vector<int >& angle_bins, double& angle_step) const;

  //: Compute samples of a biarc using cache data
  // Required: cache data have been computed
  bool compute_samples_using_cache(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
    const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
    vcl_vector<int >& xs, vcl_vector<int >& ys, vcl_vector<int >& angle_bins) const;

  //: Compute samples of a biarc - not using cache regarless of whether cache data are available
  bool compute_samples_not_using_cache(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
    const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
    vcl_vector<int >& xs, vcl_vector<int >& ys, vcl_vector<int >& angle_bins) const;



  //> Compute other attributes of biarc

  //: Compute normalized curvature difference
  bool compute_nkdiff_using_cache(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
    const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
    double& nkdiff_val) const;
            

  //: Print grid information
  vcl_ostream& print(vcl_ostream& os) const {return os;}

  
protected:
  
  ////: Compute the "profile" index of a biarc (encoding both alpha0 and alpha2)
  //// Return false if biarc is out of range
  //bool biarc_to_profile(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
  //  const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
  //  int& i_profile) const;


  //: Compute the "profile" index of a biarc (encoding both alpha0 and alpha2)
  // Return false if biarc is out of range
  bool compute_biarc_i_profile(const vgl_vector_2d<double >& biarc_chord, 
    const vgl_vector_2d<double >& start_tangent,
    const vgl_vector_2d<double >& end_tangent,
    int& i_profile) const;


  //: Compute i_dx and i_dy index of a biarc
  // Return false if biarc is out of range
  bool compute_biarc_i_dx_and_i_dy(const vgl_vector_2d<double >& biarc_chord, int& i_dx, int& i_dy) const;


  //: Compute (retrieve) samples from valid biarc index
  void retrieve_samples_from_valid_biarc_index(int i_dx, int i_dy, int i_profile,
    int start_x, int start_y,
    vcl_vector<int >& xs, vcl_vector<int >& ys, vcl_vector<int >& angle_bins) const;

  //: Compute (retrieve) samples from valid biarc index
  // These sample points are scaled up and then translated before assigning to 
  // output vectors
  void retrieve_and_scale_samples_from_valid_biarc_index(int i_dx, int i_dy, int i_profile,
    double scaled_up,
    const vgl_point_2d<double >& start,
    vcl_vector<int >& xs, vcl_vector<int >& ys, vcl_vector<int >& angle_bins) const;

  //: (Literatally) compute samples of a biarc uniformly
  bool compute_uniform_biarc_samples(const vgl_point_2d<double >& start, 
    const vgl_vector_2d<double >& start_tangent,
    const vgl_point_2d<double >& end, 
    const vgl_vector_2d<double >& end_tangent,
    double ds,
    vcl_vector<double >& x_vec, 
    vcl_vector<double >& y_vec, 
    vcl_vector<double >& tx_vec,
    vcl_vector<double >& ty_vec,
    vcl_vector<double >& angle_vec) const;

  // Member variables ----------------------------------------------------------

  // dx - difference between x-coordinates of start and end points
  double step_dx_;
  vnl_vector<double > dx_;

  // dy - difference between y-coordinates of start and end  points
  double step_dy_;
  vnl_vector<double > dy_;

  // alpha0 - angle between the chord (connecting two end points) and the curve
  // tangent at the starting point
  double step_alpha0_;
  vnl_vector<double > alpha0_;

  // alpha2 - angle between the chord (connecting two end points) and the curve 
  // tangent at the end points
  double step_alpha2_;
  vnl_vector<double > alpha2_;


  //> Cache sample points ----------

  // flag for cache availability
  bool has_cache_samples_;
  vbl_array_3d<vnl_matrix<double > > biarc_sample_;
  vbl_array_3d<vnl_matrix<int > > biarc_sample_index_;
  
  // discretization params
  int num_bins_0to2pi_; // discretization resolution of the "angle" axis


  // sampling params
  double ds_; // desired sampling rate
  int max_num_pts_per_biarc_; // max number of sample points per biarc

  //> Normalized curvature difference
  bool has_cache_nkdiff_;
  vnl_vector<double > biarc_nkdiff_;
  
};



#endif // seg/dbsks/dbsks_biarc_sampler.h


