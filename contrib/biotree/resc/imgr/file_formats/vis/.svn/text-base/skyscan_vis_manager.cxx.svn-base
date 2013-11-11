#include "skyscan_vis_manager.h"
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
#include <vil/vil_pixel_format.h>
#include <imgr/file_formats/imgr_skyscan_log.h>

skyscan_vis_manager *skyscan_vis_manager::instance_ = 0;

skyscan_vis_manager *skyscan_vis_manager::instance()
{
  if (!instance_)
  {
    instance_ = new skyscan_vis_manager();
    instance_->init();
  }
  return skyscan_vis_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
skyscan_vis_manager::skyscan_vis_manager():vgui_wrapper_tableau()
{
  start_x_ = 0;
  start_y_ = 0;
}

skyscan_vis_manager::~skyscan_vis_manager()
{
}

//: Set up the tableaux
void skyscan_vis_manager::init()
{
  dtab_ = vgui_deck_tableau_new();
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(dtab_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2D);
  this->add_child(shell);
  stop_cine_ = true;
  range_params_ = new vgui_range_map_params(0.0, 65535.0, 1.0, false);
}

void skyscan_vis_manager::quit()
{
  vcl_exit(1);
}

void skyscan_vis_manager::cine_mode()
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

void skyscan_vis_manager::stop_cine_mode()
{
  stop_cine_ = true;
}

void skyscan_vis_manager::load_view_3d()
{
  vgui_dialog load_view_3d_dlg("Load SkyScan Files");
  load_view_3d_dlg.set_ok_button("LOAD");
  load_view_3d_dlg.set_cancel_button("CANCEL");
  
  static vcl_string log_fname = "D:/BioTree_Data/CastFragmentHair/negatives/CastFragmentHair.log";

  start_x_ = 600;
  start_y_ = 300;
  size_x_ = 700;
  size_y_ = 200;
  
  static vcl_string ext = "*.*";
  load_view_3d_dlg.file("Log Filename:", ext, log_fname);
  load_view_3d_dlg.field("cropping from x0", start_x_);
  load_view_3d_dlg.field("cropping from y0", start_y_);
  load_view_3d_dlg.field("size x of cropping", size_x_);
  load_view_3d_dlg.field("size y of cropping", size_y_);

  if (!load_view_3d_dlg.ask())
    return;

  // first, read the log file and create the xscan_scan object
  vcl_FILE *fp = vcl_fopen(log_fname.data(), "r");
  imgr_skyscan_log_header skyscan_log_header(fp);
  imgr_skyscan_log skyscan_log(log_fname);
  scan_ = skyscan_log.get_scan();

  // second, read the images
  vcl_vector<vil_image_resource_sptr> img_res_sptrs;
  img_res_sptrs = skyscan_log.get_images();

  unsigned nk = img_res_sptrs.size();
  vcl_cout << nk << vcl_endl;
  img_tabs_.clear();

  for(unsigned k = 0; k<nk; ++k)
  { 
    vcl_cout <<"enter the " << k <<"th resource "<< vcl_flush;
    vil_image_view<unsigned short> v(img_res_sptrs[k]->get_copy_view(start_x_, size_x_, start_y_, size_y_));
    vcl_cout << "open view [ " << k <<"] " << v.ni() << ' ' << v.nj() << '\n';
    vgui_image_tableau_sptr itab = vgui_image_tableau_new(v, range_params_ );
    img_tabs_.push_back(itab);
    dtab_->add(itab);
  }
  vcl_cout << scan_.kk().get_matrix() << vcl_endl;
  vcl_cout << scan_.n_views() << vcl_endl;
}


void skyscan_vis_manager::save_scan()
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

void skyscan_vis_manager::set_range_params()
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

vcl_vector<vgl_homg_point_3d<double> > 
skyscan_vis_manager::read_points(vcl_string filename, 
                                 vgl_box_3d<double> box, 
                                 double resolution) 
{
  vsol_cylinder cylinder;
  vcl_vector<vgl_homg_point_3d<double> > points;
  int ver, num;
  double strength;
  //vcl_string file_name = "C:\\test_images\\filters\\aug8_straight_and_angled_hairs\\hair_negatives\\cylinders.bin";
  vsl_b_ifstream istream(filename.c_str());
  vsl_b_read(istream, ver);
  vsl_b_read(istream, num);
  
  double min_strength = 1e29, max_strength = -1e19;
  for (int i=0; i < num; i++) {
    vsl_b_read(istream, strength);
    if (min_strength > strength)
      min_strength = strength;
    if (max_strength < strength)
      max_strength = strength;

    double minx=box.min_x(), miny=box.min_y(), minz=box.min_z();
    vcl_cout << strength << " min=" << min_strength << " max=" << max_strength << vcl_endl;
    cylinder.b_read(istream);
    vgl_homg_point_3d<double> point(minx + (cylinder.center().x()*resolution), 
      miny+(cylinder.center().y()*resolution), minz+(cylinder.center().z()*resolution) );
    points.push_back(point);
    vcl_cout << cylinder << vcl_endl;
  }

  return points;
}

void skyscan_vis_manager::box_projection()
{
  if(img_tabs_.size() == 0)
    vcl_cout << "No images loaded yet!" << vcl_endl;
  else
  {
    vgui_dialog projection_dlg("Load Scan and Box Files");
    static vcl_string scan_filename = "*.scn";
    static vcl_string box_filename = "*.bx3";
    static vcl_string filter_box_filename = "*.bx3";
    static vcl_string points_filename = "*.bin";

    static vcl_string ext = "*.*";
    static double res=0.01;
    static double scale_x=0.5;
    static double scale_y=0.5;
    static double scale_z=0.5;
    projection_dlg.file("Scan Filename:", ext, scan_filename);
    projection_dlg.file("Processing Box Filename:", ext, box_filename);
    projection_dlg.field("Scale Factor in X:", scale_x);
    projection_dlg.field("Scale Factor in Y:", scale_y);
    projection_dlg.field("Scale Factor in Z:", scale_z);
    projection_dlg.file("Filter Box Filename:", ext, filter_box_filename);
    projection_dlg.file("Points Filename:", ext, points_filename);
    projection_dlg.field("Resolution (in mm):", res);
    if (!projection_dlg.ask())
      return;

    // error checks for file extensions
    int size = scan_filename.size();
    if(scan_filename[size-1] != 'n' || scan_filename[size-2] != 'c' ||
      scan_filename[size-3] != 's' || scan_filename[size-4] != '.')
    {
      vcl_cerr << "Scan file does not have a valid extension. Aborting process..." << vcl_endl;
      return;
    }
    size = box_filename.size();
    if(box_filename[size-1] != '3' || box_filename[size-2] != 'x' ||
      box_filename[size-3] != 'b' || box_filename[size-4] != '.')
    {
      vcl_cerr << "Processing box file does not have a valid extension. Aborting process..." << vcl_endl;
      return;
    }
    size = filter_box_filename.size();
    if(filter_box_filename[size-1] != '3' || filter_box_filename[size-2] != 'x' ||
      filter_box_filename[size-3] != 'b' || filter_box_filename[size-4] != '.')
    {
      vcl_cerr << "Filter box file does not have a valid extension. Aborting process..." << vcl_endl;
      return;
    }
    if(scale_x <= 0.0 || scale_y <= 0.0 || scale_z <= 0.0 || scale_x > 1.0 || scale_y > 1.0 || scale_z > 1.0)
    {
      vcl_cerr << "Scale parameters should be in (0,1) range. Aborting process..." << vcl_endl;
      return;
    }
    if (points_filename.size() > 0) {
      size = points_filename.size();
      if(points_filename[size-1] != 'n' || points_filename[size-2] != 'i' ||
        points_filename[size-3] != 'b' || points_filename[size-4] != '.')
      {
        vcl_cerr << "points file does not have a valid extension. Aborting process..." << vcl_endl;
        return;
      }
    }

    // get the processing box
    vcl_ifstream box_file(box_filename.c_str());
    vgl_box_3d<double> box3;
    box3.read(box_file);
    box_file.close();
    vcl_cout << box3 << vcl_endl;
    // get the xscan
    vcl_ifstream scan_file(scan_filename.c_str());
    xscan_scan scan;
    scan_file >> scan;
    scan_file.close();
    vcl_cout << scan << vcl_endl;
    // compute the active box
    vgl_box_3d<double> active_box3(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    vcl_cout << box3.centroid() << vcl_endl;
    active_box3.set_centroid(box3.centroid());
    active_box3.set_width(box3.width()*scale_x);
    active_box3.set_height(box3.height()*scale_y);
    active_box3.set_depth(box3.depth()*scale_z);
    // check if the file for the filter box exists
    bool filter_box_valid = 0;
    FILE *fp = vcl_fopen(filter_box_filename.c_str(),"r");
    vgl_box_3d<double> filter_box3;
    if(fp != NULL)
    {
      vcl_fclose(fp);
      filter_box_valid = 1;
      vcl_ifstream filter_box_file(filter_box_filename.c_str());
      filter_box3.read(filter_box_file);
      filter_box_file.close();
      vcl_cout << filter_box3 << vcl_endl;
    }

    // read the 3D points, which is the center of the points
    vcl_vector<vgl_homg_point_3d<double> > homg_points = read_points(points_filename, box3, res);
    vcl_vector<vcl_vector<vgl_point_2d<double> > > point_list;

    vgl_homg_point_3d<double> centroid_3d(box3.centroid());
    // compute the projection of the points, box and centroid onto each image
    unsigned num_views = img_tabs_.size();
    unsigned num_boxes = scan.n_views();
    for(unsigned k=0; k<num_boxes; k++)
    {
      vgl_box_2d<double> box2;
      vgl_box_2d<double> active_box2;
      vgl_box_2d<double> filter_box2;
      xmvg_perspective_camera<double> cam(scan(k));
      box2 = vpgl_project::project_bounding_box(cam, box3);
      active_box2 = vpgl_project::project_bounding_box(cam, active_box3);
      if(filter_box_valid == 1)
      {
        filter_box2 = vpgl_project::project_bounding_box(cam, filter_box3);
        img_filter_box_.push_back(filter_box2);
      }
      vgl_point_2d<double> c2 = cam.project(centroid_3d);
      img_box_.push_back(box2);
      img_active_box_.push_back(active_box2);
      centroids_2d_.push_back(c2);
      // project the hair points
      vcl_vector<vgl_point_2d<double> > points(homg_points.size());
      for (unsigned int i=0; i<points.size(); i++) {
        vgl_point_2d<double> point_2d = cam.project(homg_points[i]);
        points[i]=point_2d;
      }
      point_list.push_back(points);
//      vcl_cout << box2 << vcl_endl;
    }

    for(unsigned k = 0; k<num_views; ++k)
      dtab_->remove(dtab_->current());
    //visualize
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
      float xs[4] = {min.x() - start_x_, max.x() - start_x_, max.x() - start_x_, min.x() - start_x_};
      float ys[4] = {min.y() - start_y_, min.y() - start_y_, max.y() - start_y_, max.y() - start_y_};
      float active_xs[4] = {active_min.x() - start_x_, active_max.x() - start_x_, 
                            active_max.x() - start_x_, active_min.x() - start_x_};
      float active_ys[4] = {active_min.y() - start_y_, active_min.y() - start_y_, 
                            active_max.y() - start_y_, active_max.y() - start_y_};
      vgui_easy2D_tableau_sptr etab = vgui_easy2D_tableau_new(img_tabs_[view_index]);
      float filter_xs[4];
      float filter_ys[4];
      if(filter_box_valid)
      {
        vgl_point_2d<double> filter_min = img_filter_box_[box_index].min_point();
        vgl_point_2d<double> filter_max = img_filter_box_[box_index].max_point();
        filter_xs[0] = filter_min.x() - start_x_; 
        filter_xs[1] = filter_max.x() - start_x_; 
        filter_xs[2] = filter_max.x() - start_x_; 
        filter_xs[3] = filter_min.x() - start_x_;
        filter_ys[0] = filter_min.y() - start_y_; 
        filter_ys[1] = filter_min.y() - start_y_; 
        filter_ys[2] = filter_max.y() - start_y_; 
        filter_ys[3] = filter_max.y() - start_y_;
      }
      if(k < num_boxes)
      {
        etab->set_foreground(0.0, 1.0, 0.0);
        etab->add_polygon(4,xs,ys);
        etab->set_foreground(1.0, 0.0, 0.0);
        etab->add_polygon(4,active_xs,active_ys);
        etab->set_foreground(1.0, 0.0, 1.0);
        if(filter_box_valid)
          etab->add_polygon(4,filter_xs,filter_ys);
        etab->set_foreground(0.0, 0.0, 1.0);
        etab->add_point(centre.x() - start_x_, centre.y() - start_y_);
        // add the hair points
        etab->set_foreground(1.0, 1.0, 0.0);
        for (unsigned int i=0; i<point_list[box_index].size(); i++)
          etab->add_point(point_list[k][i].x() - start_x_, point_list[k][i].y() - start_y_);
      }
      ////
      dtab_->add(etab);
      starting_box++;
      starting_view++;
    }

    this->post_redraw();
  }
}

void skyscan_vis_manager::get_pixel_info(const int x, const int y,vgui_event const &e, char* msg)
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

// Handle all events for this tableau.
bool skyscan_vis_manager::handle(vgui_event const &e)
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
