
// This is brcv/rec/dbru/vis/dbru_osl_tools.h
#ifndef dbru_osl_tools_h_
#define dbru_osl_tools_h_
//:
// \file
// \brief Tools that work on an object signature library
// \author O. C. Ozcanli adapted from J. L. Mundy's dbinfo_osl_tools
// \date Aus 01, 2006
//
// This file contains tools for working with an object signature library
//
// \verbatim
//  Modifications
// \endverbatim
#include <vil/vil_image_resource_sptr.h>
#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_grid_tableau.h>
#include <dbru/dbru_osl_sptr.h>
#include <dbru/pro/dbru_osl_storage_sptr.h>
#include <vcl_utility.h>
#include <vidl1/vidl1_movie_sptr.h>


//: A tool for adding objects from an objects.xml file into an osl
class dbru_osl_add_objects_tool : public bvis1_tool {
public:
  //: Constructor
  dbru_osl_add_objects_tool(const vgui_event_condition& disp =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true));


  //: Destructor
  virtual ~dbru_osl_add_objects_tool();

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
  bool add_objects(vcl_string const& objects_file, int const videoid, vcl_string const& image_file,int movie_id, int start_frame);
  
  //: To generate an OSL where edgel based recognition methods can be run,
  //  Vishal's processes to find foreground polygons, track them and create multiple instances should be run initially,
  //  then labeled objects of those polygons can be used
  //  to add them into the OSL with additional info in the form of multiple instances over many frames.
  //  Initial foreground polygons are usually noisy, however vishal's multiple instances contain a refined
  //  version of them created via combining info over many frames after tracking,
  //  add_refined_polys: when this option is true, 
  //                     the refined polygons of multiple instances are added as object polygons into the OSL, regardless of the original
  //                     labeled polygons in the xml file.
  //  offset: the number of frames that is skipped in multiple_ins_file with respect to polygon file
  //          i.e. all the polygons in "offset" many frames in the polygon file are ignored while multiple instances are
  //               created, so begin process after skipping them here as well
  //  start_frame: In this GUI frames are loaded only if they are in the form of image files in a directory
  //               For example if original video used to create polygon file was a .m2t file
  //               then images need to be extracted and saved in a directory to be used by this GUI
  //               And in that case that directory not necessarily contains images that start from frame 0 in the original video
  //               The starting frame of the polygons in the polygon file should be supplied via this dialog
  //               If polygons had been created and labeled from an image sequence which is identically being used via this GUI
  //               then start_frame number is simply 0
  //  ratio:       threshold ratio to determine how close the centers of instance polygon and object polygon can
  //               get to declare that they belong to the same vehicle
  //               typically 0.5 (half of width of bounding box of the vehicle)
  //               If a warning is issued regarding this threshold, lower the ratio
  bool add_objects_with_ins(vcl_string const& objects_file, 
                            int const movie_id, 
                            vcl_string const& video_file, 
                            vcl_string const& poly_file, 
                            vcl_string const& multiple_ins_file,
                            int offset,
                            int start_frame,
                            double ratio = 0.5f,
                            bool add_refined_polys = false);

  
protected:
  vgui_event_condition gesture_disp_;
  dbru_osl_storage_sptr osl_storage_;
  dbru_osl_sptr osl_;
  vidl1_movie_sptr my_movie_;
  int movie_id_;
  
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};

//:------------------- OSL DELETE OBSERVATIONS TOOL ------------------------
//: A tool for editing an osl
class dbru_osl_delete_observations_tool : public bvis1_tool {
public:
  //: Constructor
  dbru_osl_delete_observations_tool(const vgui_event_condition& disp =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true));


  //: Destructor
  virtual ~dbru_osl_delete_observations_tool();

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
  dbru_osl_storage_sptr osl_storage_;
  dbru_osl_sptr osl_;
  
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};

//:------------------- OSL SAVE DATABASE TOOL ------------------------
//: A tool for editing an osl
class dbru_osl_save_db_file_tool : public bvis1_tool {
public:
  //: Constructor
  dbru_osl_save_db_file_tool(const vgui_event_condition& disp =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true));


  //: Destructor
  virtual ~dbru_osl_save_db_file_tool();

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
  void save_selected_prototypes();
  void save_based_on_bins();

protected:
  vgui_event_condition gesture_disp_;
  dbru_osl_storage_sptr osl_storage_;
  dbru_osl_sptr osl_;
  
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};


//:------------------- osl match tool ------------------------
//: A tool for match queries against osl prototypes
class dbru_osl_match_tool : public bvis1_tool {
public:
  //: Constructor
  dbru_osl_match_tool( const vgui_event_condition& disp =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                        const vgui_event_condition& drop = 
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                        const vgui_event_condition& set = 
                        vgui_event_condition(vgui_key('q'), vgui_MODIFIER_NULL, true));


  //: Destructor
  virtual ~dbru_osl_match_tool();

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
  //: load a query to match against the osl
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

  //: match the query against the current database
  bool match_query_opt(const float dx, const float dr,
                       const float ds, const float ratio, 
                       vcl_vector<float>& match_scores,
                       vcl_vector<vil_image_resource_sptr>& match_images);

  //: match the query using edge based method
  // OZGE TODO
  //bool match_query_edge();

  bool query_set(void) { return query_set_; }

protected:
  vgui_event_condition gesture_disp_;
  vgui_event_condition gesture_drop_;
  vgui_event_condition gesture_set_query_;
  dbru_osl_storage_sptr osl_storage_;
  dbru_osl_sptr osl_;
  //: query image
  //vil_image_resource_sptr image_;
  
  vcl_pair<unsigned, unsigned> query_obs_ids_;
  bool query_set_;
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};
/*
//:------------------- osl transform tool ------------------------
//: A tool to evaluate transform finding methods
class dbru_osl_transform_tool : public bvis1_tool {
public:
  //: Constructor
  dbru_osl_transform_tool( const vgui_event_condition& lift =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                        const vgui_event_condition& drop = 
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true) );


  //: Destructor
  virtual ~dbru_osl_transform_tool();

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
  //: load a query to match against the osl
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
#endif // dbru_osl_tools_h_
