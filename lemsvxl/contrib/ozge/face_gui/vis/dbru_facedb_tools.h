
// This is brcv/rec/dbru/vis/dbru_facedb_tools.h
#ifndef dbru_facedb_tools_h_
#define dbru_facedb_tools_h_
//:
// \file
// \brief Tools that work on a face library
// \author O. C. Ozcanli adapted from J. L. Mundy's dbinfo_facedb_tools
// \date Aus 01, 2006
//
// \verbatim
//  Modifications
// \endverbatim
#include <vil/vil_image_resource_sptr.h>
#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_grid_tableau.h>
#include <ozge/face_gui/dbru_facedb_sptr.h>
#include <ozge/face_gui/pro/dbru_facedb_storage_sptr.h>
#include <vcl_utility.h>
#include <vidl1/vidl1_movie_sptr.h>

//: A tool for adding subjects in a recursive manner from a root directory
class dbru_facedb_add_subjects_tool : public bvis1_tool {
public:
  //: Constructor
  dbru_facedb_add_subjects_tool(const vgui_event_condition& disp =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true));


  //: Destructor
  virtual ~dbru_facedb_add_subjects_tool();

  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated

  virtual void activate();
  //:Tool utilities
  bool add_subjects(vcl_string const& subjects_root_dir);
  
protected:
  vgui_event_condition gesture_disp_;
  dbru_facedb_storage_sptr facedb_storage_;
  dbru_facedb_sptr facedb_;
  vidl1_movie_sptr my_movie_;
  int movie_id_;
  
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};
/*
//:------------------- facedb DELETE OBSERVATIONS TOOL ------------------------
//: A tool for editing an facedb
class dbru_facedb_delete_observations_tool : public bvis1_tool {
public:
  //: Constructor
  dbru_facedb_delete_observations_tool(const vgui_event_condition& disp =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true));


  //: Destructor
  virtual ~dbru_facedb_delete_observations_tool();

  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );

  //: Allow the tool to add to the popup menu as a tableau would
  //virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated

  virtual void activate();
  //:Tool utilities

protected:
  vgui_event_condition gesture_disp_;
  dbru_facedb_storage_sptr facedb_storage_;
  dbru_facedb_sptr facedb_;
  
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};

//:------------------- facedb SAVE DATABASE TOOL ------------------------
//: A tool for editing an facedb
class dbru_facedb_save_db_file_tool : public bvis1_tool {
public:
  //: Constructor
  dbru_facedb_save_db_file_tool(const vgui_event_condition& disp =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true));


  //: Destructor
  virtual ~dbru_facedb_save_db_file_tool();

  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );

  //: Allow the tool to add to the popup menu as a tableau would
  //virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated

  virtual void activate();
  //:Tool utilities

protected:
  vgui_event_condition gesture_disp_;
  dbru_facedb_storage_sptr facedb_storage_;
  dbru_facedb_sptr facedb_;
  
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};


//:------------------- facedb match tool ------------------------
//: A tool for match queries against facedb prototypes
class dbru_facedb_match_tool : public bvis1_tool {
public:
  //: Constructor
  dbru_facedb_match_tool( const vgui_event_condition& disp =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                        const vgui_event_condition& drop = 
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                        const vgui_event_condition& set = 
                        vgui_event_condition(vgui_key('q'), vgui_MODIFIER_NULL, true));


  //: Destructor
  virtual ~dbru_facedb_match_tool();

  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated

  virtual void activate();
  //:Tool utilities
  //: load a query to match against the facedb
  //bool load_query(vcl_string const& path, const bool expand = false,
  //                const float coef = 0.6f);
  
  //: match the query against the current database
  bool match_query(const float dx, const float dr,
                   const float ds, const float da, 
                   const float ratio, const float valid_thresh,
                   const unsigned Nob, bool use_int, bool use_grad,
                   bool forward_and_reverse,
  //                 vcl_vector<vcl_string>& classes,
                   vcl_vector<float>& match_scores,
                   vcl_vector<vil_image_resource_sptr>& match_images);

  //: match the query against the current datbase in transform intervals
  bool match_query_interval(const float xmin, const float xmax,
                            const float ymin, const float ymax, 
                            const float theta_min, const float theta_max,
                            const float scale_min, const float scale_max,
                            const float aspect_min, const float aspect_max,
                            const unsigned n_intervals,
                            const float valid_thresh,
                            bool forward_and_reverse,
    //                        vcl_vector<vcl_string>& classes,
                            vcl_vector<float>& match_scores,
                            vcl_vector<vil_image_resource_sptr>& match_images);

  bool query_set(void) { return query_set_; }

protected:
  vgui_event_condition gesture_disp_;
  vgui_event_condition gesture_drop_;
  vgui_event_condition gesture_set_query_;
  dbru_facedb_storage_sptr facedb_storage_;
  dbru_facedb_sptr facedb_;
  //: query image
  //vil_image_resource_sptr image_;
  
  vcl_pair<unsigned, unsigned> query_obs_ids_;
  bool query_set_;
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};
/*
//:------------------- facedb transform tool ------------------------
//: A tool to evaluate transform finding methods
class dbru_facedb_transform_tool : public bvis1_tool {
public:
  //: Constructor
  dbru_facedb_transform_tool( const vgui_event_condition& lift =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                        const vgui_event_condition& drop = 
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true) );


  //: Destructor
  virtual ~dbru_facedb_transform_tool();

  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated

  virtual void activate();
  //:Tool utilities
  //: load a query to match against the facedb
  bool load(vcl_string const& path,
            const bool query = true,
            const bool expand = false,
            const float coef = 0.6f);
  //: transform the query to match the prototype
  bool transform_query(const float xmin, const float xmax,
                       const float ymin, const float ymax, 
                       const float theta_min, const float theta_max,
                       const float scale_min, const float scale_max,
                       const float aspect_min, const float aspect_max,
                       const unsigned n_intervals,
                       const float valid_thresh,
                       float& max_info,
                       vil_image_resource_sptr& match_image
                       );


protected:
  vgui_event_condition gesture_lift_;
  vgui_event_condition gesture_drop_;
  //: query image
  vil_image_resource_sptr query_;
  //: prototype image
  vil_image_resource_sptr proto_;
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};
*/
#endif // dbru_facedb_tools_h_
