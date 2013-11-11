// This is biotree/resc/imgr/imgrv_manager.cxx
#include <imgrv/imgrv_manager.h>
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vnl/vnl_matlab_read.h>
#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <bgui/bgui_image_tableau.h>
#include <vil/vil_image_view.h>
#include <imgr/imgr_bounded_image_view_3d.h>
#include <imgr/imgr_scan_resource.h>
#include <imgr/imgr_scan_resource_io.h>
#include <imgr/imgr_dummy_scan.h>
#include <vnl/vnl_math.h>

imgrv_manager *imgrv_manager::instance_ = 0;

imgrv_manager *imgrv_manager::instance()
{
  if (!instance_)
    {
      instance_ = new imgrv_manager();
      instance_->init();
    }
  return imgrv_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
imgrv_manager::imgrv_manager():vgui_wrapper_tableau()
{
  
}

imgrv_manager::~imgrv_manager()
{
}

//: Set up the tableaux
void imgrv_manager::init()
{
  
  dtab_ = vgui_deck_tableau_new();
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(dtab_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2D);
  this->add_child(shell);
  stop_cine_ = true;
  range_params_ = new vgui_range_map_params(10000.0, 40000.0, 1.0, false);
  range_params_->use_glPixelMap_ = true;
  range_params_->cache_mapped_pix_ = false;
}

void imgrv_manager::quit()
{
  vcl_exit(1);
}

void imgrv_manager::cine_mode()
{
 stop_cine_ = false;
  while(true )
    {
      dtab_->next();
      dtab_->post_redraw();
   vgui::run_till_idle();
    if(stop_cine_)break;
    }
}

void imgrv_manager::stop_cine_mode()
{
  stop_cine_ = true;
}

void imgrv_manager::load_view_3d()
{
  vgui_dialog load_view_3d_dlg("Load 3d View");
  static vcl_string image_filename = "dummyfile";
  static vcl_string ext = "*.*";
  load_view_3d_dlg.file("Scan Filename:", ext, image_filename);
  if (!load_view_3d_dlg.ask())
    return;

  imgr_dummy_scan scan;
  imgr_scan_resource_sptr resc = imgr_scan_resource_io::read_resource(scan);
  double minp[3], maxp[3];
  minp[0]=-1.0;   minp[1]=-1.0; minp[2]=-1.0;
  maxp[0]= 1.0;   maxp[1]= 1.0; maxp[2]= 1.0;
  vgl_box_3d<double> box(minp, maxp);
  imgr_image_view_3d_base_sptr v3d_ptr = resc->get_bounded_view(box);
  imgr_bounded_image_view_3d<unsigned short> v3d = v3d_ptr;
  unsigned nk = v3d.nk();
  for(unsigned k = 0; k<nk; ++k)
    {
      vil_image_view<unsigned short> v = (v3d.view_2d(k));
      bgui_image_tableau_sptr itab = bgui_image_tableau_new(v, range_params_);
      dtab_->add(itab);
    }
}

void imgrv_manager::set_range_params()
{
 
  static double min = 10000, max = 40000;
  static float gamma = 1.0;
  static bool invert = false;
  vgui_dialog range_dlg("Set Range Map Params");
  range_dlg.field("Range min:", min);
  range_dlg.field("Range max:", max);
  range_dlg.field("Gamma:", gamma);
  range_dlg.checkbox("Invert:", invert);
  if (!range_dlg.ask())
    return;

  range_params_ = new vgui_range_map_params(min, max, gamma, invert);
  range_params_->use_glPixelMap_ = true;
  range_params_->cache_mapped_pix_ = false;
}
