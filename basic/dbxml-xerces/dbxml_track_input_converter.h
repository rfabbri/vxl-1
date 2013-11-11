//this-sets-emacs-to-*-c++-*-mode
#ifndef dbxml_track_input_converter_h_
#define dbxml_track_input_converter_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A converter for reading a track XML file from an XML DOM Tree
//
// \author
//   T. Orechia
//
// \verbatim
// \endverbatim
//--------------------------------------------------------------------------------
#include "dbxml_input_converter.h"

//class DOM_Node;

class dbxml_track_input_converter : public dbxml_input_converter
{
 public:
  dbxml_track_input_converter();
  ~dbxml_track_input_converter();
    //static BXML_DLL_DATA vcl_vector<bxml_input_converter_sptr> input_converters_;

  vcl_string& id(){return id_;}
  //: virtual methods
 // virtual bool extract_ref_object_atrs(DOMNode * node);
  virtual bool extract_object_atrs(DOMNode   * node);
  virtual bool extract_track_atrs(DOMNode *node);
  virtual bool extract_window_atrs(DOMNode *node);

 // virtual bool extract_from_dom(DOMNode * node);
  virtual bool extract_from_dom_1(DOMNode *node) ;
   virtual bool extract_from_dom_2(DOMNode *node) ;
   virtual bool extract_from_dom_3(DOMNode *node);
  
  //: construct a new object instance
  virtual dbxml_generic_ptr construct_object_1(vcl_vector<dbxml_generic_ptr>& objs);

  virtual dbxml_generic_ptr construct_object_2(vcl_vector<dbxml_generic_ptr>& objs);
  virtual dbxml_generic_ptr construct_object_3(vcl_vector<dbxml_generic_ptr>& objs);
 int numframes_;
 int startframe_;
 int width_;
 int height_;
 float x_;
 float y_;
 vcl_vector<dbxml_generic_ptr> tracks_;
 int total_tracks_;
 //vcl_string track_;

 private:
  //members
  int new_or_ref;
  vcl_string id_;
 };

#endif // dbxml_track_input_converter_h_
