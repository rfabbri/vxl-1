//:
// \brief A process to combine the joint distribution of class and keypoints over all scenes
// \file
// \author Isabel Restrepo
// \date 10-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_scene_categories.h>

//:global variables
namespace bof_combine_quantization_process_globals 
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0; 
}


//:sets input and output types
bool bof_combine_quantization_process_cons(bprb_func_process& pro)
{
  using namespace bof_combine_quantization_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //bof_dir
  input_types_[1] = "vcl_string"; //xml prefix per scene
  input_types_[2] = "vcl_string"; //file to the common mean
  input_types_[3] = "vcl_string"; //path out. An xml file for dbcll_clusters and a bsta_histograms.txt/.svg will be saved
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_combine_quantization_process(bprb_func_process& pro)
{
  using namespace bof_combine_quantization_process_globals;
  
  //get inputs
  vcl_string bof_dir = pro.get_input<vcl_string>(0);
  vcl_string xml_scen_prfx = pro.get_input<vcl_string>(1);
  vcl_string common_mean_file = pro.get_input<vcl_string>(2);
  vcl_string path_out = pro.get_input<vcl_string>(3);

  //load the mean
  
  vcl_ifstream mean_ifs(common_mean_file.c_str());
  if(!mean_ifs.is_open()){
    vcl_cerr << "Error: Could not open file: " << common_mean_file << "\n";
    throw;
  }
  
  unsigned num_means;
  mean_ifs >> num_means;
  vcl_vector<vnl_vector_fixed<double, 10> > common_means(num_means);
  vcl_cout << "In file: " << common_mean_file  << "Parsing: " << num_means << " means \n";
  
  for(unsigned i=0; i<num_means; i++){
    vnl_vector_fixed<double,10> &mean = common_means[i];
    mean_ifs >> mean;
#ifdef DEBUG
    vcl_cout << means_[i] << ", ";
#endif
  }
  vcl_cout << "/n";
  mean_ifs.close();
  
  
  //Read the 
  bof_scene_categories categories(bof_dir);
  bof_labels_keypoint_joint  p_xc(categories.nclasses(), common_means.size()); 
  p_xc.xml_read_and_combine(xml_scen_prfx, common_means, categories.nscenes(), categories.nclasses());
  
  //write all distribution.xml files
  p_xc.xml_write(path_out);
  
  //save distributions as bsta_histograms
  p_xc.bsta_save_and_plot(path_out);
  
  return true;
}