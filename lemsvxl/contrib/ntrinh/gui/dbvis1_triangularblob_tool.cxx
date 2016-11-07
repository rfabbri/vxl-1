// This is breye1/dbvis1/tool/dbvis1_triangularblob_tool.cxx
//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 11/22/2004

#include "dbvis1_triangularblob_tool.h"
#include <shockshape/shockbranch.h>
#include <bvis1/bvis1_manager.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>

//: Constructor
dbvis1_triangularblob_tool::dbvis1_triangularblob_tool()
{
  change_r[0] = vgui_event_condition(vgui_key('r'), vgui_MODIFIER_NULL, true);
  change_r[1] = vgui_event_condition(vgui_key('r'), vgui_SHIFT, true);
  
  change_r0[0][0] = vgui_event_condition(vgui_key('1'), vgui_MODIFIER_NULL, true);
  change_r0[0][1] = vgui_event_condition(vgui_key('1'), vgui_SHIFT, true);
  change_r0[1][0] = vgui_event_condition(vgui_key('2'), vgui_MODIFIER_NULL, true);
  change_r0[1][1] = vgui_event_condition(vgui_key('2'), vgui_SHIFT, true);
  change_r0[2][0] = vgui_event_condition(vgui_key('3'), vgui_MODIFIER_NULL, true);
  change_r0[2][1] = vgui_event_condition(vgui_key('3'), vgui_SHIFT, true);

  change_rpp[0][0] = vgui_event_condition(vgui_key('q'), vgui_MODIFIER_NULL, true);
  change_rpp[0][1] = vgui_event_condition(vgui_key('q'), vgui_SHIFT, true);
  change_rpp[1][0] = vgui_event_condition(vgui_key('w'), vgui_MODIFIER_NULL, true);
  change_rpp[1][1] = vgui_event_condition(vgui_key('w'), vgui_SHIFT, true);
  change_rpp[2][0] = vgui_event_condition(vgui_key('e'), vgui_MODIFIER_NULL, true);
  change_rpp[2][1] = vgui_event_condition(vgui_key('e'), vgui_SHIFT, true);
  
  change_psi[1][0] = vgui_event_condition(vgui_key('o'), vgui_MODIFIER_NULL, true);
  change_psi[1][1] = vgui_event_condition(vgui_key('o'), vgui_SHIFT, true);
  change_psi[2][0] = vgui_event_condition(vgui_key('p'), vgui_MODIFIER_NULL, true);
  change_psi[2][1] = vgui_event_condition(vgui_key('p'), vgui_SHIFT, true);

  change_kp[0][0] = vgui_event_condition(vgui_key('t'), vgui_MODIFIER_NULL, true);
  change_kp[0][1] = vgui_event_condition(vgui_key('t'), vgui_SHIFT, true);
  change_kp[1][0] = vgui_event_condition(vgui_key('y'), vgui_MODIFIER_NULL, true);
  change_kp[1][1] = vgui_event_condition(vgui_key('y'), vgui_SHIFT, true);
  change_kp[2][0] = vgui_event_condition(vgui_key('u'), vgui_MODIFIER_NULL, true);
  change_kp[2][1] = vgui_event_condition(vgui_key('u'), vgui_SHIFT, true);

  change_L[0][0] = vgui_event_condition(vgui_key('4'), vgui_MODIFIER_NULL, true);
  change_L[0][1] = vgui_event_condition(vgui_key('4'), vgui_SHIFT, true);
  change_L[1][0] = vgui_event_condition(vgui_key('5'), vgui_MODIFIER_NULL, true);
  change_L[1][1] = vgui_event_condition(vgui_key('5'), vgui_SHIFT, true);
  change_L[2][0] = vgui_event_condition(vgui_key('6'), vgui_MODIFIER_NULL, true);
  change_L[2][1] = vgui_event_condition(vgui_key('6'), vgui_SHIFT, true);
    
  delta = 0.1;
  change_delta[0] = vgui_event_condition(vgui_key('a'), vgui_MODIFIER_NULL, true);
  change_delta[1] = vgui_event_condition(vgui_key('a'), vgui_SHIFT, true); 
  reset = vgui_event_condition(vgui_key('s'), vgui_MODIFIER_NULL, true);
  compute_again = vgui_event_condition(vgui_key('k'), vgui_MODIFIER_NULL, true);
  change_scenario = vgui_event_condition(vgui_key('0'), vgui_MODIFIER_NULL, true);
}


//: Destructor
dbvis1_triangularblob_tool::~dbvis1_triangularblob_tool()
{

}

//: Set the tableau to work with
bool 
dbvis1_triangularblob_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if(!tableau)
    return false;
  if( tableau->type_name() == "dbgui_shockshape_tableau" ){
    if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
      return false;
    tableau_.vertical_cast(tableau);
    return true;
  }
  tableau_ = 0;
  return false;
}


//: Set the storage class for the active tableau
bool 
dbvis1_triangularblob_tool::set_storage ( const bpro1_storage_sptr& storage_sptr)
{
  if (!storage_sptr)
    return false;
  //make sure its a shockshape storage class
  if (storage_sptr->type() == "shockshape"){
    storage_.vertical_cast(storage_sptr);
    return true;
    if (! storage()->get_shockshape()){
      return true;
    }
    if (storage()->get_shockshape()->cast_to_triangularblob()){
      return true;
    }
  }
  storage_ = 0;
  return false;
}

//: Return the name of this tool
vcl_string
dbvis1_triangularblob_tool::name() const
{
  return "Triangular blob";
}

//: Return the storage of the tool
vidpro1_shockshape_storage_sptr dbvis1_triangularblob_tool::
storage()
{
  vidpro1_shockshape_storage_sptr shockshape_storage;
  shockshape_storage.vertical_cast(storage_);
  return shockshape_storage;
}

//: Return the tableau of the tool
dbgui_shockshape_tableau_sptr dbvis1_triangularblob_tool::
tableau(){
  return tableau_;
}

//: Handle events
bool dbvis1_triangularblob_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{

  // check if the shockshape object has already been created
  if (!storage()->get_shockshape()){     
    shockshape_triangularblob_sptr stb2 = new shockshape_triangularblob();
    storage()->set_shockshape(stb2->cast_to_shockshape());
  }

  if (!storage()->get_shockshape()->cast_to_triangularblob())
    return false;

  //shockshape_triangularblob_sptr stb;
  stb = storage()->get_shockshape()->cast_to_triangularblob();  
  
  // redraw if there is a change in parameters
  if (change_parameters(e)){
    SS_ERROR error = stb->compute_shockbranch_params(stb->cur_scenario(), true);
    if (error == ERROR_FREE)
      vcl_cout << "Valid Triangular blob " << vcl_endl;
    else
      vcl_cout << "Invalid Triangular blob " << vcl_endl;
      vcl_cout << "Error code = " << error;

    int num_steps = 25;
    tableau()->clear();
    tableau()->add_vsol_point_2d(new vsol_point_2d(0.0, 0.0));
    for (int j = 0; j<3; j++){
      shockbranch_sptr sb = stb->a3branch(j);
      /*vcl_cout << vcl_endl << " End Point " << j << vcl_endl;
      stb->a13point(j)->print(vcl_cout);*/

      vcl_cout << vcl_endl << "Branch " << j << vcl_endl;
      stb->a3branch(j)->print(vcl_cout);

      // compute the polyline along the shock and boundaries
      vcl_vector< vsol_point_2d_sptr > shock_points;
      vcl_vector< vsol_point_2d_sptr > bnd_left_pts;
      vcl_vector< vsol_point_2d_sptr > bnd_right_pts;
    
      double step = sb->length() / num_steps;
      for (int i = 0; i <= num_steps; i++){
        double s =  step * i;
        vgl_point_2d< double > left = sb->bnd_point_left_at(s);
        vgl_point_2d< double > right = sb->bnd_point_right_at(s);
        vgl_point_2d< double > center = sb->shock_point_at(s);

        bnd_left_pts.push_back(new vsol_point_2d(left));
        bnd_right_pts.push_back(new vsol_point_2d(right));
        shock_points.push_back(new vsol_point_2d(center));
        
        vcl_vector<vsol_point_2d_sptr > triple;
        triple.push_back(bnd_left_pts.back());
        triple.push_back(shock_points.back());
        triple.push_back(bnd_right_pts.back());
        tableau()->add_vsol_polyline_2d(new vsol_polyline_2d(triple), 
          vgui_style::new_style((float)((j%3)/3.0), (float)(((j+1)%3)/3.0), (float)(((j+2)%3)/3.0), 1, 1));
      }
    
      tableau()->add_vsol_polyline_2d(new vsol_polyline_2d( bnd_left_pts ), 
        vgui_style::new_style((float)0.5, (float) 0.5, (float)0.5, 3, 3));
      tableau()->add_vsol_polyline_2d(new vsol_polyline_2d( bnd_right_pts ), 
        vgui_style::new_style((float)0.8, (float)0.1, (float)0.6, 3, 3));
      tableau()->add_vsol_polyline_2d(new vsol_polyline_2d( shock_points ), 
        vgui_style::new_style((float)0.3, (float)0.1, (float)0.8, 3, 3));
      // add two circles at two end points
      tableau()->add_circle((float)sb->start().x(), (float) sb->start().y(), (float)sb->r0());
      vgl_point_2d<double > end_pt = sb->shock_point_at(sb->length());
      tableau()->add_circle((float)end_pt.x(), (float) end_pt.y(), (float)sb->r_at(sb->length()));
    }
    tableau()->post_redraw();
    return true;
  }
  return false;
}

bool dbvis1_triangularblob_tool::
change_parameters(const vgui_event& e){
  // change delta
  for (int i = 0; i<2; i++){
    if (change_delta[i](e)){
      double old_val = delta;
      if (i == 0) delta = delta * 10.0;
      else delta = delta / 10.0;
      change_params_message(i, "delta", old_val, delta); 
      return true;
    }
  }
  
  // compute again
  if (compute_again(e) ){
    vcl_cout << "Command: Compute all parameters again" << vcl_endl;  
    return true;
  }
  
  // reset to current default values
  if (reset(e)){
    vcl_cout << "Command: Reset all parameters" << vcl_endl;  
    stb->init();
    return true;
  }
  
  if (change_scenario(e) ){
    vcl_cout << "Command: Change scenario" << vcl_endl;  
    
    vgui_dialog scenario_dialog("Choose triangular blob scenario");
    int dialog_choice;
    scenario_dialog.choice("Choose scenario", "Complete A13", "All first-order coeffients", dialog_choice);
    if( !scenario_dialog.ask() )
      return false;
    switch (dialog_choice){
      case (0):
        stb->set_cur_scenario(COMPLETE_A13) ;
        break;
      case (1):
        stb->set_cur_scenario(PSI_LENGTH_RADIUS) ;
        break;
    }
    vcl_cout << "New scenario: " << stb->cur_scenario() << vcl_endl;
    return true;
  }
  
  if (stb->cur_scenario() == COMPLETE_A13)
    return change_complete_A13_params(e);
  if (stb->cur_scenario() == PSI_LENGTH_RADIUS)
    return change_psi_length_radius_params(e);
  return false;
}

//: check if any parameters has changed 
void dbvis1_triangularblob_tool::
change_params_message(int direction, vcl_string param_name, double old_value, double new_value){
  vcl_cout << vcl_endl << "Command: ";
  if (direction == 0) vcl_cout << "increase ";
  else vcl_cout << "decrease ";
  vcl_cout << param_name << vcl_endl;
  vcl_cout << "Old value is " << old_value << vcl_endl;
  vcl_cout << "New value is " << new_value << vcl_endl;
}

//: check change parameters in complete_A13_scenario
bool dbvis1_triangularblob_tool::
change_complete_A13_params(const vgui_event & e){
  vcl_string str[3];
  // change r
  for (int i = 0; i<2; i++){
    if (change_r[i](e)){
      double old_val = stb->a13point(0)->r;
      double new_val = old_val + delta * (1 - 2.0*i);
      for (int j = 0; j < 3; j++){
        stb->a13point(j)->r = new_val;
      }
      change_params_message(i, "r", old_val, new_val); 
      return true;
    }
  }
  
  // change psi
  str[0] = "psiA";
  str[1] = "psiB";
  str[2] = "psiC";
  for (int i = 1; i < 3; i++){
    for (int j = 0; j < 2; j++){
      if (change_psi[i][j](e)){
        double old_val = stb->a13point(i)->psi;
        stb->a13point(i)->psi = fmod(old_val + delta*(1 - 2.0*j), 2*vnl_math::pi);
        change_params_message(j, str[i], old_val, stb->a13point(i)->psi);
        return true;
      }
    }
  }
  
  // change rpp
  str[0] = "rpp_A";
  str[1] = "rpp_B";
  str[2] = "rpp_C";
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 2; j++){
      if (change_rpp[i][j](e)){
        double old_val = stb->a13point(i)->rpp;
        stb->a13point(i)->rpp = old_val + delta*(1 - 2.0*j);
        change_params_message(j, str[i], old_val, stb->a13point(i)->rpp);
        return true;
      }
    }
  }
  
  // change kp
  str[0] = "kp_A";
  str[1] = "kp_B";
  str[2] = "kp_C";
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 2; j++){
      if (change_kp[i][j](e)){
        double old_val = stb->a13point(i)->kp;
        stb->a13point(i)->kp = old_val + delta*(1 - 2.0*j);
        change_params_message(j, str[i], old_val, stb->a13point(i)->kp);
        return true;
      }
    }
  }
  return false;
}

// check for parameter change in PSI_LENGTH_RADIUS scenario
bool dbvis1_triangularblob_tool::
change_psi_length_radius_params(const vgui_event & e){
  vcl_string str[3];
  // change r
  for (int i = 0; i<2; i++){
    if (change_r[i](e)){
      double old_val = stb->plr_params().r;
      double new_val = old_val + delta * (1 - 2.0*i);
      stb->plr_params().r  = new_val;
      /*for (int j = 0; j < 3; j++){
        stb->a13point(j)->r = new_val;
      }*/
      change_params_message(i, "r", old_val, new_val); 
      return true;
    }
  }

  // change psi
  str[0] = "psiA";
  str[1] = "psiB";
  str[2] = "psiC";
  for (int i = 1; i < 3; i++){
    for (int j = 0; j < 2; j++){
      if (change_psi[i][j](e)){
        double old_val = stb->plr_params().psi[i];
        double new_val = fmod(old_val + delta*(1 - 2.0*j), 2*vnl_math::pi);
        stb->plr_params().psi[i] = new_val;
        change_params_message(j, str[i], old_val, new_val);
        return true;
      }
    }
  }

  // change r0
  str[0] = "rA";
  str[1] = "rB";
  str[2] = "rC";
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 2; j++){
      if (change_r0[i][j](e)){
        double old_val = stb->plr_params().r0[i];
        double new_val = old_val + delta*(1 - 2.0*j);
        stb->plr_params().r0[i] = new_val;
        change_params_message(j, str[i], old_val, new_val);
        return true;
      }
    }
  }

  // change L
  str[0] = "LA";
  str[1] = "LB";
  str[2] = "LC";
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 2; j++){
      if (change_L[i][j](e)){
        double old_val = stb->plr_params().L[i];
        double new_val = old_val + delta*(1 - 2.0*j);
        stb->plr_params().L[i] = new_val;
        change_params_message(j, str[i], old_val, new_val);
        return true;
      }
    }
  }

  return false;
}
