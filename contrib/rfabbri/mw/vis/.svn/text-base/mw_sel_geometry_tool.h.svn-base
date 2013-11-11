// This is brcv/seg/dbdet/vis/mw_sel_geometry_tool.h
#ifndef mw_sel_geometry_tool_h
#define mw_sel_geometry_tool_h
//:
//\file
//\brief Extension of sel_explorer tool to deal with ground-truth geometry
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Thu May 11 15:14:44 EDT 2006
//

#include <dbdet/vis/dbdet_sel_explorer_tool.h>

#include <dbdif/pro/dbdif_3rd_order_geometry_storage_sptr.h>

//:  Extend sel_explorer tool to deal with ground-truth data given by a
// dbdif_3rd_order_geometry_storage in 1-1 correspondence with the edgels.
class mw_sel_geometry_tool : public dbdet_sel_explorer_tool {
public:
  //: Constructor
  mw_sel_geometry_tool();
 
  //: Destructor
  ~mw_sel_geometry_tool() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  void activate ();
  void deactivate ();

  void 
  print_ground_truth_info_at_edgel();

protected:
  dbdif_3rd_order_geometry_storage_sptr gt_;
};


#endif // mw_sel_geometry_tool_h
