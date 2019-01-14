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
#include <string>
#include <map>

//#include <map>

class dbxml_param : public vbl_ref_count
{
 public:
   dbxml_param(std::string command, std::string desc, std::string type, std::string value){
   command_=command;
   desc_=desc;
   type_=type;
   value_=value;
   };
   ~dbxml_param(){};

  std::string command_;
  std::string desc_;
  std::string type_;
  std::string value_;

  //:specific sub_class string names
 

 
#endif

};
