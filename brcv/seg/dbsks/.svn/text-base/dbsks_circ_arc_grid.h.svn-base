// This is seg/dbsks/dbsks_circ_arc_grid.h
#ifndef dbsks_circ_arc_grid_h_
#define dbsks_circ_arc_grid_h_

//:
// \file
// \brief A grid of circular arc
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Nov 5, 2007
//
// \verbatim
//  Modifications
// \endverbatim



#include <vnl/vnl_vector.h>
#include <dbgl/algo/dbgl_circ_arc.h>




struct dbsks_circ_arc_grid_params  
{
  float center_x;
  float center_y;
  float step_x;
  float step_y;
  int half_num_x;
  int half_num_y;
  int num_theta;
  float min_chord;
  float step_chord;
  int num_chord;
  float step_height;
  int half_num_height;  
  
  //: Set all the parameters
  void set(float new_center_x,
    float new_center_y,
    float new_step_x,
    float new_step_y,
    int new_half_num_x,
    int new_half_num_y,
    int new_num_theta,
    float new_min_chord,
    float new_step_chord,
    int new_num_chord,
    float new_step_height,
    int new_half_num_height)
  {
    center_x = new_center_x;
    center_y = new_center_y;
    step_x = new_step_x;
    step_y = new_step_y;
    half_num_x = new_half_num_x;
    half_num_y = new_half_num_y;
    num_theta = new_num_theta;
    min_chord = new_min_chord;
    step_chord = new_step_chord;
    num_chord = new_num_chord;
    step_height = new_step_height;
    half_num_height = new_half_num_height;  
    return;
  }
};


// ============================================================================
// dbsks_circ_arc_grid
// ============================================================================

// Shape fragment grid parameters
class dbsks_circ_arc_grid
{
public:
  //: Constructor 
  dbsks_circ_arc_grid(){};
  ~dbsks_circ_arc_grid(){};



  // x and y are coordinates of the mid-point of the arc
  // x
  double min_x_;
  double max_x_;
  int num_x_;
  double step_x_;
  vnl_vector<double > x_;

  // y
  double min_y_;
  double max_y_;
  int num_y_;
  double step_y_;
  vnl_vector<double > y_;

  // theta - it should cover all angles [0, 2pi]
  double min_theta_;
  double max_theta_;
  int num_theta_;
  double step_theta_;
  vnl_vector<double > theta_;
  
  // chord length (width of the bounding box)
  double min_chord_;
  double max_chord_;
  int num_chord_;
  double step_chord_;
  vnl_vector<double > chord_;

  // height of the bounding box of the arc
  double min_height_;
  double max_height_;
  int num_height_;
  double step_height_;
  vnl_vector<double > height_;

  
  //: Set grid parameters // default values
  void set_grid_params();

  // Set grid parameters from a user-input parameter set
  void set_grid(const dbsks_circ_arc_grid_params& params);

  //: Compute step for each dimension given min, max, and number of values
  void compute_grid();

  //: Total number of points in the grid
  int size() const;

  //: Total number of planes in the grid (size() / (num_x * num_y))
  int num_planes() const;

  //: convert linear index to grid index (multiple params)
  // all exclude x and y
  void linear_to_grid(int i_plane, int& i_chord, int& i_height, int& i_theta) const;

  //: convert grid index to linear index
  // all exclude x and y
  int grid_to_linear(int i_chord, int i_height, int i_theta) const;

  //: Return (approximate) index given parameter value
  int i_x(double x) const;
  int i_y(double y) const;
  int i_theta(double theta) const;
  int i_chord(double chord) const;
  int i_height(double height) const;

  //: Find grid indices of the arc closest to a given circular arc
  void arc_to_grid(const dbgl_circ_arc& arc, int& i_x, int& i_y,
    int& i_chord, int& i_height, int& i_theta) const;




  //: Print grid information
  void print(vcl_ostream& str) const;
};



#endif // seg/dbsks/dbsks_circ_arc_grid.h


