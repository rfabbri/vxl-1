//:
// \file
// \brief base class for ORL objects
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/03/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#if !defined(_DBORL_OBJECT_BASE_H)
#define _DBORL_OBJECT_BASE_H

#include "dborl_description_base.h"
#include "dborl_description_base_sptr.h"

class dborl_image_object;

class dborl_object_base : public vbl_ref_count
{
public:
  vcl_string name_;
  vcl_string path_;
  dborl_description_base_sptr desc_;

  inline dborl_object_base() : name_("") {}
  inline dborl_object_base(vcl_string name) : name_(name) {}

  inline void set_path(vcl_string path) { path_ = path; }
  inline vcl_string path() { return path_; }

  virtual unsigned get_object_type() = 0;

  virtual dborl_object_base* cast_to_object_base() { return this; }
  virtual dborl_image_object* cast_to_image_object() = 0;
};

#endif  //_DBORL_OBJECT_BASE_H
