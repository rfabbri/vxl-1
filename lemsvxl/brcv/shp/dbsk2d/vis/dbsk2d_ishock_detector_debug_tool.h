// This is brcv/dbsk2d/vis/dbsk2d_ishock_detector_debug_tool.h
#ifndef dbsk2d_ishock_detector_debug_tool_h_
#define dbsk2d_ishock_detector_debug_tool_h_
//:
// \file
// \brief A tool for debugging the shock computation step by step 
// \author Amir Tamrakar
// \date 02/13/05
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsk2d_ishock_getinfo_tool.h"
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include "dbsk2d_shock_tableau.h"
#include "dbsk2d_shock_tableau_sptr.h"

#include <vgui/vgui_event_condition.h>

#include <dbsk2d/algo/dbsk2d_ishock_detector.h>

//:A tool for debugging the shock computation step by step 
class dbsk2d_ishock_detector_debug_tool : public dbsk2d_ishock_getinfo_tool
{
public:
  
  dbsk2d_ishock_detector_debug_tool();
  virtual ~dbsk2d_ishock_detector_debug_tool();

  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  //: popup menu for this tool
  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

protected:

  //: intrinsic shock detector
  dbsk2d_ishock_detector* sh_det_;

  //: clear all shocks
  vgui_event_condition clear_shocks;
  //: reinitialize shocks
  vgui_event_condition re_init_shocks;
  //: initialize shocks in the current cell
  vgui_event_condition init_shocks_within_cell;
  //: propagate till the end
  vgui_event_condition prop_until_end;
  //: single step propagation
  vgui_event_condition next_prop;
  //: jump propagation
  vgui_event_condition next_jump_prop;
  //: merge cells
  vgui_event_condition merge_cells;

  //: display the projections of the shocks
  bool disp_foots;
  
};

#endif // dbsk2d_ishock_detector_debug_tool_h_
