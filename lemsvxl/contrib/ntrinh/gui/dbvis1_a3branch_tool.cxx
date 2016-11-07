// This is breye1/dbvis1/tool/dbvis1_a3branch_tool.cxx
//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 10/27/2004

#include "dbvis1_shockshape_tool.h"
#include <bvis1/bvis1_manager.h>
#include <shockshape/shockshape.h>
#include <shockshape/shockshape_sptr.h>
#include <shockshape/shockshape_a3branch.h>
#include <shockshape/shockshape_a3branch_sptr.h>
#include <vnl/vnl_math.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vgui/vgui_style.h>


//: Constructor
dbvis1_shockshape_tool::dbvis1_shockshape_tool()
{
  r0_inc = vgui_event_condition(vgui_key('r'), vgui_MODIFIER_NULL, true);
  r0_dec = vgui_event_condition(vgui_key('r'), vgui_SHIFT, true);
  k0_inc = vgui_event_condition(vgui_key('k'), vgui_MODIFIER_NULL, true);
  k0_dec = vgui_event_condition(vgui_key('k'), vgui_SHIFT, true);;
  k1_inc = vgui_event_condition(vgui_key('i'), vgui_MODIFIER_NULL, true);
  k1_dec = vgui_event_condition(vgui_key('i'), vgui_SHIFT, true);
  length_inc = vgui_event_condition(vgui_key('l'), vgui_MODIFIER_NULL, true);
  length_dec = vgui_event_condition(vgui_key('l'), vgui_SHIFT, true);
  nu0_inc = vgui_event_condition(vgui_key('b'), vgui_MODIFIER_NULL, true);
  nu0_dec = vgui_event_condition(vgui_key('b'), vgui_SHIFT, true);;
  nu1_inc = vgui_event_condition(vgui_key('h'), vgui_MODIFIER_NULL, true);
  nu1_dec = vgui_event_condition(vgui_key('h'), vgui_SHIFT, true);
  delta_inc = vgui_event_condition(vgui_key('a'), vgui_MODIFIER_NULL, true);
  delta_dec = vgui_event_condition(vgui_key('a'), vgui_SHIFT, true);
  reset = vgui_event_condition(vgui_key('s'), vgui_MODIFIER_NULL, true);

  delta = 0.1;
}


//: Destructor
dbvis1_shockshape_tool::~dbvis1_shockshape_tool()
{

}

//: Set the tableau to work with
bool 
dbvis1_shockshape_tool::set_tableau ( const vgui_tableau_sptr& tableau )
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
dbvis1_shockshape_tool::set_storage ( const bpro1_storage_sptr& storage_sptr)
{
  if (!storage_sptr)
    return false;
  //make sure its a vsol storage class
  if (storage_sptr->type() == "shockshape"){
    storage_.vertical_cast(storage_sptr);
    if (! storage()->get_shockshape()){
      return true;
    }
    
    if (storage()->get_shockshape()->cast_to_a3branch()){
      return true;
    }
  }
  storage_ = 0;
  return false;
}

//: Return the name of this tool
vcl_string
dbvis1_shockshape_tool::name() const
{
  return "A3 shock branch";
}

//: Return the storage of the tool
vidpro1_shockshape_storage_sptr dbvis1_shockshape_tool::
storage()
{
  vidpro1_shockshape_storage_sptr shockshape_storage;
  shockshape_storage.vertical_cast(storage_);
  return shockshape_storage;
}

//: Return the tableau of the tool
dbgui_shockshape_tableau_sptr dbvis1_shockshape_tool::
tableau(){
  return tableau_;
}

//: Handle events
bool dbvis1_shockshape_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{
  // check if the shockshape object has already been created

  if (!storage()->get_shockshape())
  {
    shockshape_a3branch_sptr ss2 = new shockshape_a3branch();
 //   ss2->a3branch()->set_start(100.0, 100.0);
    ss2->a3branch()->set_psi0(vnl_math::pi_over_2);
    storage()->set_shockshape(ss2->cast_to_shockshape());
  }
  if (!storage()->get_shockshape()->cast_to_a3branch())
    return false;

  sb = storage()->get_shockshape()->cast_to_a3branch()->a3branch();
  
  // redraw if there is a change in parameters
  if (change_shock_parameters(e)){
    if (sb->is_legal_a3branch())
      vcl_cout << "Shock branch is legal" << vcl_endl;
    else
      vcl_cout << "Shock branch is illegal" << vcl_endl;

    sb->update_shock_geom();
    tableau()->clear();
    
    // compute the polyline along the shock
    vcl_vector< vsol_point_2d_sptr > shock_points;
    
    // compute the two polylines along the boundary
    vcl_vector< vsol_point_2d_sptr > bnd_left_pts;
    vcl_vector< vsol_point_2d_sptr > bnd_right_pts;
    
    int num_steps = 50;
    double step = sb->length() / num_steps;
    for (int i = 0; i < num_steps; i++){
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

      tableau()->add_vsol_polyline_2d(new vsol_polyline_2d(triple));
//      tableau()->add_vsol_line_2d(new vsol_line_2d(new vsol_point_2d(center), 
//        new vsol_point_2d(left))); 
//      tableau()->add_vsol_line_2d(new vsol_line_2d(new vsol_point_2d(center), 
//        new vsol_point_2d(right))); 
    }
    
    tableau()->add_vsol_polyline_2d(new vsol_polyline_2d( bnd_left_pts ), 
      vgui_style::new_style((float)0.5, (float) 0.5, (float)0.5, 1, 1));
    tableau()->add_vsol_polyline_2d(new vsol_polyline_2d( bnd_right_pts ), 
      vgui_style::new_style((float)0.6, (float)0.6, (float)0.6, 1, 1));
    tableau()->add_vsol_polyline_2d(new vsol_polyline_2d( shock_points ), 
      vgui_style::new_style((float)0.3, (float)0.1, (float)0.8, 1, 1));
    
    tableau()->post_redraw();
    return true;
  }
  return false;
}

bool dbvis1_shockshape_tool::change_shock_parameters(const vgui_event& e){
  if( r0_inc(e) ) {
    vcl_cout << vcl_endl << "Command: increase r0" << vcl_endl;
    double r0 = sb->r0();
    if (r0+delta < 100.0) r0 += delta;
    sb->set_r0(r0);
    vcl_cout << "New r0 = " << r0 << vcl_endl;
    return true;
  }

  if (r0_dec(e) ){
    vcl_cout << "Command: decrease r0" << vcl_endl;
    double r0 = sb->r0();
    if (r0 - delta > 0.0) r0 -= delta;
    sb->set_r0(r0);
    vcl_cout << "New r0 = " << r0 << vcl_endl;
    return true;
  }
  
  if (length_inc(e) ){
    vcl_cout << "Command: increase shock length" << vcl_endl;
    double length = sb->length();
    if (length+delta < 10) length += delta;
    sb->set_length(length);
    vcl_cout << "New length = " << length << vcl_endl;
    return true;
  }

  if (length_dec(e) ){
    vcl_cout << "Command: decrease shock length" << vcl_endl;
    double length = sb->length();
    if (length-delta > 0.001) length -= delta;
    sb->set_length(length);
    vcl_cout << "New length = " << length << vcl_endl;
    return true;
  }

  if (k0_inc(e) ){
    vcl_cout << "Command: increase k0" << vcl_endl;
    //increase k0
    //(*(sb->k()))[1] += delta;
    sb->set_k0(sb->k0() + delta);
    vcl_cout << "New k0 = " << sb->k0() << vcl_endl;
    return true;
  }

  if (k0_dec(e) ){
    vcl_cout << "Command: decrease k0" << vcl_endl;
    //decrease k0
    //(*(sb->k()))[0] -= delta;
    sb->set_k0(sb->k0() - delta);
    vcl_cout << "New k0 = " << sb->k0() << vcl_endl;
    return true;
  }

  if (k1_inc(e) ){
    vcl_cout << "Command: increase k1" << vcl_endl;
    //increase k1
    // (*(sb->k()))[1] += delta;
    sb->set_k1(sb->k1() + delta);
    vcl_cout << "New k1 = " << sb->k1() << vcl_endl;
    return true;
  }

  if (k1_dec(e) ){
    vcl_cout << "Command: decrease k1" << vcl_endl;
    //decrease k0
    //(*(sb->k()))[1] -= delta;
    sb->set_k1(sb->k1() - delta);
    vcl_cout << "New k1 = " << sb->k1() << vcl_endl;
    return true;
  }

  if (nu0_inc(e) ){
    vcl_cout << "Command: increase nu0" << vcl_endl;
    //increase nu0
    //(*(sb->nu()))[0] += delta;
    sb->set_nu0(sb->nu0() + delta);
    vcl_cout << "New nu0 = " << sb->nu0() << vcl_endl;
    return true;
  }

  if (nu0_dec(e) ){
    vcl_cout << "Command: decrease nu0" << vcl_endl;
    //decrease nu0
    //(*(sb->nu()))[0] -= delta;
    sb->set_nu0(sb->nu0() - delta);
    vcl_cout << "New nu0 = " << sb->nu0() << vcl_endl;
    return true;
  }

  if (nu1_inc(e) ){
    vcl_cout << "Command: increase nu1" << vcl_endl;
    //increase nu1
    // (*(sb->nu()))[1] += delta;
    sb->set_nu1(sb->nu1() + delta);
    vcl_cout << "New nu1 = " << sb->nu1() << vcl_endl;
    return true;
  }

  if (nu1_dec(e) ){
    vcl_cout << "Command: decrease nu1" << vcl_endl;
    //decrease nu0
    //(*(sb->nu()))[1] -= delta;
    sb->set_nu1(sb->nu1() - delta);
    vcl_cout << "New nu1 = " << sb->nu1() << vcl_endl;
    return true;
  }

  if (delta_inc(e) ){
    vcl_cout << "Command: increase delta" << vcl_endl;
    delta = delta * 10.0;
    vcl_cout << "New delta = " << delta << vcl_endl;
    return true;
  }

  if (delta_dec(e) ){
    vcl_cout << "Command: decrease delta" << vcl_endl;
    delta = delta / 10.0;
    vcl_cout << "New delta = " << delta << vcl_endl;
    return true;
  }

  if (reset(e) ){
    vcl_cout << "Command: Reset all parameters" << vcl_endl;
    sb->reset_all_params(A3_BRANCH);
    sb->set_psi0(vnl_math::pi_over_2);
    return true;
  }

  return false;
}
