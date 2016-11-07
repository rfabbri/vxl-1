// This is dbxml\dbxml_io.h
#ifndef dbxml_io_h_
#define dbxmo_io_h_
//:
// \file
// \author Terry ORechia,
// \date 5/10/04
// \Updated code in bbas/bxml project.
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <vcl_utility.h>
#include <vul/vul_arg.h>
#include <dbxml/dbxml_generic_ptr.h>
#include <dbxml/dbxml_input_converter.h>
#include <dbxml/dbxml_param_input_converter.h>
#include <dbxml/dbxml_input_converter_sptr.h>
#include <dbxml/dbxml_dom.h>


/*
// input methods
  
*/
//: Manager for creating VORL compatible executables from vidpro processes
class dbxml_io
{
 public:
  //: Destructor
  ~dbxml_io();
    //: Default Constructor
  dbxml_io();

  //: Parse the command line parameters
  static bool parse_xml(char *xmlfile,vcl_vector<dbxml_generic_ptr>& objs);
  //: get input converter
  static dbxml_input_converter_sptr find_converter_from_tag(const vcl_string& tag_name);
  static dbxml_input_converter_sptr find_input_converter(const vcl_string& class_name);
  static void register_input_converter(const dbxml_input_converter_sptr& conv);
  static void register_input_converters();



 private:
  //: Initialize the arguments from the process
 static DOMNode *getroot(char *xmlfile);

 static vcl_vector<dbxml_input_converter_sptr > input_converter_;
 // dbxml_generic_ptr extract_object_attrs(DOMNode *node);
 // void * extract_object_attrs(DOMNode *node);

 protected:

};

#endif // dbxml_io.h
