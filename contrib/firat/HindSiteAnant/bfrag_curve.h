// This is contrib/dbpz_hindsite.h
#ifndef dbpz_hindsite_h_
#define dbpz_hindsite_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Curve class for the fragment contour representation and puzzle solving
//        algorithm for archeological re-assembly.
//        This class uses interpolated 2d curve that is also known as LEMS curve
//        class.
//        It is designed to have all necessary capabilities of the curve class
//        designed by Jonah
//
// \author H. Can Aras (can@lems.brown.edu)
// \date 09-February-2007
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------

#include <basic/dbsol/dbsol_interp_curve_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vnl/vnl_matrix_fixed.h>

#include <vcl_vector.h>
#include <vcl_algorithm.h>

#ifndef SAME_CURVE_DISTANCE_THRESHOLD
#define SAME_CURVE_DISTANCE_THRESHOLD 10.0
#endif

//forward declaration b/c bfrag2D includes this header
class bfrag2D;

class bfrag_curve
{
  // functions
public:
  //: empty constructor
  bfrag_curve();
  //: copy constructor
//  bfrag_curve(const bfrag_curve &that);
  //: destructor
  virtual ~bfrag_curve();
  //: constructor from a vector of vgl points
  bfrag_curve(vcl_vector<vgl_point_2d<double> > &points, bool is_open);

  // output to stream
  void write_out(vcl_ofstream &out);
  // input from stream
  void read_in(vcl_ifstream &in);

  // append a point to the end of the level3_ point list
  void append(vgl_point_2d<double> &p);

  // access functions
  double length() { return length_; };
  unsigned num_corners() { return level1_.size(); }
  unsigned num_coarse_points() { return level2_.size(); }
  unsigned num_fine_points() const { return level3_.size(); }
  unsigned num_orig_points() { return level4_.size(); }

  double length(unsigned index)
  {
    assert(index >= 0 && index < arclength_.size());
    return arclength_[index];
  }
  double angle(unsigned index)
  {
    assert(index >= 0 && index < angle_.size());
    return angle_[index];
  }

  // level-3 point getters
  double x(unsigned index)
  {
    assert(index >= 0 && index < level3_.size());
    return level3_[index].x();
  }
  double y(unsigned index)
  {
    assert(index >= 0 && index < level3_.size());
    return level3_[index].y();
  }
  vgl_point_2d<double> point(unsigned index)
  {
    assert(index >= 0 && index < level3_.size());
    return level3_[index];
  }

  // level-1 (corner) getter
  int corner(unsigned index)
  {
    assert(index >= 0 && index < level1_.size());
    return level1_[index];
  }

  // level-2 point getters
  double Cx(unsigned index)
  {
    assert(index >= 0 && index < level2_.size());
    return level2_[index].x();
  }
  double Cy(unsigned index)
  {
    assert(index >= 0 && index < level2_.size());
    return level2_[index].y();
  }
  vgl_point_2d<double> Cpoint(unsigned index)
  {
    assert(index >= 0 && index < level2_.size());
    return level2_[index];
  }

  // get the coarse-fine index reference
  int coarse_ref(unsigned index)
  {
    assert(index >= 0 && index < coarse_fine_corr_.size());
    return coarse_fine_corr_[index];
  }

  double min_x() { return box_.min_x(); }
  double min_y() { return box_.min_y(); }
  double max_x() { return box_.max_x(); }
  double max_y() { return box_.max_y(); }

  bool is_corner(int index){ return vcl_find(level1_.begin(), level1_.end(), index) != level1_.end(); }

  // Finds the length from level3_[ip] to level3_[i]
  double total_length(int ip, int i);

  // Arclength distance between level3_[ip] and level3_[i], i > ip
  // In Jonah's version, this was simply the Euclidian distance
  double merge_length(int ip, int i);
  
  // Tangent angle change from level3_[ip] to level3_[i], i > ip
  // In Jonah's version, this was the angle of the line connecting the two points
  double merge_angle(int ip, int i);

  // operator overloads

  // Assumes the number of level-3 points on this and other curve are the same, and
  // checks if the average distance between this and other curve is small enough
  // for the curves to be considered the same
  bool operator ==(bfrag_curve &other);
  // Assumes the number of level-3 points on this and other curve are the same, and finds 
  // the average distance the curves using one-to-one correspondence
  double operator -(bfrag_curve &other);
  // Find the corners (indices are with respect to level-3 representation
  void find_corners();
  // Computes arclengths and angles
  void compute_properties();
  // Computes tangent angle at each point of the level-3 points
  void update_angles(double rot);
  // Resamples the level-3 point set with the given sampling rate (in terms of arclength)
  void resample(double ds);
  // Smoothes the level-3 point set using discrete curvature-based smoothing
  void smooth_by_discrete_curvature_algo(float psi, unsigned num_times);
  // Populate the level-2 point set (coarse re-sampling)
  void resample_coarsely(double ds);
  // Convert the curve to the coarse scale representation
  void become_coarse();
  // Invert the contour direction
  void invert();
  // remove all points
  void empty();
  // Combination of rotation followed by translation
  void rotate_translate(double angle, double tx, double ty);
  // Computes the bounding box of the contour at level-3
  void compute_box();

  // debugging files
  void dump_object_to_file(vcl_string fname);
  void write_level1(vcl_string fname);
  void write_level2(vcl_string fname);
  void write_level3(vcl_string fname);
  void write_level4(vcl_string fname);
  void write_coarse_fine_correspondence(vcl_string fname);
  void write_arclengths(vcl_string fname);
  void write_angles(vcl_string fname);

protected:
  // Computes the interpolated curve representation from the level-3 point set
  void compute_level3_interpolated_curve(dbsol_interp_curve_2d &level3_curve);
  // Computes arclengths of each segment using the level-3 point set
  void compute_arclength(dbsol_interp_curve_2d &level3_curve);
  // Computes tangent angle at each point of the level-3 points
  void compute_angles(dbsol_interp_curve_2d &level3_curve);
  // Rotate all levels except level-1 since level-1 holds indices, angle is given in terms of radians
  // Rotation is in clockwise direction following Jonah's code
  void rotate(double angle);
  // Translate all levels except level-1 since level-1 holds indices
  void translate(double tx, double ty);

private:

  // variables
public:
  bool is_open_;
  vgl_box_2d<double> box_;
  unsigned frag_id_;
  // transformation applied to this curve since it was created
  vnl_matrix_fixed<double, 3, 3> transform_;

protected:
  // Level-1 is the indices of the corners with respect to Level-3 indices
  // Level-4 never gets resampled, smoothed, changed, etc.

  // Corner indices with respect to level-3 or level-2 representation,
  // depending on where we are in the progrma, replaces extrema_ variable
  // It would be wise to have level-2 and level-3 correspondences separately
  // in two different strcutures for avoiding confusion
  vcl_vector<int> level1_;

  // Although all levels represent closed point sets, the start point is not repeated at the end of the list,
  // however, the user should be aware that in the interpolated curve representation, the last segment represents
  // the segment connecting from the end point to the start point

  public:
  // evenly and coarsely sampled pointset
  vcl_vector<vgl_point_2d<double> > level2_;
  // given a level-2(coarsely-sampled) point, this vector holds the corresponding index of the 
  // level-3 (finely-sampled) point
  vcl_vector<int> coarse_fine_corr_;
  // evenly and finely sampled pointset
  vcl_vector<vgl_point_2d<double> > level3_;
  // original pointset
  vcl_vector<vgl_point_2d<double> > level4_;

  protected:
  // i'th element holds the arclength of the (i-1)'th segment
  // 0th element holds the arclength of the segment from the end point to the start point
  // if the curve is closed. Otherwise, 0th element's value is 0.
  vcl_vector<double> arclength_;
  // i'th element holds the arclength from the starting point to the i'th point
  vcl_vector<double> cum_arclength_;
  // i'th element holds the tangent angle at the i'th sample point
  // value is between [-PI, PI]
  vcl_vector<double> angle_;

  double length_;

private:
  //and if it represents the top curve or the bottom
  bool isTop;
};
#endif
