// This is shp/dbsksp/dbsksp_xshock_design_tool.cxx
//:
// \file

#include "dbsksp_xshock_design_tool.h"


#include <vnl/vnl_math.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>

#include <bvis1/bvis1_manager.h>
#include <dbsksp/vis/dbsksp_soview_xshock.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/algo/dbsksp_interp_xshock_fragment.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree_sptr.h>
//#include <dbsksp/algo/dbsksp_xgraph_algos.h>
#include <dbsksp/algo/dbsksp_shock_path.h>
#include <dbsksp/algo/dbsksp_compute_scurve.h>

//: Constructor
dbsksp_xshock_design_tool::
dbsksp_xshock_design_tool() : 
  tableau_(0), 
  storage_(0), 
  selected_xedge_(0), 
  selected_xnode_(0)
{
  // define the keyboard combination for the gestures
  this->gesture_display_info_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);

  // node gestures

  // change phi at a node
  this->gesture_increase_phi_ = vgui_event_condition(vgui_key('p'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_phi_ = vgui_event_condition(vgui_key('p'), vgui_SHIFT, true);

  // node translation gesture
  this->gesture_increase_node_x_ = vgui_event_condition(vgui_key('i'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_node_x_ = vgui_event_condition(vgui_key('i'), vgui_SHIFT, true);

  this->gesture_increase_node_y_ = vgui_event_condition(vgui_key('j'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_node_y_ = vgui_event_condition(vgui_key('j'), vgui_SHIFT, true);

  // rotate all tangents around one node
  this->gesture_rotate_xnode_cw_ = vgui_event_condition(vgui_key('a'),vgui_MODIFIER_NULL,true);
  this->gesture_rotate_xnode_ccw_ = vgui_event_condition(vgui_key('a'), vgui_SHIFT, true);

  // rotate tangent of one edge at one node
  this->gesture_rotate_xtangent_cw_ = vgui_event_condition(vgui_key('t'),vgui_MODIFIER_NULL,true);
  this->gesture_rotate_xtangent_ccw_ = vgui_event_condition(vgui_key('t'), vgui_SHIFT, true);

  // change radius at a node
  this->gesture_increase_radius_ = vgui_event_condition(vgui_key('r'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_radius_ = vgui_event_condition(vgui_key('r'), vgui_SHIFT, true);

  // gestures affecting the graph as a whole
  
  // translate the whole shape along x-axis
  this->gesture_increase_graph_x_ = vgui_event_condition(vgui_key('1'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_graph_x_ = vgui_event_condition(vgui_key('1'), vgui_SHIFT, true);

  // translate the whole shape along y-axis
  this->gesture_increase_graph_y_ = vgui_event_condition(vgui_key('2'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_graph_y_ = vgui_event_condition(vgui_key('2'), vgui_SHIFT, true);

  // rotate the whole shape around the selected node
  this->gesture_rotate_graph_cw_ = vgui_event_condition(vgui_key('3'),vgui_MODIFIER_NULL,true);
  this->gesture_rotate_graph_ccw_ = vgui_event_condition(vgui_key('3'), vgui_SHIFT, true);

  // change the size (scale) of the shape
  this->gesture_scale_up_ = vgui_event_condition(vgui_key('4'),vgui_MODIFIER_NULL,true);
  this->gesture_scale_down_ = vgui_event_condition(vgui_key('4'), vgui_SHIFT, true);
}


// -----------------------------------------------------------------------------
//: Destructor
dbsksp_xshock_design_tool::
~dbsksp_xshock_design_tool()
{
}

  
// -----------------------------------------------------------------------------
//: Set the tableau to work with
bool dbsksp_xshock_design_tool::
set_tableau ( const vgui_tableau_sptr& tableau )
{
  if(!tableau)
    return false;
  if( tableau->type_name() == "dbsksp_xgraph_tableau" )
  {
    if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
      return false;
    this->tableau_.vertical_cast(tableau);
    return true;
  }
  tableau_ = 0;
  return false;
}


// -----------------------------------------------------------------------------
//: Set the storage class for the active tableau
bool dbsksp_xshock_design_tool::
set_storage ( const bpro1_storage_sptr& storage_sptr)
{
  if (!storage_sptr)
    return false;
  
  //make sure its a vsol storage class
  if (storage_sptr->type() == "dbsksp_xgraph"){
    this->storage_.vertical_cast(storage_sptr);
    return true;
  }
  this->storage_ = 0;
  return false;
}


// ----------------------------------------------------------------------------
//: Return the name of this tool
vcl_string dbsksp_xshock_design_tool::
name() const 
{
  return "XShock design";
}


// ----------------------------------------------------------------------------
//: Set selected edge 
void dbsksp_xshock_design_tool::
set_selected_xedge(const dbsksp_xshock_edge_sptr& xe) 
{ 
  this->selected_xedge_ = xe; 
  this->storage()->set_active_xedge(xe);
}


// ----------------------------------------------------------------------------
//: Set selected node
void dbsksp_xshock_design_tool::
set_selected_xnode(const dbsksp_xshock_node_sptr& xv)
{ 
  this->selected_xnode_ = xv; 
  this->storage()->set_active_xnode(xv);
}



// ----------------------------------------------------------------------------
//: Allow the tool to add to the popup menu as a tableau would
void dbsksp_xshock_design_tool::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  menu.add("Split an A12 edge",
    new dbsksp_xshock_split_an_edge_command(this));

  menu.add("Delete an A12 node",
    new dbsksp_xshock_delete_an_A12_node_command(this));
  
  menu.separator();

  menu.add("Extend a terminal edge",
    new dbsksp_xshock_extend_A_infty_edge_command(this));
  
  menu.add("Insert an A_infty edge at selected node",
    new dbsksp_xshock_insert_A_infty_edge_command(this));

  menu.add("Delete A_infty edge at selected edge",
    new dbsksp_xshock_delete_A_infty_edge_command(this));

  menu.separator();

  menu.add("Compute xsamples for the shape",
    new dbsksp_xshock_compute_xsamples_command(this));

  menu.add("Clear xsamples for the shape",
    new dbsksp_xshock_clear_xsamples_command(this));

}



// ----------------------------------------------------------------------------
//: Handle events
bool dbsksp_xshock_design_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{
  // check condition for each gesture and handle accordingly

  // Display information of selected object
  if (this->gesture_display_info_(e))
  {
    return this->handle_display_info();
  }

  // translate a node
  if (this->gesture_increase_node_x_(e))
  {
    return this->handle_translate_xnode(1, 0);
  }

  if (this->gesture_decrease_node_x_(e))
  {
    return this->handle_translate_xnode(-1, 0);
  }

  if (this->gesture_increase_node_y_(e))
  {
    return this->handle_translate_xnode(0, 1);
  }

  if (this->gesture_decrease_node_y_(e))
  {
    return this->handle_translate_xnode(0, -1);
  }

  // rotate a node
  if (this->gesture_rotate_xnode_cw_(e))
  {
    return this->handle_rotate_xnode(vnl_math::pi/180);
  }

  if (this->gesture_rotate_xnode_ccw_(e))
  {
    return this->handle_rotate_xnode(-vnl_math::pi/180);
  }


  // -------- node -------------------

  // rotate one tangent of a node
  if (this->gesture_rotate_xtangent_cw_(e))
  {
    return this->handle_rotate_xtangent(-vnl_math::pi/180);
  }

  if (this->gesture_rotate_xtangent_ccw_(e))
  {
    return this->handle_rotate_xtangent(vnl_math::pi/180);
  }
  
  // change radius
  if (this->gesture_increase_radius_(e))
  {
    this->handle_change_radius(1);
  }

  if (this->gesture_decrease_radius_(e))
  {
    this->handle_change_radius(-1);
  }


  // change phi
  // rotate one tangent of a node
  if (this->gesture_increase_phi_(e))
  {
    return this->handle_change_phi(vnl_math::pi/180);
  }

  if (this->gesture_decrease_phi_(e))
  {
    return this->handle_change_phi(-vnl_math::pi/180);
  }


  // gestures affecting the whole graph

  // gestures affecting the graph as a whole
  // change the size (scale) of the shape
  if (this->gesture_scale_up_(e))
  {
    return this->handle_similarity_transform(0, 0, 0, 0.1);  
  }

  if (this->gesture_scale_down_(e))
  {
    return this->handle_similarity_transform(0, 0, 0, -0.1);
  }

  // rotate the whole shape around the selected node
  if (this->gesture_rotate_graph_cw_(e))
  {
    return this->handle_similarity_transform(0, 0, -vnl_math::pi/180, 0);
  }

  if (this->gesture_rotate_graph_ccw_(e))
  {
    return this->handle_similarity_transform(0, 0, vnl_math::pi/180, 0);
  }
  
  // translate the whole shape along x-axis
  if (this->gesture_increase_graph_x_(e))
  {
    return this->handle_similarity_transform(1, 0, 0, 0);
  }

  if (this->gesture_decrease_graph_x_(e))
  {
    return this->handle_similarity_transform(-1, 0, 0, 0);
  }
  
  // translate the whole shape along y-axis
  if (this->gesture_increase_graph_y_(e))
  {
    return this->handle_similarity_transform(0, 1, 0, 0);
  }
  
  if (this->gesture_decrease_graph_y_(e))
  {
    return this->handle_similarity_transform(0, -1, 0, 0);
  }

  return false;
}


// -----------------------------------------------------------------------------
//: Display info of the selected object (node, edge)
bool dbsksp_xshock_design_tool::
handle_display_info()
{
  unsigned int highlighted_id = this->tableau()->get_highlighted();
  if (highlighted_id)
  {
    vgui_soview* so = vgui_soview::id_to_object(highlighted_id);
    if (so->type_name() == "dbsksp_soview_xshock_chord")
    {
      dbsksp_soview_xshock_chord* chord = 
        static_cast<dbsksp_soview_xshock_chord* >(so);
      dbsksp_xshock_edge_sptr xedge = chord->xedge();
      xedge->print(vcl_cout);
      this->set_selected_xedge(xedge);
      
      // reset the selected xnode to neighbor the newly selected xedge
      if (!xedge->is_vertex(this->selected_xnode()))
      {
        if (xedge->source()->degree()==1)
        {
          this->set_selected_xnode(xedge->target());
        }
        else
        {
          this->set_selected_xnode(xedge->source());
        }
      }

      


     this->update_xgraph_display();
      return true;
    }

    if (so->type_name() == "dbsksp_soview_xshock_node" )
    {
      dbsksp_soview_xshock_node* so_xnode = 
        static_cast<dbsksp_soview_xshock_node* >(so);
      dbsksp_xshock_node_sptr xnode = so_xnode->xnode();

      xnode->print(vcl_cout);
      this->set_selected_xnode(xnode);

      // set the selected_edge to neighbor the newly selected node
      if (!this->selected_xedge() || !this->selected_xedge()->is_vertex(xnode))
      {
        this->set_selected_xedge(*xnode->edges_begin());
      }

     this->update_xgraph_display();
      return true;
    }
  }
  return true;
}



// -----------------------------------------------------------------------------
//: Handle translating an extrinsic node
bool dbsksp_xshock_design_tool::
handle_translate_xnode(float dx, float dy)
{
  if (!this->selected_xnode())
  {
    vcl_cerr << "ERROR: A node must be selected before translate_xnode operation\n";
    return true;
  }

  dbsksp_xshock_node_sptr xv = this->selected_xnode();

  // Check terminal node: its coordinate is dependent on its adjacent node
  if (xv->degree() == 1)
  {
    vcl_cerr << "ERROR: can't translate a degree-one node.\n";
    return true;
  }

  // Translate the point
  xv->set_pt(xv->pt() + vgl_vector_2d<double >(dx, dy));

  // if adjacent to an terminal node, move it too
  for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); 
    eit != xv->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    if (xe->is_terminal_edge())
    {
      dbsksp_xshock_node_sptr xv2 = xe->opposite(xv);
      xv2->set_pt(xv2->pt() + vgl_vector_2d<double >(dx, dy));
    }
  }


 this->update_xgraph_display();
  return false;
}


// -----------------------------------------------------------------------------
//: Handle rotating shock tangents at a node
bool dbsksp_xshock_design_tool::
handle_rotate_xnode(float dtheta)
{
  if (!this->selected_xnode())
  {
    vcl_cerr << "ERROR: A node must be selected before translate_xnode operation\n";
    return true;
  }


  dbsksp_xshock_node_sptr xv = this->selected_xnode();
  // Check terminal node: its orientation is dependent on its adjacent node
  if (xv->degree() == 1)
  {
    vcl_cerr << "ERROR: can't rotate a degree-one node.\n";
    return true;
  }


  // iterate thru the edges and modify their tangents
  for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin();
    eit != xv->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(xe);
    xdesc->psi_ += dtheta;
    
    // check if the opposite node is a terminal node
    dbsksp_xshock_node_sptr xv2 = xe->opposite(xv);
    if (xv2->degree() == 1)
    {
      this->storage()->xgraph()->update_degree_1_node(xv2);
    }
  }
  this->update_xgraph_display();
  return false;
}


// -----------------------------------------------------------------------------
//: Handle rotating individual shock tangent
bool dbsksp_xshock_design_tool::
handle_rotate_xtangent(float dtheta)
{
  if (!this->selected_xnode())
  {
    vcl_cerr << "ERROR: A node must be selected before rotate_xtangent operation\n";
    return true;
  }

  if (!this->selected_xedge())
  {
    vcl_cerr << "ERROR: An edge must be selected before rotate_xtangent operation\n";
    return true;
  }

  dbsksp_xshock_node_sptr xv = this->selected_xnode();
  dbsksp_xshock_edge_sptr xe = this->selected_xedge();

  if (!xe->is_vertex(xv))
  {
    vcl_cerr << "ERROR: selected_xnode must be a vertex of selected_xedge for rotate_xtangent operation\n";
    return true;
  }

  // Check terminal node: its orientation is dependent on its adjacent node
  if (xv->degree() == 1)
  {
    vcl_cerr << "ERROR: can't rotate a degree-one node.\n";
    return true;
  }

  // rotate one tangent
  xv->rotate_tangent(xe, dtheta);
  
  // check if any adjacent node is a terminal node
  for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); 
    eit != xv->edges_end(); ++eit)
  {
    dbsksp_xshock_node_sptr xv2 = (*eit)->opposite(xv);  
    if (xv2->degree() == 1)
    {
      this->storage()->xgraph()->update_degree_1_node(xv2);
    }
  }
  this->update_xgraph_display();
  return false;
}




// ----------------------------------------------------------------------------
//: handle changing angle phi a node with any degree
bool dbsksp_xshock_design_tool::
handle_change_phi(double dphi)
{
  if (!this->selected_xnode())
  {
    vcl_cerr << "ERROR: A node must be selected before change_phi operation\n";
    return true;
  }

  if (!this->selected_xedge())
  {
    vcl_cerr << "ERROR: An edge must be selected before change_phi operation\n";
    return true;
  }

  dbsksp_xshock_node_sptr xv = this->selected_xnode();
  dbsksp_xshock_edge_sptr xe = this->selected_xedge();

  if (!xe->is_vertex(xv))
  {
    vcl_cerr << "ERROR: selected_xnode must be a vertex of selected_xedge for change_phi operation\n";
    return true;
  }

  // Check terminal node: its orientation is dependent on its adjacent node
  if (xv->degree() == 1)
  {
    vcl_cerr << "ERROR: can't change phi angle of a degree-one node.\n";
    return true;
  }

  dbsksp_xshock_graph_sptr xg = this->storage()->xgraph();

  // handle each type of node separately
  if (xv->degree() == 2)
  {
    dbsksp_xshock_node_descriptor* xdesc0 = xv->descriptor(xe);
    dbsksp_xshock_node_descriptor* xdesc1 = xv->descriptor(xg->cyclic_adj_succ(xe, xv));
    xdesc0->phi_ += dphi;
    xdesc1->phi_ -= dphi;
  }
  else
  {
    dbsksp_xshock_node_descriptor* xdesc0 = xv->descriptor(xe);
    xdesc0->phi_ += dphi;
    xdesc0->psi_ += dphi;

    // compensate by changing phi and psi of the adjancent node
    dbsksp_xshock_edge_sptr xe1 = xg->cyclic_adj_succ(xe, xv);
    dbsksp_xshock_node_descriptor* xdesc1 = xv->descriptor(xe1);
    xdesc1->phi_ -= dphi;
    xdesc1->psi_ += dphi;
  }

  this->storage()->xgraph()->update_degree_1_nodes_around(xv);
  this->update_xgraph_display();
  return false;
}



// ----------------------------------------------------------------------------
//: Handle changing the radius at a node
bool dbsksp_xshock_design_tool::
handle_change_radius(double dr)
{
  if (!this->selected_xnode())
  {
    vcl_cerr << "ERROR: A node must be selected before translate_xnode operation\n";
    return true;
  }

  dbsksp_xshock_node_sptr xv = this->selected_xnode();
  // Check terminal node: its orientation is dependent on its adjacent node
  if (xv->degree() == 1)
  {
    vcl_cerr << "ERROR: can't change radius of a degree-one node.\n";
    return true;
  }

  xv->set_radius(xv->radius() + dr);

  // iterate thru the edges and modify their tangents
  for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin();
    eit != xv->edges_end(); ++eit)
  {
    // check if the opposite node is a terminal node
    dbsksp_xshock_node_sptr xv2 = (*eit)->opposite(xv);
    if (xv2->degree() == 1)
    {
      this->storage()->xgraph()->update_degree_1_node(xv2);
    }
  }
  this->update_xgraph_display();
  return false;
}


// ----------------------------------------------------------------------------
//: apply similarity transform to the graph
bool dbsksp_xshock_design_tool::
handle_similarity_transform(double dx, double dy, double d_angle, double d_log2scale)
{
  if (!this->selected_xnode())
  {
    vcl_cerr << "ERROR: A node must be selected before translate_xnode operation\n";
    return true;
  }
  dbsksp_xshock_node_sptr xv = this->selected_xnode();
  dbsksp_xshock_graph_sptr xg = this->storage()->xgraph();

  vgl_point_2d<double > ref_pt = xv->pt();
  double scale = vcl_exp(vnl_math::ln2 * d_log2scale);
  
  // similarity transform of the graph
  xg->similarity_transform(ref_pt, dx, dy, d_angle, scale);

  this->update_xgraph_display();
  return false;
}




//------------------------------------------------------------------------------
//: Update display of the xgraph
void dbsksp_xshock_design_tool::
update_xgraph_display()
{
  bvis1_manager::instance()->update_tableau(this->tableau(), this->storage());
  bvis1_manager::instance()->post_redraw();
  return;
}





// ============================================================================
// dbsksp_xshock_split_an_edge_command
// ============================================================================
void dbsksp_xshock_split_an_edge_command::
execute()
{
  // preliminary checks
  dbsksp_xshock_edge_sptr xe = this->tool()->selected_xedge();
  if (!xe) 
  {
    vcl_cerr << "ERROR: An edge must be selected before split_an_edge operation. " << vcl_endl;
    return;
  }

  if (xe->is_terminal_edge())
  {
    vcl_cerr << "ERROR: Cannot split an A-infinity edge;\n";
    return;
  }

  // Start Debug ////////////////////////////////////////////////////////
  // Compute sample
  dbsksp_xshock_node_descriptor start = *(xe->source()->descriptor(xe));
  dbsksp_xshock_node_descriptor end   = xe->target()->descriptor(xe)->opposite_xnode();
  dbsksp_xshock_fragment xfrag(start, end);
  dbsksp_xshock_node_descriptor xsample;
  dbsksp_compute_xfrag_sample_with_min_kdiff(xfrag, 0.5, xsample );
  this->tool()->storage()->xgraph()->insert_xshock_node(xe, xsample);


  // End Debug ////////////////////////////////////////////////////////

  //this->tool()->storage()->xgraph()->insert_xshock_node(xe, 0.5);



  this->tool()->set_selected_xedge(0);
  this->tool()->set_selected_xnode(0);

  this->tool()->update_xgraph_display();
  return;
}








//=============================================================================
// dbsksp_xshock_delete_an_A12_node_command
//=============================================================================

//:
void dbsksp_xshock_delete_an_A12_node_command::execute()
{
  // preliminary checks
  dbsksp_xshock_node_sptr xv = this->tool()->selected_xnode();
  if (!xv || xv->degree() != 2) 
  {
    vcl_cerr << "ERROR: A degree-2 node must be selected before a delete_A12_node operation. " << vcl_endl;
    return;
  }


  dbsksp_xshock_edge_sptr new_xe = this->tool()->storage()->xgraph()->remove_A12_node(xv);
  if (!new_xe)
  {
    vcl_cerr << "ERROR: Deleting degree-2 node (id = " << xv->id() << " ) failed.\n";
  }
  this->tool()->set_selected_xedge(0);
  this->tool()->set_selected_xnode(0);

 this->tool()->update_xgraph_display();
  return;
}





// ============================================================================
// dbsksp_xshock_extend_A_infty_edge_command
// ============================================================================
void dbsksp_xshock_extend_A_infty_edge_command::
execute()
{
  dbsksp_xshock_edge_sptr xe = this->tool()->selected_xedge();

  // preliminary check
  if (!xe) 
  {
    vcl_cerr << "ERROR: No active edge found." << vcl_endl;
    return;
  }

  if (!xe->is_terminal_edge())
  {
    vcl_cerr << "ERROR: Selected edge is not a terminal edge." << vcl_endl;
    return;
  }

  // insert a new shock node inside the terminal edge
  dbsksp_xshock_graph_sptr xg = this->tool()->storage()->xgraph();
  xg->insert_xshock_node_at_terminal_edge(xe, 1.0);
 this->tool()->update_xgraph_display();
  return;
}




// ============================================================================
// dbsksp_xshock_insert_A_infty_edge_command
// ============================================================================

void dbsksp_xshock_insert_A_infty_edge_command::
execute()
{
  // preliminary checks
  if (!this->tool()->selected_xnode())
  {
    vcl_cerr << "ERROR: A node must be selected before insert_A_infty operation\n";
    return;
  }

  if (!this->tool()->selected_xedge())
  {
    vcl_cerr << "ERROR: An edge must be selected before insert_A_infty operation\n";
    return;
  }

  dbsksp_xshock_node_sptr xv = this->tool()->selected_xnode();
  dbsksp_xshock_edge_sptr xe = this->tool()->selected_xedge();

  if (!xe->is_vertex(xv))
  {
    vcl_cerr << "ERROR: selected_xnode must be a vertex of selected_xedge for insert_A_infty operation\n";
    return;
  }

  // Check terminal node: its orientation is dependent on its adjacent node
  if (xv->degree() == 1)
  {
    vcl_cerr << "ERROR: can't insert an A_infty branch to a degree-one node.\n";
    return;
  }
  dbsksp_xshock_graph_sptr xg = this->tool()->storage()->xgraph();
  if (xg->insert_A_infty_branch(xv, xe))
  {
    vcl_cout << "A new A_infty branch has been created.\n";
  }
  else
  {
    vcl_cout << "Something goes wrong while creating new A_infty branch.\n"
      << "No branch was created\n";
  }
  this->tool()->storage()->xgraph()->update_degree_1_nodes_around(xv);
 this->tool()->update_xgraph_display();
  return;
}






// ============================================================================
// dbsksp_xshock_delete_A_infty_edge_command
// ============================================================================

void dbsksp_xshock_delete_A_infty_edge_command::
execute()
{
  if (!this->tool()->selected_xedge() || !this->tool()->selected_xedge()->is_terminal_edge())
  {
    vcl_cerr << "ERROR: A terminal edge must be selected before delete_A_infty operation\n";
    return;
  }

  // Collect all the necessary info
  dbsksp_xshock_graph_sptr xg = this->tool()->storage()->xgraph();
  dbsksp_xshock_edge_sptr terminal_xe = this->tool()->selected_xedge();
  dbsksp_xshock_node_sptr xv = (terminal_xe->source()->degree()==1) ?
    terminal_xe->target() : terminal_xe->source();
  vcl_cout << "\nRemoving terminal edge with id = " << terminal_xe->id() << "...";
  dbsksp_xshock_edge_sptr xe = 0;
  if (xv->degree() == 3)
  {
    dbsksp_xshock_node_sptr degree_3_xv = xv;
    xe = xg->remove_A_infty_branch(degree_3_xv, terminal_xe, 0.5);
  }
  else if (xv->degree() ==2)
  {
    xe = xg->remove_leaf_edge_at_A12_node(terminal_xe);
  }

  if (xe)
  {
    vcl_cout << "Succeeded.\n";

    // set active node and edge
    this->tool()->set_selected_xedge(xe);
    if (xv->degree() > 1)
    {
      this->tool()->set_selected_xnode(xv);
    }
    else
    {
      this->tool()->set_selected_xnode(xe->opposite(xv));
    }
   this->tool()->update_xgraph_display();
  }
  else
  {
    vcl_cout << "Failed.\n";
  }
  return;
}





//=============================================================================
// dbsksp_xshock_compute_xsamples_command
//=============================================================================

//:
void dbsksp_xshock_compute_xsamples_command::
execute()
{
  double sample_ds = 3;

  vcl_vector<dbsksp_xshock_node_descriptor >* xsamples = this->tool()->storage()->xsamples();
  xsamples->clear(); 
  
  // Compute samples for each branch
  dbsksp_xshock_directed_tree_sptr tree = new dbsksp_xshock_directed_tree();
  tree->acquire_tree_topology(this->tool()->storage()->xgraph());

  int num_darts = tree->size();
  for (int i =0; i < num_darts; ++i)
  //for (int i =0; i < 1; ++i)
  {
    vcl_vector<int > dart_list(1, i);
    dbsksp_xshock_node_sptr start_node;
    vcl_vector<dbsksp_xshock_edge_sptr > edge_list;
    tree->get_edge_list(dart_list, start_node, edge_list);

    // compute samples from the given path
    vcl_vector<dbsksp_xshock_node_descriptor > dart_xsamples;

    dbsksp_shock_path_sptr shock_path = 
      dbsksp_compute_uniform_shock_path(start_node, edge_list, sample_ds);

    
    //dbsksp_xgraph_algos::compute_xsamples(start_node, edge_list, sample_ds, dart_xsamples);
    shock_path->get_all_xdesc(dart_xsamples);
    xsamples->insert(xsamples->end(), dart_xsamples.begin(), dart_xsamples.end());
  }
 this->tool()->update_xgraph_display();
  return;
}



//=============================================================================
// dbsksp_xshock_clear_xsamples_command
//=============================================================================

//:
void dbsksp_xshock_clear_xsamples_command::
execute()
{
  this->tool()->storage()->xsamples()->clear();
  this->tool()->update_xgraph_display(); 
  return;
}



