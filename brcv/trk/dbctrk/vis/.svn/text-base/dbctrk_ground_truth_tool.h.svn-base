#ifndef dbctrk_ground_truth_tool_
#define dbctrk_ground_truth_tool_


//:
// \file
// \brief tool for loading ground truth for "dbctrk" storage class 
// \author Vishal Jain  , (vj@lems.brown.edu)
// \date 09/08/2004
//
// \verbatim
//  Modifications
// \endverbatim

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

class dbctrk_ground_truth_tool : public bvis1_tool
{
public:
  dbctrk_ground_truth_tool();
  ~dbctrk_ground_truth_tool();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  virtual vcl_string name() const;



  //: handle events
  bool handle( const vgui_event & e, const bvis1_view_tableau_sptr& view ) ;

  
  bool load_curve_ids(vcl_string filename,vcl_vector<int> & ids);


protected:

  bgui_vsol2D_tableau_sptr tableau();
  dbctrk_storage_sptr storage();

  bgui_vsol2D_tableau_sptr tableau_;
  dbctrk_storage_sptr storage_;

  vcl_vector<bgui_vsol_soview2D_edgel_curve*> dcs_;


};

bool readcorrespondence(vcl_string filename,vcl_vector<vcl_pair< vcl_vector<int>,vcl_vector<int>  > > & corr);
vcl_vector<vcl_string> parseLineForNumbers(vcl_string s);

#endif

