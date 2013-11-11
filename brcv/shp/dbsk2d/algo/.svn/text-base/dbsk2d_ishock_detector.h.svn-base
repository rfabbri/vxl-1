// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_detector.h
#ifndef dbsk2d_ishock_detector_h_
#define dbsk2d_ishock_detector_h_
//:
// \file
// \brief Intrinsic shock computation algorithm (Eulerian-Lagrangian)
// \author Amir Tamrakar
// \date 02/09/05
// 
// \verbatim
//  Modifications
//
//   Amir Tamrakar 08/29/05      Renamed the original dbsk2d_ishock_detector to 
//                               dbsk2d_lagrangian_ishock_detector because this class
//                               is more generic and is designed for Eulerian-lagrangian
//                               shock detection.
//
// \endverbatim

#include <vbl/vbl_array_2d.h>
#include <dbsk2d/dbsk2d_ishock_utils.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd_sptr.h>

//uncomment this line if you wish to trace the steps in a verbose manner
//#define DEBUG_SHOCK_VERBOSE

typedef vcl_pair<dbsk2d_ishock_belm*, dbsk2d_ishock_belm*> wf_pair;
typedef vcl_set<wf_pair > wf_pairs_list;

//: This class contains the algorithm to compute intrinsic shocks from a boundary class
//
// More specifically, it uses the partitioned boundary cells to divide the computation
// into smaller blocks because the basic lagrangian computation which of the order N^2log(N)
// becomes extremely expensive when N gets large. Once the compuation in each of the cells
// is completed, adjacent cells are merged into larger cells and wavefronts at the common 
// boundary is allowed to interact in order to compute shocks between the cells.  
//
class dbsk2d_ishock_detector
{
protected:  
  dbsk2d_ishock_graph_sptr _ishock_graph;   ///> The actual shock graph
  dbsk2d_boundary_sptr _boundary; ///> The coupled boundary class

  //: 2d array of lagrangian cells
  vbl_array_2d<dbsk2d_lagrangian_ishock_detector > _cells;

  // these are variables for the merge schedule
  // they are defined here to enable step by step debugging
  int _cur_cell_row;
  int _cur_cell_col;
  int _next_cell_row;
  int _next_cell_col;
  bool _merging_cols;
  int _row_offset; 
  int _col_offset;

public:

  //: Constructor
  dbsk2d_ishock_detector(dbsk2d_boundary_sptr boundary);

  //: Destructor
  virtual ~dbsk2d_ishock_detector();

  //-------------------------------------------------------------------
  // functions to access and set protected members
  //-------------------------------------------------------------------
  dbsk2d_ishock_graph_sptr ishock_graph(){ return _ishock_graph; }
  dbsk2d_boundary_sptr boundary() { return _boundary; }
  
  const vbl_array_2d<dbsk2d_lagrangian_ishock_detector >& cells() { return _cells; }
  dbsk2d_lagrangian_ishock_detector & cell(int row, int col) { return _cells(row, col); }

  int cur_row() { return _cur_cell_row; }
  int cur_col() { return _cur_cell_col; }

  void set_cur_row(int cur_row) { _cur_cell_row = cur_row; }
  void set_cur_col(int cur_col) { _cur_cell_col = cur_col; }

  //: delete everything (all the shocks and any other addons)
  void clear();  
  
  //-------------------------------------------------------------------
  // Shock computation functions
  //-------------------------------------------------------------------

  void detect_shocks ();
  void initialize_shocks ();
  void propagate_shocks ();

  //: merge these two lagrangian cells 
  void merge_cells(int row1, int col1, int row2, int col2);
  void reactivate_shocks_at_cell_bnd(dbsk2d_lagrangian_ishock_detector& cur_cell, 
                                     dbsk2d_lagrangian_cell_bnd_sptr bnd);
  
  void find_interacting_wavefronts_at_bnd(dbsk2d_lagrangian_cell_bnd_sptr bnd1, 
                                          dbsk2d_lagrangian_cell_bnd_sptr bnd2, 
                                          wf_pairs_list& interacting_wf_list);

  void record_wavefront_interaction_between(dbsk2d_ishock_belm* belm1, 
                                            dbsk2d_ishock_belm* belm2,
                                            dbsk2d_lagrangian_cell_bnd_sptr bnd,
                                            wf_pairs_list& interacting_wf_list);
  
  void remove_virtual_endpoints(dbsk2d_lagrangian_ishock_detector& cur_cell, 
                                dbsk2d_lagrangian_cell_bnd_sptr bnd, 
                                dbsk2d_lagrangian_cell_bnd_sptr bnd2);

  void interact_wavefronts(dbsk2d_lagrangian_ishock_detector& cur_cell, 
                           wf_pairs_list& interacting_wf_list);

  //-------------------------------------------------------------------
  // Utility functions
  //-------------------------------------------------------------------

  //: is this element a virtual belement on this bnd?
  bool is_belm_virtual_on_bnd(dbsk2d_ishock_belm* belm, 
                              dbsk2d_lagrangian_cell_bnd_sptr bnd);

  //-------------------------------------------------------------------
  // DEBUG functions
  //-------------------------------------------------------------------

  // these functions are for the debugging tool
  void propagate_next_active_shock ();  //single propagation step for detailed debugging
  void propagate_a_bunch_of_shocks ();  //A bunch of steps to speed up debugging
  void propagate_shocks_without_merge(); //propagate shocks in all the cells without merging

  void merge_next_scheduled_cells(); //merge the next pair of cells in the merge schedule

  //: validate the computation
  bool validate_shocks();

};

#endif //dbsk2d_ishock_detector_h_
