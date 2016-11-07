//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 06/22/09
//
//
#include "dbrec_pca_utils.h"
#include <mcal/mcal_pca.h>
#include <bvgl/bvgl_changes.h>
#include <vgl/vgl_box_2d.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/vis/bsta_svg_tools.h>
#include <brip/brip_vil_float_ops.h>
#include <time.h>

dbrec_pca::~dbrec_pca()
{
  data_.clear();
  classes_.clear();
  EVecs_.clear();
  evals_.clear();
  m_.clear();
  data_transformed_.clear();
}

void dbrec_pca::add_image(const vil_image_view<float>& inp, const vcl_string& class_name)
{
  unsigned ni = inp.ni();
  unsigned nj = inp.nj();

  //: convert the image into a one dimensional vector
  vnl_vector<double> v(ni*nj);
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      v[i*ni + j] = inp(i,j);

  data_.push_back(v);
  classes_.push_back(class_name);
}

//: check the consistency of the data (sizes of the vectors)
bool dbrec_pca::check_data()
{
  if (data_.size() > 0) {
    if (data_.size() != classes_.size())
      return false;

    unsigned d = data_[0].size();
    for (unsigned i = 1; i < data_.size(); i++)
      if (data_[i].size() != d) {
        return false;
      }
  }
  return true;
}

//: construct the basis
bool dbrec_pca::construct_basis_vectors(double variance_proportion)
{
  mcal_pca pca;
  pca.set_var_prop(variance_proportion);
  pca.build_from_array(&data_[0],data_.size(),m_,EVecs_,evals_);

  vcl_cout<<"evals: "<<evals_<<vcl_endl;
  vcl_cout << "e vectors size rows: " << EVecs_.rows() << " cols: " << EVecs_.columns() << vcl_endl;
  //: now transform each data for fast access during classification
  // 16384x18 for 18 eigenvectors of an 128x128 = 16384 image
  for (unsigned i = 0; i < data_.size(); i++) {
    vnl_vector<double> new_v = EVecs_.transpose()*data_[i];
    //vcl_cout << "multiplied and new size: " << new_v.size() << vcl_endl;
    data_transformed_.push_back(new_v);
  }
  return true;
}

//: return the class name of the instance in the dataset that's closes to the input image in the transformed space
bool dbrec_pca::classify_nn(const vil_image_view<float>& inp, vcl_string& out_str)
{
  unsigned ni = inp.ni();
  unsigned nj = inp.nj();
  unsigned size = ni*nj;

  if (!data_.size() || size != data_[0].size())
    return false;
  
  //: convert the image into a one dimensional vector
  vnl_vector<double> v(ni*nj);
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      v[i*ni + j] = inp(i,j);

  //: now transform the vector using the Eigen basis
  vnl_vector<double> out_v = EVecs_.transpose()*v;
  
  //: now search the transformed vecs to see which one is the nearest neighbor
  double min = 1e5;
  unsigned min_i = data_transformed_.size()+1;
  for (unsigned i = 0; i < data_transformed_.size(); i++) {
    vnl_vector<double> dif = out_v - data_transformed_[i];
    double dif_s = dif.two_norm();
    if (dif_s < min) {
      min = dif_s;
      min_i = i;
    }
  }
  if (min_i > data_transformed_.size())
    return false;
  
  //: return the class of the nearest neighbors class
  out_str = classes_[min_i];
  return true;
}
  
//: given an n x n image, extract k dimensional feature vector, k = n*n, as the 1-d coded image itself
//: WARNING: assumes that (i_start, j_start) and (i_end-1, j_end-1) are valid image coordinates
vnl_vector<double> dbrec_simplest_image_descriptor::extract(const vil_image_view<float>& img, int i_start, int j_start, int i_end, int j_end)
{
  int n = i_end-i_start;
  vnl_vector<double> v(n*n); unsigned vi = 0;
  for (int ii = i_start; ii < i_end; ii++) {
      for (int jj = j_start; jj < j_end; jj++) {
        v[vi] = img(ii, jj);
        vi++;
      }
    }
  return v;
}
//: given an n x n image, extract k dimensional PCA-SIFT feature vector, k = (n-2)*(n-2)*2, as gradients in x and y dimensions
//: WARNING: assumes that (i_start, j_start) and (i_end-1, j_end-1) are valid image coordinates
vnl_vector<double> dbrec_pca_sift_descriptor::extract(const vil_image_view<float>& img, int i_start, int j_start, int i_end, int j_end)
{
  int n = i_end-i_start;
  assert(n-2 > 0);
  vnl_vector<double> v((n-2)*(n-2)*2); unsigned vi = 0;
  for (int ii = i_start+1; ii < i_end-1; ii++) {
    for (int jj = j_start+1; jj < j_end-1; jj++) {
      v[vi] = img(ii+1, jj)-img(ii-1,jj);
      vi++;
      v[vi] = img(ii, jj+1)-img(ii,jj-1);
      vi++;
    }
  }
  return v;
}


// select random pairs
dbrec_random_binary_features_descriptor::dbrec_random_binary_features_descriptor(int n, vnl_random& rng, int dim) : dbrec_descriptor(), n_(n), dim_(dim), rng_(rng)
{
  //: select a random sample of pairs from n x n pixels, we need dim pairs, so select dim+1 numbers and use their order 
  int cnt = n*n;
  while (int(random_pixels_.size()) < dim_) {
    int pix1_id = rng_.lrand32(0, cnt-1);
    int pix2_id = rng_.lrand32(0, cnt-1);
    bool exists = false;
    for (unsigned i = 0; i < random_pixels_.size(); i++) {
      if (pix1_id == random_pixels_[i].first && pix2_id == random_pixels_[i].second) {
        exists = true; 
        break;
      }
    }
    if (!exists)
      random_pixels_.push_back(vcl_pair<int, int>(pix1_id, pix2_id));
  }
}
vnl_vector<double> dbrec_random_binary_features_descriptor::extract(const vil_image_view<float>& img, int i_start, int j_start, int i_end, int j_end)
{
  int n = i_end-i_start;
  vnl_vector<double> v(dim_);
  for (int vi = 0; vi < dim_; vi++) {
    int p1_id = random_pixels_[vi].first;
    int p2_id = random_pixels_[vi].second;
    int i1 = p1_id/n; int j1 = p1_id%n;
    int i2 = p2_id/n; int j2 = p2_id%n;
    v[vi] = img(i_start + i1, j_start + j1)-img(i_start + i2, j_start + j2);
  }
  return v;
}
dbrec_single_scale_blob_descriptor::dbrec_single_scale_blob_descriptor(int window_size, int angle_increments, float lambda0, float lambda1, bool bright) 
: angle_increments_(angle_increments), n_(window_size), lambda0_(lambda0), lambda1_(lambda1), bright_(bright)
{
  for (float angle = 0.0f; angle < 180.0f; angle+=angle_increments_) {
    float theta = brip_vil_float_ops::extrema_revert_angle(angle);
    vbl_array_2d<float> fa;
    vbl_array_2d<bool> mask;
    brip_vil_float_ops::extrema_kernel_mask(lambda0_, lambda1_, theta, fa, mask);
    unsigned nrows = fa.rows(), ncols = fa.cols();
    int rj = (nrows-1)/2, ri = (ncols-1)/2;
    int larger = ri > rj ? ri : rj;
    if (window_size < larger) {
      vcl_cout << "In dbrec_single_scale_blob_descriptor::dbrec_blob_descriptor() -- window size needs to be larger than all rotated versions of kernel!\n";
      throw 0;
    }
    vbl_array_2d<double> coef(nrows,ncols);
    for (unsigned r = 0; r<nrows; ++r)
      for (unsigned c = 0; c<ncols; ++c)
        coef[r][c]=fa[r][c];
    kernels_.push_back(coef);
    masks_.push_back(mask);
  }
}

vnl_vector<double> dbrec_single_scale_blob_descriptor::extract(const vil_image_view<float>& img, int i_start, int j_start, int i_end, int j_end)
{
  vnl_vector<double> v(kernels_.size()); 
  //: go to the central pixel in the patch
  int j = j_start + (j_end - j_start)/2;
  int i = i_start + (i_end - i_start)/2;
  for (unsigned ker = 0; ker < kernels_.size(); ker++) {
    unsigned nrows = kernels_[ker].rows(), ncols = kernels_[ker].cols();
    int rj = (nrows-1)/2, ri = (ncols-1)/2;
    double sum = 0;
    for (int jj=-rj; jj<=rj; ++jj)
      for (int ii=-ri; ii<=ri; ++ii)
        if ((masks_[ker])[jj+rj][ii+ri])
          sum += (kernels_[ker])[jj+rj][ii+ri]*img(i+ii, j+jj);
    if (bright_) // coefficients are negative at center
      v[ker] = -sum;
    else 
      v[ker] = sum;
  }
  return v;
}

dbrec_bayesian_pca::dbrec_bayesian_pca(int window_size, unsigned desc_type_id) : transformed_size_(-1) 
{
  
  rng_.reseed((unsigned long)time(NULL));

  switch(desc_type_id) {
    case dbrec_descriptor_types::simplest_image_descriptor: { desc_ = new dbrec_simplest_image_descriptor(); break; }
    case dbrec_descriptor_types::pca_sift_descriptor: { desc_ = new dbrec_pca_sift_descriptor(); break; }
    case dbrec_descriptor_types::random_binary_features_descriptor: { desc_ = new dbrec_random_binary_features_descriptor(window_size, rng_); break; }
    case dbrec_descriptor_types::single_scale_blob_descriptor: { desc_ = new dbrec_single_scale_blob_descriptor(window_size); break; }
    default: { vcl_cerr<< "In dbrec_bayesian_pca::dbrec_bayesian_pca() - Unidentified descriptor type id: " << desc_type_id << "\n"; break; }
  }
}

dbrec_bayesian_pca::~dbrec_bayesian_pca()
{
  data_.clear();
  EVecs_.clear();
  evals_.clear();
  m_.clear();
}

//: add one data point of size nxn per polygon in the ground truth
void dbrec_bayesian_pca::add_image_rectangles(const vil_image_view<float>& inp, int n, bvgl_changes_sptr gt_polygons, const vcl_string& ground_truth_type, char class_id)
{
  unsigned ni = inp.ni();
  unsigned nj = inp.nj();
  int half_n = n/2;
  vgl_box_2d<int> image_box(0, ni-1, 0, nj-1); 

  vcl_cout << "In dbrec_bayesian_pca::add_image_rectangles() - number of gt polygons: " << gt_polygons->size() << vcl_endl;
  int used_size = 0;
  for (unsigned i=0; i < gt_polygons->size(); i++)
  {
    if (gt_polygons->obj(i)->type().compare(ground_truth_type) != 0)
      continue;
    used_size++;
    //: find the centre of this polygon
    double x_cent = 0.0, y_cent = 0.0;
    gt_polygons->obj(i)->centroid(x_cent, y_cent);

    int i_start = (int)vcl_floor(x_cent+0.5)-half_n; int i_end = i_start + n;
    int j_start = (int)vcl_floor(y_cent+0.5)-half_n; int j_end = j_start + n;
    
    if (!image_box.contains(i_start, j_start) || !image_box.contains(i_end-1, j_end-1))
      continue;

    vnl_vector<double> v = desc_->extract(inp, i_start, j_start, i_end, j_end);
    /*
    vnl_vector<double> v(n*n); unsigned vi = 0;
    for (int ii = i_start; ii < i_end; ii++) {
      for (int jj = j_start; jj < j_end; jj++) {
        v[vi] = inp(ii, jj);
        vi++;
      }
    }
    */
    
    data_.push_back(v);  
    data_class_ids_.push_back(class_id);
  }
  vcl_cout << "In dbrec_bayesian_pca::add_image_rectangles() - number of type: " << ground_truth_type << " polygons: " << used_size << vcl_endl;
  return;
}
//: the method adds cnt many new data vectors for a random selection of pixels in the map with given bool value 
void dbrec_bayesian_pca::add_image_rectangles(const vil_image_view<float>& inp, int n, const vil_image_view<bool>& class_map, bool value, char class_id, int cnt)
{
  unsigned ni = inp.ni();
  unsigned nj = inp.nj();
  int half_n = n/2;
  vgl_box_2d<int> image_box(0, ni-1, 0, nj-1); 
  if (class_map.ni() != ni || class_map.nj() != nj) {
    vcl_cout << "In dbrec_bayesian_pca::add_image_rectangles() -- Input image size is inconsistent with the map size!\n";
    throw 0;
  }

  vcl_cout << "In dbrec_bayesian_pca::add_image_rectangles() -- generating: " << cnt << " random rectangles!\n";

  for (int k = 0; k < cnt; k++) {
    int i = rng_.lrand32(half_n+1, ni-1-half_n);
    int j = rng_.lrand32(half_n+1, nj-1-half_n);
    int i_start = i-half_n; int i_end = i_start + n;
    int j_start = j-half_n; int j_end = j_start + n;

    while (class_map(i,j) != value || !image_box.contains(i_start, j_start) || !image_box.contains(i_end-1, j_end-1)) {
      i = rng_.lrand32(half_n+1, ni-1-half_n);
      j = rng_.lrand32(half_n+1, nj-1-half_n);
      i_start = i-half_n; i_end = i_start + n;
      j_start = j-half_n; j_end = j_start + n;
    }
    
    vnl_vector<double> v = desc_->extract(inp, i_start, j_start, i_end, j_end);
    /*
    vnl_vector<double> v(n*n); unsigned vi = 0;
    for (int ii = i_start; ii < i_end; ii++) {
      for (int jj = j_start; jj < j_end; jj++) {
        v[vi] = inp(ii, jj);
        vi++;
      }
    }
    */
    data_.push_back(v);  
    data_class_ids_.push_back(class_id);
  }
  return;
}
//: should only be used for simplest image feature descriptor, the 1-d coded image itself: size*size equals v.size() for that case
void dbrec_bayesian_pca::save_image_rectangles(const vcl_string& prefix, int size, char class_id)
{
  for (unsigned i = 0; i < data_.size(); i++) {
    if (data_class_ids_[i] != class_id)
      continue;
    vnl_vector<double> v = data_[i];
    if (size*size == v.size()) {
      vil_image_view<float> img(size, size);
      for (int ii = 0; ii < size; ii++)
        for (int jj = 0; jj < size; jj++) {
          img(ii,jj) = (float)v[ii*size + jj];
        }
      vil_math_scale_values(img,255.0f);
      vil_image_view<vxl_byte> img_b(size, size);
      vil_convert_cast(img, img_b);
      vcl_stringstream ids; ids << i;
      vcl_string name = prefix + "img_" + ids.str() + ".png";
      vil_save(img_b, name.c_str());
    }
  }
}

//: check the consistency of the data (sizes of the vectors)
bool dbrec_bayesian_pca::check_data()
{
  if (data_.size() > 0) {
    unsigned d = data_[0].size();
    for (unsigned i = 1; i < data_.size(); i++)
      if (data_[i].size() != d) {
        return false;
      }
  }
  return true;
}

bool dbrec_bayesian_pca::construct_basis_vectors(double variance_proportion, int patch_size, const vcl_string& output_path)
{
  mcal_pca pca;
  pca.set_var_prop(variance_proportion);
  pca.build_from_array(&data_[0],data_.size(),m_,EVecs_,evals_);

  vcl_cout<<"evals: "<<evals_<<vcl_endl;
  vcl_cout << "e vectors size rows: " << EVecs_.rows() << " cols: " << EVecs_.columns() << vcl_endl;

  if (patch_size*patch_size == EVecs_.rows()) { // for instance the simplest feature descriptor, 1-d coded image itself
    //: visualize the selected eigenvectors
    for (unsigned col = 0; col < EVecs_.columns(); col++) {
     vil_image_view<float> img(patch_size, patch_size);
      for (int ii = 0; ii < patch_size; ii++)
        for (int jj = 0; jj < patch_size; jj++) {
          img(ii,jj) = (float)EVecs_(ii*patch_size + jj,col);
        }
      vil_math_scale_values(img,255.0f);
      vil_image_view<vxl_byte> img_b(patch_size, patch_size);
      vil_convert_cast(img, img_b);
      vcl_stringstream ids; ids << col;
      vcl_string name = output_path + "eigen_img_" + ids.str() + ".png";
      vil_save(img_b, name.c_str());
    }
  }


  //: now transform each data
  // 49x21 for 21 eigenvectors of an 7x7 = 49 image
  for (unsigned i = 0; i < data_.size(); i++) {
    vnl_vector<double> new_v = EVecs_.transpose()*data_[i];
    //vcl_cout << "multiplied and new size: " << new_v.size() << vcl_endl;
    data_transformed_.push_back(new_v);
    transformed_size_ = new_v.size();
  }
  //: find the number of classes
  for (unsigned i = 0; i < data_class_ids_.size(); i++) {
    vcl_map<char,int>::iterator it = classes_.find(data_class_ids_[i]);
    if (it == classes_.end())
      classes_[data_class_ids_[i]] = 1;
    else
      it->second = it->second + 1;
  }
  
  for (vcl_map<char,int>::iterator it = classes_.begin(); it != classes_.end(); it++) {
    int class_id = it->first;
    int class_cnt = it->second;
    vcl_cout << "!!!! Class ID: " << class_id << " cnt: " << class_cnt << vcl_endl;
    
    vcl_vector<double> means;
    means.insert(means.begin(), transformed_size_, 0.0);
    vcl_vector<double> vars;
    vars.insert(vars.begin(), transformed_size_, 0.0);

    for (unsigned i = 0; i < data_transformed_.size(); i++) {
      if (data_class_ids_[i] != class_id)
        continue;
      for (unsigned j = 0; j < data_transformed_[i].size(); j++) {
        means[j] += data_transformed_[i][j];
      }
    }
    for (int j = 0; j < transformed_size_; j++) {
      means[j] /= class_cnt;
    }
    for (unsigned i = 0; i < data_transformed_.size(); i++) {
      if (data_class_ids_[i] != class_id)
        continue;
      for (unsigned j = 0; j < data_transformed_[i].size(); j++) {
        double dif = data_transformed_[i][j]-means[j]; 
        vars[j] += dif*dif;
      }
    }
    for (int j = 0; j < transformed_size_; j++) {
      vars[j] /= class_cnt;
    }
    means_.push_back(means);
    vars_.push_back(vars);
  }
  return true;
}
bool dbrec_bayesian_pca::visualize_transformed_distributions(const vcl_string& output_path, char class_id)
{
  if (!data_transformed_.size() || transformed_size_ < 0) {
    vcl_cout << "In dbrec_bayesian_pca::visualize_transformed_distributions() - problems in data transformation!\n";
    return false;
  }
  //: find the range in the transformed space
  double min = 10000.0;
  double max = -10000.0;
  for (unsigned i = 0; i < data_transformed_.size(); i++) {
    for (unsigned j = 0; j < data_transformed_[i].size(); j++) {
      if (data_transformed_[i][j] < min)
        min = data_transformed_[i][j];
      if (data_transformed_[i][j] > max)
        max = data_transformed_[i][j];
    }
  }
  vcl_cout << "In the transformed space, min value is: " << min << " max value is: " << max << vcl_endl;
  bsta_histogram<double> hist(-5.0, 5.0, 100);
  vcl_vector<bsta_histogram<double> > hists(transformed_size_, hist);

  for (unsigned i = 0; i < data_transformed_.size(); i++) {
    if (data_class_ids_[i] != class_id)
        continue;
    for (unsigned j = 0; j < data_transformed_[i].size(); j++) {
      hists[j].upcount(data_transformed_[i][j], 1.0);
    }
  }
  
  //: find the class in the order of classes_ map
  int classes_id = 0;
  for (vcl_map<char,int>::iterator it = classes_.begin(); it != classes_.end(); it++) {
    if (class_id == it->first)
      break;
    classes_id++;
  }

  float width = 600.0f, height = 600.0f, margin = 40.0f;
  int font_size = 30;
  for (unsigned j = 0; j < hists.size(); j++) {
    vcl_stringstream ids; ids << j;
    vcl_string name = output_path + "hist_" + ids.str() + ".svg";
    write_svg<double>(hists[j], name, width, height, margin, font_size);
    bsta_gaussian_sphere<double, 1> gd(means_[classes_id][j], vars_[classes_id][j]);

    //: first create a histogram
    bsta_histogram<double> h(-5.0, 5.0, 100);
    for (unsigned a = 0; a < h.nbins(); a++) {
      double min_val = a*h.delta() + h.min();
      double max_val = (a+1)*h.delta() + h.min();
      double prob = gd.probability(min_val, max_val);
      h.upcount(max_val, prob);
    }
    name = output_path + "hist_fitted_" + ids.str() + ".svg";
    write_svg<double>(h, name, width, height, margin, font_size);
  }
  return true;
}

//: classify each pixel in the image as the given class or not by generating a class map as the posterior map of the class
bool dbrec_bayesian_pca::classify_image_rectangles(const vil_image_view<float>& inp, const vil_image_view<bool>& inp_valid_map, int n, char class_id, float class_prior, vil_image_view<float>& output_map)
{
  unsigned ni = inp.ni();
  unsigned nj = inp.nj();
  
  int half_n = n/2;
  vgl_box_2d<int> image_box(0, ni-1, 0, nj-1); 

  //: assumes class and non_class
  if (classes_.size() != 2) {
    vcl_cout << "In dbrec_bayesian_pca::classify_image_rectangles() -- this method assumes there are two classes!\n";
    throw 0;
  }

  //: find the class id in the order of classes_ map
  int classes_id = 0;
  for (vcl_map<char,int>::iterator it = classes_.begin(); it != classes_.end(); it++) {
    if (class_id == it->first)
      break;
    classes_id++;
  }
  int classes_non_class_id = 0;
  for (vcl_map<char,int>::iterator it = classes_.begin(); it != classes_.end(); it++) {
    if (class_id != it->first)
      break;
    classes_non_class_id++;
  }
  if (classes_id == classes_non_class_id) {
    vcl_cout << "In dbrec_bayesian_pca::classify_image_rectangles() -- problem in the class ids!\n";
    throw 0;
  }

  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      
      int i_start = i-half_n; int i_end = i_start + n;
      int j_start = j-half_n; int j_end = j_start + n;

      if (!inp_valid_map(i,j) || !image_box.contains(i_start, j_start) || !image_box.contains(i_end-1, j_end-1)) 
        continue;

      vnl_vector<double> v = desc_->extract(inp, i_start, j_start, i_end, j_end);
      /*
      vnl_vector<double> v(n*n); unsigned vi = 0;
      for (int ii = i_start; ii < i_end; ii++) {
        for (int jj = j_start; jj < j_end; jj++) {
          v[vi] = inp(ii, jj);
          vi++;
        }
      }
      */

      //: now transform the vector
      vnl_vector<double> new_v = EVecs_.transpose()*v;
      if (new_v.size() != means_[classes_id].size()) {
        vcl_cout << "In dbrec_bayesian_pca::classify_image_rectangles() -- dimension mismatch!\n";
        throw 0;
      }
      
      double class_likelihood = class_prior;
      for (unsigned jj = 0; jj < new_v.size(); jj++) {
        bsta_gaussian_sphere<double, 1> gd(means_[classes_id][jj], vars_[classes_id][jj]);
        double prob = gd.prob_density(new_v[jj]);
        class_likelihood *= prob;
      }
      double non_class_likelihood = 1.0f-class_prior;
      for (unsigned jj = 0; jj < new_v.size(); jj++) {
        bsta_gaussian_sphere<double, 1> gd(means_[classes_non_class_id][jj], vars_[classes_non_class_id][jj]);
        double prob = gd.prob_density(new_v[jj]);
        non_class_likelihood *= prob;
      }
      double den = class_likelihood + non_class_likelihood;
      double posterior = class_likelihood / den;
      
      output_map(i,j) = (float)posterior;
    }
  return true;
}


dbrec_template_matching::dbrec_template_matching() 
{
  rng_.reseed((unsigned long)time(NULL));
}

dbrec_template_matching::~dbrec_template_matching()
{
  data_.clear();
}

void dbrec_template_matching::standardize_vector(vnl_vector<double>& v) {
  double mean = 0.0;
  unsigned vi;
  for (vi = 0; vi < v.size(); vi++) 
    mean += v[vi];
  mean /= v.size();
  double sd = 0.0;
  for (vi = 0; vi < v.size(); vi++) 
    sd += vcl_pow(v[vi]-mean, 2.0);
  sd = vcl_sqrt(sd/v.size());
  if (sd != 0.0) {
    for (vi = 0; vi < v.size(); vi++)
      v[vi] = (v[vi]-mean)/sd;
  } else {
    for (vi = 0; vi < v.size(); vi++)
      v[vi] = v[vi]-mean;
  }
}

void dbrec_template_matching::add_image_rectangles(const vil_image_view<float>& inp, int n, bvgl_changes_sptr gt_polygons, const vcl_string& ground_truth_type, char class_id)
{
  unsigned ni = inp.ni();
  unsigned nj = inp.nj();
  int half_n = n/2;
  vgl_box_2d<int> image_box(0, ni-1, 0, nj-1); 

  vcl_cout << "In dbrec_bayesian_pca::add_image_rectangles() - number of gt polygons: " << gt_polygons->size() << vcl_endl;
  int used_size = 0;
  for (unsigned i=0; i < gt_polygons->size(); i++)
  {
    if (gt_polygons->obj(i)->type().compare(ground_truth_type) != 0)
      continue;
    used_size++;
    //: find the centre of this polygon
    double x_cent = 0.0, y_cent = 0.0;
    gt_polygons->obj(i)->centroid(x_cent, y_cent);

    int i_start = (int)vcl_floor(x_cent+0.5)-half_n; int i_end = i_start + n;
    int j_start = (int)vcl_floor(y_cent+0.5)-half_n; int j_end = j_start + n;
    
    if (!image_box.contains(i_start, j_start) || !image_box.contains(i_end-1, j_end-1))
      continue;

    vnl_vector<double> v(n*n); unsigned vi = 0;
    for (int ii = i_start; ii < i_end; ii++) {
      for (int jj = j_start; jj < j_end; jj++) {
        v[vi] = inp(ii, jj);
        vi++;
      }
    }
    
    //: standardize v before adding to the data
    standardize_vector(v);

    data_.push_back(v);  
    data_class_ids_.push_back(class_id);
  }
  vcl_cout << "In dbrec_bayesian_pca::add_image_rectangles() - number of type: " << ground_truth_type << " polygons: " << used_size << vcl_endl;
  return;
}
//: the method adds cnt many new data vectors for a random selection of pixels in the map with given bool value 
void dbrec_template_matching::add_image_rectangles(const vil_image_view<float>& inp, int n, const vil_image_view<bool>& class_map, bool value, char class_id, int cnt)
{
  unsigned ni = inp.ni();
  unsigned nj = inp.nj();
  int half_n = n/2;
  vgl_box_2d<int> image_box(0, ni-1, 0, nj-1); 
  if (class_map.ni() != ni || class_map.nj() != nj) {
    vcl_cout << "In dbrec_bayesian_pca::add_image_rectangles() -- Input image size is inconsistent with the map size!\n";
    throw 0;
  }

  vcl_cout << "In dbrec_bayesian_pca::add_image_rectangles() -- generating: " << cnt << " random rectangles!\n";

  for (int k = 0; k < cnt; k++) {
    int i = rng_.lrand32(half_n+1, ni-1-half_n);
    int j = rng_.lrand32(half_n+1, nj-1-half_n);
    int i_start = i-half_n; int i_end = i_start + n;
    int j_start = j-half_n; int j_end = j_start + n;

    while (class_map(i,j) != value || !image_box.contains(i_start, j_start) || !image_box.contains(i_end-1, j_end-1)) {
      i = rng_.lrand32(half_n+1, ni-1-half_n);
      j = rng_.lrand32(half_n+1, nj-1-half_n);
      i_start = i-half_n; i_end = i_start + n;
      j_start = j-half_n; j_end = j_start + n;
    }
    
    vnl_vector<double> v(n*n); unsigned vi = 0;
    for (int ii = i_start; ii < i_end; ii++) {
      for (int jj = j_start; jj < j_end; jj++) {
        v[vi] = inp(ii, jj);
        vi++;
      }
    }

    //: standardize v before adding to the data
    standardize_vector(v);
    
    data_.push_back(v);  
    data_class_ids_.push_back(class_id);
  }
  return;
}

void dbrec_template_matching::save_image_rectangles(const vcl_string& prefix, int size, char class_id)
{
  for (unsigned i = 0; i < data_.size(); i++) {
    if (data_class_ids_[i] != class_id)
      continue;
    vnl_vector<double> v = data_[i];
    if (size*size == v.size()) {
      vil_image_view<float> img(size, size);
      for (int ii = 0; ii < size; ii++)
        for (int jj = 0; jj < size; jj++) {
          img(ii,jj) = (float)((v[ii*size + jj]+1.0)/2.0);
        }
      vil_math_scale_values(img,255.0f);
      vil_image_view<vxl_byte> img_b(size, size);
      vil_convert_cast(img, img_b);
      vcl_stringstream ids; ids << i;
      vcl_string name = prefix + "img_" + ids.str() + ".png";
      vil_save(img_b, name.c_str());
    }
  }
}
//: check the consistency of the data (sizes of the vectors)
bool dbrec_template_matching::check_data()
{
  if (data_.size() > 0) {
    unsigned d = data_[0].size();
    for (unsigned i = 1; i < data_.size(); i++)
      if (data_[i].size() != d) {
        return false;
      }
  }
  return true;
}

//: assumes that the two inputs have already been standardized so that they have zero mean and standard deviation 1
double normalized_cross_correlation(vnl_vector<double>& v1, vnl_vector<double>& v2) {
  double k = 0;
  for (unsigned i = 0; i < v1.size(); i++)
    k += v1[i]*v2[i];
  //: normalize k
  k /= (v1.size()-1);
  return k;
}

//: classify each pixel in the image as the given class or not by generating a class map as the posterior map of the class
bool dbrec_template_matching::classify_image_rectangles(const vil_image_view<float>& inp, const vil_image_view<bool>& inp_valid_map, int n, char class_id, vil_image_view<float>& output_map)
{
  unsigned ni = inp.ni();
  unsigned nj = inp.nj();
  
  int half_n = n/2;
  vgl_box_2d<int> image_box(0, ni-1, 0, nj-1); 

  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      
      int i_start = i-half_n; int i_end = i_start + n;
      int j_start = j-half_n; int j_end = j_start + n;

      if (!inp_valid_map(i,j) || !image_box.contains(i_start, j_start) || !image_box.contains(i_end-1, j_end-1)) 
        continue;

      vnl_vector<double> v(n*n); unsigned vi = 0;
      for (int ii = i_start; ii < i_end; ii++) {
        for (int jj = j_start; jj < j_end; jj++) {
          v[vi] = inp(ii, jj);
          vi++;
        }
      }

      //: standardize v before using
      standardize_vector(v);

      //: now find the normalized cross correlation value to all the training instances and pick the one with the highest value
      double max_norm_cross = -1.1;
      unsigned max_kk = 0;
      for (unsigned kk = 0; kk < data_.size(); kk++) {
        if (v.size() != data_[kk].size()) {
          vcl_cout << "In dbrec_template_matching::classify_image_rectangles() -- mismatching vector sizes!\n";
          throw 0;
        }
        double norm_cross = normalized_cross_correlation(v, data_[kk]);
        if (norm_cross > max_norm_cross) {
          max_norm_cross = norm_cross;
          max_kk = kk;
        }
      }
      
      if (data_class_ids_[max_kk] == class_id) {  // correct classification
        output_map(i,j) = (float)((max_norm_cross+1.0)/2.0);  // range of norm cross correlation value is [-1,1], so normalize to [0,1] to use as a probability value
      } else { // wrong classification, just assign 0.0 as prob
        output_map(i,j) = 0.0f;
      }
    }
  return true;
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_pca as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_pca const &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_pca as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & is, dbrec_pca &ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_pca as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, dbrec_pca* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    ph = new dbrec_pca();
    vsl_b_read(is, *ph);
  }
  else
    ph = 0;
}

void vsl_b_write(vsl_b_ostream& os, const dbrec_pca* &ph)
{
  if (ph==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_pca as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_bayesian_pca const &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_bayesian_pca as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & is, dbrec_bayesian_pca &ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_bayesian_pca as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, dbrec_bayesian_pca* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    ph = new dbrec_bayesian_pca(5,0); // binary io is not used so 5x5 is arbitrary and simplest image descriptor is the default
    vsl_b_read(is, *ph);
  }
  else
    ph = 0;
}

void vsl_b_write(vsl_b_ostream& os, const dbrec_bayesian_pca* &ph)
{
  if (ph==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_pca as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_template_matching const &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_template_matching as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & is, dbrec_template_matching &ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_template_matching as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, dbrec_template_matching* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    ph = new dbrec_template_matching; // binary io is not used so 5x5 is arbitrary and simplest image descriptor is the default
    vsl_b_read(is, *ph);
  }
  else
    ph = 0;
}

void vsl_b_write(vsl_b_ostream& os, const dbrec_template_matching* &ph)
{
  if (ph==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}
