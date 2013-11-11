#ifndef bcri_calib_h_
#define bcri_calib_h_
//:
// \file
// \brief Worldcam...
// \author Dongjin Han 12-11-04



#include <vcl_string.h>
#include <vcl_cstdlib.h> // for vcl_exit()

#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_easy3D_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_composite_tableau_sptr.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h>
#include <vgui/internals/vgui_accelerate.h>

#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_picker_tableau.h>
#include <bgui/bgui_picker_tableau_sptr.h>

#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <bgui/bgui_picker_tableau_sptr.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>


#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_find.h>

#include <vgui/vgui_style.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_soview3D.h> // for vgui_lineseg3D
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_composite_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_point_3d.h>

#include <vgui/vgui_soview2D.h>
#include <bmvl/brct/kalman_filter.h>

#include <vnl/vnl_double_2.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>


#include <vnl/vnl_double_4x4.h>

#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>

#include <brip/brip_vil1_float_ops.h>







#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_string.h>
#include <vcl_ostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h> //vcl_sprintf//

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_find.h>

#include <vgui/vgui_style.h>


#include <vnl/vnl_math.h> // for pi
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_vector.h>



#include <brct/brct_algos.h>  //back projection
#include <mvl/HomgPoint2D.h>
#include <mvl/HMatrix2DCompute4Point.h>
#include <mvl/HMatrix2D.h>
#define VERTEX vgl_homg_point_3d<double>

class vgui_window;
class bcri_calib : public vgui_wrapper_tableau
{
 public: 

  bcri_calib();
  ~bcri_calib();
  static bcri_calib *instance();
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  
  
  // methods for run menu callbacks
  void creat_line();
  void quit();
  void load_image();
  void solve(); 
        void solve_reverse(); 
        void solve_robust(); 
        void solve_quiet();
        void solve_all(); 
        void change();
  
  void read_3d_real_points();
  void read_3d_real_points_reverse();
  void init();
  
  void find_epipole();
  void select_correspond();
        void select_correspond_all();
        void select_pts1_pts2();
  // void show_epipole();
  //void show_epipolar_line();
  
  void clear_display();
  void clear_image();
  void back_project();
    void back_project_reverse();
 
  
  //: the virtual handle function
  virtual bool handle(const vgui_event&);
  
 protected:
    
    //: it clean the memory allocated by init. it should be called by quit()
    void clean_up();
    bgui_vtol2D_tableau_sptr get_vtol2D_tableau_at(unsigned col, unsigned row);
    bgui_vtol2D_tableau_sptr get_selected_vtol2D_tableau();
    
    bgui_picker_tableau_sptr get_picker_tableau_at(unsigned col, unsigned row);
    bgui_picker_tableau_sptr get_selected_picker_tableau();
    
    
    vil1_image get_image_at(unsigned col, unsigned row);
    
 private:
   
    bgui_picker_tableau_sptr tab_picker_;
    vgui_composite_tableau_sptr tab_cps_;
    
    
    //vcl_vector <vgui_image_tableau_sptr> img_2d_;
    //vcl_vector <vgui_easy2D_tableau_sptr> easy_2d_;
   
    vcl_vector<bgui_vtol2D_tableau_sptr> vtol_tabs_;
    vcl_vector<bgui_picker_tableau_sptr> tabs_picker_;
    
    vgui_easy3D_tableau_sptr tab_3d_;
    //vgui_rubberband_tableau_sptr tab_rubber_;
    
    vil1_image img_;
    vgui_grid_tableau_sptr grid_;
    //vcl_string data_file_name_;
    
    //: inital epipole
    vcl_vector<vgl_homg_line_2d<double> > lines_;
    vgl_point_2d<double> *e_;
    vcl_string status_info_;
   
    
    static bcri_calib *instance_;
    
    vgui_window* win_;
    vcl_vector <VERTEX> S_;
                vcl_vector <VERTEX> S_selected_;
   
    vnl_vector_fixed <double,4> x_;
  vnl_vector_fixed <double,4> y_;
  vnl_vector_fixed <double,4> ones_;
  double x_post_,y_post_;

        vcl_vector <double> xall_;
        vcl_vector <double> yall_;
        vcl_vector <double> oneall_;
  vnl_double_3x4  P_;


        int i1_,i2_,i3_,i4_,i5_;
        bool post_;
        bool road_only_;
};

#endif // bcri_calib_h_

