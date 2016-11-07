// This is seg/dbsks/dbsks_shapelet_grid.h
#ifndef dbsks_shapelet_grid_h_
#define dbsks_shapelet_grid_h_

//:
// \file
// \brief A class for a grid of shapelets
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Nov 5, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_vector.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>




struct dbsks_shapelet_grid_params
{
  float center_x;
  float center_y;
  float step_x;
  float step_y;
  int half_num_x;
  int half_num_y;
  
  int num_psi;

  float half_range_phiA;
  int half_num_phiA;

  float half_range_phiB;
  int half_num_phiB;

  float max_m;
  int half_num_m;

  float max_log2_len;
  int half_num_len;

  float max_log2_rA;
  int half_num_rA;

  bool force_phi_centered_at_pi_over_2;
  // void init_default();
};




// ============================================================================
// dbsks_shapelet_grid
// ============================================================================

// Shape fragment grid parameters
class dbsks_shapelet_grid
{
public:
  dbsks_shapelet_grid(): has_front_arc_(false), has_rear_arc_(false){};
  dbsks_shapelet_grid(const dbsksp_shapelet_sptr& s0);

  // x
  double min_xA_;
  double max_xA_;
  int num_xA_;
  double step_xA_;
  vnl_vector<double > xA_;

  // y
  double min_yA_;
  double max_yA_;
  int num_yA_;
  double step_yA_;
  vnl_vector<double > yA_;

  // psi - shock tangent angle it should cover all angles [0, 2pi]
  double min_psiA_;
  double max_psiA_;
  int num_psiA_;
  double step_psiA_;
  vnl_vector<double > psiA_;

  // phiA - limited to [pi/4, 3pi/4]
  double min_phiA_;
  double max_phiA_;
  int num_phiA_;
  double step_phiA_;
  vnl_vector<double > phiA_;

  // phiB - limited to [pi/4, 3pi/4]
  double min_phiB_;
  double max_phiB_;
  int num_phiB_;
  double step_phiB_;
  vnl_vector<double > phiB_;

  // param_m - limited to [-0.5 .5]
  double max_m_;
  double min_m_; 
  int num_m_;
  double step_m_;
  vnl_vector<double > m_;

  // Radius and chord length require a reference shape
  // They determine the size of the object. All searched sizes should be relative
  // to the reference fragment
  // rA
  double ref_rA_;
  double min_log2_rA_;
  double max_log2_rA_;
  int num_rA_;
  double step_log2_rA_;
  vnl_vector<double > rA_;

  // chord length
  double ref_len_;
  double min_log2_len_;
  double max_log2_len_;
  int num_len_;
  double step_log2_len_;
  vnl_vector<double > len_;

  // the shapelet include the front terminal fragment
  bool has_front_arc_; 

  // the shapelet include the rear terminal fragment
  bool has_rear_arc_;

  //---------------------------------------------------------------------

  //: Set parameters of the grid
  void set_grid(const dbsksp_shapelet_sptr& sref, 
    bool has_front_arc, bool has_rear_arc,
    const dbsks_shapelet_grid_params& params);

  //: Compute step for each dimension given min, max, and number of values
  void compute_grid();

  //: Total number of points in the grid
  int size() const;

  // Properties wrt the shapelets ---------------------------------------------

  //: Total number of "planes"
  int num_planes() const;

  // convert linear index to grid index (multiple params)
  // all exclude x andy y
  void linear_to_grid(int i_plane, 
    int& i_phiA, int& i_phiB, int& i_m, int& i_rA, int& i_len, int& i_psiA) const;

  // convert grid index to linear index
  // all exclude x and y
  int grid_to_linear(int i_phiA, int i_phiB, int i_m, int i_rA, 
    int i_len, int i_psiA) const;

  //: convert between plane grid index (x,y)  and linear index (y + x*num_y)
  int grid_to_linear(int i_xA, int i_yA) const;
  void linear_to_grid(int i_xy, int& i_xA, int& i_yA) const;


  // Properties wrt the starting xnode ----------------------------------------- 

  //: Total number of planes to contain all xnodes
  int sxnode_num_planes() const;

  //: linear index to grid index (for the starting xnode)
  void sxnode_linear_to_grid(int i_plane, int& i_phiA, int& i_rA, int& i_psiA) const;

  //: grid index to linear index (for the starting xnode)
  int sxnode_grid_to_linear(int i_phiA, int i_rA, int i_psiA) const;

  // ------------------------------------------------------------------------
  //: Return (approximate) index given parameter value
  int i_xA(double xA) const;
  int i_yA(double yA) const;
  int i_psiA(double psiA) const;
  int i_psiA(const vgl_vector_2d<double >& tangentA) const;
  int i_m(double m) const;
  int i_rA(double rA) const;
  int i_len(double len) const;
  int i_phiA(double phiA) const;
  int i_phiB(double phiB) const;

  //: Check if a configuration is legal (within bounds of the allowed range
  bool is_legal(int i_phiA, int i_phiB, int i_m, int i_rA, int i_len, int i_psiA) const;

  //: Check if the position of a shapelet within bounds of the allowed range
  bool is_legal(int i_xA, int i_yA) const;
  

  //: Compute grid parameters of the discretized shapelet closest to a given shapelet
  void approx_shapelet_on_grid(const dbsksp_shapelet_sptr& s, int& i_xy, int& i_plane) const;
  dbsksp_shapelet_sptr approx_shapelet_on_grid(const dbsksp_shapelet_sptr& s) const;

  //: Return a shapelet given its indices
  dbsksp_shapelet_sptr shapelet(int i_xy, int i_plane) const;

  //: Translate grid
  void translate_grid_center_to(double center_x, double center_y);

  //: Change number of data points for xA
  void change_num_xA(int num_xA);

  //: Chang number of data points for yA
  void change_num_yA(int num_yA);

  //: Print grid information
  void print(vcl_ostream& str);
};


#endif // seg/dbsks/dbsks_shapelet_grid.h


