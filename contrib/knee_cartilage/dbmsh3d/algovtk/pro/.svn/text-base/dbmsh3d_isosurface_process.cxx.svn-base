//This is dbmsh3d/algovtk/pro/dbmsh3d_isosurface_process.cxx

//:
// \file


#include "dbmsh3d_isosurface_process.h"

#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/algovtk/dbmsh3d_isosurface.h>


// ----------------------------------------------------------------------------
//: Constructor
dbmsh3d_isosurface_process::
dbmsh3d_isosurface_process() : bpro1_process()
{
  if( !parameters()->add( "Number of frames (the current should be the last frame):" , 
    "-num_frames", (int)1 ) || 
    !parameters()->add( "mesh file <filename...>" , "-mesh_filename", 
    bpro1_filepath("","*") ) ||
    !parameters()->add( "Data spacing dx"   , "-dx" , 1.0f ) ||
    !parameters()->add( "Data spacing dy"   , "-dy" , 1.0f ) ||
    !parameters()->add( "Data spacing dz"   , "-dz" , 1.0f ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}
//
// ----------------------------------------------------------------------------
//: Destructor
dbmsh3d_isosurface_process::
~dbmsh3d_isosurface_process()
{
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbmsh3d_isosurface_process::
clone() const
{
  return new dbmsh3d_isosurface_process(*this);
}

// ----------------------------------------------------------------------------
//: Return the name of the process
vcl_string dbmsh3d_isosurface_process::
name()
{
  return "Isosurface";
}


// ----------------------------------------------------------------------------
//: Returns the number of input frames to this process
int dbmsh3d_isosurface_process::
input_frames()
{
  int num_frames = -1;
  this->parameters()->get_value( "-num_frames" , num_frames );
  if (num_frames < 0)
    vcl_cout << "ERROR: number of frames is non-negative.\n";
  return num_frames;
}


// ----------------------------------------------------------------------------
//: Returns the number of output frames from this process
int dbmsh3d_isosurface_process::
output_frames()
{
  return 0;
}



// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbmsh3d_isosurface_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("image");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbmsh3d_isosurface_process::
get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}


// ----------------------------------------------------------------------------
//: Run the process on the current frame
bool dbmsh3d_isosurface_process::
execute()
{
  // parse input data

  // mesh filename
   bpro1_filepath mesh_path;
  this->parameters()->get_value( "-mesh_filename" , mesh_path );    
  vcl_string mesh_filename = mesh_path.path;
  
  // data spacing
  float dx, dy, dz;
  this->parameters()->get_value( "-dx" , dx );    
  this->parameters()->get_value( "-dy" , dy );    
  this->parameters()->get_value( "-dz" , dz );   

  // number of frames
  int num_frames = -1;
  this->parameters()->get_value( "-num_frames" , num_frames );

  if (num_frames <= 0)
  {
    vcl_cerr << "In dbmsh3d_isosurface_process::execute() - number of frames " 
      << "must be a positive integer number.\n";
    return false;
  
  }

  if ((int)(this->input_data_.size()) < num_frames)
  {
    vcl_cerr << "In dbmsh3d_isosurface_process::execute() - not exactly " << num_frames
             << " input images \n";
    return false;
  }

  vcl_cout << "Grouping the images into volumetric data.\n";
  vil3d_image_view<vxl_byte > img3d;

  // determine size of the image
  vidpro1_image_storage_sptr top_frame;
  top_frame.vertical_cast(this->input_data_[0][0]);
  unsigned ni = top_frame->get_image()->ni();
  unsigned nj = top_frame->get_image()->nj();

  img3d.set_size(ni, nj, num_frames+2);
  img3d.fill(0);

  for (int i=0; i<num_frames; ++i)
  {
    vidpro1_image_storage_sptr image_storage;
    image_storage.vertical_cast(input_data_[num_frames-1-i][0]);
    vil_image_resource_sptr image_resource = image_storage->get_image();

    // get the grey view
    vil_image_view<vxl_byte> image;  
    if (image_resource->nplanes()==1)
    {
      image = *vil_convert_cast(vxl_byte(), image_resource->get_view());
    }
    else
    {
      image = *vil_convert_cast(vxl_byte(), 
      vil_convert_to_grey_using_rgb_weighting(image_resource->get_view()));
    }

    vil_image_view<bool> binary_img;

    // threshold the image to make sure that it is binary
    vxl_byte min_value = 0, max_value = 0;
    vil_math_value_range(image, min_value, max_value);
    vil_threshold_above<vxl_byte >(image, binary_img, (min_value+max_value+1)/2);

    // put the image to the volume data
    for (unsigned wi=0; wi<ni; ++wi)
    {
      for (unsigned int wj=0; wj<nj; ++wj)
      {
        img3d(wi, wj, i+1) = binary_img(wi, wj) ? 255 : 0 ;
      }
    }
  }

  vcl_cout << "Build mesh of isosurface.\n";
  dbmsh3d_mesh mesh;
  dbmsh3d_isosurface(mesh, img3d, 127, dx, dy, dz);

  // translate the mesh so that its z values match with the frame number
  vgl_h_matrix_3d<double > h;
  h.set_identity();
  h.set_translation(0, 0, dz* (top_frame->frame()-num_frames));
  
  vcl_cout << "Translate mesh to match with its frame number.\n";
  dbmsh3d_apply_xform (&mesh, h);
  
  vcl_cout << "Save mesh.\n";
  dbmsh3d_save_ply(&mesh, mesh_filename.c_str(), false);
  vcl_cout << "Done.\n";
  

  return true;
}


// ----------------------------------------------------------------------------
//: Finish
bool dbmsh3d_isosurface_process::
finish() 
{
  return true;
}





