// This is brcv/rec/dbru/vis/dbru_labeling_tool.h
#ifndef dbru_labeling_tool_h_
#define dbru_labeling_tool_h_
//:
// \file
// \brief Recognition Library Labeling Tool for Testing
//      Usage: Load video sequence, select tool from menu 
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
//             Press 'k' or middle point on the mouse to create an object
//             (which will hold all polygons of this object together with labeling information)
//             Press 'l' to open labeling dialog
//             Fill in the fields, when ok is pressed that label is saved as latest_label
//             
//             When LEFT mouse button is pressed on an active polygon,
//   pp          that polygon is assigned latest_label and pushed to the active object
//             
//             When SHIFT + LEFT mouse button is pressed,
//             a polygon with 0 vertices is assigned latest_label and pushed to the active object
//              
//             When 'o' is pressed at any time,
//             xml output of objeocts is printed into a selected file
//
//             When 'p' is printed at any time, information regarding
//             current objects and categories and the id of active object is displayed
//
//             When 'u' is printed at any time, removes the latest added polygon from active object
//             CAUTION make sure the polygon to be deleted was active for style change to normal to occur!!!
//
//             When 'd' is printed at any time, removes the active object
//
//              If there is a objects.xml file which contain labeled objects for this video,
//              Load it when tool is selected to assign labels to the polygons
//              which overlap sufficienltly with the current ones.
//              These labels can be changed later
//              But still objects should be created and polygons should be added to
//              objects
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

#include <dbru/dbru_label_sptr.h>
#include <dbru/dbru_object_sptr.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vidl1/vidl1_movie_sptr.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_io.h>

/*
#include <dbdet/lvwr/dbdet_lvwr.h>
*/

//: An abstract base class for tools in this file
class dbru_labeling_tool : public bvis1_tool
{
public:

  dbru_labeling_tool();

  //: This is called when the tool is activated
  virtual void activate();

  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Return the name of this tool
  virtual vcl_string name() const;
  
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );
  
  bool get_labeling_info(dbru_label_sptr label);

  void print_object_status();

int createosl(char *db_object_list_filename, char *video_list_file_name, char *osl_file_name) ;

protected:

  bool overlap(vsol_polygon_2d_sptr poly, vgui_soview2D_polygon* poly2);

  vgui_event_condition gesture_tab;
  vgui_event_condition gesture_label;
  //: add active polygon to active object
  vgui_event_condition gesture_set;    
  //: set a null polygon (with 0 vertices) for the current frame to the active object
  //  (no polygon is created for that object in this frame)
  //vgui_event_condition gesture_set_empty;    
  //: create a new object and make it active, 
  //  IMPORTANT: an object should be created at its starting frame 
  //  and its polygons should be added in order of frame number via set operation
  vgui_event_condition gesture_create;       
  vgui_event_condition gesture_create2;       
  vgui_event_condition gesture_remove_latest;       
  vgui_event_condition gesture_print;       
  vgui_event_condition gesture_output;
  vgui_event_condition gesture_output_bin;
  vgui_event_condition gesture_right_arrow;
  vgui_event_condition gesture_left_arrow;
  vgui_event_condition gesture_push_to_osl;
  vgui_event_condition gesture_delete_object;
  vgui_event_condition gesture_delete_object2;

  vcl_vector<vcl_vector<vgui_soview2D_polygon*> > polygons_;  // the array that holds each object in each frame
  //vcl_vector<vcl_vector<vgui_soview2D_polygon*> > lvwr_polygons_;  // the array that holds improved contours
  vcl_vector<vcl_vector<dbru_label_sptr> > polygon_labels_;
  int frame_no_;
  int active_polygon_no_;
  bool activation_ok_;
  int video_id_;
  bool set_objects_;
  // the offset to start using polygons read from the file
  int offset_;
  // to store the observations in the memory
  bool memory_;
  /*bool lvwr_;
  osl_canny_ox_params canny_params_;
  dbdet_lvwr_params iparams_;
  */

  unsigned int start_frame_, end_frame_;

  vcl_vector<vcl_string> choices_;
  int choice_;
  dbru_label_sptr latest_label_;

  vcl_vector<dbru_object_sptr> objects_;
  vcl_vector<vcl_vector<vgui_style_sptr> > object_polygon_styles_;
  vcl_vector<vgui_style_sptr> object_styles_;
  int active_object_no_;
  //: create a map to handle styles better
  //     object_id                    frame_id  polygon_id in the frame
  vcl_map<unsigned, vcl_map<unsigned, unsigned>* > object_polygon_map_; 
  vgui_style_sptr active_style_, normal_style_, lvwr_style_;

private:

};


#endif // dbru_labeling_tool_h_
