// This is bof_class_codebook.h
#ifndef bof_class_codebook_util_h
#define bof_class_codebook_util_h

//:
// \file
// \brief A class to learn a class specific codebook given a feature vectors
// \author Isabel Restrepo mir@lems.brown.edu
// \date  24-Sep-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bof/bof_scene_categories.h>
#include <vnl/vnl_vector_fixed.h>

#include <dbcll/dbcll_k_means.h>
#include <dbcll/dbcll_euclidean_cluster_light.h>
#include <dbcll/dbcll_euclidean_cluster_util.h>

#include <boxm/boxm_scene.h>

#include <vcl_cmath.h>

template <unsigned dim>
class bof_class_codebook_util
{
  
public:
  
  bof_class_codebook_util(bof_scene_categories_sptr category_info):category_info_(category_info){}
  
  //: Collects all features beonging to a given class
  void collect_class_features(unsigned class_id, vcl_vector<vnl_vector_fixed<double,dim> > &features);
  
  
  void random_samples(unsigned class_id, unsigned long n_samples, 
                      vcl_vector<vnl_vector_fixed<double,dim> > const &features, 
                      vcl_vector<vnl_vector_fixed<double,dim> > &rnd_samples);
  
  //: Initialize k-means according to the algorithm in Bradley98
  vcl_vector<vnl_vector_fixed<double,dim> > init_codebook(unsigned class_id, unsigned K, unsigned long nsamples, unsigned J, unsigned max_it,
                                                         vcl_vector<vnl_vector_fixed<double,dim> > const &features);
  
  //: Learns the category codebook
  void learn_codebook (unsigned class_id, unsigned K, float fraction, unsigned J, unsigned max_it,
                       vcl_vector<vnl_vector_fixed<double,dim> > &means,
                       vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters);
   
 
  //: Compute the probability of an object (for all testing objects), under a given class codebook
  void compute_p_o(unsigned class_id, vcl_vector< vnl_vector_fixed<double, dim> > const &means,
                   vcl_vector< dbcll_euclidean_cluster_light<dim> > const &class_clusters,
                   vcl_string classification_dir);
  
  //: Compute the probability of a single object, under a given class codebook
  double compute_p_o(bsta_histogram<float> const &class_dist, vcl_vector< dbcll_euclidean_cluster_light<dim> > const &obj_clusters);
  
  
  //: Read the clusters in an xml file 
  void xml_read(vcl_string xml_file, vcl_vector< vnl_vector_fixed<double, dim> > const &means,
                vcl_vector< dbcll_euclidean_cluster_light<dim> > &clusters);
  
  vcl_string path() { return category_info_->path_out_; }
  
protected:
  
  bof_scene_categories_sptr category_info_;
};



//: Collects all features belonging to a given class
template <unsigned dim>
void bof_class_codebook_util<dim>::collect_class_features(unsigned class_id, vcl_vector<vnl_vector_fixed<double,dim> > &features)
{
  vcl_cout << "Collecting category features, for class: " << category_info_->category_names_[class_id] << vcl_endl;
  
  typedef boct_tree<short,vnl_vector_fixed<double,dim> > feature_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,dim> > feature_cell_type;
  
  vcl_set<int> scenes_to_visit = category_info_->category_scenes_[class_id];
  vcl_set<int>::iterator scenes_it = scenes_to_visit.begin();
  
  for ( ; scenes_it != scenes_to_visit.end(); scenes_it++ )
  {
    
    if(!category_info_->info_.training_scenes_[*scenes_it])
      continue;
    
    //Load the scene for this id
    boxm_scene_base_sptr scene_base = category_info_->info_.load_feature_scene(*scenes_it);
    boxm_scene<feature_tree_type>* scene = dynamic_cast<boxm_scene<feature_tree_type>*> (scene_base.as_pointer());
    
    boxm_scene_base_sptr valid_scene_base = category_info_->info_.load_valid_scene(*scenes_it);
    boxm_scene<boct_tree<short, bool> >* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
    
    if(!(scene && valid_scene))
    {
      vcl_cerr << "Error in bof_scene_categories::label_objects --> Could not cast scene" << vcl_endl;
      return;
    }
    
    vcl_vector<bof_scene_object>::iterator it = category_info_->ply_paths_[*scenes_it].begin();
    for (; it!=category_info_->ply_paths_[*scenes_it].end(); it++) 
    {
      if(it->class_id != class_id)
        continue;
      
      vgl_box_3d<double> outer_bbox;
      vgl_box_3d<double> tight_bbox;
      
      category_info_->load_bbox_from_ply(it->ply_path, tight_bbox);
      outer_bbox = tight_bbox;
      outer_bbox.expand_about_centroid(category_info_->info_.finest_cell_length_[*scenes_it]*category_info_->bbox_scale_);
      
      vcl_vector<feature_cell_type*> object_leaves;
      vcl_vector<boct_tree_cell<short, bool>* > valid_leaves;
      
      scene->leaves_in_region(outer_bbox, object_leaves);
      valid_scene->leaves_in_region(outer_bbox, valid_leaves);
      
      
      //get the features
      for (unsigned long i = 0; i < object_leaves.size(); i++) {
        if ((valid_leaves[i])->data()) {
          features.push_back(object_leaves[i]->data());
        }
      }
    }      
    
    //release memory
    scene->unload_active_blocks();
    valid_scene->unload_active_blocks();
    
  }
  
}


//: Initialize k-means according to the algorithm in Bradley98
template <unsigned dim>
vcl_vector<vnl_vector_fixed<double,dim> > bof_class_codebook_util<dim>::init_codebook(unsigned class_id, unsigned K, unsigned long nsamples, unsigned J, unsigned max_it,
                                                                                vcl_vector<vnl_vector_fixed<double,dim> > const &features)
{ 
  
  vcl_vector<vnl_vector_fixed<double,dim> > rnd_means;
  vcl_vector<vnl_vector_fixed<double,dim> > CM;
  vcl_vector<vcl_vector<vnl_vector_fixed<double,dim> > > FM(J);
  vcl_vector<vcl_vector<vnl_vector_fixed<double,dim> > > initial_means(J);
  
  random_samples(class_id, K, features, rnd_means);
  
  
  for (unsigned j=0; j<J; j++) {
    
    initial_means[j] = rnd_means;
    
    vcl_vector<vnl_vector_fixed<double,dim> > subsamples;
    random_samples(class_id, nsamples, features, subsamples);
    
    vcl_vector<vcl_vector<unsigned> > clusters;
    vcl_cout << "Size subsamples: " << subsamples.size() << "\n";
    unsigned n_iterations = dbcll_fast_k_means(subsamples, clusters, initial_means[j], max_it);
    vcl_cout << "Number of means: " << initial_means[j].size() << "\n";
    subsamples.clear();
    vcl_cout <<" Number of iterations for fast-k means is: " << n_iterations << vcl_endl;  
    CM.insert(CM.end(), initial_means[j].begin(), initial_means[j].end()); 
    FM[j] = initial_means[j];
  }
  
  int min_j = -1;
  double min_sse = vcl_numeric_limits<double>::infinity();
  
  for (unsigned j=0; j<J; j++) {
    
    vcl_vector<vcl_vector<unsigned> > clusters;
    unsigned n_iterations = dbcll_fast_k_means(CM, clusters, FM[j], max_it);
    vcl_cout <<" Number of iterationsfor fast-k means is: " << n_iterations << vcl_endl;
    vcl_vector<dbcll_euclidean_cluster_light<dim> > all_clusters;
    dbcll_init_euclidean_clusters(CM, clusters, FM[j], all_clusters);
    double total_sse = 0.0;
    for(unsigned ci = 0; ci<all_clusters.size(); ci++){
      dbcll_euclidean_cluster_light<dim> cluster = all_clusters[ci];
      total_sse+=cluster.var()*(double)cluster.size();
    }
    
    if(total_sse < min_sse){
      min_sse = total_sse;
      min_j =j;
    }
    
  }
  
  return FM[min_j];
  
}

template <unsigned dim>
void bof_class_codebook_util<dim>::random_samples(unsigned class_id, unsigned long n_samples, 
                                             vcl_vector<vnl_vector_fixed<double,dim> > const &features, 
                                             vcl_vector<vnl_vector_fixed<double,dim> > &rnd_samples)
{
  vnl_random rng;
  
  for(unsigned i =0; i<n_samples; i++){
    unsigned long sample = rng.lrand32((int)(features.size()-1));
    rnd_samples.push_back(features[sample]);
  }  
}



//: Learns the category codebook
template <unsigned dim>
void bof_class_codebook_util<dim>::learn_codebook(unsigned class_id, unsigned K, float fraction, unsigned J, unsigned max_it,
                                             vcl_vector<vnl_vector_fixed<double,dim> > &means,
                                             vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters)
{
  
  vcl_vector<vnl_vector_fixed<double,dim> > features;
  collect_class_features(class_id, features);
  unsigned long nsamples = (unsigned long)((float)features.size() * fraction);
  
  means = init_codebook(class_id, K, nsamples, J, max_it, features);
  
  //Perform fast-kmeans on all samples
  vcl_vector<vcl_vector<unsigned> > clusters;
  unsigned n_iterations = dbcll_fast_k_means(features, clusters, means, max_it);
  vcl_cout <<" Number of iterationsfor fast-k means is: " << n_iterations << vcl_endl;  
  
  dbcll_init_euclidean_clusters(features, clusters, means, all_clusters);
  
}

//: Read the clusters in an xml file 
template <unsigned dim>
void bof_class_codebook_util<dim>::xml_read(vcl_string xml_file, vcl_vector<vnl_vector_fixed<double, dim> > const &means,
                                            vcl_vector<dbcll_euclidean_cluster_light<dim> > &all_clusters)
{
    all_clusters.clear();
    dbcll_xml_read_and_init(all_clusters, means, xml_file);
}


//: Compute the probability of an object (for all testing objects), under a given class codebook
template <unsigned dim>
void bof_class_codebook_util<dim>::compute_p_o(unsigned class_id, vcl_vector< vnl_vector_fixed<double, dim> > const &means,
                                               vcl_vector< dbcll_euclidean_cluster_light<dim> > const &class_clusters,
                                               vcl_string classification_dir)
{
  
  vcl_cout << "Computing object probabilities, for class: " << category_info_->category_names_[class_id] << vcl_endl;
  
  vcl_vector<float> counts(class_clusters.size(),0.0);
  for (unsigned cluster_idx=0; cluster_idx < class_clusters.size(); cluster_idx++) {
    counts[cluster_idx]=class_clusters[cluster_idx].size();
  }
  bsta_histogram<float> class_hist(1.0, (float)class_clusters.size(), counts);
  
  counts.clear();
  
  typedef boct_tree<short,vnl_vector_fixed<double,dim> > feature_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,dim> > feature_cell_type;
  
  vcl_set<int> scenes_to_visit = category_info_->category_scenes_[class_id];
  vcl_set<int>::iterator scenes_it = scenes_to_visit.begin();
  
  for ( ; scenes_it != scenes_to_visit.end(); scenes_it++ )
  {
    
    //we are only interested in testing
    if(category_info_->info_.training_scenes_[*scenes_it])
      continue;
    
    //file to write classification results
    vcl_stringstream cl_ss;
    cl_ss << classification_dir << "/scene_" << *scenes_it; 
    
    if(!vul_file::exists(cl_ss.str()))
      vul_file::make_directory(cl_ss.str());
    
    //Load the scene for this id
    boxm_scene_base_sptr scene_base = category_info_->info_.load_feature_scene(*scenes_it);
    boxm_scene<feature_tree_type>* scene = dynamic_cast<boxm_scene<feature_tree_type>*> (scene_base.as_pointer());
    
    boxm_scene_base_sptr valid_scene_base = category_info_->info_.load_valid_scene(*scenes_it);
    boxm_scene<boct_tree<short, bool> >* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
    
    if(!(scene && valid_scene))
    {
      vcl_cerr << "Error in bof_scene_categories::label_objects --> Could not cast scene" << vcl_endl;
      return;
    }
    
    vcl_vector<bof_scene_object>::iterator it = category_info_->ply_paths_[*scenes_it].begin();
    for (; it!=category_info_->ply_paths_[*scenes_it].end(); it++) 
    {
      if(it->class_id != class_id)
        continue;
      
      vgl_box_3d<double> outer_bbox;
      vgl_box_3d<double> tight_bbox;
      
      category_info_->load_bbox_from_ply(it->ply_path, tight_bbox);
      outer_bbox = tight_bbox;
      outer_bbox.expand_about_centroid(category_info_->info_.finest_cell_length_[*scenes_it]*category_info_->bbox_scale_);
      
      vcl_vector<feature_cell_type*> object_leaves;
      vcl_vector<boct_tree_cell<short, bool>* > valid_leaves;
      
      scene->leaves_in_region(outer_bbox, object_leaves);
      valid_scene->leaves_in_region(outer_bbox, valid_leaves);
            
      //get the features
      vcl_vector<vnl_vector_fixed<double,dim> > features;
      for (unsigned long i = 0; i < object_leaves.size(); i++) {
        if ((valid_leaves[i])->data()) {
          features.push_back(object_leaves[i]->data());
        }
      }
      
      //cluster this objetc's features    
      vcl_vector<dbcll_euclidean_cluster_light<dim> > clusters_out;
      dbcll_compute_euclidean_clusters(features, means, clusters_out );
      double p = compute_p_o(class_hist, clusters_out);
      
      //write the clusters to file
      vcl_stringstream ss;
      ss << classification_dir << "/scene_" << *scenes_it << "/";
      ss << "aposteriori_" << vul_file::strip_extension(vul_file::strip_directory(it->ply_path)) << ".txt";
      
      vcl_ofstream ofs(ss.str().c_str());
      if(!ofs.is_open()){
        vcl_cerr << "Failed to open " << ss.str() << " for write\n";
        continue;
      }
      
      ofs.precision(15);
      ofs << p << "\n";
      ofs.close();
      
      
      vcl_stringstream mesh_ss;
      mesh_ss << classification_dir << "/scene_" << *scenes_it << "/" << vul_file::strip_extension(vul_file::strip_directory(it->ply_path)) << ".xml";
      dbcll_xml_write(clusters_out, mesh_ss.str());
      
    }      
    
    //release memory
    scene->unload_active_blocks();
    valid_scene->unload_active_blocks();
    
  }
  
  
  
  
  }


//: Compute the probability of a single object, under a given class codebook
template <unsigned dim>
double bof_class_codebook_util<dim>::compute_p_o(bsta_histogram<float> const &class_dist, 
                                                 vcl_vector< dbcll_euclidean_cluster_light<dim> > const &obj_clusters)
{
  double p_o  = 0.0;
  for (unsigned cluster_idx=0; cluster_idx < obj_clusters.size(); cluster_idx++) {
    p_o += class_dist.p(cluster_idx) * obj_clusters[cluster_idx].size();
  }
  
  return p_o;
}



#endif
