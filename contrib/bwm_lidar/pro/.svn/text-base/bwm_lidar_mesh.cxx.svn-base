//:
// \file
// \brief Process for lidar image meshing
//
// \author Ming-Ching Chang (mcchang@lems.brown.edu)
// \date 11/29/2007
//      
// \endverbatim

#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/vidpro1_process_manager_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include "bwm_lidar_mesh.h"

bwm_lidar_mesh_process::bwm_lidar_mesh_process()
{
  //Input the two lidar images and the labelling image from Ibrahim

  if( !parameters()->add( "Lidar image 1 (max) file <filename...>" , "-image_filename1" , bpro1_filepath("","*") )) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }

  if( !parameters()->add( "Lidar image 2 (min) file <filename...>" , "-image_filename2" , bpro1_filepath("","*") )) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }

  if( !parameters()->add( "Labelled lidar image file <filename...>" , "-image_filename3" , bpro1_filepath("","*") )) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }

  /*if (!parameters()->add( "Input Path", "-ip", (vcl_string)"")) {
      vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }*/
}

//: Destructor
bwm_lidar_mesh_process::~bwm_lidar_mesh_process(){
}

bpro1_process* bwm_lidar_mesh_process::clone() const{
  return new bwm_lidar_mesh_process(*this);
}

vcl_string bwm_lidar_mesh_process::name(){
  return "BWM Lidar Mesh Process";
}

int bwm_lidar_mesh_process::input_frames(){
  return 0;
}

int bwm_lidar_mesh_process::output_frames(){
  return 1;
}

vcl_vector< vcl_string > bwm_lidar_mesh_process::get_input_type(){
  vcl_vector< vcl_string > to_return;
  return to_return;
}

vcl_vector< vcl_string > bwm_lidar_mesh_process::get_output_type(){
  vcl_vector<vcl_string > to_return;
  to_return.push_back ("image" );
  to_return.push_back ("image" );
  to_return.push_back ("image" );
  return to_return;
}

bool bwm_lidar_mesh_process::finish(){
  return true;
}

bool bwm_lidar_mesh_process::execute()
{
  bpro1_filepath image_path;
  parameters()->get_value( "-image_filename1" , image_path );
  vcl_string image_filename = image_path.path;
  vil_image_resource_sptr loaded_image = vil_load_image_resource( image_filename.c_str() );
  if( !loaded_image ) {
    vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
    return false;
  }
  vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();
  image_storage->set_image( loaded_image );

  parameters()->get_value( "-image_filename2" , image_path );
  image_filename = image_path.path;
  vil_image_resource_sptr loaded_image2 = vil_load_image_resource( image_filename.c_str() );
  if( !loaded_image2 ) {
    vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
    return false;
  }
  vidpro1_image_storage_sptr image_storage2 = vidpro1_image_storage_new();
  image_storage2->set_image( loaded_image2 );

  parameters()->get_value( "-image_filename3" , image_path );
  image_filename = image_path.path;
  vil_image_resource_sptr loaded_image3 = vil_load_image_resource( image_filename.c_str() );
  if( !loaded_image3 ) {
    vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
    return false;
  }
  vidpro1_image_storage_sptr image_storage3 = vidpro1_image_storage_new();
  image_storage3->set_image( loaded_image3 );

  output_data_.clear();
  output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,image_storage));
  output_data_[0].push_back(image_storage2);
  output_data_[0].push_back(image_storage3);

  vil_image_view<vxl_byte> img1 = loaded_image->get_view();
  vil_image_view<vxl_byte> img2 = loaded_image2->get_view();
  vil_image_view<vxl_byte> label_img = loaded_image3->get_view();

  //img1 = im2double(rgb2gray(img1)) * 100;
  /*for (unsigned int j=0; j<img1.nj(); j++)
    for (unsigned int i=0; i<img1.ni(); i++) {
      double g = (img1(i,j,0) + img1(i,j,1) + img1(i,j,2))/3;
      img1(i,j,0) = char (g*100/255);
    }
  for (unsigned int j=0; j<img1.nj(); j++)
    for (unsigned int i=0; i<img1.ni(); i++) {
      double g = (img1(i,j,0) + img1(i,j,1) + img1(i,j,2))/3;
      img1(i,j,0) = char (g*100/255);
    }*/
  //img2 = im2double(rgb2gray(img2)) * 100;

  //Convert labelled lidar images to mesh.
  //Ibo, do you remove this?
  ////dbmsh3d_mesh* result_mesh = lidar_to_mesh (img1, img2, label_img);
  
  return true;
}

