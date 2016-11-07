//
// This is basic/dbxml/dbxml_track_input_converter.cxx
//:
// \file
///

#include "dbxml_io.h"
#include <vcl_string.h>
#include <vcl_iostream.h> //cout
#include <vcl_cmath.h>
#include "dbxml_input_converter.h"
#include "dbxml_track_input_converter.h"
#include "dbxml_track.h"
#include "dbxml_tracks.h"




dbxml_track_input_converter::dbxml_track_input_converter() {
  class_name_ = "dbxml_track";
  tag_name_1_ = "object";
  tag_name_2_ = "window";
  tag_name_3_ = "track";
  width_=100;
  height_=100;
  total_tracks_=0;
}

dbxml_track_input_converter::~dbxml_track_input_converter() {
}


bool dbxml_track_input_converter::extract_object_atrs(DOMNode *node) {
  numframes_ = get_int_attr(node,"numframes");
  startframe_ = get_int_attr(node,"startframe");
 /* if (strcmp(type.c_str(),"float") ==0)
        float value_ = get_float_attr(node,"value");
  else if (strcmp(type.c_str(),"integer")==0)
        int value = get_int_attr(node,"value");
  else if (strcmp(type.c_str(),"flag")==0){
                vcl_string stringvalue = get_string_attr(node,"value");
        if (strcmp(stringvalue.c_str(), "on")== 0)
                        bool value = true;
                else
                        bool value = false;
                  }
  else
        vcl_string value = get_string_attr(node,"value");
*/
  if (numframes_ ==0) //cannot have an object with no frames
{
    vcl_cout << "dbxml_track_input_converter:: Error, object tag missing required field numframes\n";
    return(false);
  } 
  return true;
}
bool dbxml_track_input_converter::extract_window_atrs(DOMNode *node) {
  width_ = static_cast<int>(vcl_floor(get_float_attr(node,"width")+0.5));
  height_ = static_cast<int>(vcl_floor(get_float_attr(node,"height")+0.5));
 /* if (strcmp(type.c_str(),"float") ==0)
        float value_ = get_float_attr(node,"value");
  else if (strcmp(type.c_str(),"integer")==0)
        int value = get_int_attr(node,"value");
  else if (strcmp(type.c_str(),"flag")==0){
                vcl_string stringvalue = get_string_attr(node,"value");
        if (strcmp(stringvalue.c_str(), "on")== 0)
                        bool value = true;
                else
                        bool value = false;
                  }
  else
        vcl_string value = get_string_attr(node,"value");
*/
  return true;
}

bool dbxml_track_input_converter::extract_track_atrs(DOMNode *node) {
  x_ = get_float_attr(node,"x");
  y_ = get_float_attr(node,"y");
 /* if (strcmp(type.c_str(),"float") ==0)
        float value_ = get_float_attr(node,"value");
  else if (strcmp(type.c_str(),"integer")==0)
        int value = get_int_attr(node,"value");
  else if (strcmp(type.c_str(),"flag")==0){
                vcl_string stringvalue = get_string_attr(node,"value");
        if (strcmp(stringvalue.c_str(), "on")== 0)
                        bool value = true;
                else
                        bool value = false;
                  }
  else
        vcl_string value = get_string_attr(node,"value");
*/
  if (x_ == 0 || y_ == 0){
        vcl_cout << "dbxml_track_input_converter:: Error, track tag missing required fields.\n";
    return(false);
  }
  return true;
}
bool dbxml_track_input_converter::extract_from_dom_1(DOMNode *node) {
  new_or_ref = check_tag(node,1);

  if (new_or_ref == 0) {
    vcl_cout << "dbxml_track_input_converter:: Error, bad tag\n";
    return false;
  }
  bool bExtract =  extract_object_atrs(node);

  return bExtract;
}
bool dbxml_track_input_converter::extract_from_dom_2(DOMNode *node) {
  new_or_ref = check_tag(node,2);

  if (new_or_ref == 0) {
    vcl_cout << "dbxml_track_input_converter:: Error, bad tag\n";
    return false;
  }
     extract_window_atrs(node);

  return true;
}

bool dbxml_track_input_converter::extract_from_dom_3(DOMNode *node) {
  new_or_ref = check_tag(node,3);

  if (new_or_ref == 0) {
    vcl_cout << "dbxml_track_input_converter:: Error, bad tag\n";
    return false;
  }
    bool bExtract = extract_track_atrs(node);

  return bExtract;
}


//:
// Construct the object if it is not already in the object table
// It will be in the object table if it has already been constructed
// and the current occurrence is just a ref in the xml file.
// WARNING!!
// There is an issue with the use of the generic pointer, since we
// have no way to reference count the pointer. We might be able to
// deal with the problem in the destructor of the generic pointer

dbxml_generic_ptr dbxml_track_input_converter::construct_object_1(vcl_vector<dbxml_generic_ptr>& objs)
{
//  if (new_or_ref == 1) {
  if (numframes_ != total_tracks_) {
    vcl_cout << "dbxml_track_input_converter:: Error, the number of frame tags does not match numframes.\n";
    return NULL;
  }
  dbxml_track *p = new dbxml_track(startframe_,numframes_,width_,height_,tracks_);
  dbxml_generic_ptr gp(p);
  objs.push_back(p);
  total_tracks_=0;


        /*
    if ( !(id_ == null_id_) ) {
     obj_table_[id_] = gp;
     p->ref();//Keep the point alive until it is used
    }
        */
return(p);
// }
//  return NULL;
}
dbxml_generic_ptr dbxml_track_input_converter::construct_object_2(vcl_vector<dbxml_generic_ptr>& objs)
{
  return NULL;
}
dbxml_generic_ptr dbxml_track_input_converter::construct_object_3(vcl_vector<dbxml_generic_ptr>& objs)
{
  if (new_or_ref != 1 && (height_ != 0 || width_!=0)){ // only create tracks if there is a window tag 

        dbxml_tracks *p = new dbxml_tracks(x_,y_);
    dbxml_generic_ptr gp(p);
    tracks_.push_back(p);
    total_tracks_++;
        /*
    if ( !(id_ == null_id_) ) {
     obj_table_[id_] = gp;
     p->ref();//Keep the point alive until it is used
    }
        */
return(p);
 }
  return NULL;
}

/*
void dbxml_track_input_converter::update_object(dbxml_generic_ptr p)
{
  ((dbxml_tracks *)p)->height = height_;
  ((dbxml_tracks *)p)->width = width_;
}
*/


