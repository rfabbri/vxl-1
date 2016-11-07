// This is brcv/rec/dbru/vis/dbru_matching_tool.h
#ifndef dbru_matching_tool_h_
#define dbru_matching_tool_h_
//:
// \file
// \brief Recognition Library matching Tool for Testing
//      Usage: Load video sequence, 
//             ADD THE SECOND VIEW, otherwise tool won't activate correctly
//             select tool from menu 
//             browse to open poly.txt file that has segmentation output
//             from background subtraction process for that video
//             (This file should be created apriori, it is supposed to contain
//              video id and
//              a polygonal representation of the contour of each moving object in each
//              frame.)
//             The tool loads all the polygons in this file indexed by frame numbers.
//             As the user moves from frame to frame the tool (overlay) displays object contours 
//             in that frame.
//             Closest polygon to the mouse pointer becomes active.
//             Press 'o' to open matching dialog
//             Fill in the fields, when ok is pressed that matching info is saved 
//             
//             When LEFT mouse button is pressed on an active polygon,
//             that polygon is pushed to the query polygons vector
//
//             When RIGHT mouse button is pressed on an active polygon,
//             that polygon is pushed to the database polygons vector
//             
//             When 'r' button is pressed all query polygons are run against database
//             polygons and their shock match outputs are pushed to the storage classes
//
//             When 'q' button is pressed query list, and when 'd' is pressed database list is cleared
//
// \author Ozge C Ozcanli Ozbay, (ozge@lems.brown.edu)
// \date 6/28/05
//
// \verbatim
//  Modifications
//      
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>
#include <vbl/vbl_array_1d.h>
#include <vcl_utility.h>

#include <vsol/vsol_polygon_2d_sptr.h>

#include <dbskr/dbskr_tree_sptr.h>

#include <dbru/pro/dbru_rcor_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_match_storage_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

//: An abstract base class for tools in this file
class dbru_matching_tool : public bvis1_tool
{
public:

  dbru_matching_tool();

  //: This is called when the tool is activated
  virtual void activate();

  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Return the name of this tool
  virtual vcl_string name() const;
  
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );
  
  bool get_matching_info();

 // void print_object_status();


protected:

  //: add active polygon to query list
  vgui_event_condition gesture_set_query;
  //: add active polygon to database list
  vgui_event_condition gesture_set_database;
  
  vgui_event_condition gesture_run;
  vgui_event_condition gesture_open_dialog;
  
  //vgui_event_condition gesture_clear_query;    
  //vgui_event_condition gesture_clear_database;    
  vgui_event_condition gesture_right_arrow;
  vgui_event_condition gesture_left_arrow;

  vcl_vector<vcl_vector<vgui_soview2D_polygon*> > polygons_;  // the array that holds each object in each frame
  vbl_array_1d< vbl_array_1d<vsol_polygon_2d_sptr>* > normal_polygons_;  // the array that holds each object in each frame
  vbl_array_1d< vbl_array_1d<dbskr_tree_sptr>* > trees_;  // the array that holds each object in each frame
  vbl_array_1d< vbl_array_1d<dbinfo_observation_sptr> *> observations_;
  vbl_array_1d< vil_image_resource_sptr > images_;
  // only match the current query against current database instance for now
  vcl_pair<int, int> query_polygon_; 
  vcl_pair<int, int> database_polygon_;

  int size_;
  int frame_no_;
  int frame_cnt_;
  int active_polygon_no_;
  bool activation_ok_;
  int video_id_;

  // for now, only the events in this view will be handled
  bvis1_view_tableau_sptr active_view_, other_view_;

  dbru_rcor_storage_sptr rcor_storage_;
  dbskr_shock_match_storage_sptr shock_match_storage_;
  vidpro1_image_storage_sptr image_storage1_, image_storage2_, image_storage3_;

  //: algorithm names and parameters
  vcl_vector<vcl_string> choices_;
  int choice_;   // choice_ determines which algorithm to use
  float rms_;
  float R_;
  float restricted_cvmatch_ratio_;
  bool use_Amir_shock_extraction_;
  bool dynamic_pruning_;
  bool elastic_splice_cost_;
  float pruning_threshold_;
  float shock_curve_ds_;
  //if the outlining bounding box length is 100, use threshold 0.2 (base_thres_)
  //  if this length halves, use 0.2/4
  //  if this length doubles use 0.2*4
  float base_thres_;

  vgui_style_sptr active_style_, normal_style_;

  bool get_tree(dbskr_tree_sptr& tree1, vsol_polygon_2d_sptr poly1);

private:

};


#endif // dbru_matching_tool_h_
