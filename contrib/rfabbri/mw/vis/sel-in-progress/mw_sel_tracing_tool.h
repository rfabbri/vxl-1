// This is mw_sel_tracing_tool.h
#ifndef mw_sel_tracing_tool_h
#define mw_sel_tracing_tool_h
//:
//\file
//\brief Tool for exploring multiview correspondences between edge linking
// hypotheses
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 07/19/2005 12:09:14 AM EDT
//

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>

#include <dbdet/pro/dbdet_sel_storage.h>


class dbdet_edgel;

class mw_sel_tracing_tool : public bvis1_tool {
public:
  //: Constructor
  mw_sel_tracing_tool();
 
  //: Destructor
  virtual ~mw_sel_tracing_tool() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& /*tableau*/ );
  
  void activate ();
  void deactivate ();

protected:

  vgui_event_condition gesture0_;
  vcl_vector<int> frame_v_;   //: frame number for each view
  const unsigned nviews_;
  dbdet_sel_storage_sptr sel_storage_;
  dbdet_sel_sptr sel_;
  

  vcl_vector<bgui_vsol2D_tableau_sptr> tab_; //:< tableaux used to draw in each view 

  vgui_soview2D_point *p0_; //:< edgel position in left curve segment
  vgui_style_sptr p0_style_;

private: 
  /*
  void print_sel_stats() const;
  void print_edgel_stats(dbdet_edgel * e) const;
  */
};


#endif // mw_sel_tracing_tool_h
