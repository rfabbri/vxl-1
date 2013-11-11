// This is  brcv/rec/dbru/dbru_label.cxx
//:
// \file

#include "dbru_label.h"

//------------------------------------------------------------------------
// Constructors
//

dbru_label::
dbru_label(const dbru_label& tp)
{
  Init(tp.category_name_,
       tp.motion_orientation_bin_,
       tp.view_angle_bin_,
       tp.shadow_angle_bin_,
       tp.shadow_length_);
}

dbru_label::
dbru_label(const vcl_string name,
          const int motion_bin,
          const int view_bin,
          const int shadow_bin,
          const int shadow_length)
{
  Init(name,
       motion_bin,
       view_bin,
       shadow_bin,
       shadow_length);
}

void dbru_label::Init(vcl_string name,
                     int motion_bin,
                     int view_bin,
                     int shadow_bin,
                     int shadow_length)
{
  category_name_ = name;  
  motion_orientation_bin_ = motion_bin;    
  view_angle_bin_ = view_bin;      
  shadow_angle_bin_ = shadow_bin;  
  shadow_length_ = shadow_length; 
}

dbru_label& dbru_label::operator=(const dbru_label& right) 
{
  if (this != &right) {
    category_name_ = right.category_name_;
    motion_orientation_bin_ = right.motion_orientation_bin_;
    view_angle_bin_ = right.view_angle_bin_;
    shadow_angle_bin_ = right.shadow_angle_bin_;
    shadow_length_ = right.shadow_length_;
  }

  return *this;
}

void dbru_label::print(vcl_ostream& os) const
{
  os << "category: " << category_name_ << " "
     << "orientation: " << motion_orientation_bin_ << " "
     << "view_angle: " << view_angle_bin_ << " "
     << "shadow_angle: " << shadow_angle_bin_ << " "
     << "shadow_length: " << shadow_length_ << "\n";
}

//: Binary save self to stream.
void dbru_label::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->category_name_);
  vsl_b_write(os, this->motion_orientation_bin_);
  vsl_b_write(os, this->view_angle_bin_);
  vsl_b_write(os, this->shadow_angle_bin_);
  vsl_b_write(os, this->shadow_length_);
}

//: Binary load self from stream.
void dbru_label::b_read(vsl_b_istream &is)
{
  unsigned int ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        vsl_b_read(is, category_name_);
        vsl_b_read(is, motion_orientation_bin_);
        vsl_b_read(is, view_angle_bin_);
        vsl_b_read(is, shadow_angle_bin_);
        vsl_b_read(is, shadow_length_);
      }
    }
}

