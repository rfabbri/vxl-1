#ifndef poly_lidar_observer_h_
#define poly_lidar_observer_h_

#include "poly_observer.h"
#include "obj_observable.h"
#include <vcl_map.h>
//#include <vbl/vbl_smart_ptr.h>

#include <vgui/vgui_observer.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_image_tableau.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>

#include <vnl/vnl_matrix.h>
#include <bgeo/bgeo_lvcs.h>
#include <vil/vil_image_view.h>
#include <vil/file_formats/vil_geotiff_header.h>

class poly_lidar_observer: public poly_observer//public vgui_observer, public vgui_easy2D_tableau 
{
public:
  //constructors & destructor
  //poly_lidar_observer(const char* n="unnamed") {}

  poly_lidar_observer(bgui_image_tableau_sptr const& img, const char* n="unnamed");

  virtual ~poly_lidar_observer() {}

  //: Handle all events sent to this tableau.
  // bool handle(const vgui_event& e)
  // {return vgui_easy2D_tableau::handle(e);}

  //  void update(){};
  //  void update(vgui_message const& msg){};


  //: Special case of backprojecting onto the projection plane
  void backproj_poly(vsol_polygon_2d_sptr poly2d, 
    vsol_polygon_3d_sptr& poly3d);

  void proj_poly(vsol_polygon_3d_sptr poly3d, 
    vsol_polygon_2d_sptr& poly2d);

  void backproj_point(vsol_point_2d_sptr p2d,  vsol_point_3d_sptr &p3d);
  void set_lvcs(double lat, double lon, double elev);

  void set_ground_plane(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

  float ground_elev() { return ground_elev_; }
  bool get_point_cloud(const float x1, const float y1, const float x2, const float y2, bgeo_lvcs& lvcs, vcl_vector<vsol_point_3d_sptr>& points);
  bool save_meshed_point_cloud(const float x1, const float y1, const float x2, const float y2, vcl_string file);


private:
  vbl_smart_ptr<bgeo_lvcs> lvcs_;
  vnl_matrix<double> trans_matrix_;
  vcl_vector<vcl_vector<double> > tiepoints;
  vil_image_resource_sptr img_res_;
  vil_image_view<float> img_view_;
  float ground_elev_;
  double scale_;
  void set_trans_matrix(vil_geotiff_header* gtif, 
    vcl_vector<vcl_vector<double> > tiepoints);
  void proj_point(vsol_point_3d_sptr p3d,  vsol_point_2d_sptr& p2d);
  void img_to_wgs(const unsigned i, const unsigned j, double& lon, double& lat);

};

#endif
