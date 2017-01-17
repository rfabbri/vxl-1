#ifndef dbxml_param_h_
#define dbxml_param_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author T Orechia
// \brief The class for parameter conversion.
//
// 
//-----------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>
#include <vcl_map.h>

//#include <vcl_map.h>

class dbxml_param : public vbl_ref_count
{
 public:
   dbxml_param(vcl_string command, vcl_string desc, vcl_string type, vcl_string value){
   command_=command;
   desc_=desc;
   type_=type;
   value_=value;
   };
   ~dbxml_param(){};

  vcl_string command_;
  vcl_string desc_;
  vcl_string type_;
  vcl_string value_;

  //:specific sub_class string names
 

 
#endif

};
