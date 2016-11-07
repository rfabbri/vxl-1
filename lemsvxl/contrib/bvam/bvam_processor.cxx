#ifndef _bvam_processor_cxx_
#define _bvam_processor_cxx_

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_awk.h>

#include <vcl_sstream.h>

#include "bvam_processor.h"
#include "bvam_voxel_world.h"
#include "bvam_mog_grey_processor.h"


//------------------------------------------------------

bvam_processor::bvam_processor()
{

};


//------------------------------------------------------
void bvam_processor::load_model_params(
                                       const vcl_string& model_dir )
{
  vcl_cerr << "BVAM_PROCESSOR::LOAD_MODEL_PARAMS NOT YET IMPLEMENTED";
  // NOT IMPLEMENTED 
};


//------------------------------------------------------
void bvam_processor::train(
                           bvam_image_schedule& s,
                           const vcl_string& output_dir )
{
  bvam_voxel_world_base* world = make_world();
  for( unsigned i = 0; i < s.size(); i++ ){
    //update every metafile
    vil_image_view<float> prob_map;
    world->update( s[i], prob_map );    
    //save output image
    vcl_stringstream ss_name;
    ss_name.clear();
    ss_name << output_dir << "\\" << s[i].name.c_str() << "_train_" << i << ".tiff";
    vil_save(prob_map, ss_name.str().c_str()); 
  }
  delete world;
};


//------------------------------------------------------
void bvam_processor::detect_changes(
                                    const bvam_image_schedule& s,
                                    const vcl_string& output_dir )
{
  bvam_voxel_world_base* world = make_world();
  for( unsigned i = 0; i < s.size(); i++ ){
    //update every metafile
    vil_image_view<float> prob_map;
    world->pixel_probability_density( s[i], prob_map );    
    //save output image
    vcl_stringstream ss_name;
    ss_name.clear();
    ss_name << output_dir << s[i].name.c_str()<< "_detect_" << i<<".tiff";
    vil_save(prob_map, ss_name.str().c_str()); 
  }
  delete world;

};


//------------------------------------------------------
void bvam_processor::render_expected(
                                     unsigned ni,
                                     unsigned nj,
                                     const bvam_image_schedule& s,
                                     const vcl_string& output_dir )
{

  //  bvam_voxel_world_base* world = make_world();
  //for( unsigned i = 0; i < s.size(); i++ ){
  //  //update every metafile
  //  vil_image_view<float> prob_map;
  //  world->expected_image( s[i], prob_map );    
  //  //save output image
  //  vcl_stringstream ss_name;
  //  ss_name.clear();
  //  ss_name << outpur_dir << s[i].name.c_str()<< "_render_" << i<<".tiff";
  //  vil_save(prob_map, ss_name.str().c_str()); 
  //}
  //delete world;

};

//------------------------------------------------------
void bvam_processor::render_virtual_view(
  unsigned view_ni,
  unsigned view_nj,
  const bvam_image_schedule& view_s,
  unsigned view_camera_index,
  const bvam_image_schedule& s,
  const vcl_string& output_dir )
{

};


//-----------------------------------------------------
bvam_voxel_world_base* 
bvam_processor::make_world()
{
  return new bvam_voxel_world<bvam_mog_grey_processor>;
};


#endif // _bvam_processor_cxx_
