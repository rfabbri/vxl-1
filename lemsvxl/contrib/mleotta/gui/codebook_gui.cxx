#include <vgui/vgui.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>

#include <vul/vul_arg.h>
#include <vcl_ios.h>

#include <vnl/vnl_double_3.h>

#include <bgui/bgui_selector_tableau.h>
#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>


#include <vil/vil_load.h>
#include <vil/vil_new.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>

#include <imesh/imesh_fileio.h>
#include <modrec/modrec_feature_3d.h>
#include <modrec/modrec_codeword.h>

#include <vgui/vgui_style.h>
#include <dbdet/vis/dbdet_keypoint_soview2D.h>
#include <dbdet/dbdet_keypoint_sptr.h>
#include <dbdet/dbdet_keypoint.h>
#include <dbdet/dbdet_lowe_keypoint.h>
#include <dbdet/dbdet_scale_space_peaks.h>
#include <dbdet/vis/dbdet_keypoint_soview2D.h>
#include <vsl/vsl_vector_io.h>


//: A manager for the gui
class codebook_manager : public vgui_wrapper_tableau
{
 public:
  codebook_manager();
 ~codebook_manager();
  static codebook_manager *instance();

  static void open_image();
  static void open_codebook();
  static void change_settings();

  void load_codebook(const vcl_string& filename);

  void set_image(const vil_image_resource_sptr& img);

  bool handle(const vgui_event &e);

  void settings();

  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  void init();

 private: 
  void compute_keypoints(const vil_image_resource_sptr& img);
  vcl_vector<vcl_pair<double,unsigned> > search_codebook(const dbdet_keypoint_sptr& k, bool find_closest=false);
  void draw_match(const modrec_codeword<128>& cw, double d);


  vgui_image_tableau_sptr image_tab_;
  vgui_easy2D_tableau_sptr key_tab_;
  vcl_vector<dbdet_keypoint_sptr> keypoints_;
  double code_thresh_;

  bgui3d_examiner_tableau_sptr exam_tab_;
  SoSeparator* draw_root_;
  SoSeparator* marker_node_;

  vcl_vector<modrec_codeword<128> > codebook_;
  vcl_vector<unsigned> code_sizes_;


  vgui_window* win_;
  static codebook_manager *instance_;


};

//static manager instance
codebook_manager *codebook_manager::instance_ = 0;

//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
codebook_manager *codebook_manager::instance()
{
  if (!instance_)
  {
    instance_ = new codebook_manager();
    instance_->init();
  }
  return codebook_manager::instance_;
}

//==================================================================
//: constructors/destructor
//==================================================================
codebook_manager::
codebook_manager() : vgui_wrapper_tableau()
{
}

codebook_manager::~codebook_manager()
{
}

//======================================================================
//: set up the tableaux at each grid cell
//  the vtol2D_tableaux have been initialized in the constructor
//======================================================================
void codebook_manager::init()
{
  bgui_selector_tableau_new selector_tab;

  image_tab_ = vgui_image_tableau_new();
  selector_tab->add(image_tab_, "image");

  key_tab_ = vgui_easy2D_tableau_new();
  selector_tab->add(key_tab_, "keypoints");

  SoSeparator* root = new SoSeparator;
  root->ref();
  draw_root_ = new SoSeparator;
  draw_root_->ref();
  root->addChild(draw_root_);


  SoTransform *trans = new SoTransform;
  trans->translation.setValue(1, 0, 0);
  //trans->rotation.setValue(SbVec3f(0, 0, 1), 1.5707963f);
  draw_root_->addChild(trans);

  marker_node_ = new SoSeparator;
  marker_node_->ref();
  {
    // Add a red material
    SoMaterial *myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(1.0f, 0.0f, 0.0f);  // Red
    marker_node_->addChild(myMaterial);

    SoSphere *sphere = new SoSphere;
    sphere->radius.setValue(0.1);
    marker_node_->addChild(sphere);

    trans = new SoTransform;
    trans->translation.setValue(0, 0, 1.0);
    trans->rotation.setValue(SbVec3f(1, 0, 0), 1.5707963f);
    marker_node_->addChild(trans);

    SoCylinder *cylinder = new SoCylinder;
    cylinder->radius.setValue(0.025);
    cylinder->height.setValue(2.0);
    marker_node_->addChild(cylinder);

    trans = new SoTransform;
    trans->translation.setValue(0.5, 1.0, 0);
    trans->rotation.setValue(SbVec3f(0, 0, 1), 1.5707963f);
    marker_node_->addChild(trans);

    cylinder = new SoCylinder;
    cylinder->radius.setValue(0.025);
    cylinder->height.setValue(1.0);
    marker_node_->addChild(cylinder);
  }

  code_thresh_ = 0.1;

  exam_tab_ = bgui3d_examiner_tableau_new(root);

  vgui_grid_tableau_new grid_tab(2,1);
  grid_tab->add_at(vgui_viewer2D_tableau_new(selector_tab), 0,0);
  grid_tab->add_at(exam_tab_, 1,0);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_tab);
  this->add_child(shell);

}


//======================================================================
//: set the image
//======================================================================
void codebook_manager::set_image(const vil_image_resource_sptr& img)
{
  image_tab_->set_image_resource(img);
  compute_keypoints(img);

  vgui_style_sptr line_style = vgui_style::new_style(1.0f , 1.0f , 0.0f , 1.0f , 1.0f);
  vgui_style_sptr point_style = vgui_style::new_style(0.0f , 1.0f , 0.0f , 4.0f , 1.0f);

  key_tab_->clear();
  for( vcl_vector< dbdet_keypoint_sptr >::const_iterator itr = keypoints_.begin();
       itr != keypoints_.end();  ++itr ){
    if(*itr){
      dbdet_keypoint_soview2D* obj = new dbdet_keypoint_soview2D(*itr,false);
      key_tab_->add( obj );
      obj->set_style( line_style );
      obj->set_point_style( point_style );
    }
  }
}


//======================================================================
//: compute keypoints
//======================================================================
void codebook_manager::compute_keypoints(const vil_image_resource_sptr& img)
{
  vil_image_view<vxl_byte> image = vil_convert_to_grey_using_rgb_weighting(img->get_view());

  float prior_sigma = 1.6f, max_curve_ratio = 10.0f, contrast_thresh = 0.03f, orient_sigma = 1.5f;
  int orient_bins = 36;
  vgui_dialog key_dlg("Keypoint properties");
  key_dlg.field("Prior Smoothing Sigma", prior_sigma);
  key_dlg.field("Max Curvature Ratio",   max_curve_ratio);
  key_dlg.field("Contrast Threshold",    contrast_thresh);
  key_dlg.field("Orientation Bins",      orient_bins);
  key_dlg.field("Orientation Spread",    orient_sigma);
  if (!key_dlg.ask())
    return;
  

  // Cast into float and upsample by 2x
  vil_image_view<float> image2x;
  float dummy=0.0;
  vil_image_resource_sptr image_rsc = vil_new_image_resource_of_view(image);
  vil_image_view_base_sptr imagef = vil_convert_stretch_range(dummy, image_rsc->get_view());

  // determine the number of scale octaves
  int min_size = (image.ni() < image.nj())?image.ni():image.nj();
  unsigned num_octaves = 2;
  while( (min_size/=2) >= 8) ++num_octaves;

  // build the gaussian and difference of gaussian scale images
  bil_scale_image<float> gauss(3,num_octaves,prior_sigma,-1), dog(3,num_octaves,prior_sigma,-1);
  gauss.build_gaussian(imagef,&dog);
  // build the gradient scale space images
  bil_scale_image<float> g_dir, g_mag;
  gauss.compute_gradients(g_dir, g_mag);

  // detect peaks in the scale space
  vcl_vector<vgl_point_3d<float> > peak_pts;
  dbdet_scale_space_peaks(dog, peak_pts, max_curve_ratio, contrast_thresh);

  // compute orientations and descriptors at each scale peak to make a Lowe keypoint
  keypoints_.clear();
  dbdet_ssp_orientation_params o_params(g_dir, g_mag, orient_bins, orient_sigma);//, float thresh=0.8f);
  for(unsigned int i=0; i<peak_pts.size(); ++i)
  {
    vgl_point_3d<float>& pt = peak_pts[i];
    vcl_vector<float> orientations = dbdet_ssp_orientations(pt, o_params);
    for(unsigned int j=0; j<orientations.size(); ++j)
    {
      dbdet_lowe_keypoint* kp = new dbdet_lowe_keypoint(pt.x(), pt.y(), pt.z(),
                                                        orientations[j]);
      kp->compute_descriptor(g_dir, g_mag);
      keypoints_.push_back(kp);
    }
  }
}

//======================================================================
//: open an image file (static callback)
//======================================================================
void codebook_manager::open_image()
{
  vgui_dialog load_image_dlg("Open Image");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  if (!load_image_dlg.ask())
    return;

  vil_image_resource_sptr img = vil_load_image_resource(image_filename.c_str());
  instance()->set_image(img);
}


//======================================================================
//: change settings (static callback)
//======================================================================
void codebook_manager::change_settings()
{
  codebook_manager::instance()->settings();
}

//======================================================================
//: change settings 
//======================================================================
void codebook_manager::settings()
{
  vgui_dialog dlg("Settings");
  dlg.field("Codeword Threshold",code_thresh_);
  if(!dlg.ask())
    return;
}


//======================================================================
//: load the codebook data
//======================================================================
void codebook_manager::load_codebook(const vcl_string& filename)
{
  vsl_b_ifstream is(filename);
  vsl_b_read(is,codebook_);
  vsl_b_read(is,code_sizes_);
  is.close();
}

//======================================================================
//: open a codebook (static callback)
//======================================================================
void codebook_manager::open_codebook()
{
  vgui_dialog load_dlg("Open Codebook");
  static vcl_string filename = "";
  static vcl_string ext = "*.*";
  load_dlg.file("Codebook Filename:", ext, filename);
  if (!load_dlg.ask())
    return;

  instance()->load_codebook(filename);
}

//======================================================================
//: search_codebook
//======================================================================
vcl_vector<vcl_pair<double,unsigned> >
codebook_manager::search_codebook(const dbdet_keypoint_sptr& k, bool find_closest)
{
  dbdet_lowe_keypoint* kp = static_cast<dbdet_lowe_keypoint*>(k.ptr());
  vnl_vector_fixed<double,128> d = kp->descriptor();
  double best = 1.0;
  int best_ind = -1;
  typedef vcl_pair<double,unsigned> pair_du;
  vcl_vector<pair_du> matches;
  pair_du closest(codebook_[0].compare_descriptor(d),0);
  for(unsigned i=0; i<codebook_.size(); ++i){
    double dist = codebook_[i].compare_descriptor(d);
    if(dist <= code_thresh_)
      matches.push_back(pair_du(dist,i));
    if(dist <= closest.first)
      closest = pair_du(dist,i);
  }
  sort(matches.begin(), matches.end());
  if(find_closest && matches.empty())
    matches.push_back(closest);

  vgui::out << "closest distance: "<<closest.first;
  vgui::out << " var pos: "<<codebook_[closest.second].var_position()<<'\n';

  return matches;
}


void codebook_manager::draw_match(const modrec_codeword<128>& cw, double d)
{
  SoSeparator* group = new SoSeparator;
  draw_root_->addChild(group);

  SoTransform *trans = new SoTransform;
  vgl_point_3d<double> pt(cw.position());
  vgl_rotation_3d<double> rot(cw.orientation());
  vnl_double_3 axis(rot.axis());
  double angle = rot.angle();
  double s = cw.scale();
  trans->rotation.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
  trans->translation.setValue(pt.x(), pt.y(), pt.z());
  trans->scaleFactor.setValue(s,s,s);
  group->addChild(trans);

  group->addChild(marker_node_);

  exam_tab_->post_redraw();
}


//======================================================================
//: handle function for tableaux
//======================================================================
bool codebook_manager::handle(const vgui_event &e)
{
  if(e.type == vgui_MOUSE_MOTION){
    vgui_soview* so = key_tab_->get_highlighted_soview();
    if(so && draw_root_){
      dbdet_keypoint_soview2D* kso = static_cast<dbdet_keypoint_soview2D*>(so);
      vcl_vector<vcl_pair<double,unsigned> > matches = search_codebook(kso->sptr);
      if(!matches.empty()){
        draw_root_->removeAllChildren();
        key_tab_->select(key_tab_->get_highlighted());
      }
      for(unsigned i=0; i<matches.size(); ++i){
        const modrec_codeword<128>& cw = codebook_[matches[i].second];
        draw_match(cw,matches[i].first);
      }
    }
  }
  if(e.type == vgui_MOUSE_DOWN){
    key_tab_->deselect_all();
    vgui_soview* so = key_tab_->get_highlighted_soview();
    if(so && draw_root_){
      dbdet_keypoint_soview2D* kso = static_cast<dbdet_keypoint_soview2D*>(so);
      vcl_vector<vcl_pair<double,unsigned> > matches = search_codebook(kso->sptr,true);
      if(!matches.empty()){
        draw_root_->removeAllChildren();
        key_tab_->select(key_tab_->get_highlighted());
      }
      for(unsigned i=0; i<matches.size(); ++i){
        const modrec_codeword<128>& cw = codebook_[matches[i].second];
        draw_match(cw,matches[i].first);
      }
    }
  }
  return vgui_wrapper_tableau::handle(e);
}


                      
//=============================================================================

int main(int argc, char** argv)
{

  vul_arg<vcl_string>  a_image("-image", "path to image", "");
  vul_arg<vcl_string>  a_codebook("-codebook", "path to codebook", "");
  vul_arg<vcl_string>  a_mesh("-mesh", "path to mesh", "");
  vul_arg_parse(argc, argv);


  //===========================================================================
  // Make the GUI
  //===========================================================================
  
  // initialize vgui
  vgui::init(argc, argv);
  
  // initialize bgui_3d
  bgui3d_init();

  // give the manager a valid reference count
  vgui_tableau_sptr instance(codebook_manager::instance());

  if(a_image.set())
    codebook_manager::instance()->set_image(vil_load_image_resource(a_image().c_str()));

  if(a_codebook.set())
    codebook_manager::instance()->load_codebook(a_codebook());

  vgui_menu menu_bar, file_menu, opt_menu;
  file_menu.add("Open Image", codebook_manager::open_image);
  file_menu.add("Open Codebook", codebook_manager::open_codebook);

  opt_menu.add("Settings",codebook_manager::change_settings);

  menu_bar.add("File",file_menu);
  menu_bar.add("Options",opt_menu);

  vcl_string title = "Codebook GUI";
  vgui_window* win = vgui::produce_window(1000, 800, menu_bar, title);
  win->get_adaptor()->set_tableau(codebook_manager::instance());
  win->set_statusbar(true);
  win->show();
  codebook_manager::instance()->set_window(win);
  codebook_manager::instance()->post_redraw();

  return vgui::run();
}
