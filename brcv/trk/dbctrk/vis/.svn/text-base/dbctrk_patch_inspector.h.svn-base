#ifndef _dbctrk_patch_inspector_tool_
#define _dbctrk_patch_inspector_tool_



#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <dbctrk/vis/dbctrk_soview2d.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <dbctrk/pro/dbctrk_storage.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>

#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bvis1/bvis1_view_tableau_sptr.h>
#include <vnl/vnl_double_3.h>

class dbctrk_patch_inspector : public bvis1_tool
{
public:
  dbctrk_patch_inspector();
  virtual ~dbctrk_patch_inspector();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  virtual vcl_string name() const;




  //: handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  void readpatchinfo(char const * filename, vcl_string &type, vnl_double_3 & mean, vnl_double_3 &std,bool IHS);
  vnl_double_3 mean3d(vcl_vector<vnl_double_3>);
  vnl_double_3 std3d(vcl_vector<vnl_double_3>);
  int return_color(vcl_string type);
  bool doesbelong(vnl_double_3 cmean,vnl_double_3 cstd,vnl_double_3 p);
 protected:
  
  bgui_vsol2D_tableau_sptr tableau();
  dbctrk_storage_sptr storage();
  
  bgui_vsol2D_tableau_sptr tableau_;
  dbctrk_storage_sptr storage_;
  bgui_vsol2D_tableau_sptr curr_tableau_;

  vgui_style_sptr neighbor_style_;
  vgui_event_condition gesture_info_;
 
  vcl_vector<bgui_vsol_soview2D_digital_curve*> dcs_;
  dbctrk_soview2D* object_;
  dbctrk_tracker_curve_sptr curr_curve_;
  
  vcl_vector<vnl_double_3> vecmeanI;
  vcl_vector<vnl_double_3> vecstdI;
  vcl_vector<vnl_double_3> vecmeanL;
  vcl_vector<vnl_double_3> vecstdL;
  vcl_vector<vcl_string> vectype;

  vcl_map<vcl_string,vcl_vector<vnl_double_3> > maptypeL;
  vcl_map<vcl_string,vcl_vector<vnl_double_3> > maptypeI;
};



#endif

