//:
// \file
// \brief Class that ..
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/03/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#if !defined(_DBORL_IMAGE_OBJECT_H)
#define _DBORL_IMAGE_OBJECT_H

#include "dborl_object_base.h"
#include <dborl/dborl_image_description_sptr.h>

#include <vil/vil_image_resource_sptr.h>

class dborl_image_object : public dborl_object_base
{
public:
  vil_image_resource_sptr image_;
  
  inline dborl_image_object() : dborl_object_base(), image_(0) {}
  inline dborl_image_object(vil_image_resource_sptr img) : dborl_object_base(), image_(img) {}
  inline dborl_image_object(vcl_string name, vil_image_resource_sptr img) : dborl_object_base(name), image_(img) {}
  dborl_image_object(dborl_image_object& rhs);

  virtual unsigned get_object_type() { return dborl_object_type::image; }

  void set_image(vil_image_resource_sptr img);
  void set_description(dborl_image_description_sptr t);
  dborl_image_description_sptr get_description();

  virtual dborl_image_object* cast_to_image_object() { return this; }
};

#endif  //_DBORL_IMAGE_OBJECT_H
