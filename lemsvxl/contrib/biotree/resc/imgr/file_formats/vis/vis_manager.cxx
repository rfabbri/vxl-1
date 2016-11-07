#include "vis_manager.h"
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
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <vil/vil_pixel_format.h>
#include <imgr/file_formats/imgr_rsq.h>
#include <imgr/file_formats/imgr_isq_file_format.h>

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

}

vis_manager::~vis_manager()
{
}

//: Set up the tableaux
void vis_manager::init()
{

  dtab_ = vgui_deck_tableau_new();
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(dtab_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2D);
  this->add_child(shell);
  stop_cine_ = true;
  range_params_ = new vgui_range_map_params(10000.0, 50000.0, 1.0, false);
  range_params_->use_glPixelMap_ = true;
  range_params_->cache_mapped_pix_ = false;
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
    if(stop_cine_)break;
    // fake event needed to trigger the handle so that the status 
    // bar information is updated properly
    vgui_event fake_event;
    fake_event.type = vgui_MOTION;
    fake_event.wx = wx_;
    fake_event.wy = wy_;
    this->handle(fake_event);
  }
}




void vis_manager::stop_cine_mode()
{
  stop_cine_ = true;
}

void vis_manager::load_cali_bnd_views()
{
  vgui_dialog load_view_3d_dlg("Load RSQ file");
  static vcl_string image_filename = "/mnt/backup/data/scanco0405/c0000892.rsq";
  static vcl_string ext = "*.*";
  load_view_3d_dlg.file("Scan Filename:", ext, image_filename);

  if (!load_view_3d_dlg.ask())
    return;

  vil_stream* is = new vil_stream_fstream(image_filename.c_str(), "r");

  is->ref();

  vcl_cout << image_filename << '\n';

  double min = 0, max = 65536; 

  imgr_rsq rsq_reader(is);

  vgl_box_2d<double> bounds(vgl_point_2d<double>(1024, 54), vgl_point_2d<double>(2047, 107));

  vcl_vector<dbil_bounded_image_view<double> *> views = rsq_reader.get_cali_bnded_view(bounds);

  range_params_ = new vgui_range_map_params(min, max, 1.0, false);

  unsigned nk = views.size();

  img_tabs_.clear();
 // unsigned nk = 5;

  for(unsigned k = 0; k<nk; ++k)
  {
    dbil_bounded_image_view<double> * v = views[k];
    vcl_cout << "open view [ " << k <<"] " << v->ni() << ' ' << v->nj() << '\n';
    vgui_image_tableau_sptr itab = vgui_image_tableau_new(*v, range_params_);
    img_tabs_.push_back(itab);
    dtab_->add(itab);
  }

  is->unref();
  vcl_cout << img_tabs_.size() << vcl_endl;
}

void vis_manager::load_view_3d()
{
  vgui_dialog load_view_3d_dlg("Load RSQ file");
  static vcl_string image_filename = "/mnt/backup/data/scanco0405/c0000892.rsq";
  static vcl_string ext = "*.*";
  load_view_3d_dlg.file("Scan Filename:", ext, image_filename);

  // file type
  load_view_3d_dlg.choice("file type", "rsq", "isq", file_type_);

  if (!load_view_3d_dlg.ask())
    return;

  vil_stream* is = new vil_stream_fstream(image_filename.c_str(), "r");

  is->ref();

  vcl_cout << image_filename << '\n';

  vcl_vector<vil_image_resource_sptr> img_res_sptrs;
  double min, max; 

  if(file_type_ == 0){ // read rsq file

    vcl_cout << "we are in rsq\n";
    imgr_rsq rsq_reader(is);

    img_res_sptrs = rsq_reader.get_images();
    scan_ = rsq_reader.get_scan();
    min = rsq_reader.min_intensity();
    max = rsq_reader.max_intensity();
    min = 45000;
    max = 65535;


    scan_ = rsq_reader.get_scan();

  }else{

    vcl_cout << "we are in isq\n";
    imgr_isq_file_format isq_reader(is);

    img_res_sptrs = isq_reader.get_images();
    min = isq_reader.min_intensity();
    max = isq_reader.max_intensity();


  }

  range_params_ = new vgui_range_map_params(min, max, 1.0, false);

  unsigned nk = img_res_sptrs.size();
  vcl_cout << nk << vcl_endl;

  img_tabs_.clear();

  for(unsigned k = 0; k<nk; ++k)
  {
    vcl_cout <<"enter the " << k <<"th resource "<< vcl_flush;
    unsigned ni = img_res_sptrs[k]->ni() / 2;
    unsigned nj = img_res_sptrs[k]->nj() / 2;
    vil_image_view<unsigned short> v(img_res_sptrs[k]->get_copy_view(ni, ni, nj, nj));
    dbil_bounded_image_view<unsigned short> bv(v, ni, nj, 2*ni, 2*nj);
    vcl_cout << "open view [ " << k <<"] " << v.ni() << ' ' << v.nj() << '\n';
    vgui_image_tableau_sptr itab = vgui_image_tableau_new(bv, range_params_);
    img_tabs_.push_back(itab);
    dtab_->add(itab);
  }

  is->unref();
  vcl_cout << img_tabs_.size() << vcl_endl;
}


void vis_manager::save_scan()
{
  vgui_dialog save_scan_dlg("save scan file");
  static vcl_string filename = "scan.scn";
  static vcl_string ext = "*.scn";
  save_scan_dlg.file("Scan Filename:", ext, filename);

  if ( ! save_scan_dlg.ask())
    return;

  vcl_cout << filename << "\n";
  vcl_ofstream fout(filename.c_str() );

  fout << scan_;
}

void vis_manager::set_range_params()
{

  static double min = 10000, max = 40000;
  static float gamma = 1.0;
  static bool invert = false;
  vgui_dialog range_dlg("Set Range Map Params");

  if(range_params_ != 0){
    min = range_params_->min_L_;
    max = range_params_->max_L_;
  }

  range_dlg.field("Range min:", min);
  range_dlg.field("Range max:", max);
  range_dlg.field("Gamma:", gamma);
  range_dlg.checkbox("Invert:", invert);
  if (!range_dlg.ask())
    return;

  range_params_ = new vgui_range_map_params(min, max, gamma, invert);
  range_params_->use_glPixelMap_ = true;
  range_params_->cache_mapped_pix_ = false;

  for(unsigned i = 0; i < img_tabs_.size() ; i++)
  {
    img_tabs_[i]->set_mapping(range_params_);
  }
}

void vis_manager::box_projection()
{
  if(img_tabs_.size() == 0)
    vcl_cout << "No images loaded yet!" << vcl_endl;
  else
  {
    vgui_dialog projection_dlg("Box Files");
    static vcl_string box_filename = "*.bx3";
    static vcl_string ext = "*.*";
    projection_dlg.file("Box Filename:", ext, box_filename);
    if (!projection_dlg.ask())
      return;

    // error checks for file extensions
    int size = box_filename.size();
    if(box_filename[size-1] != '3' || box_filename[size-2] != 'x' ||
      box_filename[size-3] != 'b' || box_filename[size-4] != '.')
    {
      vcl_cerr << "Box file does not have a valid extension. Aborting process..." << vcl_endl;
      return;
    }

    // get the box
    vcl_ifstream box_file(box_filename.c_str());
    vgl_box_3d<double> box3;
    box3.read(box_file);
    box_file.close();
    vcl_cout << box3 << vcl_endl;
    
    // error check
    if(img_tabs_.size() != scan_.n_views())
    {
      vcl_cout << "Number of views in scan is not the same as the number of slices in rsq file. Aborting process..." << vcl_endl;
      return;
    }

    // compute the projection of the box onto each splat
    unsigned nk = img_tabs_.size();
    for(unsigned k=0; k<nk; k++)
    {
      vgl_box_2d<double> box2;
      xmvg_perspective_camera<double> cam(scan_(k));
      box2 = vpgl_project::project_bounding_box(cam, box3);
      double cx = box2.centroid_x() - 1024;
      double cy = box2.centroid_y() - 54;
      box2.set_centroid_x(cx);  
      box2.set_centroid_y(cy);
      vcl_cout << box2 << vcl_endl;
      img_box_.push_back(box2);
    }

    //visualize it
    for(unsigned k = 0; k<nk; ++k)
      dtab_->remove(dtab_->current());
    for(unsigned k = 0; k<nk; ++k)
    {
      int shift_index = (k) % (nk);
      vgl_point_2d<double> min = img_box_[shift_index].min_point();
      vgl_point_2d<double> max = img_box_[shift_index].max_point();
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
      double w = img_box_[shift_index].width();
      double h = img_box_[shift_index].height();
#endif
      vgl_point_2d<double> centre(img_box_[shift_index].centroid());
      float xs[4] = {min.x(), max.x(), max.x(), min.x()};
      float ys[4] = {min.y(), min.y(), max.y(), max.y()};
      vgui_easy2D_tableau_sptr etab = vgui_easy2D_tableau_new(img_tabs_[k]);
      etab->add_polygon(4,xs,ys);
      etab->add_point(centre.x(), centre.y());
      dtab_->add(etab);
    }
    this->post_redraw();
  }
}

void vis_manager::get_pixel_info(const int x, const int y,vgui_event const &e, char* msg)
{
  int size = img_tabs_.size();
  if(size != 0)
  {
    int index = dtab_->index();
    vgui_image_tableau_sptr tab = img_tabs_[index];
    vgui_tableau_sptr t = vgui_find_below_by_type_name(tab,"vgui_image_tableau");
    vgui_image_tableau_sptr im;
    im.vertical_cast(t);
    vil_image_view_base_sptr v(im->get_image_view());

    unsigned pix_type = static_cast<unsigned>(v->pixel_format());
    assert (v->pixel_format() >= 0);
    if(pix_type == VIL_PIXEL_FORMAT_UINT_16)
    {
      vil_image_view<unsigned short> vim(*v);
      if(x < 0 || x >= static_cast<int>(vim.ni()) || y < 0 || y>= static_cast<int>(vim.nj()))
        vcl_sprintf(msg, "pixel:(%d, %d) value:%d view:%d", x, y, -1, index);
      else
        vcl_sprintf(msg, "pixel:(%d, %d) value:%d view:%d", x, y, vim(x,y), index);
    }
    else if(pix_type == VIL_PIXEL_FORMAT_DOUBLE)
    {
      vil_image_view<double> vim(*v);
      if(x < 0 || x >= static_cast<int>(vim.ni()) || y < 0 || y >= static_cast<int>(vim.nj()))
        vcl_sprintf(msg, "pixel:(%d, %d) value:%d view:%d", x, y, -1, index);
      else
        vcl_sprintf(msg, "pixel:(%d, %d) value:%lf view:%d", x, y, vim(x,y), index);
    }
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

