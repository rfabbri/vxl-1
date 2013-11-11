// This is mw_correspond_point_tool_band.h
#ifndef mw_correspond_point_tool_band_h
#define mw_correspond_point_tool_band_h
//:
//\file
//\brief Tool for exploring multiview correspondences between edgels using epipolar error bands
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Mon Apr 24 22:36:01 EDT 2006
//

#include <mw/vis/mw_correspond_point_tool_basic.h>

class dbecl_epiband;
class dbecl_grid_cover_window;
class vgui_dialog;

class mw_correspond_point_tool_band : public mw_correspond_point_tool_basic {
public:
  //: Constructor
  mw_correspond_point_tool_band();
 
  //: Destructor
  virtual ~mw_correspond_point_tool_band() {}
  
  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Handle events.
  virtual bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  virtual void activate ();
  virtual void deactivate ();

protected:
  vcl_vector<vsol_box_2d_sptr> bbox_;  //:< bounding boxes for all vsols
  
  vcl_vector<dbecl_grid_cover_window*> w_;

  vcl_vector< vcl_vector<dbecl_epiband *> > epband_;
  vcl_vector< vcl_vector<bgui_vsol_soview2D_polygon *> > epband_soview_;

  vcl_vector< vcl_list<vgui_soview2D_point *> > covering_soviews_; 

  vcl_vector<vgui_style_sptr> epband_style_;
  
  vcl_vector<vcl_vector <vpgl_fundamental_matrix<double> > > fm_;

  double err_pos_;

  vgui_style_sptr covering_pt_style_;
  bool display_epiband_covering_;

  virtual bool handle_key(vgui_key key);
  virtual bool handle_mouse_event_whatever_view( 
      const vgui_event & e, 
      const bvis1_view_tableau_sptr& view );

  void 
  update_display_for_epipolar_bands( bool redraw );

  static bool 
  get_params(double *err_pos);

  static void 
  add_params(vgui_dialog *d,double *err_pos);
};


#endif // mw_correspond_point_tool_band_h
