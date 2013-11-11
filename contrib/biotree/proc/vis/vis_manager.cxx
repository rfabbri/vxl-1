#include "vis_manager.h"
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstring.h>
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
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <bgui/bgui_picker_tableau.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <imgr/file_formats/imgr_rsq.h>

vis_manager *vis_manager::instance_ = 0;

vis_manager *vis_manager::instance()
{
  if (!instance_)
  {
    instance_ = new vis_manager();
    instance_->init();
  }
  return vis_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
vis_manager::vis_manager():vgui_wrapper_tableau()
{
  isq_ = 0;
}

vis_manager::~vis_manager()
{
  if(isq_)
    delete isq_;
}


//: Set up the tableaux
void vis_manager::init()
{
  dtab_ = vgui_deck_tableau_new();
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(dtab_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2D);
  this->add_child(shell);
  stop_cine_ = true;
  range_params_ = new vgui_range_map_params(10000.0, 40000.0, 1.0, false);
  range_params_->use_glPixelMap_ = true;
  range_params_->cache_mapped_pix_ = true;
}

void vis_manager::quit()
{
  vcl_exit(1);
}

void vis_manager::cine_mode()
{
  stop_cine_ = false;
  while(true )
  {
    dtab_->next();
    dtab_->post_redraw();
    vgui::run_till_idle();
    if(stop_cine_)
      break;
    // fake event needed to trigger the handle so that the status 
    // bar information is updated properly
    vgui_event fake_event;
    fake_event.type = vgui_MOTION;
    fake_event.wx = wx_;
    fake_event.wy = wy_;
    this->handle(fake_event);
  }
}


void vis_manager::add_box()
{
  vgui_tableau_sptr tptr = dtab_->current();

  vgui_tableau_sptr child = tptr ->get_child(0);

  assert(child->type_name() == "bgui_picker_tableau");
  
  if(child != 0)
  {
    bgui_picker_tableau* p =(bgui_picker_tableau*) child.operator->();
    p->pick_box(&x1_, &y1_, &x2_, &y2_);
  }
}

void vis_manager::save_box()
{
  int index = dtab_->index();
  vgui_tableau_sptr tptr = dtab_->current();
  vgui_tableau_sptr child = tptr->get_child(0);
  assert(child->type_name() == "bgui_picker_tableau");
  
  if(child != 0)
  {
    vgui_tableau_sptr easy = child->get_child(0);
    assert(easy->type_name() == "vgui_easy2D_tableau");
    vgui_easy2D_tableau* p =(vgui_easy2D_tableau*) easy.operator->();

    float x[4], y[4];
    x[0]=x[3] = x1_; x[1]=x[2]=x2_;
    y[0]=y[1] = y1_; y[2]=y[3]=y2_;

    corners_.push_back(vgl_point_3d<double>(x1_, y1_, index));
    corners_.push_back(vgl_point_3d<double>(x2_, y2_, index));

    int size = corners_.size();
    vcl_cout << corners_[size-1] << " and " << corners_[size-2] << "\n";
    p->add_polygon(4, x, y);

    if(file_type_ == 3) 
    { // isq file
      vgl_point_3d<double> pt = isq_->world_point((double)x1_, (double)y1_, (double)index); 
      bounding_box_.add(pt);
      pt = isq_->world_point(x2_, y2_, index);
      bounding_box_.add(pt);
      vcl_cout << bounding_box_ << "\n";
    }
  }
  vcl_ofstream file("box.saved");
  file << bounding_box_;
  dtab_->post_redraw();
}

void vis_manager::stop_cine_mode()
{
  stop_cine_ = true;
}

void vis_manager::load_view_3d()
{
  vgui_dialog load_view_3d_dlg("Load RSQ file");
  static vcl_string image_filename = "/mnt/backup/data/scanco0405/c0000892.rsq";
  static vcl_string ext = "*.*";
  load_view_3d_dlg.file("Scan Filename:", ext, image_filename);

  if (!load_view_3d_dlg.ask())
    return;

  vil_stream* is = new vil_stream_fstream(image_filename.c_str(), "r");

  is->ref();

  //: find file type
  char check[256];

  is->read(check, 16);

  if(vcl_strncmp(check, "CTDATA-HEADER_V1", 16)==0)
  {
    vxl_byte bytes[4];
    is->read(bytes, 4);

#if VXL_LITTLE_ENDIAN
    file_type_ =  ((int)bytes[0]) | ((int)bytes[1])<<8 | ((int)bytes[2])<<16 | ((int)bytes[3])<<24;
#else
    file_type_ =  ((int)bytes[3]) | ((int)bytes[2])<<8 | ((int)bytes[1])<<16 | ((int)bytes[0])<<24;
#endif

    vcl_cout << image_filename << '\n';
    vcl_vector<vil_image_resource_sptr> img_res_sptrs;
    double min, max; 

    if(file_type_ == 1)
    { // read rsq file
      vcl_cout << "we are in rsq\n";
      imgr_rsq rsq_reader(is);

      img_res_sptrs = rsq_reader.get_images();

      min = rsq_reader.min_intensity();
      max = rsq_reader.max_intensity();
    }
    else
    {
      vcl_cout << "we are in isq\n";
      isq_ = new imgr_isq_file_format(is);

      img_res_sptrs = isq_->get_images();

      min = isq_->min_intensity();
      max = isq_->max_intensity();
    }
    range_params_ = new vgui_range_map_params(min, max, 1.0, false);
   // unsigned nk = img_res_sptrs.size();
    unsigned nk = 100;
    img_tabs_.clear();

    for(unsigned k = 0; k<nk; ++k)
    {
      vcl_cout <<"enter the " << k <<"th resource "<< vcl_flush;
      vil_image_view<unsigned short> v(img_res_sptrs[k]->get_view());
      vcl_cout << "open view [ " << k <<"] " << img_res_sptrs[k]->ni() << ' ' << v.nj() << '\n';
      vgui_image_tableau_sptr itab = vgui_image_tableau_new(v, range_params_);
#if 0
      vgui_easy2D_tableau_new easy(itab);

      bgui_picker_tableau_new picker(easy);

      vgui_viewer2D_tableau_new viewer(picker);

      img_tabs_.push_back(viewer);
#endif
      img_tabs_.push_back(itab);
      dtab_->add(itab);
    }
  }
  else
    vcl_cerr << " not a isq or rsq file \n ";

  is->unref();
}

void vis_manager::set_range_params()
{
  static double min = 10000, max = 100000;
  static float gamma = 1.0;
  static bool invert = false;
  vgui_dialog range_dlg("Set Range Map Params");

  if(range_params_ != 0)
  {
    min = range_params_->min_L_;
    max = range_params_->max_L_;
  }

  range_dlg.field("Range min:", min);
  range_dlg.field("Range max:", max);
  range_dlg.field("Gamma:", gamma);
  range_dlg.checkbox("Invert:", invert);
  if (!range_dlg.ask())
    return;

  range_params_->min_L_ = min;
  range_params_->max_L_ = max;
  range_params_->invert_ = invert;
  range_params_->use_glPixelMap_ = true;
  range_params_->cache_mapped_pix_ = false;

  dtab_->post_redraw(); 
}

void vis_manager::get_pixel_info(const int x, const int y,vgui_event const &e, char* msg)
{
  int size = img_tabs_.size();
  if(size != 0)
  {
    int index = dtab_->index();
    vgui_image_tableau_sptr itab = img_tabs_[index];
    vil_image_view_base_sptr v(itab->get_image_view());
    vil_image_view<unsigned short> vim(*v);
    if(x < 0 || x >= static_cast<int>(vim.ni()) || y < 0 || y >= static_cast<int>(vim.nj()))
      vcl_sprintf(msg, "pixel:(%d, %d) value:%d view:%d", x, y, -1, index);
    else
      vcl_sprintf(msg, "pixel:(%d, %d) value:%d view:%d", x, y, vim(x,y), index);
  }
  else
    vcl_sprintf(msg, "No images loaded...");

  return;
}

// Handle all events for this tableau.
bool vis_manager::handle(vgui_event const &e)
{
  static bool button_down = false;
  if (e.type == vgui_BUTTON_DOWN)
    button_down = true;
  else if (e.type == vgui_BUTTON_UP)
    button_down = false;
  else if (e.type == vgui_MOTION && button_down == false)
  {
    // update these only if there is motion event
    wx_ = e.wx;
    wy_ = e.wy;
    // Get X,Y position to display on status bar:
    float pointx, pointy;
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
    int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);
    char msg[100];

    this->get_pixel_info(intx, inty,e, msg);

    // Display on status bar:
    vgui::out << msg << vcl_endl;
  }
  else if(e.type == vgui_KEY_UP)
  {
    // Get X,Y position to display on status bar:
    float pointx, pointy;
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(wx_, wy_, pointx, pointy);
    int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);
    char msg[100];

    this->get_pixel_info(intx, inty,e, msg);

    // Display on status bar:
    vgui::out << msg << vcl_endl;
  }
  return base::handle(e);
}
