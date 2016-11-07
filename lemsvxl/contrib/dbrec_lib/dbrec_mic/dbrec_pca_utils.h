//:
// \file
// \brief utilities to use PCA for classification purposes in comparison to dbrec
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   June 22, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//

#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_view.h>

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_random.h>
#include <vbl/vbl_array_2d.h>

#include <bvgl/bvgl_changes_sptr.h>

class dbrec_pca : public vbl_ref_count {
public:
  dbrec_pca() {}
  virtual ~dbrec_pca();

  //: turn the image into a vector and add to the data
  void add_image(const vil_image_view<float>& inp, const vcl_string& class_name);

  //: check the consistency of the data (sizes of the vectors)
  bool check_data();

  //: construct the basis
  bool construct_basis_vectors(double variance_proportion);

  //: return the class name of the instance in the dataset that's closes to the input image in the transformed space
  bool classify_nn(const vil_image_view<float>& inp, vcl_string& out_str);
  
protected:
 
  vcl_vector<vnl_vector<double> > data_;
  vcl_vector<vcl_string> classes_; // save the class string for each data

  vnl_matrix<double> EVecs_;
  vnl_vector<double> evals_,m_;

  //: keep the transformed data for fast access during classification
  vcl_vector<vnl_vector<double> > data_transformed_;
};

////////////////// DESCRIPTORS ///////////////////////////////////////////////////////////
class dbrec_descriptor : public vbl_ref_count {
public:
  dbrec_descriptor() {}
  virtual ~dbrec_descriptor() {}
  //: given an n x n image, extract some k dimensional feature vector
  //: WARNING: assumes that (i_start, j_start) and (i_end-1, j_end-1) are valid image coordinates
  virtual vnl_vector<double> extract(const vil_image_view<float>& img, int i_start, int j_start, int i_end, int j_end) = 0;
};
typedef vbl_smart_ptr<dbrec_descriptor> dbrec_descriptor_sptr;

class dbrec_simplest_image_descriptor : public dbrec_descriptor {
public:
  //: given an n x n image, extract k dimensional feature vector, k = n*n, as the 1-d coded image itself
  virtual vnl_vector<double> extract(const vil_image_view<float>& img, int i_start, int j_start, int i_end, int j_end);
};

class dbrec_pca_sift_descriptor : public dbrec_descriptor {
public:
  //: given an n x n image, extract k dimensional PCA-SIFT feature vector, k = (n-2)*(n-2)*2, as gradients in x and y dimensions
  virtual vnl_vector<double> extract(const vil_image_view<float>& img, int i_start, int j_start, int i_end, int j_end);
};

class dbrec_random_binary_features_descriptor : public dbrec_descriptor {
public:
  dbrec_random_binary_features_descriptor(int window_size, vnl_random& rng, int dim = 600); // select random pairs
  //: given an n x n image, extract dim_ dimensional feature vector, by selecting dim_ random pairs from all possible pairs of n x n pixels, and finding their difference
  virtual vnl_vector<double> extract(const vil_image_view<float>& img, int i_start, int j_start, int i_end, int j_end);
protected:
  int n_;
  int dim_;
  vnl_random rng_;
  vcl_vector<vcl_pair<int, int> > random_pixels_;  // vector of size dim_+1, constructor fills this so each call of extract uses the same set
};

class dbrec_single_scale_blob_descriptor : public dbrec_descriptor {
public:
  dbrec_single_scale_blob_descriptor(int window_size, int angle_increments = 15, float lambda0 = 1.0f, float lambda1 = 2.0f, bool bright = true);
  virtual vnl_vector<double> extract(const vil_image_view<float>& img, int i_start, int j_start, int i_end, int j_end);
protected:
  int angle_increments_;
  int n_;
  float lambda0_;
  float lambda1_;
  vcl_vector<vbl_array_2d<double> > kernels_;
  vcl_vector<vbl_array_2d<bool> > masks_;
  bool bright_;
};

class dbrec_descriptor_types {
public: 
  enum types {
    simplest_image_descriptor,
    pca_sift_descriptor,
    random_binary_features_descriptor,
    single_scale_blob_descriptor
  };
};


/////////////////////////////// BAYESIAN PCA ////////////////////////////////////////////////////////////
class dbrec_bayesian_pca : public vbl_ref_count {
public:
  dbrec_bayesian_pca(int window_size, unsigned desc_type_id);
  virtual ~dbrec_bayesian_pca();

  //: add one data point of size nxn per polygon in the ground truth
  void add_image_rectangles(const vil_image_view<float>& inp, int n, bvgl_changes_sptr gt_polygons, const vcl_string& ground_truth_type, char class_id);
  //: the method adds cnt many new data vectors for a random selection of pixels in the map with given bool value 
  void add_image_rectangles(const vil_image_view<float>& inp, int n, const vil_image_view<bool>& class_map, bool value, char class_id, int cnt);
  void save_image_rectangles(const vcl_string& prefix, int size, char class_id);

  //: check the consistency of the data (sizes of the vectors)
  bool check_data();
  bool construct_basis_vectors(double variance_proportion, int patch_size, const vcl_string& output_path);
  bool visualize_transformed_distributions(const vcl_string& output_path, char class_id);

  //: classify each pixel in the image as the given class or not by generating a class map as the posterior map of the class
  bool classify_image_rectangles(const vil_image_view<float>& inp, const vil_image_view<bool>& inp_valid_map, int n, char class_id, float class_prior, vil_image_view<float>& output_map);

  //: apply bayesian pca to gradient image - PCA-SIFT descriptor

protected:
  vcl_vector<vnl_vector<double> > data_;
  vcl_vector<char> data_class_ids_; // assumes at max 255 classes

  vnl_matrix<double> EVecs_;
  vnl_vector<double> evals_, m_;

  //: keep the transformed data
  vcl_vector<vnl_vector<double> > data_transformed_;
  int transformed_size_;
  vcl_vector<vcl_vector<double> > means_; // a set of means for each class
  vcl_vector<vcl_vector<double> > vars_;  // a set of vars for each class
  vcl_map<char,int> classes_;

  dbrec_descriptor_sptr desc_;
  vnl_random rng_;
};

////////////////////////// TEMPLATE MATCHING ///////////////////////////////
// save nxn patches around each ground-truth polygon in the training image as positive examplers
// save nxn patches around randomly selected points in the training background image as negative examplers
// given an input image, classify each nxn patch in it into the class of its nearest neighbor in the training data, measure distance by normalized cross-correlation
class dbrec_template_matching : public vbl_ref_count {
public:
  dbrec_template_matching();
  virtual ~dbrec_template_matching();

  static void standardize_vector(vnl_vector<double>& v);

  //: add one data point of size nxn per polygon in the ground truth
  void add_image_rectangles(const vil_image_view<float>& inp, int n, bvgl_changes_sptr gt_polygons, const vcl_string& ground_truth_type, char class_id);
  //: the method adds cnt many new data vectors for a random selection of pixels in the map with given bool value 
  void add_image_rectangles(const vil_image_view<float>& inp, int n, const vil_image_view<bool>& class_map, bool value, char class_id, int cnt);
  void save_image_rectangles(const vcl_string& prefix, int size, char class_id);

  //: check the consistency of the data (sizes of the vectors)
  bool check_data();

  //: classify each pixel in the image as the given class or not by generating a class map as the posterior map of the class
  bool classify_image_rectangles(const vil_image_view<float>& inp, const vil_image_view<bool>& inp_valid_map, int n, char class_id, vil_image_view<float>& output_map);

protected:
  vcl_vector<vnl_vector<double> > data_;
  vcl_vector<char> data_class_ids_; // assumes at max 255 classes

  vnl_random rng_;
};

// Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_pca as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_pca const &ph);
void vsl_b_read(vsl_b_istream & is, dbrec_pca &ph);
void vsl_b_read(vsl_b_istream& is, dbrec_pca* ph);
void vsl_b_write(vsl_b_ostream& os, const dbrec_pca* &ph);

// Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_bayesian_pca as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_bayesian_pca const &ph);
void vsl_b_read(vsl_b_istream & is, dbrec_bayesian_pca &ph);
void vsl_b_read(vsl_b_istream& is, dbrec_bayesian_pca* ph);
void vsl_b_write(vsl_b_ostream& os, const dbrec_bayesian_pca* &ph);

// Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_bayesian_pca as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_template_matching const &ph);
void vsl_b_read(vsl_b_istream & is, dbrec_template_matching &ph);
void vsl_b_read(vsl_b_istream& is, dbrec_template_matching* ph);
void vsl_b_write(vsl_b_ostream& os, const dbrec_template_matching* &ph);



