// This is brcv/trk/dbinfo/vis/dbinfo_osl_tools.cxx
//:
// \file

#include "dbinfo_osl_tools.h"
#include <vcl_sstream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_command.h> 
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <brip/brip_vil_float_ops.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_mapper.h>
#include <bpro1/bpro1_storage.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <dbinfo/vis/dbinfo_osl_displayer.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/pro/dbinfo_osl_storage.h>
#include <dbinfo/dbinfo_osl.h>
#include <dbinfo/dbinfo_object_matcher.h>
//----------------------------------------------------------------------------
//: A vgui command to edit an osl
class dbinfo_add_prototype_command : public vgui_command
{
public:
  dbinfo_add_prototype_command(dbinfo_osl_edit_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    static vcl_string image_file ="/home/dec/images/*";
    static vcl_string ext = "*.*";
    static vcl_string cls = "";
    static vcl_string doc = "";
    vgui_dialog param_dlg("Add Prototype");
    param_dlg.field("Class", cls);
    param_dlg.field("Doc", doc);
    param_dlg.file("Image File", ext, image_file);
    if(!param_dlg.ask())
      return;
    if(tool_->load_prototype(image_file))
      if(!tool_->add_prototype(cls, doc))
        vcl_cout << "prototype not successfully added\n";
  }
  dbinfo_osl_edit_tool* tool_;
};


//get track storage from the storage pointer
static dbinfo_osl_storage_sptr get_osl_storage(bpro1_storage_sptr const& sto)
{
  dbinfo_osl_storage_sptr osl_storage;
  osl_storage.vertical_cast(sto);
  return osl_storage;
}

static bpro1_storage_sptr storage()
{
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  bpro1_storage_sptr data = res->get_data_at("dbinfo_osl_storage", -1);
  return data;
}

static vil_image_resource_sptr get_image()
{
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  if(!res->go_to_frame(bvis1_manager::instance()->current_frame()))
    return (vil_image_resource*)0;
  bpro1_storage_sptr sto = res->get_data("image");
  if(!sto)
    return (vil_image_resource*)0;
  vcl_cout << "Image Storage Name " << sto->name() << '\n';
  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(sto);
  if(!image_storage)
    return (vil_image_resource*)0;
  return image_storage->get_image();
}

//============================== OSL Edit Tool ============================

//Constructor
dbinfo_osl_edit_tool::dbinfo_osl_edit_tool( const vgui_event_condition& lift,
                                            const vgui_event_condition& drop) :
  gesture_lift_(lift), gesture_drop_(drop), osl_storage_(NULL), osl_(NULL),
  active_(false), tableau_(NULL) 
{
}


//: Destructor
dbinfo_osl_edit_tool::~dbinfo_osl_edit_tool()
{
}


//: Return the name of this tool
vcl_string dbinfo_osl_edit_tool::name() const
{
  return "Edit OSL";
}


//: Set the tableau to work with
bool
dbinfo_osl_edit_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}
//: Add popup menu items
void 
dbinfo_osl_edit_tool::get_popup( const vgui_popup_params& params, 
                                 vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Add Prototype", new dbinfo_add_prototype_command(this));
  menu.add("OSL", pop_menu); 
}


//: Handle events
bool
dbinfo_osl_edit_tool::handle( const vgui_event & e,
                              const bvis1_view_tableau_sptr& view )
{
  return false;
}

void dbinfo_osl_edit_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  osl_storage_ = get_osl_storage(junk);
  osl_ = osl_storage_->osl();
}
//------------------  OSL edit tool methods ---------------------------
bool dbinfo_osl_edit_tool::load_prototype(vcl_string const& path)
{
  image_ = vil_load_image_resource(path.c_str());
  if(!image_)
    return false;
  return true;
}

bool dbinfo_osl_edit_tool::add_prototype(vcl_string const& cls, vcl_string const& doc)
{
  //need a osl to add the prototype
  if(!osl_)
    return false;
  // create an observation fromm the image
  dbinfo_observation_sptr obs = new dbinfo_observation(0, image_, 0,
                                                       true, true, false);
  // add the documentation string to the observation
  obs->set_doc(doc);

  // insert the observation into the osl storage
  osl_->add_prototype(cls, obs);

  //display the frame
  bvis1_manager::instance()->display_current_frame(true);

  return true;
}


//============================== Match Query Tool ============================
// ==========COMMANDS=========
//
//----------------------------------------------------------------------------
//: A vgui command to match a query
class dbinfo_match_query_command : public vgui_command
{
public:
  dbinfo_match_query_command(dbinfo_osl_match_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    static vcl_string image_file ="c:/images/*";
    static vcl_string ext = "*.*";
    static float dx = 1.0f;
    static float dr = 0.2f;
    static float ds = 0.1f;
    static float da = 0.1f;
    static float ratio = 0.1f;
    static float thresh = 0.9f;
    static unsigned Nobs = 300;
    static float coef = 0.6f;
    static bool expand = false;
    static bool use_int = true;
    static bool use_grad = true;
    static bool forward_and_reverse = true;
    vgui_dialog param_dlg("Match Query");
    param_dlg.field("dx", dx);
    param_dlg.field("dr", dr);
    param_dlg.field("ds", ds);
    param_dlg.field("da", da);
    param_dlg.field("ratio", ratio);
    param_dlg.field("valid_thresh", thresh);
    param_dlg.field("Nobs", Nobs);
    param_dlg.field("Expansion Coef", coef);
    param_dlg.file("Image File", ext, image_file);
    param_dlg.checkbox("Expand", expand);
    param_dlg.checkbox("Use Intensity", use_int);
    param_dlg.checkbox("Use Gradient", use_grad);
    param_dlg.checkbox("Forward And Reverse", forward_and_reverse);
    if(!param_dlg.ask())
      return;
    if(!use_int&&!use_grad)
      {
        vcl_cout << "must use at least one information channel\n";
        return;
      }
    vcl_vector<vcl_string> classes;
    vcl_vector<float> match_scores;
    vcl_vector<vil_image_resource_sptr> match_images;
    if(tool_->load_query(image_file, expand, coef))
      if(!tool_->match_query(dx, dr, ds, da, ratio, thresh, Nobs,
                             use_int, use_grad, forward_and_reverse,
                             classes, match_scores, match_images))
        {
          vcl_cout << "query not successfully matched\n";
          return;
        }
    unsigned i = 0;
    vcl_cout << "---Class Scores ---\n";
    for(vcl_vector<vcl_string>::iterator cit = classes.begin();
        cit != classes.end(); ++cit, ++i)
      vcl_cout << *cit << " = " << match_scores[i] << '\n'<< vcl_flush;
    unsigned n_match = match_images.size();
    if(n_match==0)
      return;
    vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(n_match, 1);    
    for(i=0; i<n_match; ++i)
      {
        bgui_image_tableau_sptr itab = bgui_image_tableau_new(match_images[i]);
        vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
        vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(vtab);
        grid->add_at(stab, i, 0);
      }
    //popup adjuster
      
    vgui_dialog inline_tab("Matched Query");
    inline_tab.inline_tableau(grid, 500, 100);
    if (!inline_tab.ask())
      return;
  }
  dbinfo_osl_match_tool* tool_;
};


class dbinfo_match_query_interval_command : public vgui_command
{
public:
  dbinfo_match_query_interval_command(dbinfo_osl_match_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    static vcl_string image_file ="c:/images/*";
    static vcl_string ext = "*.*";
    static float xmin = -3.0f;
    static float xmax = 3.0f;
    static float ymin = -3.0f;
    static float ymax = 3.0f;
    static float theta_min = -0.2f;
    static float theta_max = 0.2f;
    static float scale_min = 0.9f;
    static float scale_max = 1.1f;
    static float aspect_min = 0.9f;
    static float aspect_max = 1.1f;
    static float valid_thresh = 0.9f;
    static unsigned n_intervals = 500;
    static float coef = 0.6f;
    static bool expand = false;
    static bool forward_and_reverse = false;
    vgui_dialog param_dlg("Transform Query");
    param_dlg.file("Image File", ext, image_file);
    param_dlg.checkbox("Expand ", expand);
    param_dlg.field("Coef ", coef);
    param_dlg.field("xmin", xmin);
    param_dlg.field("xmax", xmax);
    param_dlg.field("ymin", ymin);
    param_dlg.field("ymax", ymax);
    param_dlg.field("theta_min", theta_min);
    param_dlg.field("theta_max", theta_max);
    param_dlg.field("scale_min", scale_min);
    param_dlg.field("scale_max", scale_max);
    param_dlg.field("aspect_min", aspect_min);
    param_dlg.field("aspect_max", aspect_max);
    param_dlg.field("valid_thresh", valid_thresh);
    param_dlg.field("Nintervals", n_intervals);
    param_dlg.checkbox("Forward and Reverse ", forward_and_reverse);
    if(!param_dlg.ask())
      return;
    vcl_vector<vcl_string> classes;
    vcl_vector<float> match_scores;
    vcl_vector<vil_image_resource_sptr> match_images;
    if(tool_->load_query(image_file, expand, coef))
      if(!tool_->match_query_interval(xmin, xmax, ymin, ymax,
                                      theta_min, theta_max,
                                      scale_min, scale_max,
                                      aspect_min, aspect_max,
                                      n_intervals, valid_thresh, 
                                      forward_and_reverse,
                                      classes, match_scores, match_images))
        {
          vcl_cout << "query not successfully matched\n";
          return;
        }
    unsigned i = 0;
    vcl_cout << "---Class Scores ---\n";
    for(vcl_vector<vcl_string>::iterator cit = classes.begin();
        cit != classes.end(); ++cit, ++i)
      vcl_cout << *cit << " = " << match_scores[i] << '\n'<< vcl_flush;
    unsigned n_match = match_images.size();
    if(n_match==0)
      return;
    vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(n_match, 1);    
    for(i=0; i<n_match; ++i)
      {
        bgui_image_tableau_sptr itab = bgui_image_tableau_new(match_images[i]);
        vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
        vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(vtab);
        grid->add_at(stab, i, 0);
      }
    //popup adjuster
      
    vgui_dialog inline_tab("Matched Query");
    inline_tab.inline_tableau(grid, 500, 100);
    if (!inline_tab.ask())
      return;
  }
  dbinfo_osl_match_tool* tool_;
};

//Constructor
dbinfo_osl_match_tool::dbinfo_osl_match_tool( const vgui_event_condition& lift,
                                              const vgui_event_condition& drop) :
  gesture_lift_(lift), gesture_drop_(drop), osl_storage_(NULL), osl_(NULL),
  active_(false), tableau_(NULL) 
{
}


//: Destructor
dbinfo_osl_match_tool::~dbinfo_osl_match_tool()
{
}


//: Return the name of this tool
vcl_string dbinfo_osl_match_tool::name() const
{
  return "Match OSL";
}


//: Set the tableau to work with
bool
dbinfo_osl_match_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}
//: Add popup menu items
void 
dbinfo_osl_match_tool::get_popup( const vgui_popup_params& params, 
                                  vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Match Query", new dbinfo_match_query_command(this));
  pop_menu.add("Match Query Interval",
               new dbinfo_match_query_interval_command(this));
  menu.add("OSL", pop_menu); 
}


//: Handle events
bool
dbinfo_osl_match_tool::handle( const vgui_event & e,
                               const bvis1_view_tableau_sptr& view )
{
  return false;
}

void dbinfo_osl_match_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  osl_storage_ = get_osl_storage(junk);
  if(osl_storage_)
    osl_ = osl_storage_->osl();
  else
    vcl_cout << "Failed to activate osl_tools\n";
}
//------------------  OSL match tool methods ---------------------------
bool dbinfo_osl_match_tool::load_query(vcl_string const& path,
                                       const bool expand,
                                       const float coef)
{
  image_ = vil_load_image_resource(path.c_str());
  if(!image_)
    return false;
  if(expand)
    {
      vil_image_view<float> fview = brip_vil_float_ops::convert_to_float(image_);
      image_ = 
        vil_new_image_resource_of_view(brip_vil_float_ops::double_resolution(fview, coef));
    }
  return true;
}

bool dbinfo_osl_match_tool::match_query(const float dx, const float dr,
                                        const float ds, const float da, 
                                        const float ratio, 
                                        const float valid_thresh,
                                        const unsigned Nob, 
                                        bool use_int, bool use_grad, 
                                        bool forward_and_reverse,
                                        vcl_vector<vcl_string>& classes,
                                        vcl_vector<float>& match_scores,
                                        vcl_vector<vil_image_resource_sptr>& match_images){
  //need a osl to add the prototype
  if(!osl_)
    return false;

  // create a query observation from the image
  dbinfo_observation_sptr obsq = new dbinfo_observation(0, image_, 0,
                                                        true, true, false);
  //for now use only the first prototype in each class (FIXME)
  classes = osl_->classes();
  for(vcl_vector<vcl_string>::iterator cit = classes.begin();
      cit != classes.end(); ++cit)
    {
      dbinfo_observation_sptr obsdb = osl_->prototype(*cit, 0);
      if(!obsdb)
        continue;
      vil_image_resource_sptr match_imagef;
      vil_image_resource_sptr match_imager;
      vil_image_resource_sptr dummy;
      float minfor = 0.0;
      float minfof = 
        dbinfo_object_matcher::minfo_rigid_alignment_rand(obsq, obsdb,
                                                          dx, dr, ds,
                                                          ratio, 
                                                          Nob, match_imagef,dummy, dummy,
                                                          false,
                                                          1.0f,
                                                          use_int,
                                                          use_grad);
      if(forward_and_reverse)
        minfor = 
          dbinfo_object_matcher::minfo_rigid_alignment_rand(obsdb, obsq,
                                                            dx, dr, ds, 
                                                            ratio,
                                                            Nob, match_imager,dummy, dummy,
                                                            false,
                                                            1.0f,
                                                            use_int,
                                                            use_grad);
      if(minfof>minfor)
        {
          //          match_scores.push_back(minfof);
          if(match_imagef)
            match_images.push_back(match_imagef);
        }
      else
        {
          //          match_scores.push_back(minfor);
          if(match_imager)
            match_images.push_back(match_imager);
        }
      match_scores.push_back(minfof+minfor);
    }
  return true;
}
bool dbinfo_osl_match_tool::match_query_interval(const float xmin, const float xmax,
                                                 const float ymin, const float ymax, 
                                                 const float theta_min, const float theta_max,
                                                 const float scale_min, const float scale_max,
                                                 const float aspect_min, const float aspect_max,
                                                 const unsigned n_intervals,
                                                 const float valid_thresh,
                                                 bool forward_and_reverse,
                                                 vcl_vector<vcl_string>& classes,
                                                 vcl_vector<float>& match_scores,
                                                 vcl_vector<vil_image_resource_sptr>& match_images)
{
  //need a osl to add the prototype
  if(!osl_)
    return false;

  // create a query observation from the image
  dbinfo_observation_sptr obsq = new dbinfo_observation(0, image_, 0,
                                                        true, true, false);
  //for now use only the first prototype in each class (FIXME)
  classes = osl_->classes();
  for(vcl_vector<vcl_string>::iterator cit = classes.begin();
      cit != classes.end(); ++cit)
    {
      dbinfo_observation_sptr obsdb = osl_->prototype(*cit, 0);
      if(!obsdb)
        continue;
      vil_image_resource_sptr match_imagef;
      vil_image_resource_sptr match_imager;
      float minfor = 0.0;
      float minfof = dbinfo_object_matcher::
        minfo_alignment_in_interval(obsq, obsdb, 
                                    xmin, xmax,
                                    ymin, ymax, 
                                    theta_min, theta_max,
                                    scale_min, scale_max,
                                    aspect_min, aspect_max,
                                    n_intervals,
                                    valid_thresh,
                                    match_imagef);
      if(forward_and_reverse)
        minfor = dbinfo_object_matcher::
          minfo_alignment_in_interval(obsdb, obsq, 
                                      xmin, xmax,
                                      ymin, ymax, 
                                      theta_min, theta_max,
                                      scale_min, scale_max,
                                      aspect_min, aspect_max,
                                      n_intervals,
                                      valid_thresh,
                                      match_imager);
          
      if(minfof>minfor)
        {
          if(match_imagef)
            match_images.push_back(match_imagef);
        }
      else
        {
          if(match_imager)
            match_images.push_back(match_imager);
        }
      match_scores.push_back(minfof+minfor);
    }
  return true;
}

//============================== Transform Query Tool ============================
// ==========COMMANDS=========
//
//----------------------------------------------------------------------------
//: A vgui_command to load a query
class dbinfo_load_command : public vgui_command
{
public:
  dbinfo_load_command(dbinfo_osl_transform_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    static vcl_string image_file ="c:/images/*";
    static vcl_string ext = "*.*";
    static bool query = true;
    static bool expand = false;
    static float coef=0.6f;
    vgui_dialog param_dlg("Load Prototype/Query");
    param_dlg.checkbox("Query(check)/Prototype(no check)", query);
    param_dlg.file("Image File", ext, image_file);
    param_dlg.checkbox("Expand", expand);
    param_dlg.field("Expansion Coef", coef);
    if (!param_dlg.ask())
      return;
    if(!tool_->load(image_file, query, expand, coef))
      vcl_cout << "Image not sucessfully loaded\n";
  }
  dbinfo_osl_transform_tool* tool_;
};
//: A vgui command to transform a query
class dbinfo_transform_query_command : public vgui_command
{
public:
  dbinfo_transform_query_command(dbinfo_osl_transform_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    static vcl_string image_file ="c:/images/*";
    static vcl_string ext = "*.*";
    static float xmin = -3.0f;
    static float xmax = 3.0f;
    static float ymin = -3.0f;
    static float ymax = 3.0f;
    static float theta_min = -0.2f;
    static float theta_max = 0.2f;
    static float scale_min = 0.9f;
    static float scale_max = 1.1f;
    static float aspect_min = 0.9f;
    static float aspect_max = 1.1f;
    static float valid_thresh = 0.9f;
    static unsigned n_intervals = 500;
    vgui_dialog param_dlg("Transform Query");
    param_dlg.field("xmin", xmin);
    param_dlg.field("xmax", xmax);
    param_dlg.field("ymin", ymin);
    param_dlg.field("ymax", ymax);
    param_dlg.field("theta_min", theta_min);
    param_dlg.field("theta_max", theta_max);
    param_dlg.field("scale_min", scale_min);
    param_dlg.field("scale_max", scale_max);
    param_dlg.field("aspect_min", aspect_min);
    param_dlg.field("aspect_min", aspect_max);
    param_dlg.field("valid_thresh", valid_thresh);
    param_dlg.field("Nintervals", n_intervals);
    if(!param_dlg.ask())
      return;
    vil_image_resource_sptr match_image;
    float max_info =0;
    if(!tool_->transform_query(xmin, xmax, ymin, ymax,
                               theta_min, theta_max,
                               scale_min, scale_max,
                               aspect_min, aspect_max,
                               n_intervals, valid_thresh,
                               max_info,
                               match_image))
      {
        vcl_cout << "query not successfully transformed\n";
        return;
      }

    
    bgui_image_tableau_sptr itab = bgui_image_tableau_new(match_image);
    vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
    vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(vtab);

    //popup the match image
    vgui_dialog inline_tab("Transformed Query");
    inline_tab.inline_tableau(stab, 500, 100);
    if (!inline_tab.ask())
      return;
  }
  dbinfo_osl_transform_tool* tool_;
};


//Constructor
dbinfo_osl_transform_tool::dbinfo_osl_transform_tool( const vgui_event_condition& lift,
                                                      const vgui_event_condition& drop) :
  gesture_lift_(lift), gesture_drop_(drop), 
  active_(false), tableau_(NULL), query_(NULL), proto_(NULL) 
{
}


//: Destructor
dbinfo_osl_transform_tool::~dbinfo_osl_transform_tool()
{
}


//: Return the name of this tool
vcl_string dbinfo_osl_transform_tool::name() const
{
  return "Transform OSL";
}


//: Set the tableau to work with
bool
dbinfo_osl_transform_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}
//: Add popup menu items
void 
dbinfo_osl_transform_tool::get_popup( const vgui_popup_params& params, 
                                      vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Load", new dbinfo_load_command(this));
  pop_menu.add("Transform Query", new dbinfo_transform_query_command(this));
  menu.add("OSL", pop_menu); 
}


//: Handle events
bool
dbinfo_osl_transform_tool::handle( const vgui_event & e,
                                   const bvis1_view_tableau_sptr& view )
{
  return false;
}

void dbinfo_osl_transform_tool::activate()
{
  vcl_cout << "Transform tool active\n";
}
//------------------  OSL transform tool methods ---------------------------
bool dbinfo_osl_transform_tool::load(vcl_string const& path,
                                     const bool query,
                                     const bool expand,
                                     const float coef)
{
  vil_image_resource_sptr image = vil_load_image_resource(path.c_str());
  if(!image)
    return false;
  if(expand)
    {
      vil_image_view<float> fview = brip_vil_float_ops::convert_to_float(image);
      image = vil_new_image_resource_of_view(brip_vil_float_ops::
                                             double_resolution(fview, coef));
    }
  if(query)
    query_ = image;
  else proto_ = image;
  return true;
}

bool dbinfo_osl_transform_tool::
transform_query(const float xmin, const float xmax,
                const float ymin, const float ymax, 
                const float theta_min, const float theta_max,
                const float scale_min, const float scale_max,
                const float aspect_min, const float aspect_max,
                const unsigned n_intervals,
                const float valid_thresh,
                float& max_info,
                vil_image_resource_sptr& match_image
                )
{
  if(!query_||!proto_)
    {
      vcl_cout << "Missing image data \n";
      return false;
    }
  // create a query observation from the image
  dbinfo_observation_sptr obsq = new dbinfo_observation(0, query_, 0,
                                                        true, true, false);
  // create a prototype observation from the image
  dbinfo_observation_sptr obsp = new dbinfo_observation(0, proto_, 0,
                                                        true, true, false);
  
  float minfo = dbinfo_object_matcher::
    minfo_alignment_in_interval(obsq, obsp, xmin, xmax,
                                ymin, ymax, 
                                theta_min, theta_max,
                                scale_min, scale_max,
                                aspect_min, aspect_max,
                                n_intervals,
                                valid_thresh,
                                match_image);
  return true;
}

