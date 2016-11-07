//:
// \brief A process that computes class probability process for all testing objects given the class codebook
// \file
// \author Isabel Restrepo
// \date 28-Sep-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vul/vul_file.h>

#include <bof/bof_scene_categories.h>
#include <bof/bof_class_codebook_util.h>


//:global variables
namespace bof_class_probability_process_globals 
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bof_class_probability_process_cons(bprb_func_process& pro)
{
  using namespace bof_class_probability_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i =0;
  input_types_[i++] = "bof_codebook_sptr"; //codebook containing the means
  input_types_[i++] = "bof_scene_categories_sptr"; //categories
  input_types_[i++] = "unsigned"; //class_id
  input_types_[i++] = "vcl_string"; //path to save the results for objects (classification_dir)

  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_class_probability_process(bprb_func_process& pro)
{
  using namespace bof_class_probability_process_globals;
  
  //get inputs
  unsigned i =0;
  bof_codebook_sptr codebook = pro.get_input<bof_codebook_sptr>(i++);
  bof_scene_categories_sptr categories = pro.get_input<bof_scene_categories_sptr>(i++);
  unsigned class_id = pro.get_input<unsigned>(i++);
  vcl_string classification_dir = pro.get_input<vcl_string>(i++);
  
  //read the clusters
  vcl_string clusters_xml = vul_file::dirname(codebook->file_) + "/lowest_sse_means_info.xml" ;
  vcl_vector< dbcll_euclidean_cluster_light<10> > clusters;
  bof_class_codebook_util<10> cc_util(categories);
  cc_util.xml_read(clusters_xml, codebook->means_,clusters);
  
  //compute class probability of all testing objects
  cc_util.compute_p_o(class_id, codebook->means_, clusters, classification_dir);

  return true;
}