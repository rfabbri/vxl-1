// This is breye/dbvis/tool/dbvis_shockshape_tool.h
#ifndef dbvis_shockshape_tool_h_
#define dbvis_shockshape_tool_h_
//:
// \file
// \brief A tool for designing an A3 shock branch and its boundary 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 10/27/2004
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis/bvis_tool.h>
#include <dvidpro/storage/vidpro_shockshape_storage.h>
#include <dvidpro/storage/vidpro_shockshape_storage_sptr.h>
#include <dbgui/dbgui_shockshape_tableau_sptr.h>
#include <dbgui/dbgui_shockshape_tableau.h>
#include <vgui/vgui_event_condition.h>

#include <shockshape/shockbranch_sptr.h>

//: A tool for designing an A3 shock branch and its boundary
class dbvis_shockshape_tool : public bvis_tool
{
public:
  
  dbvis_shockshape_tool();
  virtual ~dbvis_shockshape_tool();

  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro_storage_sptr& storage);

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis_view_tableau_sptr& view );

  bool change_shock_parameters(const vgui_event& e);


private:

protected:
  dbgui_shockshape_tableau_sptr tableau();
  vidpro_shockshape_storage_sptr storage();

  dbgui_shockshape_tableau_sptr tableau_;
  vidpro_shockshape_storage_sptr storage_;

  shockbranch_sptr sb;

  // increment or decrement of parameter each time 
  // a key is pressed
  double delta; 

  vgui_event_condition r0_inc;
  vgui_event_condition r0_dec;
  vgui_event_condition k0_inc;
  vgui_event_condition k0_dec;
  vgui_event_condition k1_inc;
  vgui_event_condition k1_dec;
  vgui_event_condition length_inc;
  vgui_event_condition length_dec;
  vgui_event_condition nu0_inc;
  vgui_event_condition nu0_dec;
  vgui_event_condition nu1_inc;
  vgui_event_condition nu1_dec;   
  vgui_event_condition delta_inc;
  vgui_event_condition delta_dec;   
  vgui_event_condition reset;
};

#endif // dbvis_bvrl_tools_h_
