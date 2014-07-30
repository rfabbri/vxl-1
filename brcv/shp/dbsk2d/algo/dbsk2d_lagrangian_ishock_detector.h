// This is brcv/shp/dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.h
#ifndef dbsk2d_lagrangian_ishock_detector_h_
#define dbsk2d_lagrangian_ishock_detector_h_
//:
// \file
// \brief Intrinsic shock computation algorithm (Lagrangian)
// \author Amir Tamrakar
// \date 02/09/05
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/09/2005    Initial version. Conversion to VXL standard.
//                               Moved it out of dbsk2d_ishock class.
//
//   Amir Tamrakar 05/04/05      Added the ordered shock list from the ishock class.
//                               This way the repository for shock elements becomes
//                               independent of the shock computation algorithm.
//
//   Amir Tamrakar 07/07/05      Moved it to dbsk2d/algo
//
//   Amir Tamrakar 08/29/05      Renamed it to dbsk2d_lagrangian_ishock_detector
//                               because I'm using the name dbsk2d_ishock_detector for 
//                               the more generic Eulerian Lagrangian Detector
//
//   Amir Tamrakar 01/24/06      Removed a bunch of defunct methods
//
// \endverbatim

#include <dbsk2d/dbsk2d_ishock_utils.h>

#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph.h>

#include <dbsk2d/dbsk2d_ishock_node.h>

#include <dbsk2d/dbsk2d_ishock_contact.h>             
#include <dbsk2d/dbsk2d_ishock_pointpoint.h>         
#include <dbsk2d/dbsk2d_ishock_pointline.h>          
#include <dbsk2d/dbsk2d_ishock_pointarc.h>           
#include <dbsk2d/dbsk2d_ishock_lineline.h>           
#include <dbsk2d/dbsk2d_ishock_linearc.h>            
#include <dbsk2d/dbsk2d_ishock_arcarc.h>             
#include <dbsk2d/dbsk2d_ishock_lineline_thirdorder.h>
#include <dbsk2d/dbsk2d_ishock_arcarc_thirdorder.h> 

#include <dbsk2d/algo/dbsk2d_ishock_compute_intersection.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd_sptr.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd.h>

//uncomment this line if you wish to trace the steps in a verbose manner
//#define DEBUG_SHOCK_VERBOSE

//: This class contains the algorithm to compute intrinsic shocks from a boundary class
//
// This class employs two time ordered shock lists: the active shocks list and the 
// candidate sources list. These are needed to perform the wave propagation simulation.
// Both these lists contain pointers only while the actual shock elements are stored
// in the shock graph class.
//
// The list is ordered using the following key: (time(R), id).
// Time is either the time of formation or the the time of next intersection 
// or the time of termination and id is the unique id of the shock element 
// which only exists in the key to make it unique
// because the first two properties can produce duplicate keys. 
class dbsk2d_lagrangian_ishock_detector
{
public:

  //:PROPAGATION AND INTERSECTIONS TYPES
  enum PROPAGATION_TYPE
  {
    BOGUS_PROPAGATION_TYPE=-1,
    NO_PROPAGATION=0,
    PROPAGATION_DONE,
    PROPAGATION_TO_INFINITY,
    PROPAGATION_TO_BND,

    INVALID_CANDIDATE_SOURCE,
    A3_FORMATION,
    REGULAR_JUNCT,
    DEGENERATE_JUNCT,
    SINK_FORMATION,

    NEW_SHOCK_FROM_A3,
    NEW_SHOCKS_FROM_SOURCE,
    NEW_SHOCK_FROM_JUNCT,
    NEW_CAND_SRC,

    LEFT_INTERSECTION,
    RIGHT_INTERSECTION,
    BOTH_INTERSECTION,
    THIRD_ORDER_FORMATION,
    ARC_THIRD_ORDER_FORMATION,

    PROPAGATION_ERROR_DETECTED,
  };

  enum JUNCTION_TYPE 
  {
    BOGUS_JUNCTION_TYPE,
    SINK_JUNCTION,
    DEGENERATE_JUNCTION,
    LEFT_REGULAR_JUNCTION,
    RIGHT_REGULAR_JUNCTION,
    NO_JUNCTION,
  };

protected:  
  
  dbsk2d_ishock_graph_sptr _ishock_graph;   ///> The actual shock graph
  dbsk2d_boundary_sptr _boundary; ///> The coupled boundary class
  dbsk2d_bnd_cell_sptr _bnd_cell; ///> The coupled boundary class

  dbsk2d_lagrangian_cell_bnd_sptr _left_bnd;
  dbsk2d_lagrangian_cell_bnd_sptr _right_bnd;
  dbsk2d_lagrangian_cell_bnd_sptr _top_bnd;
  dbsk2d_lagrangian_cell_bnd_sptr _bottom_bnd;

  //: All the active shock elements ordered by simtime
  ordered_shock_list active_shocks_list;

  //: This is a list of candidate sources that is generated during the N^2 
  //  initialization process and is also ordered by time
  ordered_src_list candidate_src_list;

  dbsk2d_ishock_edge* _nextShockToPropagate;  ///> why do we need this???

  double _sim_time; ///> current simulation time

  bool _bValid; ///> is this lagrangian cell still valid?

  vcl_map<unsigned int,dbsk2d_ishock_belm*> deleted_bnd_elements_;

  vcl_set<int> deleted_ishock_elements_;

  bool local_shock_;

public:
  
  //: Default Constructor
  dbsk2d_lagrangian_ishock_detector();

  //: Constructor
  dbsk2d_lagrangian_ishock_detector(dbsk2d_boundary_sptr boundary);

  //: Destructor
  virtual ~dbsk2d_lagrangian_ishock_detector();

  //-------------------------------------------------------------------
  // functions to access and set protected members
  //-------------------------------------------------------------------
  
  dbsk2d_ishock_graph_sptr ishock_graph(){ return _ishock_graph; }
  void set_ishock_graph(dbsk2d_ishock_graph_sptr ishock_graph){ _ishock_graph = ishock_graph; }
  
  dbsk2d_boundary_sptr boundary() { return _boundary; }
  void set_boundary(dbsk2d_boundary_sptr boundary) { _boundary = boundary; }
  void set_bnd_cell(dbsk2d_bnd_cell_sptr bnd_cell);
  void set_local_shock(bool local_shock){local_shock_=local_shock;}

  dbsk2d_lagrangian_cell_bnd_sptr left_bnd() { return _left_bnd; }
  dbsk2d_lagrangian_cell_bnd_sptr right_bnd() { return _right_bnd; }
  dbsk2d_lagrangian_cell_bnd_sptr top_bnd() { return _top_bnd; }
  dbsk2d_lagrangian_cell_bnd_sptr bottom_bnd() { return _bottom_bnd; }

  void set_left_bnd(dbsk2d_lagrangian_cell_bnd_sptr bnd) { _left_bnd = bnd; }
  void set_right_bnd(dbsk2d_lagrangian_cell_bnd_sptr bnd) { _right_bnd = bnd; }
  void set_top_bnd(dbsk2d_lagrangian_cell_bnd_sptr bnd) { _top_bnd = bnd; }
  void set_bottom_bnd(dbsk2d_lagrangian_cell_bnd_sptr bnd) { _bottom_bnd = bnd; }
  vcl_map<unsigned int,dbsk2d_ishock_belm*> get_deleted_bnd_elements()
  {return deleted_bnd_elements_;}

  void clear_deleted_elements(){deleted_bnd_elements_.clear();}
  void clear_active_shock(dbsk2d_ishock_edge* cur_edge)
  {
      active_shocks_list.erase(r_id_pair(cur_edge->simTime(),cur_edge->id()));
  }

  ordered_shock_list& active_shocks() { return active_shocks_list; }
  ordered_src_list& cand_src_list() { return candidate_src_list; }

  double sim_time() { return _sim_time; }
  bool is_valid() { return _bValid; }
  void make_invalid() { _bValid = false; }

  //: is a given point inside this lagrangian cell?
  bool is_point_inside_the_cell(vgl_point_2d<double> pt);

  //: delete everything (all the shocks and any other addons)
  void clear();  

  //-------------------------------------------------------------------
  // functions to manage the ordered shock elements list
  //-------------------------------------------------------------------
  //: number of active shock edges
  long number_of_active_shocks() { return active_shocks_list.size(); }
  //: number of candidate sources remaining
  long number_of_candidate_sources() { return candidate_src_list.size(); }

  //: delete all the candidate sources
  void delete_the_candidate_src_list();
  //: add all the active shock edges to the active shocks list
  void compile_the_active_selm_list(); 
  //: delete all the shocks from the active shocks list
  void delete_the_active_selm_list();
  void add_an_ishock_edge (dbsk2d_ishock_edge* Sedge);
  void add_an_ishock_node (dbsk2d_ishock_node* Snode);
  void delete_an_ishock_elm (dbsk2d_ishock_elm* selm);
  void update_an_ishock_elms_simtime (dbsk2d_ishock_edge* selm, double stime);
  void deactivate_an_ishock_elm (dbsk2d_ishock_elm* selm);
  void reactivate_an_ishock_elm (dbsk2d_ishock_elm* selm);

  //-------------------------------------------------------------------
  // functions to manage dynamic changes in boundary
  //-------------------------------------------------------------------
  void delete_a_belm(dbsk2d_ishock_belm* belm);

  //-------------------------------------------------------------------
  // Shock computation functions
  //-------------------------------------------------------------------
  virtual bool detect_shocks ();

  //-------------------------------------------------------------------
  // Shock initialization functions
  //-------------------------------------------------------------------
  virtual void initialize_shocks ();
    void initialize_contacts_and_A3s();
    void initialize_contacts_and_A3s(vcl_vector<dbsk2d_ishock_belm*> belm_list);
    void initialize_contacts_and_A3s_recompute();

    void init_cand_src_between(dbsk2d_ishock_belm* belm1, dbsk2d_ishock_belm* belm2);
    void init_cand_src_between(dbsk2d_ishock_bpoint* bp1, dbsk2d_ishock_bpoint* bp2);
    void init_cand_src_between(dbsk2d_ishock_bpoint* bp, dbsk2d_ishock_bline* bl);
    void init_cand_src_between(dbsk2d_ishock_bpoint* bp, dbsk2d_ishock_barc* ba);
    void init_cand_src_between(dbsk2d_ishock_bline* bl1, dbsk2d_ishock_bline* bl2);
    void init_cand_src_between(dbsk2d_ishock_bline* bl, dbsk2d_ishock_barc* ba);
    void init_cand_src_between(dbsk2d_ishock_barc* ba1, dbsk2d_ishock_barc* ba2);

    dbsk2d_ishock_contact* form_a_contact_shock(dbsk2d_ishock_belm* lbelm, dbsk2d_ishock_belm* rbelm);
    dbsk2d_ishock_node* form_a_corner_a3(dbsk2d_ishock_bcurve* lbcurve, dbsk2d_ishock_bcurve* rbcurve);
  
  //-------------------------------------------------------------------
  // Shock propagation functions
  //-------------------------------------------------------------------
  virtual bool propagate_shocks ();
    PROPAGATION_TYPE  propagate_next_active_shock ();  //single propagation step for detailed debugging
    void propagate_a_bunch_of_shocks ();               //A group of steps

    void finalize_propagation();  //finalize shock propagation in this cell by assigning wavefronts to 
                                  //cell boundaries that do not have shock intersections
  
    //: dynamically validate a candidate source using wavefront information
    PROPAGATION_TYPE  validate_candidate_source(dbsk2d_ishock_node* candsrc);
      //: update point eta before propagation 
      void update_point_etas(dbsk2d_ishock_node* cand_src);

    //: propagate shock branches from 2nd order sources
    PROPAGATION_TYPE propagate_from_a_source (dbsk2d_ishock_node* source);

    //: propagate a shock branch from a junction
    PROPAGATION_TYPE propagate_from_a_junction(dbsk2d_ishock_node* junct);

    //: propagate a shock path due to a pair of wavefronts, from a give shock node 
    dbsk2d_ishock_edge* propagate_from_a_node(dbsk2d_ishock_node* cur_node,
                                              dbsk2d_ishock_edge* lshock, dbsk2d_ishock_edge* rshock,
                                              dbsk2d_ishock_belm* lbelm, dbsk2d_ishock_belm* rbelm, 
                                              double lEta, double rEta);

    //: Look at the possibility of an approximate degenerate junction arising at a node (due to degeneracy transitions)
    dbsk2d_ishock_intersection_data compute_degenerate_intersection(dbsk2d_ishock_node* cur_node,
                                                                    dbsk2d_ishock_belm* lbelm, dbsk2d_ishock_belm* rbelm, 
                                                                    double lsEta, double rsEta, 
                                                                    dbsk2d_ishock_edge* Nshock,
                                                                    DIRECTION dir);

    //: force a shock to propagate to a node (for handling degeneracies) to a given tau
    void force_propagate_a_shock_to_a_junction(dbsk2d_ishock_edge* neighbor, 
                                               dbsk2d_ishock_node* cur_node, 
                                               double tau, DIRECTION dir);

    //: force a shock to propagate to a node (for handling degeneracies) to a given radius
    void force_propagate_a_shock_to_a_junction(dbsk2d_ishock_edge* neighbor, 
                                               dbsk2d_ishock_node* cur_node,
                                               DIRECTION dir);

    //: fix etas at a degenerate node
    void fix_etas_at_a_degen_node(dbsk2d_ishock_node* cur_node, 
                                  dbsk2d_ishock_edge* lshock, dbsk2d_ishock_edge* rshock,
                                  dbsk2d_ishock_belm* lbelm, dbsk2d_ishock_belm* rbelm, 
                                  double &lsEta, double &rsEta);

    //:form a child shock edge from a node
    dbsk2d_ishock_edge* form_a_child_shock_from_a_node(dbsk2d_ishock_node* src, 
                                                       dbsk2d_ishock_belm* lbelm, dbsk2d_ishock_belm* rbelm, 
                                                       double lsEta, double rsEta,
                                                       bool constrained=UNCONSTRAINED,
                                                       bool temp_shock = false);

    PROPAGATION_TYPE propagate_shock_to_the_nearest_junction (dbsk2d_ishock_edge* current);
      virtual void propagate_shock_to_a_left_junction (dbsk2d_ishock_edge* lslink, dbsk2d_ishock_edge* current, 
                                                       dbsk2d_ishock_intersection_data inter);
      virtual void propagate_shock_to_a_right_junction (dbsk2d_ishock_edge* current, dbsk2d_ishock_edge* rslink, 
                                                        dbsk2d_ishock_intersection_data inter);
      virtual void propagate_shock_to_infinity (dbsk2d_ishock_edge* elm);

      void update_intrinsic_parameters_of_a_shock (dbsk2d_ishock_edge* elm, double time,
                                                   double letau, double retau);
    
    //: form a junction node
    PROPAGATION_TYPE form_a_junction(dbsk2d_ishock_edge* current);

    //: form a sink node
    PROPAGATION_TYPE form_a_sink(dbsk2d_ishock_edge* current);
  
  //-------------------------------------------------------------------
  // intersection functions
  //-------------------------------------------------------------------
  //: get the neighboring shock to intersect with on the side specified
  //  Basically, return the shock sharing the same wavefront 
  dbsk2d_ishock_edge* get_neighboring_shock(dbsk2d_ishock_edge* current, DIRECTION dir);

  void intersect_with_cell_boundaries(dbsk2d_ishock_edge* sh);

  //-------------------------------------------------------------------
  //utility functions
  //-------------------------------------------------------------------
  JUNCTION_TYPE get_junction_type (dbsk2d_ishock_edge* sedge);

  //-------------------------------------------------------------------
  // DEBUG functions
  //-------------------------------------------------------------------
  //: validate the computation
  void validate_shocks();

  void print_ordered_selm_list (bool print_all=false);
  void print_selm_info_from_id(int id);

  void DebugPrintOnePropagation (int id, PROPAGATION_TYPE action);
  void MessageOutDetectionResults (int wndid);

};

#endif //dbsk2d_lagrangian_ishock_detector_h_
