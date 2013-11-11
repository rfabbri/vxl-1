//:
// \file
// \author Isabel Restrepo
// \date 1-Apr-2011

#include "bof_util.h"
#include <boxm/boxm_scene.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>

#include <vnl/vnl_random.h>

#if 0
//: Labels for training a random sample of the specified block
void bof_util::random_label_for_training(int scene_id, int block_i, int block_j, int block_k, double fraction)
{
  typedef boct_tree<short, bool> bool_tree_type;
  typedef boct_tree_cell<short, bool> bool_cell_type;
  
  boxm_scene_base_sptr valid_scene_base = info_.load_valid_scene(scene_id);
  boxm_scene_base_sptr train_scene_base = info_.load_train_scene(scene_id);
  
  boxm_scene<bool_tree_type>* valid_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (valid_scene_base.as_pointer());
  boxm_scene<bool_tree_type>* train_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (train_scene_base.as_pointer());
  
  if(!(valid_scene && train_scene))
  {
    vcl_cerr << "Error in bof_util::random_label_for_training: Could not cast scenes" << vcl_endl;
    return;
  }
  
  //init variables
  valid_scene->unload_active_blocks();
  train_scene->unload_active_blocks();
  
  //get the cells for this block
  if(!(valid_scene->valid_index(block_i, block_j, block_k) && train_scene->valid_index(block_i, block_j, block_k))){
    vcl_cerr << "In bof_util::random_label_for_training: Invalid block" << vcl_endl;
    return;
  }
  
  valid_scene->load_block(block_i, block_j, block_k);
  train_scene->load_block(block_i, block_j, block_k);
  
  //get the trees
  bool_tree_type* valid_tree = valid_scene->get_block(block_i, block_j, block_k)->get_tree();
  bool_tree_type* train_tree = valid_tree->clone();
  train_tree->init_cells(false);
  
  //get leaf cells
  vcl_vector<bool_cell_type *> valid_leaves = valid_tree->leaf_cells();
  vcl_vector<bool_cell_type *> train_leaves = train_tree->leaf_cells();
  
  float tree_ncells = valid_leaves.size();
  unsigned long tree_nsamples = (unsigned long)tree_ncells*fraction;
  vnl_random rng;
  for(unsigned long i =0; i<tree_nsamples; i++)
  {
    unsigned long sample = rng.lrand32(tree_ncells-1);
    
    if(!valid_leaves[sample]){
      i--;
      continue;
    }
    
    train_leaves[sample]->set_data(true);     
  }
  
  // write and release memory
  train_scene->get_block(block_i, block_j, block_k)->init_tree(train_tree);
  train_scene->write_active_block();
  valid_scene->unload_active_blocks();
}

#endif

//: Samples randomnly 10-d features (from those scenes labeled for training). 
// The randomly selected features are returned in a vector
void bof_util::sample_from_train(int scene_id, int block_i, int block_j, int block_k, double fraction,
                                  vcl_vector<vnl_vector_fixed<double,10> > &features)
{
  if(!info_.training_scenes_[scene_id])
    return;
  
  typedef boct_tree<short, bool> bool_tree_type;
  typedef boct_tree_cell<short, bool> bool_cell_type;
  
  typedef boct_tree<short,vnl_vector_fixed<double,10> > feature_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > feature_cell_type;
  
  boxm_scene_base_sptr feature_scene_base = info_.load_feature_scene(scene_id);
  boxm_scene_base_sptr valid_scene_base = info_.load_valid_scene(scene_id);
  //boxm_scene_base_sptr train_scene_base = info_.load_train_scene(scene_id);
  
  boxm_scene<feature_tree_type>* feature_scene = dynamic_cast<boxm_scene<feature_tree_type>* >(feature_scene_base.as_pointer());
  boxm_scene<bool_tree_type>* valid_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (valid_scene_base.as_pointer());
  //boxm_scene<bool_tree_type>* train_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (train_scene_base.as_pointer());
  
  
  if(!(feature_scene && valid_scene /*&& train_scene*/))
  {
    vcl_cerr << "Error in bof_util::sample_from_train: Could not cast scenes" << vcl_endl;
    return;
  }
  
  //init variables
  feature_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  //train_scene->unload_active_blocks();
  
  //get the cells for this block
  if(!(feature_scene->valid_index(block_i, block_j, block_k) && valid_scene->valid_index(block_i, block_j, block_k) /*&& train_scene->valid_index(block_i, block_j, block_k)*/)){
    vcl_cerr << "In bof_util::sample_from_train: Invalid block" << vcl_endl;
    return;
  }
  
  feature_scene->load_block(block_i, block_j, block_k);
  valid_scene->load_block(block_i, block_j, block_k);
  //train_scene->load_block(block_i, block_j, block_k);
  
  //get the trees
  feature_tree_type* feature_tree = feature_scene->get_block(block_i, block_j, block_k)->get_tree();
  bool_tree_type* valid_tree = valid_scene->get_block(block_i, block_j, block_k)->get_tree();
  //bool_tree_type* train_tree = train_scene->get_block(block_i, block_j, block_k)->get_tree();
  
  //get leaf cells
  vcl_vector<feature_cell_type *> feature_leaves = feature_tree->leaf_cells();
  vcl_vector<bool_cell_type *> valid_leaves = valid_tree->leaf_cells();
  //vcl_vector<bool_cell_type *> train_leaves = train_tree->leaf_cells();
  
  features.clear();
  float tree_ncells = valid_leaves.size();
  unsigned long tree_nsamples = (unsigned long)tree_ncells*fraction;
  vnl_random rng;
  for(unsigned long i =0; i<tree_nsamples; i++)
  {
    unsigned long sample = rng.lrand32(tree_ncells-1);
    
    if(valid_leaves[sample]/*&&train_leaves[sample]*/){
      features.push_back(feature_leaves[sample]->data());
    }else {
      i--;
    }    
  }
   
  // write and release memory
  feature_scene->unload_active_blocks();
  //train_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
}


double bof_util::parse_variance_from_xml_cluster(vcl_string xml_file)
{
  
  vcl_ifstream xml_ifs(xml_file.c_str());
  if(!xml_ifs.is_open()){
    vcl_cerr << "Error: could not open xml info file: " << xml_file << " \n";
    return -1.0;
  }
  bxml_document doc = bxml_read(xml_ifs);
  bxml_element root_query("dbcll_clusters");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), root_query);
  if (!root) {
    vcl_cerr << "Error: bof_info - could not parse xml root\n";
    return -1.0;
  }
  
  //Parse scenes
  bxml_element query("general");
  bxml_data_sptr data = bxml_find_by_name(root, query);
  
  bxml_element* elm = dynamic_cast<bxml_element*>(data.ptr());
  double total_sse = 0.0;
  elm->get_attribute("total_sse", total_sse);
  
  return total_sse;
}


/******************bof_feature_vector**************************/
//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & , bof_feature_vector const & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & , bof_feature_vector & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_print_summary(vcl_ostream & , const bof_feature_vector & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}

void vsl_b_read(vsl_b_istream& is,bof_feature_vector* p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new bof_feature_vector();
    vsl_b_read(is, *p);
  }
  else
    p = 0;
}

void vsl_b_write(vsl_b_ostream& os, const bof_feature_vector* &p)
{
  if (p==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*p);
  }
}

void vsl_print_summary(vcl_ostream& os, const bof_feature_vector* &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}

