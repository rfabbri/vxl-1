// This is dbdet/pro/dbdet_seg3d_info_storage.cxx

//:
// \file

#include "dbdet_seg3d_info_storage.h"

//: Constructor
dbdet_seg3d_info_storage::
dbdet_seg3d_info_storage()
{
}


//: Destructor
dbdet_seg3d_info_storage::
~dbdet_seg3d_info_storage()
{
}


//: Return IO version number;
short dbdet_seg3d_info_storage::
version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbdet_seg3d_info_storage::
clone() const
{
  return new dbdet_seg3d_info_storage(*this);
}


//: Binary save self to stream.
void dbdet_seg3d_info_storage::
b_write(vsl_b_ostream &os) const
{
  ////
  //vsl_b_write(os, version());
  //bpro1_storage::b_write(os);
  //if(image_){
  //  vsl_b_write(os, true);
  //  vsl_b_write(os, int(image_->pixel_format()));
  //  switch(image_->pixel_format())
  //  {
  //  case VIL_PIXEL_FORMAT_BYTE:
  //    {  
  //      vil_image_view<vxl_byte> image = this->image_->get_view();
  //      vsl_b_write(os, image);
  //      break;
  //    }
  //  case VIL_PIXEL_FORMAT_FLOAT:
  //    {  
  //      vil_image_view<float> image = this->image_->get_view();
  //      vsl_b_write(os, image);
  //      break;
  //    }
  //  case VIL_PIXEL_FORMAT_DOUBLE:
  //    {  
  //      vil_image_view<double> image = this->image_->get_view();
  //      vsl_b_write(os, image);
  //      break;
  //    }    
  //  default:
  //    vcl_cerr << "I/O ERROR: This pixel format is not supported\n";
  //    return; 
  //  }
  //}
  //else
  //  vsl_b_write(os, false);
}


//: Binary load self from stream.
void dbdet_seg3d_info_storage::
b_read(vsl_b_istream &is)
{

  ////
  //if (!is) return;

  //short ver;
  //vsl_b_read(is, ver);
  //switch(ver)
  //{
  //case 1:
  //{
  //  bpro1_storage::b_read(is);
  //  vil_image_view<vxl_byte> image;
  //  vsl_b_read(is, image);
  //  this->image_ = vil_new_image_resource_of_view(image);
  //  break;
  //}
  //case 2:
  //{
  //  bpro1_storage::b_read(is);
  //  bool is_valid;
  //  vsl_b_read(is, is_valid);
  //  if(is_valid){
  //    vil_image_view<vxl_byte> image;
  //    vsl_b_read(is, image);
  //    this->image_ = vil_new_image_resource_of_view(image);
  //  }
  //  else
  //    this->image_ = NULL;
  //  break;
  //}
  //case 3:
  //{
  //  bpro1_storage::b_read(is);
  //  bool is_valid;
  //  vsl_b_read(is, is_valid);
  //  if(is_valid){
  //    int format;
  //    vil_pixel_format pixel_format;
  //    vsl_b_read(is, format); pixel_format=vil_pixel_format(format);
  //    switch(pixel_format)
  //    {
  //    case VIL_PIXEL_FORMAT_BYTE:
  //      {
  //        vil_image_view<vxl_byte> image;
  //        vsl_b_read(is, image);
  //        this->image_ = vil_new_image_resource_of_view(image);
  //        break;
  //      }
  //    case VIL_PIXEL_FORMAT_FLOAT:
  //      {
  //        vil_image_view<float> image;
  //        vsl_b_read(is, image);
  //        this->image_ = vil_new_image_resource_of_view(image);
  //        break;
  //      }
  //    case VIL_PIXEL_FORMAT_DOUBLE:
  //      {
  //        vil_image_view<double> image;
  //        vsl_b_read(is, image);
  //        this->image_ = vil_new_image_resource_of_view(image);
  //        break;
  //      }      
  //    default:
  //      vcl_cerr << "I/O ERROR: This pixel format is not supported\n";
  //      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  //      return; 
  //    }
  //    
  //  }
  //  else
  //    this->image_ = NULL;
  //  break;
  //}

  //default:
  //  vcl_cerr << "I/O ERROR: dbdet_seg3d_info_storage::b_read(vsl_b_istream&)\n"
  //           << "           Unknown version number "<< ver << '\n';
  //  is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  //  return;
  //}
}

