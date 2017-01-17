// This is brcv/rec/dbru/vis/dbru_facedb_tools.cxx
//:
// \file

#include "dbru_facedb_tools.h"
#include <vcl_sstream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
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
#include <ozge/face_gui/vis/dbru_facedb_displayer.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
//#include <dbinfo/dbinfo_observation.h>

#include <ozge/face_gui/pro/dbru_facedb_storage.h>
#include <ozge/face_gui/dbru_facedb.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
//#include <dbinfo/dbinfo_observation.h>

//#include <dbinfo/dbinfo_object_matcher.h>

//----------------------------------------------------------------------------
//: A vgui command to edit an facedb
class dbru_add_subjects_command : public vgui_command
{
public:
  dbru_add_subjects_command(dbru_facedb_add_subjects_tool* tool) : tool_(tool) {}
  void execute()
  {
    static vcl_string subjects_dir ="/home/dec/subjects";
    static vcl_string ext = "";
    vgui_dialog param_dlg("Add Subjects");
    param_dlg.file("Subjects Dir", ext, subjects_dir);
    if(!param_dlg.ask())
      return;
    if(!tool_->add_subjects(subjects_dir))
      vcl_cout << "subjects not successfully added\n";
  }
  dbru_facedb_add_subjects_tool* tool_;
};


//get track storage from the storage pointer
static dbru_facedb_storage_sptr get_facedb_storage(bpro1_storage_sptr const& sto)
{
  dbru_facedb_storage_sptr facedb_storage;
  facedb_storage.vertical_cast(sto);
  return facedb_storage;
}

static bpro1_storage_sptr storage()
{
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  bpro1_storage_sptr data = res->get_data_at("dbru_facedb_storage", -1);
  return data;
}

//get facedb from the storage pointer
static dbru_facedb_sptr get_facedb()
{
  dbru_facedb_storage_sptr facedb_storage;
  facedb_storage.vertical_cast(storage());
  if (!facedb_storage)
    return 0;
  else
    return facedb_storage->facedb();
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

static bool get_current_database(vcl_vector<vcl_pair<unsigned, unsigned> >* db, vgui_grid_tableau_sptr tableau)
{
  if (!tableau) {
    vcl_cout << " get_current_database() - tableau_ is not set in dbru_facedb_add_objects_tool\n";
    return false;
  }

  vcl_vector<int> *col_pos = new vcl_vector<int>();
  vcl_vector<int> *row_pos = new vcl_vector<int>();
  vcl_vector<int> *times = new vcl_vector<int>();
  tableau->get_selected_positions(col_pos, row_pos, times);

  for (unsigned i = 0; i<col_pos->size(); i++) {
    if ((*col_pos)[i] < 0 || (*row_pos)[i] < 0)
      continue;
    
    vcl_pair<unsigned, unsigned> p;
    p.first = unsigned((*row_pos)[i]);
    p.second = unsigned((*col_pos)[i]);
    db->push_back(p);
  }

  col_pos->clear();
  row_pos->clear();
  times->clear();
  delete col_pos;
  delete row_pos;
  delete times;
  
  return true;
}

//============================== facedb Edit Tool ============================

//Constructor
dbru_facedb_add_subjects_tool::dbru_facedb_add_subjects_tool(const vgui_event_condition& disp) :
  gesture_disp_(disp), facedb_storage_(NULL), facedb_(NULL), active_(false), tableau_(NULL) 
{
}

//: Destructor
dbru_facedb_add_subjects_tool::~dbru_facedb_add_subjects_tool()
{
}


//: Return the name of this tool
vcl_string dbru_facedb_add_subjects_tool::name() const
{
  return "Add subjects to facedb";
}


//: Set the tableau to work with
bool
dbru_facedb_add_subjects_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}
//: Add popup menu items
void 
dbru_facedb_add_subjects_tool::get_popup( const vgui_popup_params& params, 
                                 vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Add subjects", new dbru_add_subjects_command(this));
  menu.add("facedb", pop_menu); 
}


//: Handle events
bool
dbru_facedb_add_subjects_tool::handle( const vgui_event & e,
                              const bvis1_view_tableau_sptr& view )
{
  return false;
}

void dbru_facedb_add_subjects_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  facedb_storage_ = get_facedb_storage(junk);
  facedb_ = facedb_storage_->facedb();
}
//------------------  facedb edit tool methods ---------------------------

bool dbru_facedb_add_subjects_tool::add_subjects(vcl_string const& subjects_root_dir)
{
  //need a facedb to add the prototype
  if(!facedb_) {
    vcl_cout << "dbru_facedb_add_subjects_tool::add_subjects() - facedb problems, load an facedb or create a new facedb, subjects are not added!\n";
    return false;
  }

  vcl_string s(subjects_root_dir);
  s += "/*.*";
  vcl_cout << "s: " << s << vcl_endl;
  for (vul_file_iterator fit = s;fit; ++fit) {
    // check to see if file is a directory.
    // all the images of a subject are assumed to be in a separate directory under root
    if (vul_file::is_directory(fit())) {
      vcl_vector<vcl_string> subjimages;
      for (vul_file_iterator sub_fit = fit(); sub_fit; ++sub_fit) {
        if (vul_file::is_directory(sub_fit()))
          continue;
        subjimages.push_back(sub_fit());
      }
      vcl_vector<vil_image_resource_sptr> subj;
      for (unsigned k = 0; k < subjimages.size(); k++) {
        vil_image_resource_sptr img = vil_load_image_resource(subjimages[k].c_str());
        subj.push_back(img);
      }
      facedb_->add_subject(subj);
    }
    
  }
  
  //display the frame
  bvis1_manager::instance()->display_current_frame(true);
  //bvis1_manager::instance()->regenerate_all_tableaux();

  return true;
}

/*
//============================== Match Query Tool ============================
// ==========COMMANDS=========
//
//----------------------------------------------------------------------------
//: A vgui command to match a query
class dbru_match_query_command : public vgui_command
{
public:
  dbru_match_query_command(dbru_facedb_match_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    //static vcl_string image_file ="c:/images/*";
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
    //param_dlg.field("Expansion Coef", coef);
    //param_dlg.file("Image File", ext, image_file);
    //param_dlg.checkbox("Expand", expand);
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
    //vcl_vector<vcl_string> classes;
    vcl_vector<float> match_scores;
    vcl_vector<vil_image_resource_sptr> match_images;
    //if(tool_->load_query(image_file, expand, coef))
    if(tool_->query_set())  
      if(!tool_->match_query(dx, dr, ds, da, ratio, thresh, Nobs,
                             use_int, use_grad, forward_and_reverse,
                             match_scores, match_images))
        {
          vcl_cout << "query not successfully matched\n";
          return;
        }
    unsigned i = 0;
    vcl_cout << "--- Scores ---\n";
    //for(vcl_vector<vcl_string>::iterator cit = classes.begin();
    //    cit != classes.end(); ++cit, ++i)
    for ( i = 0; i<match_scores.size(); i++)
      vcl_cout << "(" << i << " = " << match_scores[i] << ") " << vcl_flush;
    vcl_cout << "\n";
    unsigned n_match = match_images.size();
    if(n_match==0)
      return;
    for(i=0; i<n_match; ++i) {
      char buffer[1000];
      sprintf(buffer, "output_%d.png", i);
      vil_save_image_resource(match_images[i], buffer);
    }
    /*
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
      *//*
  }
  dbru_facedb_match_tool* tool_;
};


class dbru_match_query_interval_command : public vgui_command
{
public:
  dbru_match_query_interval_command(dbru_facedb_match_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    //static vcl_string image_file ="c:/images/*";
    //static vcl_string ext = "*.*";
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
    //static float coef = 0.6f;
    //static bool expand = false;
    static bool forward_and_reverse = false;
    vgui_dialog param_dlg("Transform Query");
    //param_dlg.file("Image File", ext, image_file);
    //param_dlg.checkbox("Expand ", expand);
    //param_dlg.field("Coef ", coef);
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
    //vcl_vector<vcl_string> classes;
    vcl_vector<float> match_scores;
    vcl_vector<vil_image_resource_sptr> match_images;
    //if(tool_->load_query(image_file, expand, coef))
    if(tool_->query_set()) {
      if(!tool_->match_query_interval(xmin, xmax, ymin, ymax,
                                      theta_min, theta_max,
                                      scale_min, scale_max,
                                      aspect_min, aspect_max,
                                      n_intervals, valid_thresh, 
                                      forward_and_reverse,
                                      match_scores, match_images))
        {
          vcl_cout << "query not successfully matched\n";
          return;
        }
    } else {
      vcl_cout << "query observation is not picked, press p on the query prototype to select it.\n";
    }
    unsigned i = 0;
    //vcl_cout << "---Class Scores ---\n";
    //for(vcl_vector<vcl_string>::iterator cit = classes.begin();
    //    cit != classes.end(); ++cit, ++i)
    //  vcl_cout << *cit << " = " << match_scores[i] << '\n'<< vcl_flush;
    for ( ; i < match_scores.size(); i++)
      vcl_cout << "(" << i << " = " << match_scores[i] << ") " << vcl_flush;
    vcl_cout << "\n";
    unsigned n_match = match_images.size();
    if(n_match==0)
      return;
    for(i=0; i<n_match; ++i) {
      char buffer[1000];
      sprintf(buffer, "output_%d.png", i);
      vil_save_image_resource(match_images[i], buffer);
    }
    /*
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
      *//*
  }
  dbru_facedb_match_tool* tool_;
};

//Constructor
dbru_facedb_match_tool::dbru_facedb_match_tool( const vgui_event_condition& disp,
                                          const vgui_event_condition& drop,
                                          const vgui_event_condition& set) :
  gesture_disp_(disp), gesture_drop_(drop), gesture_set_query_(set), facedb_storage_(NULL), facedb_(NULL),
  active_(false), tableau_(NULL), query_set_(false) 
{
}


//: Destructor
dbru_facedb_match_tool::~dbru_facedb_match_tool()
{
}


//: Return the name of this tool
vcl_string dbru_facedb_match_tool::name() const
{
  return "Match dbru_facedb";
}


//: Set the tableau to work with
bool
dbru_facedb_match_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}
//: Add popup menu items
void 
dbru_facedb_match_tool::get_popup( const vgui_popup_params& params, 
                                  vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Info Match Query", new dbru_match_query_command(this));
  pop_menu.add("Info Match Query Interval",
               new dbru_match_query_interval_command(this));
  menu.add("facedb", pop_menu); 
}


//: Handle events
bool
dbru_facedb_match_tool::handle( const vgui_event & e,
                               const bvis1_view_tableau_sptr& view )
{
  if (gesture_set_query_(e)) {
    if (!tableau_) {
      vcl_cout << "tableau_ is not set in dbru_facedb_match_tool\n";
      return false;
    }
    vcl_pair<unsigned, unsigned> query_obs_ids_;
    unsigned int col_pos, row_pos;
    tableau_->get_active_position(&col_pos, &row_pos);
    query_obs_ids_.first = row_pos;
    query_obs_ids_.second = col_pos;
    vcl_cout << "query is set with col_pos: " << col_pos << " row_pos: " << row_pos << vcl_endl;
    query_set_ = true;
    return false;
  }

  if (gesture_disp_(e))
    if (tableau_) {
      unsigned col_pos, row_pos;
      tableau_->get_active_position(&col_pos, &row_pos);
      if (row_pos < facedb_->n_objects()) {
        dbru_object_sptr obj = facedb_->get_object(row_pos);
        if (col_pos < obj->n_polygons()) {
          dbru_label_sptr lbl = facedb_->get_label(row_pos, col_pos);
          vcl_cout << *lbl;
        }
      }
    }

  return false;
}

void dbru_facedb_match_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  facedb_storage_ = get_facedb_storage(junk);
  if(facedb_storage_)
    facedb_ = facedb_storage_->facedb();
  else
    vcl_cout << "Failed to activate facedb_tools\n";
}
//------------------  facedb match tool methods ---------------------------
/*bool dbru_facedb_match_tool::load_query(vcl_string const& path,
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
}*//*

bool dbru_facedb_match_tool::match_query(const float dx, const float dr,
                                        const float ds, const float da, 
                                        const float ratio, 
                                        const float valid_thresh,
                                        const unsigned Nob, 
                                        bool use_int, bool use_grad, 
                                        bool forward_and_reverse,
                                        //vcl_vector<vcl_string>& classes,
                                        vcl_vector<float>& match_scores,
                                        vcl_vector<vil_image_resource_sptr>& match_images){
  //need a facedb to add the prototype
  if(!facedb_)
    return false;

  // get the query observation from the image
  dbinfo_observation_sptr obsq = facedb_->get_prototype(query_obs_ids_.first, query_obs_ids_.second);

  if (!obsq) {
    vcl_cout << "Query observation is not appropriately set!\n";
    return false;
  }

  //for now use only the first prototype in each class (FIXME)
  //classes = facedb_->classes();
  //for(vcl_vector<vcl_string>::iterator cit = classes.begin();
  //    cit != classes.end(); ++cit)
  //  {
  //    dbinfo_observation_sptr obsdb = facedb_->prototype(*cit, 0);
  vcl_vector<vcl_pair<unsigned, unsigned> > *db = new vcl_vector<vcl_pair<unsigned, unsigned> >();
  if (!get_current_database(db, tableau_)) {
    vcl_cout << "Problems in getting db!\n";
    return false;
  }

  vcl_pair<unsigned, unsigned> p;
  for (unsigned i = 0; i<db->size(); i++) 
  {
    p = (*db)[i];
    dbinfo_observation_sptr obsdb = facedb_->get_prototype(p.first, p.second);
    if(!obsdb)
      continue;
    vil_image_resource_sptr match_imagef;
    vil_image_resource_sptr match_imager;
    float minfor = 0.0;
    float minfof = 
      dbinfo_object_matcher::minfo_rigid_alignment_rand(obsq, obsdb,
                                                        dx, dr, ds, da,
                                                        ratio, valid_thresh,
                                                        Nob, match_imagef,
                                                        false,
                                                        1.0f,
                                                        use_int,
                                                        use_grad);
    if(forward_and_reverse)
      minfor = 
        dbinfo_object_matcher::minfo_rigid_alignment_rand(obsdb, obsq,
                                                          dx, dr, ds, da,
                                                          ratio,
                                                          valid_thresh,
                                                          Nob, match_imager,
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
bool dbru_facedb_match_tool::match_query_interval(const float xmin, const float xmax,
                                                 const float ymin, const float ymax, 
                                                 const float theta_min, const float theta_max,
                                                 const float scale_min, const float scale_max,
                                                 const float aspect_min, const float aspect_max,
                                                 const unsigned n_intervals,
                                                 const float valid_thresh,
                                                 bool forward_and_reverse,
                                                 //vcl_vector<vcl_string>& classes,
                                                 vcl_vector<float>& match_scores,
                                                 vcl_vector<vil_image_resource_sptr>& match_images)
{
  //need a facedb to add the prototype
  if(!facedb_)
    return false;

  // get the query observation from the image
  dbinfo_observation_sptr obsq = facedb_->get_prototype(query_obs_ids_.first, query_obs_ids_.second);

  if (!obsq) {
    vcl_cout << "Query observation is not appropriately set!\n";
    return false;
  }

  //for now use only the first prototype in each class (FIXME)
  //classes = facedb_->classes();
  //for(vcl_vector<vcl_string>::iterator cit = classes.begin();
   //   cit != classes.end(); ++cit)
   // {
  vcl_vector<vcl_pair<unsigned, unsigned> > *db = new vcl_vector<vcl_pair<unsigned, unsigned> >();
  if (!get_current_database(db, tableau_)) {
    vcl_cout << "Problems in getting db!\n";
    return false;
  }

  vcl_pair<unsigned, unsigned> p;
  for (unsigned i = 0; i<db->size(); i++) 
  {
      p = (*db)[i];
      dbinfo_observation_sptr obsdb = facedb_->get_prototype(p.first, p.second);
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

//============================== Delete Observations Tool ============================
// ==========COMMANDS=========
//
//----------------------------------------------------------------------------

//Constructor
dbru_facedb_delete_observations_tool::dbru_facedb_delete_observations_tool(const vgui_event_condition& disp) :
  gesture_disp_(disp), active_(false), tableau_(NULL)
{
}


//: Destructor
dbru_facedb_delete_observations_tool::~dbru_facedb_delete_observations_tool()
{
}


//: Return the name of this tool
vcl_string dbru_facedb_delete_observations_tool::name() const
{
  return "Delete Selected Observations from facedb";
}


//: Set the tableau to work with
bool
dbru_facedb_delete_observations_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}

//: Handle events
bool
dbru_facedb_delete_observations_tool::handle( const vgui_event & e,
                                   const bvis1_view_tableau_sptr& view )
{
  if (gesture_disp_(e))
    if (tableau_) {
      unsigned col_pos, row_pos;
      tableau_->get_active_position(&col_pos, &row_pos);
      if (row_pos < facedb_->n_objects()) {
        dbru_object_sptr obj = facedb_->get_object(row_pos);
        if (col_pos < obj->n_polygons()) {
          dbru_label_sptr lbl = facedb_->get_label(row_pos, col_pos);
          vcl_cout << *lbl;
        }
      }
    }

  return false;
}

void dbru_facedb_delete_observations_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  facedb_storage_ = get_facedb_storage(junk);
  if(facedb_storage_)
    facedb_ = facedb_storage_->facedb();
  else {
    vcl_cout << "Failed to activate facedb_tools\n";
    return;
  }

  vcl_cout << "delete_observations tool active\n";
  vcl_cout << "USAGE: Selected observations will be deleted, if all observations of an object are deleted then its removed completely\n";

  if (!tableau_) {
    vcl_cout << " dbru_facedb_delete_observations_tool::activate() - tableau_ is not set in dbru_facedb_add_objects_tool\n";
    return;
  }

  vcl_vector<int> *col_pos = new vcl_vector<int>();
  vcl_vector<int> *row_pos = new vcl_vector<int>();
  vcl_vector<int> *times = new vcl_vector<int>();
  tableau_->get_selected_positions(col_pos, row_pos, times);

  for (unsigned i = 0; i<col_pos->size(); i++) {
    if ((*col_pos)[i] < 0 || (*row_pos)[i] < 0)
      continue;
     
    if (unsigned((*row_pos)[i]) < facedb_->n_objects()) 
      facedb_->null_prototype( (*row_pos)[i], (*col_pos)[i] );
  }

  for (unsigned i = 0; i < facedb_->n_objects(); i++) {
    dbru_object_sptr obj = facedb_->get_object(i);
    bool remove = true;
    for (unsigned j = 0; j < obj->n_observations(); j++) 
      if (obj->get_observation(j)) {
        remove = false;
        break;
      }
    if (remove)
      if (!facedb_->remove_object(i))
        vcl_cout << "problems in removing object in row: " << i << vcl_endl;
  }

  col_pos->clear();
  row_pos->clear();
  times->clear();
  delete col_pos;
  delete row_pos;
  delete times;
  bvis1_manager::instance()->display_current_frame(true);
  return;
}


//============================== Save Current Database into Txt File Tool ============================
// ==========COMMANDS=========
//
//----------------------------------------------------------------------------

//Constructor
dbru_facedb_save_db_file_tool::dbru_facedb_save_db_file_tool(const vgui_event_condition& disp) :
  gesture_disp_(disp), active_(false), tableau_(NULL)
{
}


//: Destructor
dbru_facedb_save_db_file_tool::~dbru_facedb_save_db_file_tool()
{
}


//: Return the name of this tool
vcl_string dbru_facedb_save_db_file_tool::name() const
{
  return "Save current selected database into a TXT file";
}


//: Set the tableau to work with
bool
dbru_facedb_save_db_file_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}

//: Handle events
bool
dbru_facedb_save_db_file_tool::handle( const vgui_event & e,
                                   const bvis1_view_tableau_sptr& view )
{
  if (gesture_disp_(e))
    if (tableau_) {
      unsigned col_pos, row_pos;
      tableau_->get_active_position(&col_pos, &row_pos);
      if (row_pos < facedb_->n_objects()) {
        dbru_object_sptr obj = facedb_->get_object(row_pos);
        if (col_pos < obj->n_polygons()) {
          dbru_label_sptr lbl = facedb_->get_label(row_pos, col_pos);
          vcl_cout << *lbl;
        }
      }
    }

  return false;
}

void dbru_facedb_save_db_file_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  facedb_storage_ = get_facedb_storage(junk);
  if(facedb_storage_)
    facedb_ = facedb_storage_->facedb();
  else {
    vcl_cout << "Failed to activate facedb_tools\n";
    return;
  }

  vcl_cout << "save_db_file tool active\n";

  if (!tableau_) {
    vcl_cout << " dbru_facedb_delete_observations_tool::activate() - tableau_ is not set in dbru_facedb_add_objects_tool\n";
    return;
  }

  static vcl_string db_file ="/home/dec/";
  static vcl_string ext = "*.txt";
  vgui_dialog param_dlg("Save Database File");
  param_dlg.file("Database File", ext, db_file);
  if(!param_dlg.ask())
    return;

  vcl_vector<int> *col_pos = new vcl_vector<int>();
  vcl_vector<int> *row_pos = new vcl_vector<int>();
  vcl_vector<int> *times = new vcl_vector<int>();
  tableau_->get_selected_positions(col_pos, row_pos, times);

  vcl_ofstream of(db_file.c_str());
  if (!of) {
    vcl_cout << "dbru_facedb_delete_observations_tool::activate() - file not opened\n";
    return;
  }

  vcl_vector<unsigned> temp1, temp2;
  
  for (unsigned i = 0; i<col_pos->size(); i++) {
    unsigned osi = (*col_pos)[i];
    unsigned oi = (*row_pos)[i];
    if (oi < 0 || osi < 0)
      continue;
    
    if (oi < facedb_->n_objects()) {
      if (facedb_->get_prototype(oi, osi)) { // if not null
        temp1.push_back(oi);
        temp2.push_back(osi);
      }
    }
  }

  of << temp1.size() << vcl_endl;
  for (unsigned i = 0; i < temp1.size(); i++) {
    of << temp1[i] << " " << temp2[i] << "\n";
  }
  of.close();
  
  vcl_cout << "selected observations are saved to: " << db_file << "\n";
  col_pos->clear();
  row_pos->clear();
  times->clear();
  delete col_pos;
  delete row_pos;
  delete times;
  
  return;
}

/*
//============================== Transform Query Tool ============================
// ==========COMMANDS=========
//
//----------------------------------------------------------------------------
//: A vgui_command to load a query
class dbru_load_command : public vgui_command
{
public:
  dbru_load_command(dbru_facedb_transform_tool* tool) 
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
  dbru_facedb_transform_tool* tool_;
};
//: A vgui command to transform a query
class dbru_transform_query_command : public vgui_command
{
public:
  dbru_transform_query_command(dbru_facedb_transform_tool* tool) 
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
  dbru_facedb_transform_tool* tool_;
};


//Constructor
dbru_facedb_transform_tool::dbru_facedb_transform_tool( const vgui_event_condition& lift,
                                                      const vgui_event_condition& drop) :
  gesture_lift_(lift), gesture_drop_(drop), 
  active_(false), tableau_(NULL), query_(NULL), proto_(NULL) 
{
}


//: Destructor
dbru_facedb_transform_tool::~dbru_facedb_transform_tool()
{
}


//: Return the name of this tool
vcl_string dbru_facedb_transform_tool::name() const
{
  return "Transform facedb";
}


//: Set the tableau to work with
bool
dbru_facedb_transform_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}
//: Add popup menu items
void 
dbru_facedb_transform_tool::get_popup( const vgui_popup_params& params, 
                                      vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Load", new dbru_load_command(this));
  pop_menu.add("Transform Query", new dbru_transform_query_command(this));
  menu.add("facedb", pop_menu); 
}


//: Handle events
bool
dbru_facedb_transform_tool::handle( const vgui_event & e,
                                   const bvis1_view_tableau_sptr& view )
{
  return false;
}

void dbru_facedb_transform_tool::activate()
{
  vcl_cout << "Transform tool active\n";
}
//------------------  facedb transform tool methods ---------------------------
bool dbru_facedb_transform_tool::load(vcl_string const& path,
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

bool dbru_facedb_transform_tool::
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

*/

