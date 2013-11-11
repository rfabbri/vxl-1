// This is dbdet/pro/dbdet_load_seg3d_info_xml_process.cxx

//:
// \file

#include "dbdet_load_seg3d_info_xml_process.h"

#include <vil/vil_load.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <bpro1/bpro1_parameters.h>
#include <dbsol/dbsol_file_io.h>
#include <dbdet/xio/dbdet_xio_seg3d_info.h>
#include <dbdet/pro/dbdet_seg3d_info_storage.h>
#include <dbdet/pro/dbdet_seg3d_info_storage_sptr.h>



// ----------------------------------------------------------------------------
//: Constructor
dbdet_load_seg3d_info_xml_process::
dbdet_load_seg3d_info_xml_process() : bpro1_process()
{
  if( !parameters()->add( "xml file <filename...>" , "-xml_filename", 
    bpro1_filepath("","*") ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbdet_load_seg3d_info_xml_process::
~dbdet_load_seg3d_info_xml_process()
{
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbdet_load_seg3d_info_xml_process::
clone() const
{
  return new dbdet_load_seg3d_info_xml_process(*this);
}

// ----------------------------------------------------------------------------
//: Return the name of the process
vcl_string dbdet_load_seg3d_info_xml_process::
name()
{
  return "load volume seg. xml";
}


// ----------------------------------------------------------------------------
void dbdet_load_seg3d_info_xml_process::
clear_output(int resize)
{
  this->set_num_frames(0);
  bpro1_process::clear_output(resize);
}


// ----------------------------------------------------------------------------
//: Returns the number of input frames to this process
int dbdet_load_seg3d_info_xml_process::
input_frames()
{
  return 0;
}


// ----------------------------------------------------------------------------
//: Returns the number of output frames from this process
int dbdet_load_seg3d_info_xml_process::
output_frames()
{
  return this->num_frames();
}



// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbdet_load_seg3d_info_xml_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;

  // no input type required
  to_return.clear();
  return to_return;
}


// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbdet_load_seg3d_info_xml_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;

  // output type
  to_return.push_back( "vsol2D" );
  to_return.push_back( "image" );
  to_return.push_back( "seg3d_info" );
  return to_return;
}


// ----------------------------------------------------------------------------
//: Run the process on the current frame
bool dbdet_load_seg3d_info_xml_process::
execute()
{
  this->clear_output();
  bpro1_filepath xml_path;
  this->parameters()->get_value( "-xml_filename" , xml_path );    
  vcl_string xml_filename = xml_path.path;

  dbdet_seg3d_info_sptr seg3d = new dbdet_seg3d_info();

  
  x_read(xml_filename, seg3d);

  vcl_vector<vidpro1_image_storage_sptr > image_storage_list;
  vcl_vector<vidpro1_vsol2D_storage_sptr > vsol_storage_list;

  for (int i=0; i<seg3d->num_frames(); ++i)
  {
    dbdet_seg3d_info_frame frame = seg3d->frame(i);

    // 1) load image
    vcl_string image_filename = seg3d->image_folder() + 
      "/" + frame.image_file;

    vil_image_resource_sptr loaded_image = 
      vil_load_image_resource( image_filename.c_str() );
    if( !loaded_image ) 
    {
      vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
      return false;
    }

    // create a storage for the image
    vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();
    image_storage->set_image( loaded_image );
    
    // push to the storage vector
    image_storage_list.push_back(image_storage);

    // 2) load the contours
    vcl_vector<vsol_spatial_object_2d_sptr > contour_list;
    contour_list.reserve(frame.contour_file_list.size());
    for (unsigned int k=0; k<frame.contour_file_list.size(); ++k)
    {
      // full filename
      vcl_string contour_filename = seg3d->contour_folder() + 
        "/" + frame.contour_file_list[k];

      // load the contour
      vsol_spatial_object_2d_sptr contour = dbsol_load_con_file(
        contour_filename.c_str());

      if (! contour)
      {
        vcl_cerr << "Failed to load contour file" << contour_filename << vcl_endl;
        return false;
      }
      contour_list.push_back(contour);
    }

    // create the storage class for contours
    vidpro1_vsol2D_storage_sptr vsol_storage = vidpro1_vsol2D_storage_new();
    vsol_storage->add_objects(contour_list, frame.image_file);

    // push to the storage vector
    vsol_storage_list.push_back(vsol_storage);
  }

  // Now everything has been loaded sucessfully, save them to output data
  for (int i=seg3d->num_frames()-1; i>=0; --i)
  {
    vcl_vector<bpro1_storage_sptr > frame_storage;
    frame_storage.push_back(vsol_storage_list[i]);
    frame_storage.push_back(image_storage_list[i]);
    output_data_.push_back(frame_storage);
  }
  this->set_num_frames(seg3d->num_frames());

  // global storage
  dbdet_seg3d_info_storage_sptr seg3d_info = dbdet_seg3d_info_storage_new();
  seg3d_info->set_seg3d_info(seg3d);
  seg3d_info->set_frame(-2);
  seg3d_info->set_vsol_storage_list(vsol_storage_list);
  output_data_[0].push_back(seg3d_info);

  return true;
}


// ----------------------------------------------------------------------------
//: Finish
bool dbdet_load_seg3d_info_xml_process::
finish() 
{
  return true;
}
