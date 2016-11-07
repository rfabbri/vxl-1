// This is shp/dbsksp/dbsksp_interp_two_xnodes_tool.cxx
//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Sep 28, 2006

#include "dbsksp_interp_two_xnodes_tool.h"

#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <vnl/vnl_math.h>

#include <dbsksp/vis/dbsksp_soview_shock.h>
#include <dbsksp/dbsksp_bnd_arc.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/algo/dbsksp_interp_two_xnodes.h>



//const vcl_string demo_types[] = {"Shapelets from 2 extrinsic nodes"};

//: Constructor
dbsksp_interp_two_xnodes_tool::
dbsksp_interp_two_xnodes_tool() : 
  tableau_(0), 
  storage_(0),
  increment_(1.0)
{
  // gestures
  this->gesture_display_info_ = vgui_event_condition(vgui_key('i'), vgui_MODIFIER_NULL, true);

  // change the value of increment (used in increase_param_value)
  this->gesture_increase_increment_value_ = vgui_event_condition(vgui_key('e'), 
    vgui_MODIFIER_NULL, true);
  this->gesture_decrease_increment_value_ = vgui_event_condition(vgui_key('e'), 
    vgui_SHIFT, true);

  this->gesture_increase_param_value_ = vgui_event_condition(vgui_key('p'), 
    vgui_MODIFIER_NULL, true);
  this->gesture_decrease_param_value_ = vgui_event_condition(vgui_key('m'), 
    vgui_MODIFIER_NULL, true);

  


  // task-specific params
  this->param_list_.insert(vcl_make_pair("x0", 0));
  this->param_list_.insert(vcl_make_pair("y0", 0));
  this->param_list_.insert(vcl_make_pair("psi0", 0));
  this->param_list_.insert(vcl_make_pair("phi0", vnl_math::pi*2/3));
  this->param_list_.insert(vcl_make_pair("radius0", 10));
  this->param_list_.insert(vcl_make_pair("x2", 100));
  this->param_list_.insert(vcl_make_pair("y2", 0));
  this->param_list_.insert(vcl_make_pair("psi2", 0));
  this->param_list_.insert(vcl_make_pair("phi2", vnl_math::pi*2/3));
  this->param_list_.insert(vcl_make_pair("radius2", 50));
  this->param_list_.insert(vcl_make_pair("param_t", 0.5));  
  this->active_param_name_ = "x0";


  // Interpolation modes
  //this->interp_mode_list_.push_back("phi1 free, = avg(phi0, phi2)");
  this->interp_mode_list_.push_back("param t(0..1) free, estimate phi1 from t, no optimization");
  this->interp_mode_list_.push_back("param t(0..1) free, optimize phi1 to fit xnodes");
  this->interp_mode_list_.push_back("optimize param t(0..1) to minimize curvature variation");
  this->active_interp_mode_ = this->interp_mode_list_[2];
}


//: Destructor
dbsksp_interp_two_xnodes_tool::
~dbsksp_interp_two_xnodes_tool()
{
}


//: Set the tableau to work with
bool dbsksp_interp_two_xnodes_tool::
set_tableau ( const vgui_tableau_sptr& tableau )
{
  if(!tableau)
    return false;
  if( tableau->type_name() == "dbsksp_shock_tableau" )
  {
    if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
      return false;
    this->tableau_.vertical_cast(tableau);
    return true;
  }
  tableau_ = 0;
  return false;
}


//: Set the storage class for the active tableau
bool dbsksp_interp_two_xnodes_tool::
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
//: Return the storage of the tool
dbsksp_shock_storage_sptr dbsksp_interp_two_xnodes_tool::
storage()
{
  return this->storage_;
}



// ----------------------------------------------------------------------------
//: Return the tableau of the tool
dbsksp_shock_tableau_sptr dbsksp_interp_two_xnodes_tool::
tableau(){
  return this->tableau_;
}



// the two extrinsic nodes
dbsksp_xshock_node_descriptor dbsksp_interp_two_xnodes_tool::
xnode_start()
{
  dbsksp_xshock_node_descriptor start_node;
  
  double x = this->param_list_.find("x0")->second;
  double y = this->param_list_.find("y0")->second;
  start_node.pt_.set(x, y);
  start_node.psi_ = this->param_list_.find("psi0")->second;
  start_node.phi_ = this->param_list_.find("phi0")->second;
  start_node.radius_ = this->param_list_.find("radius0")->second;
  return start_node;
}


dbsksp_xshock_node_descriptor dbsksp_interp_two_xnodes_tool::
xnode_end()
{
  dbsksp_xshock_node_descriptor end_node;
  double x = this->param_list_.find("x2")->second;
  double y = this->param_list_.find("y2")->second;
  end_node.pt_.set(x, y);
  end_node.psi_ = this->param_list_.find("psi2")->second;
  end_node.phi_ = this->param_list_.find("phi2")->second;
  end_node.radius_ = this->param_list_.find("radius2")->second;
  return end_node;
}




// fixed param
double dbsksp_interp_two_xnodes_tool::
param_t()
{
  return this->param_list_.find("param_t")->second;
}



//: Allow the tool to add to the popup menu as a tableau would
void dbsksp_interp_two_xnodes_tool::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  // construct shapelets from two extrinsic nodes
  for (vcl_map<vcl_string, double >::iterator itr = 
    this->param_list_.begin(); 
    itr != this->param_list_.end(); ++itr)
  {
    vcl_string checkbox;
    if (itr->first == this->active_param_name_)
    {
      checkbox = "[x] ";
    }
    else
      checkbox = "[ ] ";
    menu.add(checkbox + itr->first, 
      new dbsksp_set_param_command<vcl_string >(&this->active_param_name_, itr->first));    
  }

  // Interpolation mode menu
  vgui_menu mode_menu;
  for (vcl_vector<vcl_string >::iterator itr = this->interp_mode_list_.begin();
    itr != this->interp_mode_list_.end(); ++itr)
  {
    vcl_string checkbox;
    if (*itr == this->active_interp_mode_)
    {
      checkbox = "[x] ";
    }
    else
      checkbox = "[ ] ";
    mode_menu.add(checkbox + *itr, 
      new dbsksp_set_param_command<vcl_string >(&this->active_interp_mode_, *itr ));    
  }


  menu.separator();
  menu.add("Interpolation mode", mode_menu);
}

// ----------------------------------------------------------------------------
//: Handle events
bool dbsksp_interp_two_xnodes_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{
  // Increase increment value
  if (this->gesture_increase_increment_value_(e))
  {
    this->increment_ *= 10;
    vgui::out << "increment=" << this->increment_<< vcl_endl;
    return false;
  }

  // Decrease increment value
  if (this->gesture_decrease_increment_value_(e))
  {
    this->increment_ /= 10;
    vgui::out << "increment=" << this->increment_<< vcl_endl;
    return false;
  }


  // Increase parameter value
  if (this->gesture_increase_param_value_(e))
  {
    return this->handle_change_param_value(this->increment_);
  }

  // Decrease parameter value
  if (this->gesture_decrease_param_value_(e))
  {
    return this->handle_change_param_value(-this->increment_);
  }

  // Event: Redraws all curves, windows, etc
  if (e.type == vgui_DRAW_OVERLAY) 
  {
    this->handle_draw_overlay();
    return true;
  }

  
  
  return false;
}




bool dbsksp_interp_two_xnodes_tool::
handle_display_info()
{
  vcl_cout << "Parameters = \n";
  for (vcl_map<vcl_string, double >::iterator itr = 
    this->param_list_.begin(); 
    itr != this->param_list_.end(); ++itr)
  {
    vcl_cout << itr->first << " = " << itr->second << vcl_endl;  
  }
  vcl_cout << "increment = " << this->increment_ << vcl_endl;

  return true;
}



// ----------------------------------------------------------------------------
bool dbsksp_interp_two_xnodes_tool::
handle_change_param_value(double increment)
{
  vcl_map<vcl_string, double >::iterator itr = 
    this->param_list_.find(this->active_param_name_);
  if (itr != this->param_list_.end())
  {
    vcl_cout << "-----------------------------------------------------\n";
    vcl_cout << "Parameter name = " << itr->first << vcl_endl;
    vcl_cout << "Old value = " << itr->second << vcl_endl;
    itr->second += increment;
    vcl_cout << "New value = " << itr->second << vcl_endl;
    vcl_cout << "-----------------------------------------------------\n";
  }

  // compute two shapelets from two extrinsic nodes
  dbsksp_xshock_node_descriptor start_xnode = this->xnode_start();
  dbsksp_xshock_node_descriptor end_xnode = this->xnode_end();

   dbsksp_twoshapelet_sptr ss = 0;
  
  
  // --------------------------------------------------------------------------

  // Interpolation method 2: choose t (0..1) as fixed param
  // estimate alpha0 (fixed) 
  // estimate phi1 
  // show initialization
  if (this->active_interp_mode_ == this->interp_mode_list_[0])
  {
    dbsksp_optimal_interp_two_xnodes interpolator(start_xnode, end_xnode);
    this->shock_estimate_ = interpolator.shock_estimate();
    this->left_bnd_estimate_ = interpolator.left_bnd_estimate();
    this->right_bnd_estimate_ = interpolator.right_bnd_estimate();
    double t = this->param_t();
    if (t > 0 && t < 1)
    {
      ss = interpolator.init(t);
      vcl_cout << "alpha0 = " << ss->shapelet_start()->alpha_start() << vcl_endl;
    }
    else
    {
      vcl_cout << "ERROR: t must be in (0, 1). Current t = " << t << vcl_endl;
      return true;
    }
  }

  // --------------------------------------------------------------------------
  
  // Interpolation method 3: choose t (0..1) as fixed param
  // estimate alpha0 (fixed) 
  // optimize phi1 to fit to the xnodes
  // show initialization
  if (this->active_interp_mode_ == this->interp_mode_list_[1])
  {
    dbsksp_optimal_interp_two_xnodes interpolator(start_xnode, end_xnode);
    this->shock_estimate_ = interpolator.shock_estimate();
    this->left_bnd_estimate_ = interpolator.left_bnd_estimate();
    this->right_bnd_estimate_ = interpolator.right_bnd_estimate();
    double t = this->param_t();
    if (t > 0 && t < 1)
    {
      ss = interpolator.optimize(t);
      //vcl_cout << "alpha0 = " << ss->shapelet_start()->alpha_start() << vcl_endl;
    }
    else
    {
      vcl_cout << "ERROR: t must be in (0, 1). Current t = " << t << vcl_endl;
      return true;
    }
  }

 
  // --------------------------------------------------------------------------

  

  // Interpolation method 4: optimize alpha0 to minimize sum of curvature
  // variation
  if (this->active_interp_mode_ == this->interp_mode_list_[2])
  {
    dbsksp_optimal_interp_two_xnodes interpolator(start_xnode, end_xnode);
    this->shock_estimate_ = interpolator.shock_estimate();
    this->left_bnd_estimate_ = interpolator.left_bnd_estimate();
    this->right_bnd_estimate_ = interpolator.right_bnd_estimate();
    ss = interpolator.optimize();
  }

  // --------------------------------------------------------------------------
  this->set_active_twoshapelet(ss);
  bvis1_manager::instance()->post_overlay_redraw();
  return true;
}


// ----------------------------------------------------------------------------
bool dbsksp_interp_two_xnodes_tool::
handle_draw_overlay()
{
  dbsksp_xshock_node_descriptor start_node = this->xnode_start();;
  dbsksp_xshock_node_descriptor end_node = this->xnode_end();;

  this->draw_extrinsic_node(start_node);
  this->draw_extrinsic_node(end_node);

  if (this->active_twoshapelet())
  {
    dbsksp_soview_shapelet so_start(this->active_twoshapelet()->shapelet_start());
    so_start.set_colour(0.0f, 1.0f, 0.0f);
    so_start.get_style()->apply_all();
    so_start.draw();
    dbsksp_soview_shapelet so_end(this->active_twoshapelet()->shapelet_end());
    so_end.set_colour(0.0f, 1.0f, 0.0f);
    so_end.get_style()->apply_all();
    so_end.draw();
  }

  // draw the shock and boundary estimate
  // shock estimate
  dbgl_circ_arc ss(this->shock_estimate_.start(), this->shock_estimate_.mid_pt(), 
    this->shock_estimate_.k1());
  dbsksp_soview_bnd_arc so_ss = dbsksp_soview_bnd_arc( new dbsksp_bnd_arc(ss));
  so_ss.set_colour(1.0f, 0.0f, 0.0f);
  so_ss.get_style()->apply_all();
  so_ss.draw();


  dbgl_circ_arc se(this->shock_estimate_.mid_pt(), 
    this->shock_estimate_.point_at(this->shock_estimate_.len()), 
    this->shock_estimate_.k2());
  dbsksp_soview_bnd_arc so_se = dbsksp_soview_bnd_arc( new dbsksp_bnd_arc(se));
  so_se.set_colour(1.0f, 0.0f, 0.0f);
  so_se.get_style()->apply_all();
  so_se.draw();

  // left boundary estimate
  dbgl_circ_arc ls(this->left_bnd_estimate_.start(), this->left_bnd_estimate_.mid_pt(), 
    this->left_bnd_estimate_.k1());
  dbsksp_soview_bnd_arc so_ls = dbsksp_soview_bnd_arc( new dbsksp_bnd_arc(ls));
  so_ls.set_colour(0.0f, 0.0f, 1.0f);
  so_ls.get_style()->apply_all();
  so_ls.draw();


  dbgl_circ_arc le(this->left_bnd_estimate_.mid_pt(), 
    this->left_bnd_estimate_.point_at(this->left_bnd_estimate_.len()), 
    this->left_bnd_estimate_.k2());
  dbsksp_soview_bnd_arc so_le = dbsksp_soview_bnd_arc( new dbsksp_bnd_arc(le));
  so_le.set_colour(0.0f, 0.0f, 1.0f);
  so_le.get_style()->apply_all();
  so_le.draw();

  // right boundary estimate
  dbgl_circ_arc rs(this->right_bnd_estimate_.start(), this->right_bnd_estimate_.mid_pt(), 
    this->right_bnd_estimate_.k1());
  dbsksp_soview_bnd_arc so_rs = dbsksp_soview_bnd_arc( new dbsksp_bnd_arc(rs));
  so_rs.set_colour(0.0f, 0.0f, 1.0f);
  so_rs.get_style()->apply_all();
  so_rs.draw();


  dbgl_circ_arc re(this->right_bnd_estimate_.mid_pt(), 
    this->right_bnd_estimate_.point_at(this->right_bnd_estimate_.len()), 
    this->right_bnd_estimate_.k2());
  dbsksp_soview_bnd_arc so_re = dbsksp_soview_bnd_arc( new dbsksp_bnd_arc(re));
  so_re.set_colour(0.0f, 0.0f, 1.0f);
  so_re.get_style()->apply_all();
  so_re.draw();

  // draw a straight line connecting the starting shock point to a point on the 
  // biarc shock estimate corresponding to parameter t
  vgl_point_2d<double > pt0 = this->xnode_start().pt_;
  vgl_point_2d<double > pt1 = this->shock_estimate_.point_at(
    this->param_t() * this->shock_estimate_.len());

  vgui_soview2D_lineseg line(float(pt0.x()), float(pt0.y()), float(pt1.x()), float(pt1.y()));
  line.draw();

  
  return true;
}





// ----------------------------------------------------------------------------
void dbsksp_interp_two_xnodes_tool::
draw_extrinsic_node(const dbsksp_xshock_node_descriptor& node)
{
  // i. circle
  vgui_soview2D_circle circle((float)node.pt_.x(), (float)node.pt_.y(), (float)node.radius_);
  circle.set_colour(0.0f, 1.0f, 0.1f);
  circle.get_style()->apply_all();
  circle.draw();

  // ii. tangent
  vgui_soview2D_lineseg lineseg((float)node.pt_.x(), (float)node.pt_.y(), 
    (float)(node.pt_.x() + node.radius_ * vcl_cos(node.psi_)), 
    (float)(node.pt_.y() + node.radius_ * vcl_sin(node.psi_)));
  
  lineseg.draw();

  // iii. contact shocks
  // left
  lineseg.x0 = (float)node.pt_.x();
  lineseg.y0 = (float)node.pt_.y();
  lineseg.x1 = (float)(node.pt_.x() + node.radius_*vcl_cos(node.psi_+node.phi_)); 
  lineseg.y1 = (float)(node.pt_.y() + node.radius_*vcl_sin(node.psi_+node.phi_));
  lineseg.set_colour(0.0f, 1.0f, 0.0f);
  lineseg.draw();

  // right
  lineseg.x0 = (float)node.pt_.x();
  lineseg.y0 = (float)node.pt_.y();
  lineseg.x1 = (float)(node.pt_.x() + node.radius_*vcl_cos(node.psi_-node.phi_)); 
  lineseg.y1 = (float)(node.pt_.y() + node.radius_*vcl_sin(node.psi_-node.phi_));
  lineseg.draw();

}








