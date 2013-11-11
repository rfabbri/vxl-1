// This is contrib/bwm_lidar/gui/bwm_lidar_manager.h
#ifndef bwm_lidar_manager_h_
#define bwm_lidar_manager_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief Manager for LIDAR processing GUI
// \author
//   Matt Leotta
//
// \verbatim
//  Modifications:
// \endverbatim
//-----------------------------------------------------------------------------

#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vil/vil_image_view.h>

#include <bmdl/bmdl_classify.h>
#include <bgui/bgui_image_tableau_sptr.h>
#include <imesh/imesh_mesh.h>

//: A manager for the gui
class bwm_lidar_manager : public vgui_wrapper_tableau
{
 public:
  bwm_lidar_manager();
  virtual ~bwm_lidar_manager();
  static bwm_lidar_manager *instance();

  //: methods for menu callbacks
  void quit();
  void load_images();
  void save_images();
  void load_flimap();
  void save_mesh();
  void label_lidar();
  void segment_lidar();
  void cluster_buildings();
  void refine_buildings();
  void construct_mesh();
  
  //: initialize the manager
  void init();

  //: the virtual handle function
  virtual bool handle(const vgui_event&);


 private:
  static bwm_lidar_manager *instance_;

  bmdl_classify<double> classifier_;
  vcl_vector<double> building_heights_;
  vcl_vector<unsigned int> building_sizes_;
  vil_image_view<vxl_byte> color_img_;
  
  imesh_mesh mesh_;

  bgui_image_tableau_sptr first_return_tab_;
  bgui_image_tableau_sptr last_return_tab_;
  bgui_image_tableau_sptr color_img_tab_;
  bgui_image_tableau_sptr labels_img_tab_;
  vgui_easy2D_tableau_sptr boundaries_tab_;
};

#endif // bwm_lidar_manager_h_
