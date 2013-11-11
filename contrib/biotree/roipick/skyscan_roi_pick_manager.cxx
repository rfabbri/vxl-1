#include "skyscan_roi_pick_manager.h"
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
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <vgui/vgui_image_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_pixel_format.h>
#include <resc/imgr/file_formats/imgr_skyscan_log.h>

skyscan_roi_pick_manager *skyscan_roi_pick_manager::instance_ = 0;

skyscan_roi_pick_manager *skyscan_roi_pick_manager::instance()
{
  if (!instance_)
  {
    instance_ = new skyscan_roi_pick_manager();
    instance_->init();
  }
  return skyscan_roi_pick_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
skyscan_roi_pick_manager::skyscan_roi_pick_manager():vgui_wrapper_tableau(),grid_initialized_(0),deleteme_(0)
{
}

skyscan_roi_pick_manager::~skyscan_roi_pick_manager()
{
}

//: Set up the tableaux
void skyscan_roi_pick_manager::init()
{
  dtab0_ = vgui_deck_tableau_new();
  dtab1_ = vgui_deck_tableau_new();
  dtab2_ = vgui_deck_tableau_new();
  dtab3_ = vgui_deck_tableau_new();

  /*
  vgui_viewer2D_tableau_sptr v2D0 = vgui_viewer2D_tableau_new(dtab0_);
  vgui_viewer2D_tableau_sptr v2D1 = vgui_viewer2D_tableau_new(dtab1_);
  vgui_viewer2D_tableau_sptr v2D2 = vgui_viewer2D_tableau_new(dtab2_);
  vgui_viewer2D_tableau_sptr v2D3 = vgui_viewer2D_tableau_new(dtab3_);

  grid_ = new vgui_grid_tableau(2,2);
  grid_->add_at(v2D0, 0,0);
  grid_->add_at(v2D1, 0,1);
  grid_->add_at(v2D2, 1,0);
  grid_->add_at(v2D3, 1,1);

  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
  */
  grid_ = new vgui_grid_tableau(2,2);
  grid_->add_at(dtab0_, 0,0);
  grid_->add_at(dtab1_, 0,1);
  grid_->add_at(dtab2_, 1,0);
  grid_->add_at(dtab3_, 1,1);
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(grid_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2D);

  this->add_child(shell);
  stop_cine_ = true;
  range_params_ = new vgui_range_map_params(0.0, 65535.0, 1.0, false);


  vgl_point_3d<double> origin; 
  roi_ = vgl_box_3d<double>(origin,1,1,1,vgl_box_3d<double>::centre);
}

void skyscan_roi_pick_manager::quit()
{
  vcl_exit(1);
}

void skyscan_roi_pick_manager::cine_mode()
{
  stop_cine_ = false;
  while(true )
  {
          dtab0_->next();
          dtab0_->post_redraw();
          dtab1_->next();
          dtab1_->post_redraw();
          dtab2_->next();
          dtab2_->post_redraw();
          dtab3_->next();
          dtab3_->post_redraw();

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

void skyscan_roi_pick_manager::stop_cine_mode()
{
  stop_cine_ = true;
}

void skyscan_roi_pick_manager::load_bx3(){
  vgui_dialog load_bx3_dlg("Load BX3 File");
  load_bx3_dlg.set_ok_button("Load");
  load_bx3_dlg.set_cancel_button("Cancel");
  static vcl_string bx3_fname;
  static vcl_string ext = "*.*";
  load_bx3_dlg.file("BX3 Filename:", ext, bx3_fname);
  if (!load_bx3_dlg.ask())
    return;
  vcl_ifstream in(bx3_fname.c_str());
  if(in.good()){
     roi_.read(in);
     this->box_projection();
  }
  else{
    vcl_cerr << "Bad in file " << bx3_fname << "\n";
  }
  return;


}
void skyscan_roi_pick_manager::save_bx3()
{
  vgui_dialog save_bx3_dlg("Save BX3 File");
  save_bx3_dlg.set_ok_button("Save");
  save_bx3_dlg.set_cancel_button("Cancel");
  static vcl_string bx3_fname;
  static vcl_string ext = "*.*";
  save_bx3_dlg.file("BX3 Filename:", ext, bx3_fname);
  if (!save_bx3_dlg.ask())
    return;

  vcl_ofstream out(bx3_fname.c_str());
  if(out.good()){
     roi_.write(out);
  }
  else{
    vcl_cerr << "Bad out file " << bx3_fname << "\n";
  }
  return;
}
void skyscan_roi_pick_manager::load_view_3d()
{
  vgui_dialog load_view_3d_dlg("Load SkyScan Files");
  load_view_3d_dlg.set_ok_button("LOAD");
  load_view_3d_dlg.set_cancel_button("CANCEL");
  
  static vcl_string log_fname = "specimen.log";
  static vcl_string scan_fname = "cali.scan";

  static vcl_string ext = "*.*";
  load_view_3d_dlg.file("Log Filename:", ext, log_fname);
  load_view_3d_dlg.file("Scan Filename:", ext, scan_fname);

  if (!load_view_3d_dlg.ask())
    return;

  vcl_ifstream scan_file(scan_fname.c_str());
  scan_file >> scan_;
  scan_file.close();


  // first, read the log file and create the xscan_scan object
  vcl_FILE *fp = vcl_fopen(log_fname.data(), "r");
  imgr_skyscan_log_header skyscan_log_header(fp);
  imgr_skyscan_log skyscan_log(log_fname);

  //correct the calibration scan orbit according to the rotation step in the log file(if necessary)
  xscan_scan::correct_uniform_orbit(scan_,skyscan_log_header.rotation_step_,skyscan_log_header.number_of_files_);


  // second, read the images
  /*
  vcl_vector<vil_image_resource_sptr> img_res_sptrs;
  img_res_sptrs = skyscan_log.get_images();
  unsigned nk = img_res_sptrs.size();
  */
  vcl_vector<vcl_string> img_names;
  img_names = skyscan_log.get_imagenames();

  unsigned nk = img_names.size(); 
  img_tabs_.clear();
  easy2D_tabs_.clear();

  for(unsigned k = 0; k<nk; ++k)
  { 

    vil_image_view<vxl_uint_16> v = vil_load(img_names[k].c_str());
    /*
    vcl_cout <<"enter the " << k <<"th resource "<< vcl_flush;
    vil_image_view<unsigned short> v(img_res_sptrs[k]->get_copy_view());
    vcl_cout << "open view [ " << k <<"] " << v.ni() << ' ' << v.nj() << '\n';
    */
    //vgui_image_tableau_sptr itab = vgui_image_tableau_new(v, range_params_ );
    ///img_tabs_.push_back(itab);
    bgui_image_tableau_sptr itab = bgui_image_tableau_new(v, range_params_ );
    vgui_image_tableau_sptr facade; facade.vertical_cast(itab);
    img_tabs_.push_back(facade);
    vgui_easy2D_tableau_sptr etab = vgui_easy2D_tableau_new(itab);
    easy2D_tabs_.push_back(etab);
    dtab0_->add(etab);
    dtab1_->add(etab);
    dtab2_->add(etab);
    dtab3_->add(etab);
  }

  dtab1_->index(nk/4);
  dtab2_->index(nk/2);
  dtab3_->index(3*nk/4);
  grid_initialized_=1;

  this->box_projection();
}

void skyscan_roi_pick_manager::box_projection()
{
  if(img_tabs_.size() == 0)
    vcl_cout << "No images loaded yet!" << vcl_endl;
  else
  {
    // compute the active box
    vgl_box_3d<double> active_roi(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    //vcl_cout << roi_.centroid() << vcl_endl;
    active_roi.set_centroid(roi_.centroid());
    active_roi.set_width(roi_.width());
    active_roi.set_height(roi_.height());
    active_roi.set_depth(roi_.depth());

    vgl_homg_point_3d<double> centroid_3d(roi_.centroid());
    // compute the projection of the points, box and centroid onto each image
    unsigned num_views = img_tabs_.size();
    unsigned num_boxes = num_views;
    img_active_box_.clear();
    for(unsigned k=0; k<num_boxes; k++)
    {
      vgl_box_2d<double> active_box2;
      xmvg_perspective_camera<double> cam(scan_(k));
      active_box2 = vpgl_project::project_bounding_box(cam, active_roi);
      vgl_point_2d<double> c2 = cam.project(centroid_3d);
      img_active_box_.push_back(active_box2);
      centroids_2d_.push_back(c2);
    }

    unsigned starting_box = 0;
    unsigned starting_view = 0;
    for(unsigned k = 0; k<num_views; k++)
    {
      unsigned box_index = starting_box % num_boxes;
      unsigned view_index = starting_view % num_views;
      vgl_point_2d<double> active_min = img_active_box_[box_index].min_point();
      vgl_point_2d<double> active_max = img_active_box_[box_index].max_point();
      vgl_point_2d<double> centre = centroids_2d_[box_index];
      float active_xs[4] = {active_min.x() , active_max.x() , 
                            active_max.x() , active_min.x() };
      float active_ys[4] = {active_min.y() , active_min.y() , 
                            active_max.y() , active_max.y() };
      if(k < num_boxes)
      {
        easy2D_tabs_[k]->clear();
        easy2D_tabs_[k]->set_foreground(1.0, 0.0, 0.0);
        easy2D_tabs_[k]->add_polygon(4,active_xs,active_ys);
        easy2D_tabs_[k]->set_foreground(1.0, 0.0, 1.0);
      } 

      starting_box++;
      starting_view++;
    }

    dtab0_->post_redraw();
    dtab1_->post_redraw();
    dtab2_->post_redraw();
    dtab3_->post_redraw();
    this->post_redraw();
  }
}

/*
void skyscan_roi_pick_manager::box_projection()
{
  if(img_tabs_.size() == 0)
    vcl_cout << "No images loaded yet!" << vcl_endl;
  else
  {
    static double scale_x=0.5;
    static double scale_y=0.5;
    static double scale_z=0.5;

    // compute the active box
    vgl_box_3d<double> active_roi(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    //vcl_cout << roi_.centroid() << vcl_endl;
    active_roi.set_centroid(roi_.centroid());
    active_roi.set_width(roi_.width()*scale_x);
    active_roi.set_height(roi_.height()*scale_y);
    active_roi.set_depth(roi_.depth()*scale_z);

    vgl_homg_point_3d<double> centroid_3d(roi_.centroid());
    // compute the projection of the points, box and centroid onto each image
    unsigned num_views = img_tabs_.size();
    unsigned num_boxes = num_views;
    img_box_.clear();
    img_active_box_.clear();
    for(unsigned k=0; k<num_boxes; k++)
    {
      vgl_box_2d<double> box2;
      vgl_box_2d<double> active_box2;
      xmvg_perspective_camera<double> cam(scan_(k));
      box2 = vpgl_project::project_bounding_box(cam, roi_);
      active_box2 = vpgl_project::project_bounding_box(cam, active_roi);
      vgl_point_2d<double> c2 = cam.project(centroid_3d);
      img_box_.push_back(box2);
      img_active_box_.push_back(active_box2);
      centroids_2d_.push_back(c2);
    }

    unsigned starting_box = 0;
    unsigned starting_view = 0;
    for(unsigned k = 0; k<num_views; k++)
    {
      unsigned box_index = starting_box % num_boxes;
      unsigned view_index = starting_view % num_views;
      vgl_point_2d<double> min = img_box_[box_index].min_point();
      vgl_point_2d<double> max = img_box_[box_index].max_point();
      vgl_point_2d<double> active_min = img_active_box_[box_index].min_point();
      vgl_point_2d<double> active_max = img_active_box_[box_index].max_point();
      vgl_point_2d<double> centre = centroids_2d_[box_index];
      float xs[4] = {min.x() , max.x() , max.x() , min.x() };
      float ys[4] = {min.y() , min.y() , max.y() , max.y() };
      float active_xs[4] = {active_min.x() , active_max.x() , 
                            active_max.x() , active_min.x() };
      float active_ys[4] = {active_min.y() , active_min.y() , 
                            active_max.y() , active_max.y() };
      if(k < num_boxes)
      {
        easy2D_tabs_[k]->clear();
        easy2D_tabs_[k]->set_foreground(0.0, 1.0, 0.0);
        easy2D_tabs_[k]->add_polygon(4,xs,ys);
        easy2D_tabs_[k]->set_foreground(1.0, 0.0, 0.0);
        easy2D_tabs_[k]->add_polygon(4,active_xs,active_ys);
        easy2D_tabs_[k]->set_foreground(1.0, 0.0, 1.0);
      } 

      starting_box++;
      starting_view++;
    }

    dtab0_->post_redraw();
    dtab1_->post_redraw();
    dtab2_->post_redraw();
    dtab3_->post_redraw();
    this->post_redraw();
  }
}
*/

/*
void skyscan_roi_pick_manager::get_pixel_info(const int x, const int y,vgui_event const &e, char* msg)
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

    int pix_type = v->pixel_format();
    if(pix_type == VIL_PIXEL_FORMAT_UINT_16)
    {
      vil_image_view<unsigned short> vim(*v);
      if(x < 0 || x >= static_cast<int>(vim.ni()) || y < 0 || y >= static_cast<int>(vim.nj()))
        vcl_sprintf(msg, "pixel:(%d, %d) value:%d view:%d", start_x_ + x, start_y_ + y, -1, index);
      else
        vcl_sprintf(msg, "pixel:(%d, %d) value:%d view:%d", start_x_ + x, start_y_ + y, vim(x,y), index);
    }
    else if(pix_type == VIL_PIXEL_FORMAT_DOUBLE)
    {
      vil_image_view<double> vim(*v);
      if(x < 0 || x >= static_cast<int>(vim.ni()) || y < 0 || y >= static_cast<int>(vim.nj()))
        vcl_sprintf(msg, "pixel:(%d, %d) value:%d view:%d", start_x_ + x, start_y_ + y, -1, index);
      else
        vcl_sprintf(msg, "pixel:(%d, %d) value:%lf view:%d", start_x_ + x, start_y_ + y, vim(x,y), index);
    }
  }
  else
    vcl_sprintf(msg, "No images loaded...");

  return;
}
*/

// Handle all events for this tableau.
bool skyscan_roi_pick_manager::handle(vgui_event const &e)
{
  static bool button_down = false;
  if (e.type == vgui_BUTTON_DOWN)
    button_down = true;
  else if (e.type == vgui_BUTTON_UP)
    button_down = false;
  else if (e.type == vgui_MOTION && button_down == false)
  {
          /*
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
    */
  }
  else if(e.type == vgui_KEY_UP)
  {
          /*
    // Get X,Y position to display on status bar:
    float pointx, pointy;
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(wx_, wy_, pointx, pointy);
    int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);
    char msg[100];

    this->get_pixel_info(intx, inty,e, msg);

    // Display on status bar:
    vgui::out << msg << vcl_endl;
    */
  }
  else if (e.key == vgui_PAGE_UP){
          if(grid_initialized_){
                  dtab0_->next();
                  dtab0_->post_redraw();
                  dtab1_->next();
                  dtab1_->post_redraw();
                  dtab2_->next();
                  dtab2_->post_redraw();
                  dtab3_->next();
                  dtab3_->post_redraw();
          }
  }
  else if (e.key == vgui_PAGE_DOWN){
          if(grid_initialized_){
                  dtab0_->prev();
                  dtab0_->post_redraw();
                  dtab1_->prev();
                  dtab1_->post_redraw();
                  dtab2_->prev();
                  dtab2_->post_redraw();
                  dtab3_->prev();
                  dtab3_->post_redraw();
          }
  }
#define MOVEMENT_SCALE 0.01
  else if (e.key == vgui_CURSOR_LEFT ){
          vgl_point_3d<double> centroid = roi_.centroid();
          centroid.set(centroid.x()- MOVEMENT_SCALE*roi_.width(),
                       centroid.y(),
                       centroid.z());
          roi_ = vgl_box_3d<double>(centroid,
                                    roi_.width(),
                                    roi_.height(),
                                    roi_.depth(),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }
  else if (e.key == vgui_CURSOR_RIGHT ){
          vgl_point_3d<double> centroid = roi_.centroid();
          centroid.set(centroid.x() + MOVEMENT_SCALE*roi_.width(),
                       centroid.y(),
                       centroid.z());
          roi_ = vgl_box_3d<double>(centroid,
                                    roi_.width(),
                                    roi_.height(),
                                    roi_.depth(),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }
  else if (e.key == vgui_CURSOR_UP ){
          vgl_point_3d<double> centroid = roi_.centroid();
          centroid.set(centroid.x(),
                       centroid.y()+ MOVEMENT_SCALE*roi_.height(),
                       centroid.z());
          roi_ = vgl_box_3d<double>(centroid,
                                    roi_.width(),
                                    roi_.height(),
                                    roi_.depth(),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }
  else if (e.key == vgui_CURSOR_DOWN){
          vgl_point_3d<double> centroid = roi_.centroid();
          centroid.set(centroid.x(),
                       centroid.y() - MOVEMENT_SCALE*roi_.height(),
                       centroid.z());
          roi_ = vgl_box_3d<double>(centroid,
                                    roi_.width(),
                                    roi_.height(),
                                    roi_.depth(),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }
  else if (e.key == '\\'){
          vgl_point_3d<double> centroid = roi_.centroid();
          centroid.set(centroid.x(),
                       centroid.y() ,
                       centroid.z()+ MOVEMENT_SCALE*roi_.depth());
          roi_ = vgl_box_3d<double>(centroid,
                                    roi_.width(),
                                    roi_.height(),
                                    roi_.depth(),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }
  else if (e.key == '/'){
          vgl_point_3d<double> centroid = roi_.centroid();
          centroid.set(centroid.x(),
                       centroid.y() ,
                       centroid.z() - MOVEMENT_SCALE*roi_.depth());
          roi_ = vgl_box_3d<double>(centroid,
                                    roi_.width(),
                                    roi_.height(),
                                    roi_.depth(),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }
  else if (e.key == 'x' && e.modifier == vgui_SHIFT){
          roi_ = vgl_box_3d<double>(roi_.centroid(),
                                    roi_.width()*(1+MOVEMENT_SCALE),
                                    roi_.height(),
                                    roi_.depth(),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }

  else if (e.key == 'x' ){
          roi_ = vgl_box_3d<double>(roi_.centroid(),
                                    roi_.width()*(1-MOVEMENT_SCALE),
                                    roi_.height(),
                                    roi_.depth(),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }
  else if (e.key == 'y'&& e.modifier == vgui_SHIFT){
          roi_ = vgl_box_3d<double>(roi_.centroid(),
                                    roi_.width(),
                                    roi_.height()*(1+MOVEMENT_SCALE),
                                    roi_.depth(),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }

  else if (e.key == 'y'){
          roi_ = vgl_box_3d<double>(roi_.centroid(),
                                    roi_.width(),
                                    roi_.height()*(1-MOVEMENT_SCALE),
                                    roi_.depth(),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }
  else if (e.key == 'z'&& e.modifier == vgui_SHIFT){
          roi_ = vgl_box_3d<double>(roi_.centroid(),
                                    roi_.width(),
                                    roi_.height(),
                                    roi_.depth()*(1+MOVEMENT_SCALE),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }

  else if (e.key == 'z'){
          roi_ = vgl_box_3d<double>(roi_.centroid(),
                                    roi_.width(),
                                    roi_.height(),
                                    roi_.depth()*(1-MOVEMENT_SCALE),
                                    vgl_box_3d<double>::centre);
          this->box_projection();
  }
  else{
          return base::handle(e);
  }

}

void skyscan_roi_pick_manager::set_range_params()
{

  static double min = 0, max = 65535;
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

  for(unsigned i = 0; i < img_tabs_.size() ; i++)
  {
    img_tabs_[i]->set_mapping(range_params_);
  }
}


