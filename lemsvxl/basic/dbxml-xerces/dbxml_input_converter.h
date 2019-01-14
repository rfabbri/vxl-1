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
#include <string>
#include <map>
#include "dbxml_dom.h"
#include "dbxml_generic_ptr.h"
#include "dll.h"

class dbxml_input_converter : public vbl_ref_count
{
 public:
  dbxml_input_converter();
  ~dbxml_input_converter();

  //:specific sub_class string names
  std::string get_tag_name_1() {return tag_name_1_;}
  std::string get_tag_name_2() {return tag_name_2_;}
  std::string get_tag_name_3() {return tag_name_3_;}
  std::string get_class_name() {return class_name_;}
  std::string get_ref_tag_name() {return ref_tag_name_;}

  //:helper functions
  static std::string get_DOM_tag(DOMNode *node);
  int check_tag(DOMNode *node,int level);
  int check_tag(DOMNode *node);
  bool getNextElementSibling(DOMNode * node);
 
  //DOMNode getChild(DOMNode& node,std::string tname);


  //:attribute access and conversion
  bool has_attr(DOMNode * node, std::string attr_name);
  std::string get_string_attr(DOMNode *node,std::string attr_name);
  int    get_int_attr(DOMNode *node,std::string attr_name);
  float get_float_attr(DOMNode *node ,std::string attr_name);
  bool   get_bool_attr(DOMNode *node,std::string attr_name);

  //:basic conversion methods
  virtual bool extract_object_attrs(DOMNode * /*node*/) {return false;}
//  virtual bool extract_from_dom(DOMNode * /*node*/) = 0;
  virtual bool extract_from_dom_1(DOMNode * /*node*/) = 0;
  virtual bool extract_from_dom_2(DOMNode * /*node*/) = 0;
  virtual bool extract_from_dom_3(DOMNode * /*node*/) = 0;
  virtual dbxml_generic_ptr construct_object_1(std::vector<dbxml_generic_ptr>& objs)=0;
  virtual dbxml_generic_ptr construct_object_2(std::vector<dbxml_generic_ptr>& objs)=0;
  virtual dbxml_generic_ptr construct_object_3(std::vector<dbxml_generic_ptr>& objs)=0;




  //:utilities
  void set_debug(bool debug){debug_=debug;}
  //:clear spatial object instances in the table. move lower in hierarchy JLM
  static void clear();
 protected:
  bool debug_;
  std::string  null_id_;
  static std::map<std::string,dbxml_generic_ptr,std::less<std::string> >  obj_table_;
  std::string class_name_;
  std::string tag_name_1_;
  std::string tag_name_2_;
  std::string tag_name_3_;
  std::string ref_tag_name_;
};

#endif
