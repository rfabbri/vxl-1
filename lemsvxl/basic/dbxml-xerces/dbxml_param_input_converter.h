//this-sets-emacs-to-*-c++-*-mode
#ifndef bxml_param_input_converter_h_
#define bxml_param_input_converter_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A converter for reading a TargetJR XML version of param from an XML DOM Tree
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 07, 2002    Initial version.
//   Based on the TargetJr design by R. Kaucic
//   T Orechia May 16, 2004 updated version
// \endverbatim
//--------------------------------------------------------------------------------
#include "dbxml_input_converter.h"

//class DOM_Node;

class dbxml_param_input_converter : public dbxml_input_converter
{
 public:
  dbxml_param_input_converter();
  ~dbxml_param_input_converter();
    //static BXML_DLL_DATA vcl_vector<bxml_input_converter_sptr> input_converters_;

  vcl_string& id(){return id_;}
  //: virtual methods
  // virtual bool extract_ref_object_atrs(DOMNode * node);
  virtual bool extract_object_atrs(DOMNode   * node);
//  virtual bool extract_from_dom(DOMNode * node);
  virtual bool extract_from_dom_1(DOMNode *node) ;
   virtual bool extract_from_dom_2(DOMNode *node) ;
   virtual bool extract_from_dom_3(DOMNode *node);

  virtual dbxml_generic_ptr construct_object_2(vcl_vector<dbxml_generic_ptr>& objs);
  virtual dbxml_generic_ptr construct_object_3(vcl_vector<dbxml_generic_ptr>& objs);


  //: construct a new object instance
  virtual dbxml_generic_ptr construct_object_1(vcl_vector<dbxml_generic_ptr>& objs);
 vcl_string command_;
 vcl_string desc_;
  vcl_string type_;
  vcl_string value_;

 private:
  //members
  int new_or_ref;
  vcl_string id_;
 };

#endif // bxml_param_input_converter_h_
