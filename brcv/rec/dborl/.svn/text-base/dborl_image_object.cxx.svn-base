//:
// \file
// \brief 
// \author     Ozge Can Ozcanli (ozge@lems.brown.edu)
// \date        10/03/07
#include "dborl_image_object.h"

#include <dborl/dborl_image_description.h>

dborl_image_object::dborl_image_object(dborl_image_object& rhs) 
{
  image_ = rhs.image_;
  desc_ = rhs.desc_;
  name_ = rhs.name_;
}

void dborl_image_object::set_image(vil_image_resource_sptr img)
{
  image_ = img;
}

void dborl_image_object::set_description(dborl_image_description_sptr t)
{
  desc_ = t->cast_to_description_base();
}

dborl_image_description_sptr dborl_image_object::get_description()
{
  return desc_->cast_to_image_description();
}

