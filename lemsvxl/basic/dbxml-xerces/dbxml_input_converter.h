#ifndef dbxml_input_converter_h_
#define dbxml_input_converter_h_
//-----------------------------------------------------------------------------
//:
// \file
// \Original author J.L. Mundy
// \brief The base class for xml input conversion in bbas/bxml.
//  
//
// The current clear operation explicitly uses vsol_spatial_object_2d
// class methods.  These clear operations should be lower in the
// class hierarchies.  When more class roots are needed the clear should
// be abstracted.
//
// \verbatim
//  Initial version December 07, 2002
//  Based on the TargetJr design by R. Kaucic
//  Updated on May 16,2004 for dbxml
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include "dbxml_dom.h"
#include "dbxml_generic_ptr.h"
#include "dll.h"

class dbxml_input_converter : public vbl_ref_count
{
 public:
  dbxml_input_converter();
  ~dbxml_input_converter();

  //:specific sub_class string names
  vcl_string get_tag_name_1() {return tag_name_1_;}
  vcl_string get_tag_name_2() {return tag_name_2_;}
  vcl_string get_tag_name_3() {return tag_name_3_;}
  vcl_string get_class_name() {return class_name_;}
  vcl_string get_ref_tag_name() {return ref_tag_name_;}

  //:helper functions
  static vcl_string get_DOM_tag(DOMNode *node);
  int check_tag(DOMNode *node,int level);
  int check_tag(DOMNode *node);
  bool getNextElementSibling(DOMNode * node);
 
  //DOMNode getChild(DOMNode& node,vcl_string tname);


  //:attribute access and conversion
  bool has_attr(DOMNode * node, vcl_string attr_name);
  vcl_string get_string_attr(DOMNode *node,vcl_string attr_name);
  int    get_int_attr(DOMNode *node,vcl_string attr_name);
  float get_float_attr(DOMNode *node ,vcl_string attr_name);
  bool   get_bool_attr(DOMNode *node,vcl_string attr_name);

  //:basic conversion methods
  virtual bool extract_object_attrs(DOMNode * /*node*/) {return false;}
//  virtual bool extract_from_dom(DOMNode * /*node*/) = 0;
  virtual bool extract_from_dom_1(DOMNode * /*node*/) = 0;
  virtual bool extract_from_dom_2(DOMNode * /*node*/) = 0;
  virtual bool extract_from_dom_3(DOMNode * /*node*/) = 0;
  virtual dbxml_generic_ptr construct_object_1(vcl_vector<dbxml_generic_ptr>& objs)=0;
  virtual dbxml_generic_ptr construct_object_2(vcl_vector<dbxml_generic_ptr>& objs)=0;
  virtual dbxml_generic_ptr construct_object_3(vcl_vector<dbxml_generic_ptr>& objs)=0;




  //:utilities
  void set_debug(bool debug){debug_=debug;}
  //:clear spatial object instances in the table. move lower in hierarchy JLM
  static void clear();
 protected:
  bool debug_;
  vcl_string  null_id_;
  static vcl_map<vcl_string,dbxml_generic_ptr,vcl_less<vcl_string> >  obj_table_;
  vcl_string class_name_;
  vcl_string tag_name_1_;
  vcl_string tag_name_2_;
  vcl_string tag_name_3_;
  vcl_string ref_tag_name_;
};

#endif
