
// This is brcv/trk/dbinfo/vis/dbinfo_osl_tools.h
#ifndef dbinfo_osl_tools_h_
#define dbinfo_osl_tools_h_
//:
// \file
// \brief Tools that work on an object signature library
// \author J. L. Mundy
// \date May 12, 2006
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
#include <dbinfo/dbinfo_osl_sptr.h>
#include <dbinfo/pro/dbinfo_osl_storage_sptr.h>

//: A tool for editing an osl
class dbinfo_osl_edit_tool : public bvis1_tool {
public:
  //: Constructor
  dbinfo_osl_edit_tool( const vgui_event_condition& lift =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                        const vgui_event_condition& drop = 
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true) );


  //: Destructor
  virtual ~dbinfo_osl_edit_tool();

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
  //: create a new prototype from an image snippet
  bool load_prototype(vcl_string const& path);
  //: add the prototype to the osl and display it in the grid
  bool add_prototype(vcl_string const& path, vcl_string const& doc);
protected:
  vgui_event_condition gesture_lift_;
  vgui_event_condition gesture_drop_;
  dbinfo_osl_storage_sptr osl_storage_;
  dbinfo_osl_sptr osl_;
  //: new prototype information
  vil_image_resource_sptr image_;
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};

//:------------------- osl match tool ------------------------
//: A tool for match queries against osl prototypes
class dbinfo_osl_match_tool : public bvis1_tool {
public:
  //: Constructor
  dbinfo_osl_match_tool( const vgui_event_condition& lift =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                        const vgui_event_condition& drop = 
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true) );


  //: Destructor
  virtual ~dbinfo_osl_match_tool();

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
  bool load_query(vcl_string const& path, const bool expand = false,
                  const float coef = 0.6f);
  //: match the query against the osl
  bool match_query(const float dx, const float dr,
                   const float ds, const float da, 
                   const float ratio, const float valid_thresh,
                   const unsigned Nob, bool use_int, bool use_grad,
                   bool forward_and_reverse,
                   vcl_vector<vcl_string>& classes,
                   vcl_vector<float>& match_scores,
                   vcl_vector<vil_image_resource_sptr>& match_images);

  //: match the query against the osl in transform intervals
  bool match_query_interval(const float xmin, const float xmax,
                            const float ymin, const float ymax, 
                            const float theta_min, const float theta_max,
                            const float scale_min, const float scale_max,
                            const float aspect_min, const float aspect_max,
                            const unsigned n_intervals,
                            const float valid_thresh,
                            bool forward_and_reverse,
                            vcl_vector<vcl_string>& classes,
                            vcl_vector<float>& match_scores,
                            vcl_vector<vil_image_resource_sptr>& match_images);
protected:
  vgui_event_condition gesture_lift_;
  vgui_event_condition gesture_drop_;
  dbinfo_osl_storage_sptr osl_storage_;
  dbinfo_osl_sptr osl_;
  //: query image
  vil_image_resource_sptr image_;
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
};

//:------------------- osl transform tool ------------------------
//: A tool to evaluate transform finding methods
class dbinfo_osl_transform_tool : public bvis1_tool {
public:
  //: Constructor
  dbinfo_osl_transform_tool( const vgui_event_condition& lift =
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                        const vgui_event_condition& drop = 
                        vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true) );


  //: Destructor
  virtual ~dbinfo_osl_transform_tool();

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

#endif // dbinfo_osl_tools_h_
