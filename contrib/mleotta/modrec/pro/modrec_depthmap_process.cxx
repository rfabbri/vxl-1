// This is contrib/mleotta/modrec/pro/modrec_depthmap_process.cxx

//:
// \file

#include "modrec_depthmap_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>
#include <vcl_limits.h>


#include <vpgl/vpgl_proj_camera.h>
#include <imesh/imesh_fileio.h>
#include <imesh/imesh_mesh.h>
#include <imesh/algo/imesh_project.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_triangle_2d.h>

#include "modrec_pro_utils.h"


//: Constructor
modrec_depthmap_process::modrec_depthmap_process()
{
  if( !parameters()->add( "Model" ,     "-model" ,  bpro1_filepath("","*")  ) ||
      !parameters()->add( "Camera" ,    "-camera" , bpro1_filepath("","*")  ) ||
      !parameters()->add( "ni" ,        "-ni" ,     int(1024) ) ||
      !parameters()->add( "nj" ,        "-nj" ,     int(768)  ) ||
      !parameters()->add( "mean bg" ,   "-mean_bg", false  ) ||
      !parameters()->add( "invert" ,    "-invert" , false  ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
modrec_depthmap_process::~modrec_depthmap_process()
{
}


//: Clone the process
bpro1_process* 
modrec_depthmap_process::clone() const
{
  return new modrec_depthmap_process(*this);
}


//: Return the name of this process
vcl_string
modrec_depthmap_process::name()
{
  return "Model Depth Map";
}


//: Return the number of input frame for this process
int
modrec_depthmap_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
modrec_depthmap_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > modrec_depthmap_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > modrec_depthmap_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );

  return to_return;
}


//: Returns a vector of strings with suggested names for output classes
vcl_vector< vcl_string > modrec_depthmap_process::suggest_output_names()
{
  vcl_vector< vcl_string > names;
  names.push_back("depth map");

  return names;
}


//: Execute the process
bool
modrec_depthmap_process::execute()
{
  clear_output();

  // get the process parameters
  bpro1_filepath model_file, camera_file;
  int ini,inj;
  bool invert, mean_bg;
  parameters()->get_value( "-model",  model_file);
  parameters()->get_value( "-camera", camera_file);
  parameters()->get_value( "-ni",     ini);
  parameters()->get_value( "-nj",     inj);
  parameters()->get_value( "-invert", invert);
  parameters()->get_value( "-mean_bg",mean_bg);
  
  unsigned int ni = static_cast<unsigned int>(ini);
  unsigned int nj = static_cast<unsigned int>(inj);

  vil_image_view<double> depths(ni,nj);

  imesh_mesh mesh;
  if(!imesh_read_obj(model_file.path,mesh))
    return false;

  vnl_double_3x4 camera;
  vcl_fstream fh(camera_file.path.c_str());
  fh >> camera;
  fh.close();

  imesh_project_depth(mesh, vpgl_proj_camera<double>(camera), depths);

  if(mean_bg){
    double min_val = vcl_numeric_limits<double>::infinity();
    double max_val = -min_val;
    for (unsigned i=0; i<ni; ++i){
      for (unsigned j=0; j<nj; ++j){
        const double& pixel = depths(i,j);
        if(pixel != vcl_numeric_limits<double>::infinity()){
          if(pixel > max_val) max_val = pixel;
          if(pixel < min_val) min_val = pixel;
        }
      }
    }
    double mean = (min_val + max_val)/2.0;
    for (unsigned i=0; i<ni; ++i)
      for (unsigned j=0; j<nj; ++j){
        if(depths(i,j) == vcl_numeric_limits<double>::infinity())
          depths(i,j) = mean;
      }
  }

  if(invert){
    for (unsigned i=0; i<ni; ++i)
      for (unsigned j=0; j<nj; ++j)
          depths(i,j) = 1.0/depths(i,j);
  }


  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view( depths ));
  output_data_[0].push_back(output_storage);

  return true;
}



bool
modrec_depthmap_process::finish()
{

  return true;
}




