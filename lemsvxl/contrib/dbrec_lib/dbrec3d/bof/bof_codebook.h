// This is bof_codebook.h
#ifndef bof_codebook_h
#define bof_codebook_h

//:
// \file
// \brief A class to learn a codebook given a scene of feature vectors 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  4-Mar-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <dbrec3d/bof/bof_info.h>

#include <vnl/vnl_vector_fixed.h>
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>

#include <dbcll/dbcll_euclidean_cluster_light.h>


class bof_codebook: public vbl_ref_count 
{
public:
  //:
  
  //: Constructor from ascii file containing the means
  bof_codebook(vcl_string file){
    file_=file;
    
    vcl_ifstream mean_ifs(file.c_str());
    if(!mean_ifs.is_open()){
      vcl_cerr << "Error: Could not open file: " << file << "\n";
      throw;
    }
    
    unsigned num_means;
    mean_ifs >> num_means;
    means_.clear();
    means_.resize(num_means);
    vcl_cout << "In file: " << file << "Parsing: " << num_means << " means \n";
    
    for(unsigned i=0; i<num_means; i++){
      vnl_vector_fixed<double,10> &mean = means_[i];
      mean_ifs >> mean;
#ifdef DEBUG
      vcl_cout << means_[i] << ", ";
#endif
    }
    vcl_cout << "/n";
    mean_ifs.close();
  }
  
  //: The means
  vcl_vector< vnl_vector_fixed<double,10> > means_;
  
  //: The file associated with this class
  vcl_string file_;
};

typedef vbl_smart_ptr<bof_codebook> bof_codebook_sptr;

//: Bag of features utilities for a across-category code book
class bof_codebook_utils
{
  
public:
  //: Constructor from bof_info_file
  bof_codebook_utils(vcl_string bof_info_path):info_(bof_info(bof_info_path)){}
  
  //: Choose K random means from all scenes and all blocks.
  // This is done by choosing K random means from every block M_b. Then the set of all means M = Union{M_b} for all b
  // Then from the set M we choose K random points
  void sample_rnd_k_means(vcl_vector<vnl_vector_fixed<double,10> > &features, const unsigned K);

  //: Preform fast_k-means on a subsample of the training set
  bool fast_k_means_for_train_subsample(vcl_vector<vnl_vector_fixed<double,10> > &means,
                                        double subsample_fraction, unsigned max_it);
  
  //: Perform fast k-means on the means of subsamples
  //  Setp 2 in Bradley98
  bool fast_k_means_for_train_subsample(vcl_vector<vnl_vector_fixed<double,10> > &CM_j,
                                        vcl_vector<vnl_vector_fixed<double,10> > &CM, unsigned max_it);
  
  //: Preform fast_k-means on the training set of a barticular block. The means should be initialized with K points
  bool fast_k_means_on_train_samples(vcl_vector<vnl_vector_fixed<double,10> > &means,
                                     int scene_id, int block_i, int block_j, int block_k, unsigned max_it);
  
  //: Clusters points. This function does not modify the centers. It just associates a point to the closest (Euclidean) mean
  void  compute_euclidean_clusters(const vcl_vector<vnl_vector_fixed<double,10> > &means,
                                   bool is_train, int class_id, int scene_id, int block_i, int block_j, int block_k,
                                   vcl_vector<dbcll_euclidean_cluster_light<10> > &clusters);
  
  //: Assign a cluster id to each feature in the scene, corresponding to the closest one
  void assign_cluster_id(const vcl_vector<vnl_vector_fixed<double,10> > &means,
                         int scene_id, int block_i, int block_j, int block_k);
  
protected:
  
  //: Bag of fetures infor. e.g. scenes to process, number of means.
  bof_info info_;
    
};

/**************** Binary IO for bof_codebook *********************/

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bof_codebook const &v);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bof_codebook &v);


void vsl_print_summary(vcl_ostream &os, const bof_codebook &v);


void vsl_b_read(vsl_b_istream& is, bof_codebook* v);


void vsl_b_write(vsl_b_ostream& os, const bof_codebook* &v);

void vsl_print_summary(vcl_ostream& os, const bof_codebook* &v);



#endif
