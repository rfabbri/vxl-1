// This is brcv/dbsk2d/vis/dbsk2d_shock_tableau.h
#ifndef dbsk2d_shock_tableau_h_
#define dbsk2d_shock_tableau_h_

//:
// \file
// \brief A tableau to display dbsk2d_shock_storage objects.
// \author Mark Johnson
// \date Aug 28 2003
//
// \verbatim
//  Modifications
//
//    Amir Tamrakar  06/21/05   Renamed from dbsk2d_ishock_tableau
//                              because it needs to draw the coarse
//                              level shock graph too.
//                              Added code for displaying 
//                              dbsk2d_shock_graph object
//
//    Amir Tamrakar 06/24/05    Added a popup menu to select display options 
//
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <dbsk2d/dbsk2d_base_gui_geometry.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_rich_map_sptr.h>

#include <vgui/vgui_gl.h>

#include "dbsk2d_shock_tableau_sptr.h"

//: A tableau to display dbsk2d_shock_storage objects
class dbsk2d_shock_tableau : public vgui_tableau 
{
public:

  //: Constructor
  dbsk2d_shock_tableau();
  virtual ~dbsk2d_shock_tableau();

  //: handle the render and select events
  virtual bool handle( const vgui_event & );

  //: return the boundary
  dbsk2d_boundary_sptr get_boundary() { return boundary_; }
  //: set the boundary
  void set_boundary( dbsk2d_boundary_sptr new_boundary ) { boundary_ = new_boundary; }

  //: return the intrinsic shock graph
  dbsk2d_ishock_graph_sptr get_ishock_graph() { return ishock_graph_; }
  //: set the intrinsic shock graph
  void set_ishock_graph( dbsk2d_ishock_graph_sptr new_ishock_graph ) { ishock_graph_ = new_ishock_graph; }

  //: return the coarse shock graph
  dbsk2d_shock_graph_sptr get_shock_graph() { return shock_graph_; }
  //: set the coarse shock graph
  void set_shock_graph( dbsk2d_shock_graph_sptr new_shock_graph ) { shock_graph_ = new_shock_graph; }

  //: return the rich_map
  dbsk2d_rich_map_sptr get_rich_map() { return rich_map_; }
  //: set the rich map
  void set_rich_map( dbsk2d_rich_map_sptr new_rich_map ) { rich_map_ = new_rich_map; }

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  // functions to set the display parameters
  void display_contact_shocks(bool dcon_shocks) { display_contact_shocks_ = dcon_shocks; }
  void display_shock_nodes(bool dshock_nodes) { display_shock_nodes_ = dshock_nodes; }

  void toggle_contact_shocks( void ) { display_contact_shocks_ = !display_contact_shocks_; }
  void toggle_shock_nodes( void ) { display_shock_nodes_ = !display_shock_nodes_; }

  //: select an object displayed in the tableau at these coordinates
  dbsk2d_base_gui_geometry * select( int ax , int ay );
 
  // rendering funtions

  //: render the boundary class
  void draw_boundary_select();
  void draw_boundary_render();

  //: render an intrinsic shock graph
  void draw_ishock_select();
  void draw_ishock_render();

  //: render the coarse shock graph
  void draw_shock_select();
  void draw_shock_render();

  //: render the shock fragments
  void draw_shock_fragments_render();

  //: render the rich map
  void draw_rich_map_select();
  void draw_rich_map_render();

private:
  GLenum gl_mode;  

  dbsk2d_base_gui_geometry * current;     ///< the current selection

  dbsk2d_boundary_sptr boundary_;         ///< the associated boundary class
  dbsk2d_ishock_graph_sptr ishock_graph_; ///< the intrinsic shock graph 
  dbsk2d_shock_graph_sptr shock_graph_;   ///< the coarse shock graph
  dbsk2d_rich_map_sptr rich_map_;         ///< the rich map

  bool display_boundary_;                 ///< display the boundary
  bool display_boundary_cells_;           ///< display the boundary cells
  bool display_rich_map_grid_;            ///< display the grid lines of the rich map
  bool display_boundary_burnt_regions_;   ///< display the burnt regions of the boundary elements?
  bool display_contact_shocks_;           ///< display the contact shock?
  bool display_shock_nodes_;              ///< display shock nodes
  bool display_intrinsic_shocks_;         ///< display the intrinsic shock graph?
  bool display_coarse_shocks_;            ///< display the coarse shock graph?
  bool display_shock_fragments_;          ///< display the shock fragments?
  
  bool display_xshock_samples_;           ///< display the extrinsic shock samples?
  bool display_xshock_node_samples_;           ///< display the extrinsic shock samples?
  bool display_xshock_edge_samples_;           ///< display the extrinsic shock samples?
  
  float rnd_colormap[100][3];               ///< random colormap

};


//: Create a smart-pointer to a dbsk2d_shock_tableau.
struct dbsk2d_shock_tableau_new : public dbsk2d_shock_tableau_sptr
{
  typedef dbsk2d_shock_tableau_sptr base;

  //: Constructor - creates a pointer to a dbsk2d_shock_tableau
  dbsk2d_shock_tableau_new() : base(new dbsk2d_shock_tableau()) { }
};

#endif //dbsk2d_shock_tableau_h_
