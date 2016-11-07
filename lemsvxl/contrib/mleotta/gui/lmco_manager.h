// This is contrib/mleotta/gui/lmco_manager.h
#ifndef lmco_manager_h_
#define lmco_manager_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief Manager Lockheed Martin tests
// \author
//   Matt Leotta
//
// \verbatim
//  Modifications:
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_soview2D.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <imesh/imesh_mesh.h>
#include <bgui/bgui_image_tableau_sptr.h>
#include <bgui3d/bgui3d_project2d_tableau_sptr.h>
#include <bgui3d/bgui3d_examiner_tableau_sptr.h>

#include <Inventor/nodes/SoMaterial.h>

class vgui_window;
class vgui_soview;
class SoNode;
class SoTransform;

//: A manager for the gui
class lmco_manager : public vgui_wrapper_tableau
{
 public:
  lmco_manager();
 ~lmco_manager();
  static lmco_manager *instance();

  //: methods for menu callbacks
  void quit();
  void load_image();
  void load_camera();
  void load_mesh();
  
  void set_scale();
  void set_style();
  void adjust_view();
  
  double compute_mi();
  void optimize_position();
  void set_options();
  
  //: compute mutual info of the data using the active method
  double compute_mi(const vcl_vector<vcl_vector<double> >& d1_ir,
                    const vcl_vector<vcl_vector<double> >& d2_ir,
                    const vcl_vector<vcl_vector<double> >& d1_eo,
                    const vcl_vector<vcl_vector<double> >& d2_eo);
  
  enum mi_method {EO, IR, SUM, POLAR};


  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  void init();
  
  unsigned int active_view() const;

  //: the virtual handle function
  virtual bool handle(const vgui_event&);

  class view{
    public:
      view();
   
      enum display_img_t {ORIG, EO_INT, EO_ORT, EO_MAG, IR_INT, IR_ORT, IR_MAG};

      void set_pos(double x, double y);
      void set_scale(double s);
      void set_angle(double a);

      double pos_x() const { return mesh_pos_x_;}
      double pos_y() const { return mesh_pos_y_;}
      double angle() const { return mesh_angle_;}
      double scale() const { return mesh_scale_;}
      
      display_img_t display_mode() const { return display_mode_;}
      double data_scale() const { return data_scale_; }
      void set_display_mode(display_img_t mode);

      void image_to_ground(double ix, double iy, double& gx, double& gy) const;
      
      void set_camera(const vpgl_perspective_camera<double>& cam);
      void set_mesh(const imesh_mesh& mesh);
      void set_image(const vil_image_resource_sptr& img, double scale, bool rgb_to_eo);
      
      vpgl_perspective_camera<double> camera(bool use_scale = true) const;
      vil_image_resource_sptr image() const { return img_; }
      
      const vil_image_view<float>& ir_data() const { return ir_data_; }
      const vil_image_view<float>& eo_data() const { return eo_data_; }
      
      vgl_h_matrix_3d<double> homography() const;
      
      void set_style(double r, double g, double b, double a);
      void get_style(double& r, double& g, double& b, double& a);
      
      vgui_tableau_sptr tableau_root();
      bgui_image_tableau_sptr image_tableau() const { return itab_; } 
      bgui3d_project2d_tableau_sptr project2d_tableau() const { return ptab_; } 
      
      SoNode* build_SoNode(const imesh_mesh& mesh);
      
    private:
      //: compute gradient orientation (plane 1) and magnitude (plane 2)
      // from intensity data in plane 0
      void compute_data_image(vil_image_view<float>& data_img);  
        
      vil_image_resource_sptr img_;
      vpgl_perspective_camera<double> cam_;
      
      vil_image_view<float> ir_data_;
      vil_image_view<float> eo_data_;
      double data_scale_;
      
      display_img_t display_mode_;

      double mesh_scale_;
      double mesh_pos_x_;
      double mesh_pos_y_;
      double mesh_angle_;
      vnl_double_3x3 img_to_gnd_;

      SoTransform * mesh_xform_;
      SoMaterial * material_;

      bgui_image_tableau_sptr itab_;
      bgui3d_project2d_tableau_sptr ptab_;
  };


  static vgui_range_map_params_sptr range_params(vil_image_view_base_sptr const& image);
  
  
  vgui_soview2D_image* build_texture_image(const view* const v, unsigned mode) const;

 private:
  vgui_grid_tableau_sptr grid_;
  
  mi_method active_method_;
  bool use_gradients_;

  imesh_mesh mesh_;

  // views of the data
  view v[2];

  vgui_window* win_;
  static lmco_manager *instance_;


};

#endif // lmco_manager_h_
