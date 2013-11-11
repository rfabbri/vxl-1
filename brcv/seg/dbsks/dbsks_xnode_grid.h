// This is seg/dbsks/dbsks_xnode_grid.h
#ifndef dbsks_xnode_grid_h_
#define dbsks_xnode_grid_h_

//:
// \file
// \brief A class for a grid of extrinsic nodes
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 13, 2008
//
// \verbatim
//  Modifications
//    Nhon Trinh   Feb 6, 2010     sample r linearly (instead of log)
// \endverbatim

#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <vnl/vnl_vector.h>


//: one type of parameter set to construct a xnode grid
struct dbsks_xnode_grid_params
{
  double min_x;
  double step_x;
  int num_x;
  
  double min_y;
  double step_y;
  int num_y;
  
  // psi - shock tangent angle it should cover all angles [0, 2pi]
  double min_psi;
  double step_psi;
  int num_psi;
  
  // phi0
  double min_phi0;
  double step_phi0;
  int num_phi0;

  // r
  double min_r;
  double step_r;
  int num_r;
  
  // phi1 -- ONLY for degree-3 nodes
  double min_phi1;
  double step_phi1;
  int num_phi1;
};
  

  



// ============================================================================
// dbsks_xnode_grid
// ============================================================================

// Shape fragment grid parameters
class dbsks_xnode_grid
{
public:
  dbsks_xnode_grid(){};
  ~dbsks_xnode_grid(){};

  //: Compute from grid parameters
  bool compute(const dbsks_xnode_grid_params& params);


  //: Set psi vector
  void set_psi_vec(double min_psi, double max_psi, double step_psi);

  //: Set phi0 vector
  void set_phi0_vec(double min_phi0, double max_phi0, double step_phi0);


  
  //: Return an xnode descriptor
  dbsksp_xshock_node_descriptor xdesc(int i_x, int i_y, int i_psi, int i_phi0, int i_r) const;

  //: Compute indices of a node descriptor. Return false if out of range
  bool xdesc_to_grid(const dbsksp_xshock_node_descriptor& xdesc, 
    int& i_x, int& i_y, int& i_psi, int& i_phi0, int& i_r) const;

  //: Compute indices of a node descriptor. Return false if out of range
  int grid_to_linear(int i_x, int i_y, int i_psi, int i_phi0, int i_r) const;

  //: Compute indices of a node descriptor. Return false if out of range
  int grid_to_linear(int i_x, int i_y, int i_psi, int i_phi0, int i_r, int i_phi1) const;

  //: Compute indices of a node descriptor. Return false if out of range
  bool linear_to_grid(int i_linear, int& i_x, int& i_y, int& i_psi, int& i_phi0, int& i_r);

  //: Compute indices of a node descriptor. Return false if out of range
  bool linear_to_grid(int i_linear, int& i_x, int& i_y, int& i_psi, int& i_phi0, int& i_r, int& i_phi1);

  //: Convert position within a cell (in x-y plane) from grid index to linear index
  int cell_grid_to_linear(int i_psi, int i_phi0, int i_r) const;

  //: Convert position within a cell (in x-y plane) from linear index to grid index
  bool cell_linear_to_grid(int i_linear, int& i_psi, int& i_phi0, int& i_r);

  //: Number of cells
  int num_cells() const {return this->x_.size() * this->y_.size(); }

  //: Number of states per cell
  int num_states_per_cell() const
  { return this->psi_.size()*this->phi0_.size()*this->r_.size(); };

  int size() const {return this->x_.size()*this->y_.size()*this->psi_.size()*
    this->phi0_.size()*this->r_.size()*this->phi1_.size(); }
  

  //: Return index in the grid. No range check
  int i_x(double x) const;
  int i_y(double y) const;
  int i_psi(double psi) const;
  int i_phi0(double phi0) const;
  int i_r(double r) const;
  int i_phi1(double phi1) const;

  //: Generate a grid of points surrounding a reference location
  // The generated points are guaranteed to lie inside the grid
  bool sample_xy(double x0, double y0, int num_x_backward, int num_x_forward,
    int num_y_backward, int num_y_forward, 
    vcl_vector<double >& xs, vcl_vector<double >& ys,
    vcl_vector<int >& index_xs, vcl_vector<int >& index_ys);

  //: sample x
  bool sample_x(double ref_x, int num_x_backward, int num_x_forward,
    vcl_vector<double >& x_vec, vcl_vector<int >& index_x_vec) const;

  //: sample y
  bool sample_y(double ref_y, int num_y_backward, int num_y_forward,
    vcl_vector<double >& y_vec, vcl_vector<int >& index_y_vec) const;

  //: samples of psi on the grid around a given psi value "ref_psi"
  bool sample_psi(double ref_psi, int num_psi_backward, int num_psi_forward, 
    vcl_vector<double >& psi_vec, vcl_vector<int >& index_psi_vec) const;

  //: sample phi0
  bool sample_phi0(double ref_phi0, int num_phi0_backward, int num_phi0_forward,
    vcl_vector<double >& phi0_vec, vcl_vector<int >& index_phi0_vec) const;
  
  //: sample r
  bool sample_r(double ref_r, int num_r_backward, int num_r_forward,
    vcl_vector<double >& r_vec, vcl_vector<int >& index_r_vec) const;

  //: sample phi1
  bool sample_phi1(double ref_phi1, int num_phi1_backward, int num_phi1_forward,
    vcl_vector<double >& phi1_vec, vcl_vector<int >& index_phi1_vec) const;

  

  // Properties wrt the starting xnode ----------------------------------------- 
  vcl_ostream& print(vcl_ostream& os);

public:
  //: x
  double step_x_;
  vnl_vector<double > x_;

  //: y
  double step_y_;
  vnl_vector<double > y_;

  //: psi - shock tangent angle it should cover all angles [0, 2pi]
  double step_psi_;
  vnl_vector<double > psi_;

  //: phi0
  double step_phi0_;
  vnl_vector<double > phi0_;

  //: r
  double step_r_;
  vnl_vector<double > r_;
  
  //: phi1 -- ONLY for degree-3 nodes
  double step_phi1_;
  vnl_vector<double > phi1_;

};


#endif // seg/dbsks/dbsks_xnode_grid.h


