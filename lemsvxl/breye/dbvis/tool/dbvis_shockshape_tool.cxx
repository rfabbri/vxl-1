// This is breye/dbvis/tool/dbvis_shockshape_tool.cxx
//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 10/27/2004

#include "dbvis_shockshape_tool.h"
#include <bvis/bvis_manager.h>
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
dbvis_shockshape_tool::dbvis_shockshape_tool()
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
dbvis_shockshape_tool::~dbvis_shockshape_tool()
{

}

//: Set the tableau to work with
bool 
dbvis_shockshape_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if(!tableau)
    return false;
  if( tableau->type_name() == "dbgui_shockshape_tableau" ){
    if( !this->set_storage(bvis_manager::instance()->storage_from_tableau(tableau)) )
      return false;
    tableau_.vertical_cast(tableau);
    return true;
  }
  tableau_ = 0;
  return false;
}


//: Set the storage class for the active tableau
bool 
dbvis_shockshape_tool::set_storage ( const bpro_storage_sptr& storage_sptr)
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
std::string
dbvis_shockshape_tool::name() const
{
  return "A3 shock branch";
}

//: Return the storage of the tool
vidpro_shockshape_storage_sptr dbvis_shockshape_tool::
storage()
{
  vidpro_shockshape_storage_sptr shockshape_storage;
  shockshape_storage.vertical_cast(storage_);
  return shockshape_storage;
}

//: Return the tableau of the tool
dbgui_shockshape_tableau_sptr dbvis_shockshape_tool::
tableau(){
  return tableau_;
}

//: Handle events
bool dbvis_shockshape_tool::
handle( const vgui_event & e, const bvis_view_tableau_sptr& view )
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
      std::cout << "Shock branch is legal" << std::endl;
    else
      std::cout << "Shock branch is illegal" << std::endl;

    sb->update_shock_geom();
    tableau()->clear();
    
    // compute the polyline along the shock
    std::vector< vsol_point_2d_sptr > shock_points;
    
    // compute the two polylines along the boundary
    std::vector< vsol_point_2d_sptr > bnd_left_pts;
    std::vector< vsol_point_2d_sptr > bnd_right_pts;
    
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
      std::vector<vsol_point_2d_sptr > triple;
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

bool dbvis_shockshape_tool::change_shock_parameters(const vgui_event& e){
  if( r0_inc(e) ) {
    std::cout << std::endl << "Command: increase r0" << std::endl;
    double r0 = sb->r0();
    if (r0+delta < 100.0) r0 += delta;
    sb->set_r0(r0);
    std::cout << "New r0 = " << r0 << std::endl;
    return true;
  }

  if (r0_dec(e) ){
    std::cout << "Command: decrease r0" << std::endl;
    double r0 = sb->r0();
    if (r0 - delta > 0.0) r0 -= delta;
    sb->set_r0(r0);
    std::cout << "New r0 = " << r0 << std::endl;
    return true;
  }
  
  if (length_inc(e) ){
    std::cout << "Command: increase shock length" << std::endl;
    double length = sb->length();
    if (length+delta < 10) length += delta;
    sb->set_length(length);
    std::cout << "New length = " << length << std::endl;
    return true;
  }

  if (length_dec(e) ){
    std::cout << "Command: decrease shock length" << std::endl;
    double length = sb->length();
    if (length-delta > 0.001) length -= delta;
    sb->set_length(length);
    std::cout << "New length = " << length << std::endl;
    return true;
  }

  if (k0_inc(e) ){
    std::cout << "Command: increase k0" << std::endl;
    //increase k0
    //(*(sb->k()))[1] += delta;
    sb->set_k0(sb->k0() + delta);
    std::cout << "New k0 = " << sb->k0() << std::endl;
    return true;
  }

  if (k0_dec(e) ){
    std::cout << "Command: decrease k0" << std::endl;
    //decrease k0
    //(*(sb->k()))[0] -= delta;
    sb->set_k0(sb->k0() - delta);
    std::cout << "New k0 = " << sb->k0() << std::endl;
    return true;
  }

  if (k1_inc(e) ){
    std::cout << "Command: increase k1" << std::endl;
    //increase k1
    // (*(sb->k()))[1] += delta;
    sb->set_k1(sb->k1() + delta);
    std::cout << "New k1 = " << sb->k1() << std::endl;
    return true;
  }

  if (k1_dec(e) ){
    std::cout << "Command: decrease k1" << std::endl;
    //decrease k0
    //(*(sb->k()))[1] -= delta;
    sb->set_k1(sb->k1() - delta);
    std::cout << "New k1 = " << sb->k1() << std::endl;
    return true;
  }

  if (nu0_inc(e) ){
    std::cout << "Command: increase nu0" << std::endl;
    //increase nu0
    //(*(sb->nu()))[0] += delta;
    sb->set_nu0(sb->nu0() + delta);
    std::cout << "New nu0 = " << sb->nu0() << std::endl;
    return true;
  }

  if (nu0_dec(e) ){
    std::cout << "Command: decrease nu0" << std::endl;
    //decrease nu0
    //(*(sb->nu()))[0] -= delta;
    sb->set_nu0(sb->nu0() - delta);
    std::cout << "New nu0 = " << sb->nu0() << std::endl;
    return true;
  }

  if (nu1_inc(e) ){
    std::cout << "Command: increase nu1" << std::endl;
    //increase nu1
    // (*(sb->nu()))[1] += delta;
    sb->set_nu1(sb->nu1() + delta);
    std::cout << "New nu1 = " << sb->nu1() << std::endl;
    return true;
  }

  if (nu1_dec(e) ){
    std::cout << "Command: decrease nu1" << std::endl;
    //decrease nu0
    //(*(sb->nu()))[1] -= delta;
    sb->set_nu1(sb->nu1() - delta);
    std::cout << "New nu1 = " << sb->nu1() << std::endl;
    return true;
  }

  if (delta_inc(e) ){
    std::cout << "Command: increase delta" << std::endl;
    delta = delta * 10.0;
    std::cout << "New delta = " << delta << std::endl;
    return true;
  }

  if (delta_dec(e) ){
    std::cout << "Command: decrease delta" << std::endl;
    delta = delta / 10.0;
    std::cout << "New delta = " << delta << std::endl;
    return true;
  }

  if (reset(e) ){
    std::cout << "Command: Reset all parameters" << std::endl;
    sb->reset_all_params(A3_BRANCH);
    sb->set_psi0(vnl_math::pi_over_2);
    return true;
  }

  return false;
}
