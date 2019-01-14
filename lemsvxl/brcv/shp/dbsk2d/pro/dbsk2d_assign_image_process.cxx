// This is brcv/shp/dbsk2d/pro/dbsk2d_assign_image_process.cxx

//:
// \file

#include "dbsk2d_assign_image_process.h"

#include "dbsk2d_shock_storage.h"
#include "dbsk2d_shock_storage_sptr.h"
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>

dbsk2d_assign_image_process::dbsk2d_assign_image_process()
{
}

dbsk2d_assign_image_process::~dbsk2d_assign_image_process()
{
}


//: Clone the process
bpro1_process* dbsk2d_assign_image_process::clone() const
{
  return new dbsk2d_assign_image_process(*this);
}


std::string dbsk2d_assign_image_process::name()
{
  return "Assign an Image";
}

std::vector< std::string > dbsk2d_assign_image_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "shock" );
  return to_return;
}

std::vector< std::string > dbsk2d_assign_image_process::get_output_type()
{
  std::vector< std::string > to_return;
  return to_return;
}

int
dbsk2d_assign_image_process::input_frames()
{
  return 1;
}

int
dbsk2d_assign_image_process::output_frames()
{
  return 0;
}

bool
dbsk2d_assign_image_process::execute()
{
  // get input storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  dbsk2d_shock_storage_sptr shock;
  shock.vertical_cast(input_data_[0][1]);

  if (frame_image){
    vil_image_resource_sptr image_sptr = frame_image->get_image();
    //shock->set_image(image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj()));
    shock->set_image(image_sptr);
  }

  return true;
}

bool
dbsk2d_assign_image_process::finish()
{
  return true;
}


