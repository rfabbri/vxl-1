// This is shp/dbsksp/dbsksp_shock_snake.h
#ifndef dbsksp_shock_snake_h_
#define dbsksp_shock_snake_h_

//:
// \file
// \brief A deformable contour modeled by shock of the shape
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Dec 26, 2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsksp/algo/dbsksp_external_energy_function.h>
#include <vil/vil_image_view.h>




//: A base class for a deformable contour
class dbsksp_shock_snake_base : public vbl_ref_count
{
  typedef int RETURN_CODE;
public:
  // --------------------------------------------------------------------------
  // CONSTRUCTORS / DESTRUCTORS 
  // --------------------------------------------------------------------------

  //: Constructor
  dbsksp_shock_snake_base(){};
  
  //: Destructor
  virtual ~dbsksp_shock_snake_base(){};
  
  
  // DATA ACCESS --------------------------------------------------------------
  
  dbsksp_shock_graph_sptr shock_graph() const {return this->shock_graph_; }
  void set_shock_graph(const dbsksp_shock_graph_sptr& shock_graph)
  { this->shock_graph_ = shock_graph; }

  
protected:
  // MEMBER VARIABLES ---------------------------------------------------------

  //: the deformable shock graph
  dbsksp_shock_graph_sptr shock_graph_;

  
};

class dbsksp_shock_snake : public dbsksp_shock_snake_base
{
  public:
  
  // CONSTRUCTORS / DESTRUCTORS -----------------------------------------------

  //: Constructor
    dbsksp_shock_snake() : 
        ext_cost_integrator_(0),
        verbal_(true)
    {};
  
  //: Destructor
  virtual ~dbsksp_shock_snake(){};
  
  
  // DATA ACCESS --------------------------------------------------------------
  
  // Set and get the external cost
  dbsksp_external_energy_function* ext_cost_integrator() const 
  {return this->ext_cost_integrator_; }

  void set_ext_cost_integrator(dbsksp_external_energy_function* ext_cost_integrator)
  { this->ext_cost_integrator_ = ext_cost_integrator; }

  //: Set and Get potential image
  void set_potential_image(const vil_image_view<float >& potential_image)
  { this->potential_image_ = potential_image; }
  

  //: Return ratio between internal and external energy
  void set_lambda(double new_lambda) {this->lambda_ = new_lambda; }
  double lambda() const {return lambda_;}

  //: Get and set verbal
  bool verbal() const {return this->verbal_; }
  void set_verbal(bool verbal) {this->verbal_ = verbal; };


  
  // --------------------------------------------------------------------------
  // MAIN FUNCTIONS
  // --------------------------------------------------------------------------


  //: Optimize a two shapelet to fit to the image
  // Return ERROR code
  int optimize_twoshapelet_method_1(const dbsksp_twoshapelet_sptr& init_ss,
    dbsksp_twoshapelet_sptr& final_ss);

  //: Optimize a two shapelet to fit to the image - method 2
  // variables are the position, radius, and phi of the end point
  // Return ERROR code
  int optimize_twoshapelet_method_2(const dbsksp_twoshapelet_sptr& init_ss,
    dbsksp_twoshapelet_sptr& final_ss);


  //: Optimize a ``terminal twoshapelet'' to fit to the image
  // Return ERROR code
  int optimize_terminal_twoshapelet(const dbsksp_shapelet_sptr& init_ss,
    dbsksp_shapelet_sptr& final_ss);


  //: Optimize the two active edges to fit the boundary
  // Return ERROR code
  int optimize_two_active_edges(const dbsksp_shock_node_sptr& source_node,
    const dbsksp_shock_edge_sptr& source_edge,
    dbsksp_twoshapelet_sptr& final_twoshapelet);

  //: Extend the shock graph at the ``terminal_edge" with two fragments whose
  // chord lengths are ``chord0" and ``chord1", then deform the two new fragments
  // to match with the underlying image
  int extend_and_deform_2_edges(dbsksp_shock_edge_sptr& terminal_edge,
    double chord0, double chord1, 
    dbsksp_twoshapelet_sptr& final_twoshapelet);

  //: Extend the shock graph at an edge with one fragment so that the total 
  // chord length is ``chord_length" then deform the two new fragments to match
  // with the underlying image
  int add_one_edge_and_deform(const dbsksp_shock_node_sptr& source_node,
    dbsksp_shock_edge_sptr& source_edge,
    double total_chord, 
    dbsksp_twoshapelet_sptr& final_twoshapelet);

  //: Extend the shock graph at the ``terminal_edge" with two fragments whose
  // chord lengths are ``chord0" and ``chord1", then deform the two new fragments
  // to match with the underlying image. Do this ``num_times" times
  int extend_and_deform_2_edges_n_times(dbsksp_shock_edge_sptr& terminal_edge,
    double chord0, double chord1,
    int num_times,
    dbsksp_twoshapelet_sptr& final_twoshapelet);

  //: Extend and deform the shock graph at the ``terminal_edge" with two
  // fragments untill a ``target_point" is within reach.
  int extend_and_deform_2_edges_till_reach_target(dbsksp_shock_edge_sptr& terminal_edge,
    double chord0, double chord1,
    const vgl_point_2d<double >& target_point, 
    int max_num_times,
    dbsksp_twoshapelet_sptr& final_twoshapelet);



  






  // --------------------------------------------------------------------------
  // SUPPORT FUNCTIONS
  // --------------------------------------------------------------------------

  //: Build a two-shapelet given a source_node and a source_edge
  // A group of two edges, starting from source_node and extending in the direction
  // of source_edge, are used to construct the two-shapelet
  // Return 0 if no twoshapelet is constructed
  // Possible reason for failure
  // - source_edge is not incident to source_node
  // - there is ambiguity in extending to two edges (hitting a A_1^3 node)
  dbsksp_twoshapelet_sptr build_twoshapelet(const dbsksp_shock_node_sptr& source_node,
    const dbsksp_shock_edge_sptr& source_edge);


  //: Print debugging info to the screen

  
  // HACK
  // Hard constraints
  double max_width_;
  double min_width_;
  //////////////////////////

  
protected:
  // --------------------------------------------------------------------------
  // MEMBER VARIABLES 
  // --------------------------------------------------------------------------

  //: external cost integrator - summing up cost along the object
  dbsksp_external_energy_function* ext_cost_integrator_;

  vil_image_view<float > potential_image_;

  //: displaying debug info while running
  bool verbal_;

  double lambda_;

  
};

#endif // shp/dbsksp/dbsksp_shock_snake.h

