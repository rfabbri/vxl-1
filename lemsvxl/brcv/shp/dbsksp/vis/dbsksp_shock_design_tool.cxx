// This is shp/dbsksp/dbsksp_shock_design_tool.cxx
//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Sep 28, 2006

#include "dbsksp_shock_design_tool.h"

#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>

#include <vnl/vnl_math.h>

#include <dbsksp/vis/dbsksp_soview_shock.h>
#include <dbsksp/dbsksp_shock_edge.h>
#include <dbsksp/dbsksp_shock_fragment.h>
#include <dbsksp/dbsksp_shock_node.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/algo/dbsksp_interp_two_xnodes.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <vsol/vsol_point_2d.h>

//: Constructor
dbsksp_shock_design_tool::
dbsksp_shock_design_tool() : 
  tableau_(0), 
  storage_(0), 
  operating_mode_("normal")
{
  this->gesture_display_info_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);

  this->gesture_increase_m_ = vgui_event_condition(vgui_key('m'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_m_ = vgui_event_condition(vgui_key('m'), vgui_SHIFT, true);
  
  this->gesture_increase_length_ = vgui_event_condition(vgui_key('l'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_length_ = vgui_event_condition(vgui_key('l'), vgui_SHIFT, true);
  
  this->gesture_increase_phi_ = vgui_event_condition(vgui_key('p'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_phi_ = vgui_event_condition(vgui_key('p'), vgui_SHIFT, true);
  

  // modify active node
  this->gesture_increase_active_node_x_ = vgui_event_condition(vgui_key('['), vgui_MODIFIER_NULL,true);
  this->gesture_decrease_active_node_x_ = vgui_event_condition(vgui_key('['), vgui_SHIFT,true);

  this->gesture_increase_active_node_y_ = vgui_event_condition(vgui_key(']'), vgui_MODIFIER_NULL,true);
  this->gesture_decrease_active_node_y_ = vgui_event_condition(vgui_key(']'), vgui_SHIFT,true);

  this->gesture_increase_active_node_psi_ = vgui_event_condition(vgui_key(';'), vgui_MODIFIER_NULL,true);
  this->gesture_decrease_active_node_psi_ = vgui_event_condition(vgui_key(';'), vgui_SHIFT,true);

  this->gesture_increase_active_node_phi_ = vgui_event_condition(vgui_key('\\'), vgui_MODIFIER_NULL,true);
  this->gesture_decrease_active_node_phi_ = vgui_event_condition(vgui_key('\\'), vgui_SHIFT,true);


  this->gesture_increase_active_node_radius_ = vgui_event_condition(vgui_key('\''), vgui_MODIFIER_NULL,true);
  this->gesture_decrease_active_node_radius_ = vgui_event_condition(vgui_key('\''), vgui_SHIFT,true);

  // global change

  this->gesture_increase_radius_ = vgui_event_condition(vgui_key('r'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_radius_ = vgui_event_condition(vgui_key('r'), vgui_SHIFT, true);

  this->gesture_increase_ref_x_ = vgui_event_condition(vgui_key('i'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_ref_x_ = vgui_event_condition(vgui_key('i'), vgui_SHIFT, true);

  this->gesture_increase_ref_y_ = vgui_event_condition(vgui_key('j'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_ref_y_ = vgui_event_condition(vgui_key('j'), vgui_SHIFT, true);

  this->gesture_increase_ref_theta_ = vgui_event_condition(vgui_key('t'),vgui_MODIFIER_NULL,true);
  this->gesture_decrease_ref_theta_ = vgui_event_condition(vgui_key('t'), vgui_SHIFT, true);

  this->gesture_increase_scale_ = vgui_event_condition(vgui_key('e'),vgui_MODIFIER_NULL,true);;
  this->gesture_decrease_scale_ = vgui_event_condition(vgui_key('e'), vgui_SHIFT, true);

  this->gesture_set_ref_ = vgui_event_condition(vgui_key('s'),vgui_MODIFIER_NULL,true);

  this->gesture_select_e0_ = vgui_event_condition(vgui_key('1'),vgui_MODIFIER_NULL,true);
  this->gesture_select_e1_ = vgui_event_condition(vgui_key('2'),vgui_MODIFIER_NULL,true);

  this->gesture_select_target_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
}


//: Destructor
dbsksp_shock_design_tool::
~dbsksp_shock_design_tool()
{
}


//: Set the tableau to work with
bool dbsksp_shock_design_tool::
set_tableau ( const vgui_tableau_sptr& tableau ){
  if(!tableau)
    return false;
  if( tableau->type_name() == "dbsksp_shock_tableau" ){
    if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
      return false;
    this->tableau_.vertical_cast(tableau);
    return true;
  }
  tableau_ = 0;
  return false;
}


//: Set the storage class for the active tableau
bool dbsksp_shock_design_tool::
set_storage ( const bpro1_storage_sptr& storage_sptr){
  if (!storage_sptr)
    return false;
  
  //make sure its a vsol storage class
  if (storage_sptr->type() == "dbsksp_shock"){
    this->storage_.vertical_cast(storage_sptr);
    return true;
  }
  this->storage_ = 0;
  return false;
}


// ----------------------------------------------------------------------------
//: Return the name of this tool
vcl_string dbsksp_shock_design_tool::
name() const {
  return "Shock design";
}


// ----------------------------------------------------------------------------
//: Return the storage of the tool
dbsksp_shock_storage_sptr dbsksp_shock_design_tool::
storage() const
{
  return this->storage_;
}



// -----------------------------------------------------------------------------
//: Get the selected edge
dbsksp_shock_edge_sptr dbsksp_shock_design_tool::
selected_edge() const
{
  return this->storage()->active_edge();
}


//------------------------------------------------------------------------------
//: set the selected edge
void dbsksp_shock_design_tool::
set_selected_edge(const dbsksp_shock_edge_sptr& e)
{
  this->storage()->set_active_edge(e);
}


//------------------------------------------------------------------------------
//: Get and set the selected node
dbsksp_shock_node_sptr dbsksp_shock_design_tool::
selected_node() const
{
  return this->storage()->active_node();
}

//------------------------------------------------------------------------------
//: Get and set the selected node
void dbsksp_shock_design_tool::
set_selected_node(const dbsksp_shock_node_sptr& node)
{
  this->storage()->set_active_node(node);
  return;
}



// ----------------------------------------------------------------------------
//: Return the tableau of the tool
dbsksp_shock_tableau_sptr dbsksp_shock_design_tool::
tableau() const
{
  return this->tableau_;
}



//: Allow the tool to add to the popup menu as a tableau would
void dbsksp_shock_design_tool::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  menu.add("Form shapelet from active edge", 
    new dbsksp_tool_form_shapelet_command(this));
  
  menu.add("Clear shapelet list", 
    new dbsksp_tool_clear_shapelet_list_command(this));

  menu.separator();

  // Insert node
  menu.add("Insert node at active edge's midpoint", 
    dbsksp_shock_design_insert_node_at_active_edge_midpoint_wrapper, (void*)(this));

  // Extend branch
  menu.add("Add a new edge to the end of current shock branch",
    dbsksp_shock_design_extend_branch_wrapper, (void*)(this));
   
  // Insert branch
  menu.add("Insert A_infty branch at active node", 
    new dbsksp_tool_insert_A_infty_branch_command(this));

  menu.separator();
  menu.add("Delete A_infty branch at active node", 
    new dbsksp_tool_delete_A_infty_branch_command(this));

  menu.add("Squeeze a short edge",
    new dbsksp_tool_squeeze_shock_edge_command(this));

  menu.add("Remove a leaf A_1^2 edge",
    new dbsksp_tool_remove_leaf_edge_command(this));

  menu.add("Remove an internal edge",
    new dbsksp_tool_remove_internal_edge_command(this));


  menu.add("Split a node",
    new dbsksp_tool_split_node_command(this));

  menu.separator();

  menu.add("Select target point",
    new dbsksp_tool_select_point_mode_command(this));

}

// ----------------------------------------------------------------------------
//: Handle events
bool dbsksp_shock_design_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{

  //: Handle all the special mode first
  if (this->operating_mode() == "select_target")
  {
    vgui::out << "Click to select target\n";
    if ( this->gesture_select_target_(e) )
    {
      float ix, iy;
      vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
      return this->handle_select_target(ix, iy);
    }

    return true;
  }




  if (this->gesture_display_info_(e))
  {
    return this->handle_display_info();
  }

  // -------- edge -------------------
  // change length
  if (this->gesture_increase_length_(e))
  {
    return this->handle_change_length(1);
  }

  if (this->gesture_decrease_length_(e))
  {
    return this->handle_change_length(-1);
  }

  // change parameter m
  if (this->gesture_increase_m_(e))
  {
    this->handle_change_m(0.02);
  }

  if (this->gesture_decrease_m_(e))
  {
    this->handle_change_m(-0.02);
  }

  // -------- node -------------------
  // change phi
  if (this->gesture_increase_phi_(e))
  {
    this->handle_change_phi(0.02);
  }

  if (this->gesture_decrease_phi_(e))
  {
    this->handle_change_phi(-0.02);
  }

  // -------- global -------------------
  // change radius
  if (this->gesture_increase_radius_(e))
  {
    return this->handle_change_radius(1);
  }

  if (this->gesture_decrease_radius_(e))
  {
    return this->handle_change_radius(-1);
  }

  if (this->gesture_increase_ref_x_(e))
  {
    return this->handle_change_ref_origin(1, 0);
  }

  if (this->gesture_decrease_ref_x_(e))
  {
    return this->handle_change_ref_origin(-1, 0);
  }

  if (this->gesture_increase_ref_y_(e))
  {
    return this->handle_change_ref_origin(0, 1);
  }

  if (this->gesture_decrease_ref_y_(e))
  {
    return this->handle_change_ref_origin(0, -1);
  }

  if (this->gesture_increase_ref_theta_(e))
  {
    return this->handle_change_ref_dir(0.02);
  }

  if (this->gesture_decrease_ref_theta_(e))
  {
    return this->handle_change_ref_dir(-0.02);
  }

  if (this->gesture_increase_scale_(e))
  {
    return this->handle_change_scale(0.1);
  }

  if (this->gesture_decrease_scale_(e))
  {
    return this->handle_change_scale(-0.1);
  }

  if (this->gesture_set_ref_(e))
  {
    return this->handle_set_ref();
  }
  

  // part of graph

  // change active node's x-coordinate
  if (this->gesture_increase_active_node_x_(e))
  {
    return this->handle_change_active_node_geom(1, 0, 0, 0, 0);
  }

  // change active node's x-coordinate
  if (this->gesture_decrease_active_node_x_(e))
  {
    return this->handle_change_active_node_geom(-1, 0, 0, 0, 0);
  }


  // change active node's y-coordinate
  if (this->gesture_increase_active_node_y_(e))
  {
    return this->handle_change_active_node_geom(0, 1, 0, 0, 0);
  }

  if (this->gesture_decrease_active_node_y_(e))
  {
    return this->handle_change_active_node_geom(0, -1, 0, 0, 0);
  }


  // change active node's orientation
  if (this->gesture_increase_active_node_psi_(e))
  {
    return this->handle_change_active_node_geom(0, 0, 0.1, 0, 0);
  }

  if (this->gesture_decrease_active_node_psi_(e))
  {
    return this->handle_change_active_node_geom(0, 0, -0.1, 0, 0);
  }

  // change active node's phi
  if (this->gesture_increase_active_node_phi_(e))
  {
    return this->handle_change_active_node_geom(0, 0, 0, 0, 0.1);
  }

  if (this->gesture_decrease_active_node_phi_(e))
  {
    return this->handle_change_active_node_geom(0, 0, 0, 0, -0.1);
  }



  // change active node's radius
  if (this->gesture_increase_active_node_radius_(e))
  {
    return this->handle_change_active_node_geom(0, 0, 0, 1, 0);
  }

  if (this->gesture_decrease_active_node_radius_(e))
  {
    return this->handle_change_active_node_geom(0, 0, 0, -1, 0);
  }



  // misc gestures

  if (this->gesture_select_e0_(e))
  {
    vcl_cout << "gesture_select_e0_.\n";
    return this->handle_select_e0();
  }

  
  if (this->gesture_select_e1_(e))
  {
    vcl_cout << "gesture_select_e1_.\n";
    return this->handle_select_e1();
  }

  
  return false;
}


bool dbsksp_shock_design_tool::
handle_display_info()
{
  unsigned int highlighted_id = this->tableau()->get_highlighted();
  if (highlighted_id)
  {
    vgui_soview* so = vgui_soview::id_to_object(highlighted_id);
    if (so->type_name() == "dbsksp_soview_shock_edge_chord" ||
      so->type_name() == "dbsksp_soview_shock_edge_curve" )
    {
      dbsksp_soview_shock_edge_chord* chord = 
        static_cast<dbsksp_soview_shock_edge_chord* >(so);
      dbsksp_shock_edge_sptr edge = chord->edge();
      edge->print(vcl_cout);
      this->set_selected_edge(edge);
      if (!edge->is_vertex(this->selected_node()))
      {
        if (edge->source()->degree() == 1)
        {
          this->set_selected_node(edge->target());
        }
        else
        {
          this->set_selected_node(edge->source());
        }
      }
      bvis1_manager::instance()->display_current_frame();
      return true;
    }

    if (so->type_name() == "dbsksp_soview_shock_node" )
    {
      dbsksp_soview_shock_node* so_node = 
        static_cast<dbsksp_soview_shock_node* >(so);
      dbsksp_shock_node_sptr node = so_node->node();

      node->print(vcl_cout);
      this->set_selected_node(node);
      if (!this->selected_edge() || !this->selected_edge()->is_vertex(node))
      {
        this->set_selected_edge(*node->edges_begin());
      }
      bvis1_manager::instance()->display_current_frame();
      
      return true;
    }
  }
  return true;
}

// ----------------------------------------------------------------------------
bool dbsksp_shock_design_tool::
handle_change_m(double dm)
{
  if (!this->selected_edge())
  {
    vcl_cout << "ERROR: An edge must be selected before change_m operation\n";
    return true;
  }

  if (this->selected_edge()->source()->degree()==1 || 
    this->selected_edge()->target()->degree()==1 )
  {
    vcl_cerr << "ERROR: A-infinity edge has a constant m = 0;\n";
    return true;
  }

  double new_m = this->selected_edge()->param_m() + dm;
  dbsksp_shock_node_sptr source = this->selected_edge()->source();
  dbsksp_shock_node_sptr target = this->selected_edge()->target();
  double phi_source = source->descriptor(this->selected_edge())->phi;
  double phi_target = target->descriptor(this->selected_edge())->phi;

  if (vcl_abs(new_m*vcl_sin(phi_source)) > 1 || 
    vcl_abs(new_m*vcl_sin(phi_target)) > 1)
  {
    vcl_cerr << "ERROR: m would be out of the valid range. Nothing was changed.\n";
    return true;
  }
  this->selected_edge()->set_param_m(new_m);
  this->tableau()->shock_graph()->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return false;
}


// ----------------------------------------------------------------------------
bool dbsksp_shock_design_tool::
handle_change_length(double dl)
{
  if (!this->selected_edge())
  {
    vcl_cerr << "ERROR: An edge must be selected before change_length operation\n";
    return true;
  }

  //if (this->selected_edge()->is_degenerate())
  if (this->selected_edge()->source()->degree()==1 || 
    this->selected_edge()->target()->degree()==1 )
  {
    vcl_cerr << "ERROR: A-infinity edge has length=radius. It cannot be mannual changed;\n";
    return true;
  }

  double old_len = this->selected_edge()->chord_length();
  if (old_len + dl < 0)
  {
    vcl_cerr << "ERROR: length would be < 0. Nothing was changed.\n";
    return true;
  }
  this->selected_edge()->set_chord_length(old_len + dl);
  this->tableau()->shock_graph()->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return false;
}

// ----------------------------------------------------------------------------
//: handle changing angle phi a node with any degree
bool dbsksp_shock_design_tool::
handle_change_phi(double dphi)
{
  if (!this->selected_node())
  {
    vcl_cerr << "ERROR: A node must be selected before change_phi operation\n";
    return true;
  }

  if (!this->storage()->active_edge())
  {
    vcl_cerr << "ERROR: An active edge must be selected before change_phi operation\n";
    return true;
  }

  if ((this->selected_node() != this->storage()->active_edge()->source()) &&
    (this->selected_node() != this->storage()->active_edge()->target()))
  {
    vcl_cerr << "ERROR: Active edge must be incident to this node.\n";
    return true;
  }

  dbsksp_shock_edge_sptr active_edge = this->storage()->active_edge();
  dbsksp_shock_edge_sptr succ_edge = 
    this->storage()->shock_graph()->cyclic_adj_succ(active_edge, this->selected_node());
  
  // set the new values
  this->selected_node()->descriptor(active_edge)->phi += dphi;
  this->selected_node()->descriptor(succ_edge)->phi -= dphi;
  
  this->storage()->shock_graph()->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return false;

}


// ----------------------------------------------------------------------------
//: Handle changing the global radius parameter
bool dbsksp_shock_design_tool::
handle_change_radius(double dr)
{
  // check validity of the change
  dbsksp_shock_graph_sptr shock_graph = this->tableau()->shock_graph();

  // set the new values
  shock_graph->set_ref_node_radius(shock_graph->ref_node_radius() + dr);
  
  shock_graph->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return false;
}


// ----------------------------------------------------------------------------
bool dbsksp_shock_design_tool::
handle_change_ref_origin(double dx, double dy)
{
  dbsksp_shock_graph_sptr shock_graph(this->tableau()->shock_graph());
  vgl_point_2d<double > pt = shock_graph->ref_origin();
  shock_graph->set_ref_origin(vgl_point_2d<double >(pt.x()+dx, pt.y() + dy));
  
  shock_graph->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return false;
}




// ----------------------------------------------------------------------------
bool dbsksp_shock_design_tool::
handle_change_ref_dir(double rot_angle)
{
  dbsksp_shock_graph_sptr shock_graph(this->tableau()->shock_graph());
  vgl_vector_2d<double > v = shock_graph->ref_direction();
  shock_graph->set_ref_direction(rotated(v, rot_angle));

  shock_graph->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return false;
}



bool dbsksp_shock_design_tool::
handle_change_scale(double d_log2scale)
{
  double scale = vcl_exp(vnl_math::ln2 * d_log2scale);
  this->tableau()->shock_graph()->scale_up(scale);

  bvis1_manager::instance()->display_current_frame();
  return false;
}


// ----------------------------------------------------------------------------
//: Handle setting the reference node and shock
bool dbsksp_shock_design_tool::
handle_set_ref()
{
  if (!this->selected_node())
  {
    vcl_cerr << "ERROR: A node must be selected before change reference nodes or direction\n";
    return true;
  }

  dbsksp_shock_graph_sptr shock_graph(this->tableau()->shock_graph());


  // When a new node is selected:
  // 1st time: reset ref_node
  // 2nd and more time: reset ref_edge to the next adjacent successor edge
  if (shock_graph->ref_node() != this->selected_node())
  {
    shock_graph->set_ref_node(this->selected_node());
    shock_graph->set_ref_edge(*this->selected_node()->edges_begin());
  }
  // 
  else
  {
    shock_graph->set_ref_edge(
      shock_graph->cyclic_adj_succ(shock_graph->ref_edge(), shock_graph->ref_node()));
  }

  // reset the extrinsic info to match with new ref_node and ref_edge
  shock_graph->set_ref_origin(shock_graph->ref_node()->pt());
  shock_graph->set_ref_node_radius(shock_graph->ref_node()->radius());
  shock_graph->set_ref_direction(shock_graph->ref_edge()->chord_dir(shock_graph->ref_node()));

  // recompute the dependent params
  shock_graph->compute_all_dependent_params();
  
  bvis1_manager::instance()->display_current_frame();
  return false;
}







//: change orientation of active node/edge
bool dbsksp_shock_design_tool::
handle_change_active_node_geom(double dx, double dy, double dpsi, double dr, double dphi)
{
  if (!this->selected_node() || !this->selected_edge())
  {
    vcl_cout << "\nERROR: a node and an edge must be selected before applying change_active_node_orient.\n";
    return true;
  }
  dbsksp_shock_node_sptr v2 = this->selected_node();
  dbsksp_shock_edge_sptr e1 = this->selected_edge();
  dbsksp_shock_graph_sptr graph = this->storage()->shock_graph();

  // make sure selected node is distance 2 away from
  dbsksp_shock_node_sptr v1 = e1->opposite(v2);
  if (v1->degree() != 2)
  {
    vcl_cout << "\nERROR: There should only 1 degree-2 node between active node and ref node.\n";
    return true;
  }
  dbsksp_shock_edge_sptr e0 = graph->cyclic_adj_succ(e1, v1);
  dbsksp_shock_node_sptr v0 = e0->opposite(v1);
  if (v0 != graph->ref_node())
  {
    vcl_cout << "\nERROR: Reference node must be distance-2 away from active node along active edge.\n";
    return true;
  }


  dbsksp_shapelet_sptr s0 = e0->fragment()->get_shapelet();
  if (v0 != e0->source())
    s0 = s0->reversed_dir();

  dbsksp_shapelet_sptr s1 = e1->fragment()->get_shapelet();
  if (v1 != e1->source())
    s1 = s1->reversed_dir();

  // retrieve the original descriptors
  dbsksp_xshock_node_descriptor xdesc0(s0->start(), s0->tangent_start(), 
    s0->phi_start(), s0->radius_start());

  dbsksp_xshock_node_descriptor xdesc2(s1->end(), s1->tangent_end(),
    s1->phi_end(), s1->radius_end());


  // record the orientation at the reference edge
  dbsksp_shapelet_sptr sref = graph->ref_edge()->fragment()->get_shapelet();
  if (graph->ref_node() != graph->ref_edge()->source())
  {
    sref = sref->reversed_dir();
  }
  
  vgl_vector_2d<double > ref_tangent = sref->tangent_start();

  
  //>>  rotate the end descriptor, then compute intrinsic paramters to make it happen
  xdesc2.psi_ += dpsi;
  xdesc2.pt_ += vgl_vector_2d<double >(dx, dy);
  xdesc2.radius_ += dr;
  xdesc2.phi_ += dphi;

  // interpolate
  dbsksp_optimal_interp_two_xnodes interpolator(xdesc0, xdesc2);
  dbsksp_twoshapelet_sptr ss = interpolator.optimize();

  // check consistency
  if (!ss)
  {
    vcl_cout << "\nERROR: xnode interpolation failed.\n";
    return true;
  }

  s0 = ss->shapelet_start();
  s1 = ss->shapelet_end();

  double dshock = (s1->end()-xdesc2.pt()).length();
  double dleft = (s1->bnd_arc_left().end()-xdesc2.bnd_pt_left()).length();
  double dright = (s1->bnd_arc_right().end()-xdesc2.bnd_pt_right()).length();

  bool is_consistent =  dshock < 1e-2 && dleft < 1e-2 && dright < 1e-2;

  if (!is_consistent)
  {
    vcl_cout << "\nERROR: xnode interpolation result is not consistent.\n";
    return true;
  }

  
  // e0 and e1
  e0->set_chord_length(s0->len());
  e0->set_param_m(s0->m_start(), v0);
  e1->set_chord_length(s1->len());
  e1->set_param_m(s1->m_start(), v1);

  // v1
  v1->descriptor(e0)->phi = vnl_math::pi - s1->phi_start();
  v1->descriptor(e1)->phi = s1->phi_start();

  // v2
  for (dbsksp_shock_node::edge_iterator eit = v2->edges_begin(); eit != v2->edges_end();
    ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (e == e1)
    {
      v2->descriptor(e)->phi += dphi;
    }
    else
    {
      v2->descriptor(e)->phi -= dphi / (v2->degree()-1);
    }
  }

  // update the extrinsic parameters
  graph->compute_all_dependent_params();

  // Rotate the graph so that the ref tangent remains unchanged
  double new_ref_alpha = graph->ref_node()->descriptor(graph->ref_edge())->alpha;
  vgl_vector_2d<double > new_ref_chord = graph->ref_edge()->chord_dir(graph->ref_node());
  vgl_vector_2d<double > new_ref_tangent = rotated(new_ref_chord, new_ref_alpha);

  // angle difference between previous reference tangent and new reference tangent
  double angle_diff = signed_angle(new_ref_tangent, ref_tangent);
  graph->set_ref_direction(rotated(graph->ref_direction(), angle_diff));

  graph->compute_all_dependent_params();

  bvis1_manager::instance()->display_current_frame();
  
  return false;
}










// ----------------------------------------------------------------------------
//: Handle inserting a node
bool dbsksp_shock_design_tool::
handle_insert_node_at_active_edge_midpoint()
{
  if (!this->selected_edge())
  {
    vcl_cerr << "ERROR: An edge must be selected before insert_node operation\n";
    return true;
  }

  //if (this->selected_edge()->is_degenerate())
  if (this->selected_edge()->source()->degree()==1 || 
    this->selected_edge()->target()->degree()==1 )
  {
    vcl_cerr << "ERROR: Cannot insert a node to an A-infinity edge;\n";
    return true;
  }

  this->tableau()->shock_graph()->insert_shock_node(this->selected_edge(), 0.5);
  this->tableau()->shock_graph()->compute_all_dependent_params();
  this->set_selected_edge(0);
  this->set_selected_node(0);
  this->tableau()->shock_graph()->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return false;
}


// ----------------------------------------------------------------------------
//: Handle extending a branch
bool dbsksp_shock_design_tool::
handle_extend_branch()
{
  if (!this->selected_edge() || !this->selected_edge()->is_terminal_edge() )
  {
    vcl_cerr << "ERROR: a terminal edge must be selected to extend the branch";
    return true;
  }

  if (!this->storage()->active_edge()->is_vertex(this->storage()->active_node()))
  {
    vcl_cerr << "ERROR: active edge must be incidennt at active node\n";
    return true;
  }

  dbsksp_shock_graph_sptr g = this->storage()->shock_graph();
  dbsksp_shock_node_sptr v0 = this->storage()->active_node();
  dbsksp_shock_edge_sptr e0 = this->storage()->active_edge();

  g->insert_shock_edge(e0, v0->radius()*0.75, v0 == e0->source(), 0);
  this->storage()->shock_graph()->compute_all_dependent_params();

  bvis1_manager::instance()->display_current_frame();
  return false;
}

// ----------------------------------------------------------------------------
//: Handle selecting e0
bool dbsksp_shock_design_tool::
handle_select_e0()
{
  this->storage()->set_edge0(this->selected_edge());
  return true;
}


// ----------------------------------------------------------------------------
//: Handle selecting e1
bool dbsksp_shock_design_tool::
handle_select_e1()
{
  this->storage()->set_edge1(this->selected_edge());
  return true;
}


// ----------------------------------------------------------------------------
//: Handle selecting point
bool dbsksp_shock_design_tool::
handle_select_target(float ix, float iy)
{
  vsol_point_2d_sptr target = new vsol_point_2d(ix, iy);
  this->storage()->set_target_point(target);
  this->set_operating_mode("normal");

  bvis1_manager::instance()->display_current_frame();
  return true;
}




// ==============================================================================
// Wrapper functions
// ==============================================================================

// -----------------------------------------------------------------------------
//: Wrapper to call the "handle_insert_node_at_active_edge_midpoint" from outside
void dbsksp_shock_design_insert_node_at_active_edge_midpoint_wrapper(const void* toolref)
{
  dbsksp_shock_design_tool* tool = (dbsksp_shock_design_tool*) toolref;
  tool->handle_insert_node_at_active_edge_midpoint();
}



// -----------------------------------------------------------------------------
//: Wrapper to call the "handle_insert_node_at_active_edge_midpoint" from outside
void dbsksp_shock_design_extend_branch_wrapper(const void* toolref)
{
  dbsksp_shock_design_tool* tool = (dbsksp_shock_design_tool*) toolref;
  tool->handle_extend_branch();
}






// ============================================================================
// dbsksp_tool_form_shapelet_command
// ============================================================================

// ----------------------------------------------------------------------------
//: Execute forming a shaplet from an active node and an active edge
void dbsksp_tool_form_shapelet_command::
execute()
{
  dbsksp_shock_edge_sptr e = this->tool()->storage()->active_edge();
  dbsksp_shock_node_sptr v = this->tool()->storage()->active_node();

  if (!e || !v) 
  {
    vcl_cerr << "ERROR: No active edge or node found. " << vcl_endl;
    return;
  }

  if (v != e->source() && v != e->target()) 
  {
    vcl_cerr << "ERROR: Active edge is not incident to active node.\n";
    return;
  }
    
  if (!e->fragment()) 
  {
    e->form_fragment();
  }

  dbsksp_shapelet_sptr shapelet = e->fragment()->get_shapelet();
  if (v==e->target())
  {
    shapelet = shapelet->reversed_dir();
  }

  dbsksp_shapelet_sptr s = shapelet;
  this->tool()->storage()->add_shapelet(s);
  this->tool()->tableau()->add_shapelet(s);
}




// ============================================================================
// dbsksp_tool_clear_shapelet_list_command
// ============================================================================

// ----------------------------------------------------------------------------
//: Execute clearing all existing shapelets
void dbsksp_tool_clear_shapelet_list_command::
execute()
{
  this->tool()->storage()->clear_shapelet_list();
  bvis1_manager::instance()->display_current_frame();
  return;
}


// ============================================================================
// dbsksp_tool_insert_A_infty_branch_command
// ============================================================================

// ----------------------------------------------------------------------------
//: Execute inserting an A_infty branch to a A_1^2 point
void dbsksp_tool_insert_A_infty_branch_command::
execute()
{
  dbsksp_shock_storage_sptr storage = this->tool()->storage();
  if (!storage->active_node())
  {
    vcl_cerr << "ERROR: A node must be selected before insert_A_infty_branch"
      << " operation\n";
    return;
  }

  if (!storage->active_edge())
  {
    vcl_cerr << "ERROR: An active edge must be selected before insert_A_infty_branch"
      << " operation\n";
    return;
  }

  if (! storage->active_edge()->is_vertex(storage->active_node()))
  {
    vcl_cerr << "ERROR: Active edge must be incident to active node.\n";
    return;
  }

  dbsksp_shock_edge_sptr active_edge = storage->active_edge();
  dbsksp_shock_node_sptr active_node = storage->active_node();
  
  if (storage->shock_graph()->insert_A_infty_branch(active_node, active_edge))
  {
    vcl_cout << "A new A_infty branch has been created.\n";
  }
  else
  {
    vcl_cout << "Something goes wrong while creating new A_infty branch.\n"
      << "No branch was created\n";
  }
  
  storage->shock_graph()->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return;
}





// ============================================================================
// dbsksp_tool_delete_A_infty_branch_command
// ============================================================================

void dbsksp_tool_delete_A_infty_branch_command::
execute()
{
  dbsksp_shock_storage_sptr storage = this->tool()->storage();
  
  // Preliminary checks
  if (!storage->active_edge() || !storage->active_node())
  {
    vcl_cerr << "ERROR: Need to specify active_node and active_edge before "
      << " delete_A_infty_branch operation\n";
    return;
  }

  if ( !storage->active_edge()->is_vertex(storage->active_node()) )
  {
    vcl_cerr << "ERROR: active_edge needs to be incident to active_node.\n";
    return;
  }

  if ( !storage->active_edge()->is_terminal_edge() )
  {
    vcl_cerr << "ERROR: active_edge needs to be a terminal edge.\n";
  }

  if ( storage->active_node()->degree() < 3 )
  {
    vcl_cerr << "ERROR: active_node must have degree at least 3.\n";
  }

  // The mission : Remove the terminal edge !

  dbsksp_shock_node_sptr degree_one_node = 
    storage->active_edge()->opposite(storage->active_node());

  assert (degree_one_node->degree() == 1);

  storage->shock_graph()->remove_A_infty_edge(degree_one_node);

  storage->shock_graph()->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return;
}







// ============================================================================
// dbsksp_tool_select_point_mode_command
// ============================================================================

void dbsksp_tool_select_point_mode_command::
execute()
{
  this->tool()->set_operating_mode("select_target");
  return;
}




// ============================================================================
// dbsksp_tool_squeeze_shock_edge_command
// ============================================================================

void dbsksp_tool_squeeze_shock_edge_command::
execute()
{
  vcl_cout << "Command: convert a short edge to a node.\n";
  dbsksp_shock_graph_sptr graph = this->tool()->storage()->shock_graph();
  dbsksp_shock_edge_sptr edge = this->tool()->storage()->active_edge();

  if (!edge)
  {
    vcl_cerr << "ERROR: an edge has to be selected first.\n";
    return;
  }

  // Avoid involving ref_edge and ref_node in this operation
  if ( edge == graph->ref_edge() || edge->source() == graph->ref_node() ||
    edge->target() == graph->ref_node() )
  {
    vcl_cerr << "ERROR: Either active_node is a ref_node or active_edge is a ref_edge.\n";
    return;
  }


  dbsksp_shock_node_sptr node = graph->squeeze_shock_edge(edge);
  if (!node)
  {
    vcl_cout << "Operation failed.\n";
  }
  else
  {
    vcl_cout << "Conversion compleleted.\n";
    node->print(vcl_cout);
  }

  graph->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return;
}









// ============================================================================
// dbsksp_tool_remove_leaf_edge_command
// ============================================================================

void dbsksp_tool_remove_leaf_edge_command::
execute()
{
  vcl_cout << "Command: Remove a leaf edge.\n";
  dbsksp_shock_graph_sptr graph = this->tool()->storage()->shock_graph();
  dbsksp_shock_edge_sptr edge = this->tool()->storage()->active_edge();

  if (!edge)
  {
    vcl_cerr << "ERROR: an edge has to be selected first.\n";
    return;
  }

  // Avoid involving ref_edge and ref_node in this operation
  if ( edge == graph->ref_edge() || edge->source() == graph->ref_node() ||
    edge->target() == graph->ref_node() )
  {
    vcl_cerr << "ERROR: Either active_node is a ref_node or active_edge is a ref_edge.\n";
    return;
  }

  dbsksp_shock_node_sptr node = graph->remove_leaf_A_1_2_edge(edge);
  if (!node)
  {
    vcl_cerr << "ERROR: active edge is not a leaf edge.\n"
      << "Operation failed.\n";
  }
  else
  {
    vcl_cout << "Leaf edge removal compleleted.\n";
    node->print(vcl_cout);
  }

  graph->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return;
}



// ============================================================================
// dbsksp_tool_remove_internal_edge_command
// ============================================================================

void dbsksp_tool_remove_internal_edge_command::
execute()
{
  vcl_cout << "Command: Remove a leaf edge.\n";
  dbsksp_shock_graph_sptr graph = this->tool()->storage()->shock_graph();
  dbsksp_shock_edge_sptr edge = this->tool()->storage()->active_edge();

  if (!edge)
  {
    vcl_cerr << "ERROR: an edge has to be selected first.\n";
    return;
  }

  // Avoid involving ref_edge and ref_node in this operation
  if ( edge == graph->ref_edge() || edge->source() == graph->ref_node() ||
    edge->target() == graph->ref_node() )
  {
    vcl_cerr << "ERROR: Either active_node is a ref_node or active_edge is a ref_edge.\n";
    return;
  }

  dbsksp_shock_node_sptr node = graph->remove_internal_edge(edge);
  if (!node)
  {
    vcl_cerr << "ERROR: active edge is not an internal edge.\n"
      << "Operation failed.\n";
  }
  else
  {
    vcl_cout << "Internal edge removal compleleted.\n";
    node->print(vcl_cout);
  }

  graph->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();
  return;
}











// ============================================================================
// dbsksp_tool_split_node_command
// ============================================================================

//: Execute the command
void dbsksp_tool_split_node_command::
execute()
{
  dbsksp_shock_node_sptr node = this->tool()->storage()->active_node();
  dbsksp_shock_edge_sptr e0 = this->tool()->storage()->edge0();
  dbsksp_shock_edge_sptr e1 = this->tool()->storage()->edge1();

  dbsksp_shock_graph_sptr graph = this->tool()->storage()->shock_graph();

  // Preliminary checks
  if (!node || !e0 || !e1)
  {
    vcl_cerr << "ERROR: Active node and e0 and e1 must be specified.\n";
    return;
  }

  if (node->degree() < 4)
  {
    vcl_cerr << "ERROR: degree of node has to be at least 4.\n";
    return;
  }

  if (!e0->is_vertex(node) || !e1->is_vertex(node))
  {
    vcl_cerr << "ERROR: e0 and e1 must both be incident at active node.\n";
  }

  // Now run the main command
  double chord_length = 1;
  dbsksp_shock_edge_sptr new_edge = 
    graph->split_shock_node(node, chord_length, e0, e1);

  // Report and clean up
  if (!new_edge)
  {
    vcl_cerr << "ERROR: Something went wrong when splitting the node.\n";
  }
  else
  {
    vcl_cout << "Parameters of the new edge = \n";
    new_edge->print(vcl_cout);
    vcl_cout << "The entire graph = \n";
    graph->print(vcl_cout);
  }

  graph->compute_all_dependent_params();
  bvis1_manager::instance()->display_current_frame();

  return;

}





