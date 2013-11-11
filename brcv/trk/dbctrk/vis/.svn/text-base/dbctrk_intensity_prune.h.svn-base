#ifndef dbctrk_intensity_prune_tool_
#define dbctrk_intensity_prune_tool_
#include "engine.h"
#include "matrix.h"


//:
// \file
// \brief tool for "dbctrk" storage class to visualize the intensity attribute
// \author Vishal Jain , (vj@lems.brown.edu)
// \date 07/21/2004
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <dbctrk/vis/dbctrk_soview2D.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <dbctrk/pro/dbctrk_storage.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>

#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>

#include <dbctrk/dbctrk_curve_clustering.h>
#include <bvis1/bvis1_view_tableau_sptr.h>
#include <vbl/vbl_bounding_box.h>
#include <vbl/vbl_array_3d.h>
class dbctrk_intensity_prune_tool : public bvis1_tool
{
public:
  dbctrk_intensity_prune_tool();
  virtual ~dbctrk_intensity_prune_tool();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  virtual vcl_string name() const;



  //: handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );
  bool disjoint(vbl_bounding_box<double,3> const &a,
    vbl_bounding_box<double,3> const &b);
  bool norm3dhist(vcl_vector<double> dim1,vcl_vector<double> dim2,vcl_vector<double> dim3, vbl_array_3d<double> & hist, int numbin);
  double dist3pdf(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int numbins);
  bool norm2dpolar(vcl_vector<double> & dim1,vcl_vector<double> & dim2,vcl_vector<double> & dim3, vbl_array_2d<double> &polarhist,int thetabins,int rbins,double r1);
  bool hist2img(vbl_array_2d<double> hist, int thetabins,int satbins, double r1, vil_image_view<vil_rgb<vxl_byte> > &polarimg, int offx, int offy,float h,float s);
  bool displayIHSchart(vil_image_view<vil_rgb<vxl_byte> > & histimg,int radius);
  double dist3pdf_bhat(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int xbins,int ybins,int zbins);
  double dist3pdf_chi_square(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int xbins,int ybins,int zbins);
  bool norm3dpolar(vcl_vector<double> & dim1,vcl_vector<double> & dim2,vcl_vector<double> & dim3, vbl_array_3d<double> &polarhist,int thetabins,int rbins,int valuebins,double r1,double v1);
  double dist3pdf_poly(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int xbins,int ybins,int zbins,double tau,int n);
  double dist3pdf_KL(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int xbins,int ybins,int zbins);
  bool buildprior(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2,vbl_array_2d<double> pxy);
protected:

  bgui_vsol2D_tableau_sptr tableau();
  dbctrk_storage_sptr storage();

  bgui_vsol2D_tableau_sptr tableau_;
  dbctrk_storage_sptr storage_;


  vgui_style_sptr neighbor_style_;
  vgui_event_condition gesture_info_;
  vcl_vector<dbctrk_tracker_curve_sptr> tc1,tc2;
  vcl_vector<bgui_vsol_soview2D_digital_curve*> dcs_;
  dbctrk_soview2D* object_;
  bgui_vsol2D_tableau_sptr curr_tableau_;
  dbctrk_tracker_curve_sptr curr_curve_;
  void prepare_vector(char matlab_name[], double data[], int dim1,Engine *ep) ;
  double motion;
  int thetabins;
  double r1;
  int satbins;

  Engine *ep;
};

vcl_vector<int> find_matching_ids(int query,vcl_vector<vcl_pair<vcl_vector<int>,vcl_vector<int> > > gmatches);

#endif
