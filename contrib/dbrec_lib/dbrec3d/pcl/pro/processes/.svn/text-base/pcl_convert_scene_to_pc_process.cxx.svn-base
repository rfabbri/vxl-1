//:
// \brief
// \file
// \author Isabel Restrepo
// \date 11/1/11

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/pcl/dbrec3d_scene_to_pc_utils.h>
#include <pcl/io/pcd_io.h>


//:global variables
namespace pcl_convert_scene_to_pc_process_globals 
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ =0;
}


//:sets input and output types
bool pcl_convert_scene_to_pc_process_cons(bprb_func_process& pro)
{
  using namespace pcl_convert_scene_to_pc_process_globals ;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "boxm_scene_base_sptr";  //gradient scene
  input_types_[i++] = "boxm_scene_base_sptr";  //valid scene      
  input_types_[i++] = "vcl_string";    //output path to pcd file -- this can be changed to a smart  pointer for point cloud

  vcl_vector<vcl_string> output_types_(n_outputs_);
  //    output_types_[i++] = ";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool pcl_convert_scene_to_pc_process(bprb_func_process& pro)
{

  //get inputs
  unsigned i = 0;
  boxm_scene_base_sptr grad_scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  boxm_scene_base_sptr valid_scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  vcl_string pcd_file = pro.get_input<vcl_string>(i++);

  //cast valid scene
  boxm_scene<boct_tree<short, bool > > *valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool > > *> (valid_scene_base.as_pointer());
  if (!valid_scene) {
    vcl_cerr << "Failed to valid scene\n";
    return false;
  }
  
  //cast the gradients scene scene
  if (boxm_scene<boct_tree<short, vnl_vector_fixed< float,10 > > > *grad_scene = dynamic_cast<boxm_scene<boct_tree<short, vnl_vector_fixed< float,10 > > > *> (grad_scene_base.as_pointer())){
    
    pcl::PointCloud<pcl::PointNormal>::Ptr cloud (new pcl::PointCloud<pcl::PointNormal>);
    
    boxm_gradient_scene_to_pc(grad_scene, valid_scene, cloud, 0);
    
    pcl::io::savePCDFileASCII (pcd_file, *cloud);   
    
  }

  return true;
}