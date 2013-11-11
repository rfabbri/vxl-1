//:
// \file
// \author Isabel Restrepo
// \date 4-Mar-2011

#include "bof_codebook.h"

#include <dbcll/dbcll_k_means.h>
#include <dbcll/dbcll_euclidean_cluster_light.h>
#include <dbcll/dbcll_euclidean_cluster_util.h>

#include <boxm/boxm_scene.h>
#include <boct/util/boct_construct_tree.h>

void bof_codebook_utils::sample_rnd_k_means(vcl_vector<vnl_vector_fixed<double,10> > &rnd_means, const unsigned K)
{
  typedef boct_tree<short, bool> bool_tree_type;
  typedef boct_tree_cell<short, bool> bool_cell_type;
  
  typedef boct_tree<short,vnl_vector_fixed<double,10> > feature_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > feature_cell_type;
  vcl_vector<vnl_vector_fixed<double,10> > features;
  vnl_random rng;
  
  //Get K samples from each block in every training scene
  for(unsigned scene_id=0; scene_id<info_.nscenes(); scene_id++)
  { 
    if(!info_.training_scenes_[scene_id])
      continue;
  
    boxm_scene_base_sptr feature_scene_base = info_.load_feature_scene(scene_id);
    boxm_scene_base_sptr valid_scene_base = info_.load_valid_scene(scene_id);
    //boxm_scene_base_sptr train_scene_base = info_.load_train_scene(scene_id);
    
    boxm_scene<feature_tree_type>* feature_scene = dynamic_cast<boxm_scene<feature_tree_type>* >(feature_scene_base.as_pointer());
    boxm_scene<bool_tree_type>* valid_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (valid_scene_base.as_pointer());
    //boxm_scene<bool_tree_type>* train_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (train_scene_base.as_pointer());
    
    
    if(!(feature_scene && valid_scene /*&& train_scene*/))
    {
      vcl_cerr << "Error in bof_util::random_label_for_training: Could not cast scenes" << vcl_endl;
      return;
    }
    
    //init variables
    feature_scene->unload_active_blocks();
    valid_scene->unload_active_blocks();
    //train_scene->unload_active_blocks();
    
    //get the cells for these blocks
    boxm_block_iterator<feature_tree_type> it = feature_scene->iterator();
    for (it.begin(); !it.end(); ++it) 
    {
      if(!(feature_scene->valid_index(it.index()) && valid_scene->valid_index(it.index()))){
        vcl_cerr << "In bof_util::random_label_for_training: Invalid block" << vcl_endl;
        return;
      }
      
      feature_scene->load_block(it.index());
      valid_scene->load_block(it.index());
      
      //get the trees
      feature_tree_type* feature_tree = feature_scene->get_block(it.index())->get_tree();
      bool_tree_type* valid_tree = valid_scene->get_block(it.index())->get_tree();
      
      //get leaf cells
      vcl_vector<feature_cell_type *> feature_leaves = feature_tree->leaf_cells_at_level(0);
      vcl_vector<bool_cell_type *> valid_leaves = valid_tree->leaf_cells_at_level(0);
      
      float tree_ncells = feature_leaves.size();
  
      //get the number of valid features
      unsigned long tree_valid_ncells = 0;
      vcl_vector<bool_cell_type *>::iterator valid_it = valid_leaves.begin();
      for (; valid_it != valid_leaves.end(); valid_it++) {
        if ((*valid_it)->data()) {
          tree_valid_ncells++;
        }
      }
      
      vcl_cout <<" In Scene: " << scene_id << ". In block (" << it.index() << "), number of valid leaves is: "
      << tree_valid_ncells << " and the number of leaves is: " << (unsigned long)tree_ncells <<  vcl_endl;

      if((int)tree_valid_ncells<K)
        continue;
      for(unsigned long i =0; i<K; i++)
      {
        unsigned long sample = rng.lrand32((int)(tree_ncells-1));
        vnl_vector_fixed<double,10> feature = feature_leaves[sample]->data();
        
        if(valid_leaves[sample]->data()){
          features.push_back(feature);
        }else {
          i--;
        }    
      }
    }
    
    // write and release memory
    feature_scene->unload_active_blocks();
    valid_scene->unload_active_blocks();
  }
  
  //From all random samples choose k- random means
  rnd_means.clear();
  for(unsigned i=0; i<K; i++){
    unsigned long sample = rng.lrand32(features.size());
    rnd_means.push_back(features[sample]);
  }
}

//: Preform fast_k-means on a subsample of the training set. The means should be initialized with K points
bool bof_codebook_utils::fast_k_means_for_train_subsample(vcl_vector<vnl_vector_fixed<double,10> > &means,
                                                    double subsample_fraction, unsigned max_it)
{
  typedef boct_tree<short, bool> bool_tree_type;
  typedef boct_tree_cell<short, bool> bool_cell_type;
  
  typedef boct_tree<short,vnl_vector_fixed<double,10> > feature_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > feature_cell_type;
  
  //collect subsmaples
  vcl_vector<vnl_vector_fixed<double,10> > subsamples;
  subsamples.clear();
  vnl_random rng;
  
  for(unsigned scene_id=0; scene_id<info_.nscenes(); scene_id++)
  { 
    if (!info_.training_scenes_[scene_id]) 
      continue;
    
    vcl_cout << "Scene " << scene_id << " labeled for training \n";
    
    boxm_scene_base_sptr feature_scene_base = info_.load_feature_scene(scene_id);
    boxm_scene_base_sptr valid_scene_base = info_.load_valid_scene(scene_id);
    
    boxm_scene<feature_tree_type>* feature_scene = dynamic_cast<boxm_scene<feature_tree_type>* >(feature_scene_base.as_pointer());
    boxm_scene<bool_tree_type>* valid_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (valid_scene_base.as_pointer());
    
    
    if(!(feature_scene && valid_scene))
    {
      vcl_cerr << "Error in bof_util::random_label_for_training: Could not cast scenes" << vcl_endl;
      return false;
    }
    
    //init variables
    feature_scene->unload_active_blocks();
    valid_scene->unload_active_blocks();
    
    //get the cells for these blocks
    boxm_block_iterator<feature_tree_type> it = feature_scene->iterator();
    for (it.begin(); !it.end(); ++it) 
    {
      
      if(!feature_scene->load_block(it.index()))
        continue;
      
      if(!valid_scene->load_block(it.index()))
        continue;
      
      //get the trees
      feature_tree_type* feature_tree = feature_scene->get_active_block()->get_tree();
      bool_tree_type* valid_tree = valid_scene->get_active_block()->get_tree();
      
      //get leaf cells
      vcl_vector<feature_cell_type *> feature_leaves = feature_tree->leaf_cells_at_level(0);
      vcl_vector<bool_cell_type *> valid_leaves = valid_tree->leaf_cells_at_level(0);
      
      
      //get the percentage of number of valid features
      unsigned long tree_valid_ncells = 0;
      vcl_vector<bool_cell_type *>::iterator valid_it = valid_leaves.begin();
      for (; valid_it != valid_leaves.end(); valid_it++) {
        if ((*valid_it)->data()) {
          tree_valid_ncells++;
        }
      }
      
      unsigned long tree_ncells = valid_leaves.size();
      unsigned long tree_nsamples = (unsigned long)((float)tree_valid_ncells*subsample_fraction);
      vcl_cout <<" In Scene: " << scene_id << ". In block (" << it.index() << "), number of valid leaves is: " << tree_valid_ncells << vcl_endl;
      for(unsigned long i =0; i<tree_nsamples; i++)
      {
        unsigned long sample = rng.lrand32((int)(tree_ncells-1));
        
        if(valid_leaves[sample]->data()){
          subsamples.push_back(feature_leaves[sample]->data());
        }else {
          i--;
        }    
      }
      feature_leaves.clear();
      valid_leaves.clear();
      
      // write and release memory
      feature_scene->unload_active_blocks();
      valid_scene->unload_active_blocks();
      
      vcl_cout <<" In Scene: " << scene_id << ". In block (" << it.index() << "), number of subsamples is: " << tree_nsamples << vcl_endl;
    }
   
  }
  
  //perform fast k-means on samples
  vcl_vector<vcl_vector<unsigned> > clusters;
  vcl_cout << "Size subsamples: " << subsamples.size() << "\n";
  unsigned n_iterations = dbcll_fast_k_means(subsamples, clusters, means, max_it);
  subsamples.clear();
  vcl_cout <<" Number of iterationsfor fast-k means is: " << n_iterations << vcl_endl;  
  return true;
  
}

//: Preform fast_k-means on the training set of a barticular block. The means should be initialized with K points
bool bof_codebook_utils::fast_k_means_on_train_samples(vcl_vector<vnl_vector_fixed<double,10> > &means,
                                                int scene_id, int block_i, int block_j, int block_k, unsigned max_it)
{
  if (!info_.training_scenes_[scene_id]) {
    return false;
  }
  typedef boct_tree<short, bool> bool_tree_type;
  typedef boct_tree_cell<short, bool> bool_cell_type;
  
  typedef boct_tree<short,vnl_vector_fixed<double,10> > feature_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > feature_cell_type;
 
  boxm_scene_base_sptr feature_scene_base = info_.load_feature_scene(scene_id);
  boxm_scene_base_sptr valid_scene_base = info_.load_valid_scene(scene_id);
  
  boxm_scene<feature_tree_type>* feature_scene = dynamic_cast<boxm_scene<feature_tree_type>* >(feature_scene_base.as_pointer());
  boxm_scene<bool_tree_type>* valid_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (valid_scene_base.as_pointer());
  
  
  if(!(feature_scene && valid_scene))
  {
    vcl_cerr << "Error in bof_util::random_label_for_training: Could not cast scenes" << vcl_endl;
    return false;
  }
  
  //init variables
  feature_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  
  //get the cells for this block
  if(!(feature_scene->valid_index(block_i, block_j, block_k) && valid_scene->valid_index(block_i, block_j, block_k) )){
    vcl_cerr << "In bof_util::random_label_for_training: Invalid block" << vcl_endl;
    return false;
  }
  
  feature_scene->load_block(block_i, block_j, block_k);
  valid_scene->load_block(block_i, block_j, block_k);
  
  //get the trees
  feature_tree_type* feature_tree = feature_scene->get_block(block_i, block_j, block_k)->get_tree();
  bool_tree_type* valid_tree = valid_scene->get_block(block_i, block_j, block_k)->get_tree();
  
  //get leaf cells
  vcl_vector<feature_cell_type *> feature_leaves = feature_tree->leaf_cells_at_level(0);
  vcl_vector<bool_cell_type *> valid_leaves = valid_tree->leaf_cells_at_level(0);
  
  float tree_ncells = valid_leaves.size();
  vcl_vector<vnl_vector_fixed<double,10> > features;
  for(unsigned long i =0; i<tree_ncells; i++)
  {
    if(!valid_leaves[i]->data())
      continue;

    
    features.push_back(feature_leaves[i]->data());
    
  }
  
  vcl_cout <<" In block (" << block_i << ',' << block_j <<',' <<  block_k << "), number of features is: " << features.size() << vcl_endl;

  if (features.empty()) {
    return false;
  }  
  
  // write and release memory
  feature_scene->unload_active_blocks();
  //train_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();

  
  //perform fast k-means on samples
  vcl_vector<vcl_vector<unsigned> > clusters;
  unsigned n_iterations = dbcll_fast_k_means(features, clusters, means, max_it);
  vcl_cout <<" Number of iterationsfor fast-k means is: " << n_iterations << vcl_endl;
  return true;
}

//: Clusters points. This function does not modify the centers. It just associates a point to the closest (Euclidean) mean
void  
bof_codebook_utils::compute_euclidean_clusters(const vcl_vector<vnl_vector_fixed<double,10> > &means,
                                               bool is_train, int class_id, int scene_id, int block_i, int block_j, int block_k,
                                               vcl_vector<dbcll_euclidean_cluster_light<10> > &clusters)
{
  
  typedef boct_tree<short, bool> bool_tree_type;
  typedef boct_tree_cell<short, bool> bool_cell_type;
  
  typedef boct_tree<short,vnl_vector_fixed<double,10> > feature_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > feature_cell_type;
  
  typedef boct_tree<short, char> char_tree_type;
  typedef boct_tree_cell<short, char> char_cell_type;
  
  boxm_scene_base_sptr feature_scene_base = info_.load_feature_scene(scene_id);
  boxm_scene_base_sptr valid_scene_base = info_.load_valid_scene(scene_id);
  //boxm_scene_base_sptr train_scene_base = info_.load_train_scene(scene_id);
  boxm_scene_base_sptr class_id_scene_base = info_.load_category_scene(scene_id);
  
  boxm_scene<feature_tree_type>* feature_scene = dynamic_cast<boxm_scene<feature_tree_type>* >(feature_scene_base.as_pointer());
  boxm_scene<bool_tree_type>* valid_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (valid_scene_base.as_pointer());
  //boxm_scene<bool_tree_type>* train_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (train_scene_base.as_pointer());
  boxm_scene<char_tree_type>* class_id_scene = dynamic_cast<boxm_scene<char_tree_type>*> (class_id_scene_base.as_pointer());
  
  if(!(feature_scene && valid_scene /* && train_scene*/ && class_id_scene))
  {
    vcl_cerr << "Error in bof_util::random_label_for_training: Could not cast scenes" << vcl_endl;
    return;
  }
  
  //init variables
  feature_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  //train_scene->unload_active_blocks();
  class_id_scene->unload_active_blocks();
  
  //get the cells for this block
  if(!(feature_scene->valid_index(block_i, block_j, block_k) && valid_scene->valid_index(block_i, block_j, block_k) /*&& 
       train_scene->valid_index(block_i, block_j, block_k)*/ &&  (class_id_scene->valid_index(block_i, block_j, block_k)) )){
    vcl_cerr << "In bof_util::random_label_for_training: Invalid block" << vcl_endl;
    return ;
  }
  
  feature_scene->load_block(block_i, block_j, block_k);
  valid_scene->load_block(block_i, block_j, block_k);
  //train_scene->load_block(block_i, block_j, block_k);
  class_id_scene->load_block(block_i, block_j, block_k);
  
  //get the trees
  feature_tree_type* feature_tree = feature_scene->get_block(block_i, block_j, block_k)->get_tree();
  bool_tree_type* valid_tree = valid_scene->get_block(block_i, block_j, block_k)->get_tree();
  //bool_tree_type* train_tree = train_scene->get_block(block_i, block_j, block_k)->get_tree();
  char_tree_type* class_id_tree = class_id_scene->get_block(block_i, block_j, block_k)->get_tree();
  
  //get leaf cells
  vcl_vector<feature_cell_type *> feature_leaves = feature_tree->leaf_cells();
  vcl_vector<bool_cell_type *> valid_leaves = valid_tree->leaf_cells();
  //vcl_vector<bool_cell_type *> train_leaves = train_tree->leaf_cells();
  vcl_vector<char_cell_type *> class_id_leaves = class_id_tree->leaf_cells();

  
  float tree_ncells = valid_leaves.size();
  vcl_vector<vnl_vector_fixed<double,10> > features;
  vcl_cout <<" In block (" << block_i << ',' << block_j <<',' <<  block_k << "), number of cells is: " << tree_ncells << vcl_endl;
  for(unsigned long i =0; i<tree_ncells; i++)
  {
    if(!valid_leaves[i]->data())
      continue;
     
    /*if(train_leaves[i]->data() != is_train)
      continue;*/
      
    if(class_id>0)
      if(class_id != class_id_leaves[i]->data())
        continue;
      
    features.push_back(feature_leaves[i]->data());
  }
  vcl_cout <<" In block (" << block_i << ',' << block_j <<',' <<  block_k << "), number of relevant cells is: " << features.size() << vcl_endl;

  // write and release memory
  feature_scene->unload_active_blocks();
  //train_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  
  
  //perform fast k-means on samples
  dbcll_compute_euclidean_clusters(features, means, clusters);
  
  return; 
  
}

//: Assign cluster id. This function does not modify the centers. It just associates a point to the closest (Euclidean) mean
//  The tree-structure of the cluster-id-scene is created from scratch such that there is only id per cell, 
void  
bof_codebook_utils::assign_cluster_id(const vcl_vector<vnl_vector_fixed<double,10> > &means,
                                      int scene_id, int block_i, int block_j, int block_k)
{
  
  typedef boct_tree<short, bool> bool_tree_type;
  typedef boct_tree_cell<short, bool> bool_cell_type;
  
  typedef boct_tree<short,vnl_vector_fixed<double,10> > feature_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > feature_cell_type;
  
  typedef boct_tree<short, short> short_tree_type;
  typedef boct_tree_cell<short, short> short_cell_type;
  
  boxm_scene_base_sptr feature_scene_base = info_.load_feature_scene(scene_id);
  boxm_scene_base_sptr valid_scene_base = info_.load_valid_scene(scene_id);
  boxm_scene_base_sptr cluster_id_scene_base = info_.load_cluster_id_scene(scene_id);
  
  boxm_scene<feature_tree_type>* feature_scene = dynamic_cast<boxm_scene<feature_tree_type>* >(feature_scene_base.as_pointer());
  boxm_scene<bool_tree_type>* valid_scene = dynamic_cast<boxm_scene<bool_tree_type>*> (valid_scene_base.as_pointer());
  boxm_scene<short_tree_type>* cluster_id_scene = dynamic_cast<boxm_scene<short_tree_type>*> (cluster_id_scene_base.as_pointer());
  
  if(!(feature_scene && valid_scene && cluster_id_scene))
  {
    vcl_cerr << "Error in bof_codebook_utils::assign_cluster_id: Could not cast scenes" << vcl_endl;
    return;
  }
  
  //init variables
  feature_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  cluster_id_scene->unload_active_blocks();
  
  //get the cells for this block
  if(!(feature_scene->valid_index(block_i, block_j, block_k) && valid_scene->valid_index(block_i, block_j, block_k) &&  (cluster_id_scene->valid_index(block_i, block_j, block_k)) )){
   vcl_cerr << "In bof_codebook_utils::assign_cluster_id: Invalid block" << vcl_endl;
   return ;
 }
  
  feature_scene->load_block(block_i, block_j, block_k);
  valid_scene->load_block(block_i, block_j, block_k);
  cluster_id_scene->load_block(block_i, block_j, block_k); 
  
  //get the trees
  feature_tree_type* feature_tree = feature_scene->get_block(block_i, block_j, block_k)->get_tree();
  bool_tree_type* valid_tree = valid_scene->get_block(block_i, block_j, block_k)->get_tree();
 
  
  vcl_vector<feature_cell_type *> feature_cells = feature_tree->leaf_cells_at_level(0);
  vcl_vector<short_cell_type> cloned_cells;
  vcl_vector<bool_cell_type*> valid_cells = valid_tree->leaf_cells_at_level(0);
  
  dbcll_euclidean_cluster_util<10> cluster_util(means);
  
  for (unsigned i=0; i<valid_cells.size(); i++) {
    if(valid_cells[i]->data()){
      vnl_vector_fixed<double,10> data = feature_cells[i]->data();
      double dist =0.0;
      short cluster_id =(short)cluster_util.closest_euclidean_cluster(data, dist);
      boct_tree_cell<short, short> id_cell(feature_cells[i]->code_);
      id_cell.set_data(cluster_id);
      cloned_cells.push_back(id_cell);
    }
  }
  
  boct_tree_cell<short, short>* cloned_root = boct_construct_tree(cloned_cells, feature_tree->number_levels(), (short)-1 );
  boct_tree<short,short>* cloned_tree = new boct_tree<short,short>(cloned_root,  feature_tree->number_levels());
  cloned_tree->set_bbox(feature_tree->bounding_box());
   
  // write and release memory
  feature_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  
  cluster_id_scene->get_block(block_i, block_j, block_k)->init_tree(cloned_tree);
  cluster_id_scene->write_active_block();
  
  return; 
  
}


/****************** BInary IO fo bof_codebook**************************/
//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & , bof_codebook const & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & , bof_codebook & )
{
  vcl_cerr << "Error: Trying to read but binary io not implemented\n";
  return;
}

void vsl_print_summary(vcl_ostream & , const bof_codebook & )
{
  vcl_cerr << "Error: Trying to print but binary io not implemented\n";
  return;
}

void vsl_b_read(vsl_b_istream& is,bof_codebook* p)
{
  delete p;
  vcl_cerr << "Error: Trying to read but binary io not implemented\n";
  return;
}

void vsl_b_write(vsl_b_ostream& os, const bof_codebook* &p)
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

void vsl_print_summary(vcl_ostream& os, const bof_codebook* &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}



