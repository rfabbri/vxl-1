// This is breye1/dbvis1/tool/dbvis1_triangularblob_tool.h
#ifndef dbvis1_triangularblob_tool_h_
#define dbvis1_triangularblob_tool_h_
//:
// \file
// \brief A tool for designing a triangular blob (3 A3 shock branches)
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 11/22/2004
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include "vidpro1_shockshape_storage.h"
#include "vidpro1_shockshape_storage_sptr.h"
#include "dbgui_shockshape_tableau_sptr.h"
#include "dbgui_shockshape_tableau.h"
#include <vgui/vgui_event_condition.h>

#include <shockshape/shockbranch_sptr.h>
#include <shockshape/shockshape_triangularblob.h>
#include <shockshape/shockshape_triangularblob_sptr.h>

//: A tool for designing an triangular blob
class dbvis1_triangularblob_tool : public bvis1_tool
{
public:
  
  dbvis1_triangularblob_tool();
  virtual ~dbvis1_triangularblob_tool();

  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  void change_params_message(int direction, vcl_string param_name, double old_value, double new_value);

private:

protected:
  dbgui_shockshape_tableau_sptr tableau();
  vidpro1_shockshape_storage_sptr storage();

  dbgui_shockshape_tableau_sptr tableau_;
  vidpro1_shockshape_storage_sptr storage_;

  bool change_parameters(const vgui_event& e);
  bool change_complete_A13_params(const vgui_event & e);
  bool change_psi_length_radius_params(const vgui_event & e);

  // increment or decrement of parameter each time a key is pressed
  double delta; 
  vgui_event_condition change_r0[3][2];
  vgui_event_condition change_r[2];
  vgui_event_condition change_rpp[3][2];
  vgui_event_condition change_psi[3][2];
  vgui_event_condition change_kp[3][2];
  vgui_event_condition change_L[3][2];
  vgui_event_condition change_delta[2];
  vgui_event_condition reset;
  vgui_event_condition compute_again;
  
  vgui_event_condition change_scenario;

  shockshape_triangularblob_sptr stb;
};

#endif // dbvis1_triangularblob_tools_h_
