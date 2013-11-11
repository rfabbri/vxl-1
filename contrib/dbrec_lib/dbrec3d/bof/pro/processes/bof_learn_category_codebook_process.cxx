//:
// \brief A process to learn a category-specific codebook
// \file
// \author Isabel Restrepo
// \date 26-Sep-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bof/bof_scene_categories.h>
#include <bof/bof_class_codebook_util.h>


//:global variables
namespace bof_learn_category_codebook_process_globals 
{
  const unsigned n_inputs_ = 7;
  const unsigned n_outputs_ = 0 ;
}


//:sets input and output types
bool bof_learn_category_codebook_process_cons(bprb_func_process& pro)
{
  using namespace bof_learn_category_codebook_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bof_scene_categories_sptr"; //categories
  input_types_[i++] = "unsigned"; //class_id
  input_types_[i++] = "unsigned"; //K, number of means
  input_types_[i++] = "double"; //fraction of samples to use during initialization refinement
  input_types_[i++] = "unsigned"; //number of iterations to use during initialization refinement
  input_types_[i++] = "unsigned"; //max number of iterations during k-means
  input_types_[i++] = "vcl_string"; //means path

  
  vcl_vector<vcl_string> output_types_(n_outputs_);
   
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bof_learn_category_codebook_process(bprb_func_process& pro)
{
  using namespace bof_learn_category_codebook_process_globals;
  
  //get inputs
  unsigned i =0;
  bof_scene_categories_sptr categories = pro.get_input<bof_scene_categories_sptr>(i++);
  unsigned class_id = pro.get_input<unsigned>(i++);
  unsigned K = pro.get_input<unsigned>(i++);
  double fraction = pro.get_input<double>(i++);
  unsigned J = pro.get_input<unsigned>(i++);
  unsigned max_it = pro.get_input<unsigned>(i++);
  vcl_string means_path = pro.get_input<vcl_string>(i++);
  
  bof_class_codebook_util<10> cc_util(categories);
  vcl_vector<vnl_vector_fixed<double,10> > means;
  vcl_vector<dbcll_euclidean_cluster_light<10> > all_clusters;
  cc_util.learn_codebook(class_id, K, fraction, J, max_it, means,all_clusters);
  
  dbcll_xml_write(all_clusters, means_path + "/lowest_sse_means_info.xml");
  
  //write new means to file
  vcl_ofstream means_ofs((means_path + "/lowest_sse_means.txt").c_str());
  means_ofs.precision(15);
  means_ofs << means.size() << "\n";
  if(means_ofs.is_open())
    for (unsigned i =0; i<means.size(); i++) {
      means_ofs << means[i] << "\n";
    }
  
  else
    vcl_cerr << "Could not open file: " << (means_path + "/lowest_sse_means.txt") << "\n";
  
  means_ofs.close();
  
  
  return true;
}