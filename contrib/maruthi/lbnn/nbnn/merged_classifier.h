// Copyright (c) 2011, Sancho McCann

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:

// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef NAIVE_BAYES_NEAREST_NEIGHBOR_MERGED_CLASSIFIER_H_
#define NAIVE_BAYES_NEAREST_NEIGHBOR_MERGED_CLASSIFIER_H_

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include "glog/logging.h"

// TODO(sanchom): Extract Result to a common header.
#include "nbnn_classifier.h"
#include "sift/sift_descriptors.pb.h"
#include "vl/ikmeans.h"

namespace sjm {
namespace nbnn {

class MergedClassifier {
 public:
  MergedClassifier() :
      nearest_neighbors_(1),
      background_index_(2),
      alpha_(0), checks_(1), data_size_(0),
      index_built_(false), data_(NULL), data_dimensions_(0), index_(NULL),
      params_set_(false),beta_(2.2),factor_(0.0) {}

  ~MergedClassifier() {
    if (data_) {
      delete[] data_->ptr();
      delete data_;
    }

    if (dataset_) {
      delete[] dataset_->ptr();
      delete dataset_;
    }
  
    if (index_) {
      delete index_;
    }

    if (index_int32_) {
      delete index_int32_;
    }
 
  }

  void SetClassifierParams(const int nearest_neighbors,
                           const int background_index,
                           const float alpha,
                           const int checks,
                           const int trees,
                           const double beta,
                           const double factor) {
    nearest_neighbors_ = nearest_neighbors;
    background_index_ = background_index;
    alpha_ = alpha;
    checks_ = checks;
    trees_ = trees;
    params_set_ = true;
    beta_=beta;
    factor_=factor;
  }

  float distance(uint8_t* a,uint8_t* b,size_t size )
  {
      float result=0;
      for(size_t i = 0; i < size; ++i ) {
          float diff = *a++ - *b++;
          result += diff*diff;
      }
      return result;

  }

  float distance(int32_t* a,int32_t* b,size_t size )
  {
      float result=0;
      for(size_t i = 0; i < size; ++i ) {
          float diff = *a++ - *b++;
          result += diff*diff;
      }
      return result;

  }

  void AddData(const std::string& class_name,
               const sjm::sift::DescriptorSet& descriptors,
               const std::string& file_name) {
    CHECK(params_set_) << "Must SetClassifierParams() before adding data.";
    class_set_.insert(class_name);

    if (descriptors.sift_descriptor_size() > 0 && data_dimensions_ == 0) {
      data_dimensions_ = descriptors.sift_descriptor(0).bin_size();
      if (alpha_ > 0) {
        data_dimensions_ += 2;
      }
    } else if (descriptors.sift_descriptor_size() == 0) {
      // No data to add. Do nothing.
      return;
    }

    if (!data_) {
      // On the initial AddData, we just allocate enough storage for
      // 100% of the descriptors.
      data_ = new flann::Matrix<uint8_t>(
          new uint8_t[descriptors.sift_descriptor_size() * data_dimensions_],
          descriptors.sift_descriptor_size(), data_dimensions_);
    } else {
      // On subsequent additions, we double the size of the storage
      // until there's enough storage for the 100% of the additional
      // data.
      int required_data_size = data_size_ + descriptors.sift_descriptor_size();
      // TODO(sanchom): Handle the case where doubling the memory
      // allocation would fail, and adaptively back-off the requested
      // amount by calling new(nothrow).
      while (data_->rows < required_data_size) {
        uint64_t new_size = data_->rows * 2;
        LOG(INFO) << "Growing data matrix to " << new_size * data_dimensions_ <<
            " bytes";
        flann::Matrix<uint8_t>* larger_data =
            new flann::Matrix<uint8_t>(new uint8_t[new_size * data_dimensions_],
                                     new_size, data_dimensions_);
        // Copy old data into larger data space.
        std::copy((*data_)[0],
                  (*data_)[data_->rows - 1] + (data_->cols),
                  (*larger_data)[0]);
        // Delete smaller data structure.
        delete[] data_->ptr();
        delete data_;
        // Re-assign pointer to point to new, larger data structure.
        data_ = larger_data;
      }
    }

    // Put the descriptors into the data.
    for (int i = 0; i < descriptors.sift_descriptor_size(); ++i) {
      int converted_length =
          sjm::sift::ConvertProtobufDescriptorToWeightedArray(
              descriptors.sift_descriptor(i), alpha_,
              (*data_)[data_size_]);
      CHECK(converted_length == data_dimensions_) <<
          "Adding data with inconsistent dimensions.";
      class_vector_.push_back(class_name);
      ++data_size_;
      file_set_.push_back(file_name);
    }
  }

  int DataSize() const {
    return data_size_;
  }

  void ClusterData() {

    
      std::mt19937 rng;
      unsigned int codebook=100;

      dataset_ =
          new flann::Matrix<int32_t>(
              new int32_t[codebook*class_set_.size() * data_dimensions_],
              codebook*class_set_.size(), data_dimensions_);
      
      unsigned int i=0;
      std::set<std::string>::iterator it;
      for ( it = class_set_.begin() ; it != class_set_.end() ; ++it)
      {
          std::vector<std::string> temp_class;
          temp_class.push_back(*it);

          std::vector<std::string>::iterator vit_start;
          std::vector<std::string>::iterator vit_end;

          vit_start=find_first_of(class_vector_.begin(),
                            class_vector_.end(),
                            temp_class.begin(),
                            temp_class.end());

          vit_end=find_end(class_vector_.begin(),
                             class_vector_.end(),
                             temp_class.begin(),
                             temp_class.end());
                                 
          unsigned int start=std::distance(class_vector_.begin(),vit_start);
          unsigned int maximum_elements= std::distance(vit_start,vit_end); 
          maximum_elements=maximum_elements+1;

          std::cout<<"Maximum of class set: "
                   <<*it
                   <<" sift: "
                   <<maximum_elements<<std::endl;

          std::cout<<start<<" "<<class_vector_[start]<<std::endl;
          std::cout<<start+maximum_elements-1<<" "
                   <<class_vector_[start+maximum_elements-1]<<std::endl;

          /* std::uniform_int<int> distribution */
          /*     (start, */
          /*      start+maximum_elements-1); */

          /* unsigned int rand_sample=10; */
          //maximum_elements=codebook000;

          const size_t class_sample_size =
              maximum_elements * data_dimensions_;
          uint8_t* class_sift =
              new uint8_t[class_sample_size];
          //uint8_t* index_data=(data_->ptr()+start);
          std::copy((*data_)[start],
		    (*data_)[start+maximum_elements],
		    class_sift);
  
          /* for ( unsigned int i=0; i < rand_sample; ++i) */
          /* { */
          /*     int key=distribution(rng); */
          /*     int offset=data_dimensions_*key; */

          /*     std::copy(index_data+offset, */
          /*               index_data+offset+data_dimensions_, */
          /*               class_sift+i*data_dimensions_); */

          /*     std::cout<<"randkey: "<<key<<std::endl; */
          /* } */

          // start clustering

          VlIKMFilt* 	ikmeans=vl_ikm_new(VL_IKM_ELKAN);
          vl_ikm_set_verbosity(ikmeans,5);
          
          vl_ikm_init_rand(ikmeans,data_dimensions_,codebook);
          
          vl_tic();
          vl_ikm_train(ikmeans,class_sift,maximum_elements);
          double time_to_cluster = vl_toc();
          std::cout<<"Cluster time takes: "<<time_to_cluster<<std::endl;
 
          const vl_ikm_acc* centers=vl_ikm_get_centers (ikmeans); 

          std::copy(centers,centers+codebook*data_dimensions_,
                    (*dataset_)[i*codebook]);
          vl_ikm_delete(ikmeans);
          delete[] class_sift; 
          ikmeans=0;
          class_sift=0;

          i=i+1;
      }

      delete[] data_->ptr();
      delete data_;
      data_=0;

      index_int32_ = new flann::Index<flann::L2<int32_t> >(
          *dataset_, flann::KDTreeIndexParams(trees_));
      index_int32_->buildIndex();
      index_built_ = true;
      index_=0;
      data_size_=class_set_.size()*codebook;

      // Redo class vector
      class_vector_.clear();
      for ( it = class_set_.begin() ; it != class_set_.end() ; ++it)
      {

          for ( unsigned int k=0; k < codebook ; ++k)
          {
              class_vector_.push_back(*it);
          }


      }
  }

  void BuildIndex() {
    // First, truncate the data to the actual usage. This
    // unfortunately requires allocating additional memory, just to
    // get rid of memory.
    //
    // TODO(sanchom): Use realloc here, but it's complicated because
    // the Matrix object holds pointers to and info about the
    // allocated memory.
    flann::Matrix<uint8_t>* truncated_matrix =
        new flann::Matrix<uint8_t>(new uint8_t[data_size_ * data_dimensions_],
                                   data_size_, data_dimensions_);
    // Copy the data into smaller data space.
    std::copy((*data_)[0],
              (*data_)[data_size_ - 1] + (data_->cols),
              (*truncated_matrix)[0]);
    // Delete original, larger data structure.
    delete[] data_->ptr();
    delete data_;
    // Re-assign pointer to new, smaller data structure.
    data_ = truncated_matrix;
    // TODO(sanchom): Make num trees a parameter.
    index_ = new flann::Index<flann::L2<uint8_t> >(
        *data_, flann::KDTreeIndexParams(trees_));
    index_->buildIndex();
    index_built_ = true;
  }

  Result Classify(const sjm::sift::DescriptorSet& descriptor_set,
                  const float subsample_percentage) const {
    CHECK(index_built_) << "Must call .BuildIndex() before .Classify()";
    // We'll fetch background_index_ features for estimation, capped
    // at the data_size_.
    int b =
        std::min(data_size_, static_cast<uint64_t>(background_index_));
    // We'll use nearest_neighbors_ for foreground, capped at b - 1.
    int k =
        std::min(b - 1, nearest_neighbors_);
    // Set up the class distance accumulator.
    std::map<std::string, float> category_totals;
    for (std::set<std::string>::const_iterator it = class_set_.begin();
         it != class_set_.end();
         ++it) {
      category_totals[*it] = 0;
    }
    // Set up the data for the batch query.
    // First, create a temp array for up to as many descriptors as 100%.
    const size_t kTempSize =
        descriptor_set.sift_descriptor_size() * data_dimensions_;
    uint8_t* temp =
        new uint8_t[kTempSize];
    // Put a subsample of the data into the temp array.
    int next_matrix_index = 0;
    for (int i = 0; i < descriptor_set.sift_descriptor_size(); ++i) {
      if (std::rand() / static_cast<float>(RAND_MAX) < subsample_percentage) {
        sjm::sift::ConvertProtobufDescriptorToWeightedArray(
            descriptor_set.sift_descriptor(i),
            alpha_,
            temp + (next_matrix_index * data_dimensions_));
        ++next_matrix_index;
      }
    }
    // Move the actually used data from the temp array into one that
    // fits.  We don't need to delete this later because it's cleaned up
    // when we delete[] batch_query->data.
    const size_t kQuerySize =
        next_matrix_index * data_dimensions_;
    uint8_t* query_array =
        new uint8_t[kQuerySize];
    std::copy(temp, temp + (next_matrix_index * data_dimensions_),
              query_array);
    delete[] temp;

    // Set up the query for k + 1 nn.
    flann::Matrix<uint8_t> batch_query =
        flann::Matrix<uint8_t>(query_array,
                               next_matrix_index,
                               data_dimensions_);
    flann::Matrix<int> nn_index(new int[batch_query.rows * b],
                                batch_query.rows, b);
    flann::Matrix<float> dists(new float[batch_query.rows * b],
                               batch_query.rows, b);
    // Execute the query, getting indices and dists.
    index_->knnSearch(batch_query, nn_index, dists, b,
                      flann::SearchParams(checks_));
    // For each row, make a category adjustment map
    for (size_t row = 0; row < dists.rows; ++row) {
      std::map<std::string, float> category_distances;
      // The k+1st neighbor is used for the background distance.
      float background_distance = dists[row][b - 1] / 16129.0;
      for (size_t neighbor = 0; neighbor < k; ++neighbor) {
        // Find the category of this neighbor.
        int neighbor_index = nn_index[row][neighbor];
        std::string neighbor_class = class_vector_[neighbor_index];
        // If it's not already been seen,
        if (category_distances.find(neighbor_class) ==
            category_distances.end()) {
          // Put its distance in the map, substracting the background
          // distance.

          // This scales down the distance to be as if the original values
          // had been in [0,1] instead of in [0,127]. Useful in order to
          // avoid overflow errors in some of the probability estimate
          // models. (16129 = 127 * 127
          float distance_squared = dists[row][neighbor] / 16129.0;
          category_distances[neighbor_class] =
              distance_squared - background_distance;
        }
      }
      // Now, adjust the distance totals.
      for (std::map<std::string, float>::const_iterator it =
               category_distances.begin();
           it != category_distances.end();
           ++it) {
        category_totals[it->first] += it->second;
      }
    }
    delete[] batch_query.ptr();
    delete[] nn_index.ptr();
    delete[] dists.ptr();

    // Get the result.
    std::string best_class = "";
    float smallest_distance = 99999999999;
    for (std::map<std::string, float>::const_iterator it =
             category_totals.begin();
         it != category_totals.end(); ++it) {
      if (it->second < smallest_distance) {
        best_class = it->first;
        smallest_distance = it->second;
      }
    }
    Result result;
    result.category = best_class;
    return result;
  }

  Result Debug_Classify(const sjm::sift::DescriptorSet& descriptor_set,
                        const float subsample_percentage,
                        std::string title,
                        std::string true_category) {
    CHECK(index_built_) << "Must call .BuildIndex() before .Classify()";
    // We'll fetch background_index_ features for estimation, capped
    // at the data_size_.
    int b =
        std::min(data_size_, static_cast<uint64_t>(background_index_));
    // We'll use nearest_neighbors_ for foreground, capped at b - 1.
    int k =
        std::min(b - 1, nearest_neighbors_);
    // Set up the class distance accumulator.
    std::map<std::string, float> category_totals;
    for (std::set<std::string>::const_iterator it = class_set_.begin();
         it != class_set_.end();
         ++it) {
      category_totals[*it] = 0;
    }
    // Set up the data for the batch query.
    // First, create a temp array for up to as many descriptors as 100%.
    const size_t kTempSize =
        descriptor_set.sift_descriptor_size() * data_dimensions_;
    uint8_t* temp =
        new uint8_t[kTempSize];
    // Put a subsample of the data into the temp array.
    int next_matrix_index = 0;
    for (int i = 0; i < descriptor_set.sift_descriptor_size(); ++i) {
      if (std::rand() / static_cast<float>(RAND_MAX) < subsample_percentage) {
        sjm::sift::ConvertProtobufDescriptorToWeightedArray(
            descriptor_set.sift_descriptor(i),
            alpha_,
            temp + (next_matrix_index * data_dimensions_));
        ++next_matrix_index;
      }
    }
    // Move the actually used data from the temp array into one that
    // fits.  We don't need to delete this later because it's cleaned up
    // when we delete[] batch_query->data.
    const size_t kQuerySize =
        next_matrix_index * data_dimensions_;
    uint8_t* query_array =
        new uint8_t[kQuerySize];
    std::copy(temp, temp + (next_matrix_index * data_dimensions_),
              query_array);
    delete[] temp;

    // Set up the query for k + 1 nn.
    flann::Matrix<uint8_t> batch_query =
        flann::Matrix<uint8_t>(query_array,
                               next_matrix_index,
                               data_dimensions_);
    flann::Matrix<int> nn_index(new int[batch_query.rows * b],
                                batch_query.rows, b);
    flann::Matrix<float> dists(new float[batch_query.rows * b],
                               batch_query.rows, b);

    // Execute the query, getting indices and dists.
    index_->knnSearch(batch_query, nn_index, dists, b,
                      flann::SearchParams(checks_));
    std::map<size_t ,std::set<int> > pg_graph;

    int randkey=0;
    // Find the category of this neighbor.
    for (size_t ni = 0; ni < dists.rows; ++ni)
    {
        int neighbor_index=nn_index[ni][0];
        std::string neighbor_class = class_vector_[neighbor_index];
        if ( true_category==neighbor_class)
        {
            randkey=ni;
            break;
        }
    }

    //int randkey = rand() % batch_query.rows + 0; 
    std::cout<<"Index size: "<<index_->size()<<std::endl;
    std::cout<<"File set size: "<<file_set_.size()<<std::endl;
    std::cout<<" Sift key: "<<randkey<<std::endl;
    {
        std::string dist_title(title+"_dist_mat.txt");
        std::string label_title(title+"_label_mat.txt");
        std::string file_title(title+"_file_mat.txt");

        std::ofstream my_file(dist_title.c_str());

        // Compute proximity graph depending on beta
        for (size_t a = 0; a < k; ++a)
        {
            my_file<<dists[randkey][a]<<std::endl;            
        }

        for (size_t b = 0; b < k; ++b)
        {
            // Grab knn neighbor
            uint8_t* sj=index_->getPoint(nn_index[randkey][b]);
          
            for (size_t c = b+1; c < k; ++c)
            {
                    // Get distance between two shapes
                    uint8_t* sk=index_->getPoint(nn_index[randkey][c]);
                    float djk=distance(sj,sk,data_dimensions_);
                    my_file<<djk<<std::endl;
            }

        }
        my_file.close();
   
        std::ofstream my_file2(label_title.c_str());
        std::ofstream my_file3(file_title.c_str());
        //std::set<std::string> knn_classes;
        for (size_t neighbor = 0; neighbor < k ; ++neighbor) 
        {
            // Find the category of this neighbor.
            int neighbor_index = nn_index[randkey][neighbor];
            
            std::string neighbor_class = class_vector_[neighbor_index];
            std::string file_id = file_set_[neighbor_index];
         
            my_file2<<neighbor_class<<std::endl;
            my_file3<<file_id<<std::endl;
        }
        my_file2.close();
        my_file3.close();
    

    }

    delete[] batch_query.ptr();
    delete[] nn_index.ptr();
    delete[] dists.ptr();

    Result result2;
    result2.category = "emu";
    return result2;

  }

  Result Debug_Classify_Codebook(const sjm::sift::DescriptorSet& descriptor_set,
                                 const float subsample_percentage,
                                 std::string title,
                                 std::string true_category) {
    CHECK(index_built_) << "Must call .BuildIndex() before .Classify()";
    // We'll fetch background_index_ features for estimation, capped
    // at the data_size_.
    int b =
        std::min(data_size_, static_cast<uint64_t>(background_index_));
    // We'll use nearest_neighbors_ for foreground, capped at b - 1.
    int k =
        std::min(b - 1, nearest_neighbors_);
    // Set up the class distance accumulator.
    std::map<std::string, float> category_totals;
    for (std::set<std::string>::const_iterator it = class_set_.begin();
         it != class_set_.end();
         ++it) {
      category_totals[*it] = 0;
    }
    // Set up the data for the batch query.
    // First, create a temp array for up to as many descriptors as 100%.
    const size_t kTempSize =
        descriptor_set.sift_descriptor_size() * data_dimensions_;
    int32_t* temp =
        new int32_t[kTempSize];
    // Put a subsample of the data into the temp array.
    int next_matrix_index = 0;
    for (int i = 0; i < descriptor_set.sift_descriptor_size(); ++i) {
      if (std::rand() / static_cast<float>(RAND_MAX) < subsample_percentage) {
        sjm::sift::ConvertProtobufDescriptorToWeightedArray(
            descriptor_set.sift_descriptor(i),
            alpha_,
            temp + (next_matrix_index * data_dimensions_));
        ++next_matrix_index;
      }
    }
    // Move the actually used data from the temp array into one that
    // fits.  We don't need to delete this later because it's cleaned up
    // when we delete[] batch_query->data.
    const size_t kQuerySize =
        next_matrix_index * data_dimensions_;
    int32_t* query_array =
        new int32_t[kQuerySize];
    std::copy(temp, temp + (next_matrix_index * data_dimensions_),
              query_array);
    delete[] temp;

    // Set up the query for k + 1 nn.
    flann::Matrix<int32_t> batch_query =
        flann::Matrix<int32_t>(query_array,
                               next_matrix_index,
                               data_dimensions_);
    flann::Matrix<int> nn_index(new int[batch_query.rows * b],
                                batch_query.rows, b);
    flann::Matrix<float> dists(new float[batch_query.rows * b],
                               batch_query.rows, b);

    // Execute the query, getting indices and dists.
    index_int32_->knnSearch(batch_query, nn_index, dists, b,
                      flann::SearchParams(checks_));
    std::map<size_t ,std::set<int> > pg_graph;

    int randkey=0;
    // Find the category of this neighbor.
    for (size_t ni = 0; ni < dists.rows; ++ni)
    {
        int neighbor_index=nn_index[ni][0];
        std::string neighbor_class = class_vector_[neighbor_index];
        if ( true_category==neighbor_class)
        {
            randkey=ni;
            break;
        }
    }

    //int randkey = rand() % batch_query.rows + 0; 
    std::cout<<"Index size: "<<index_int32_->size()<<std::endl;
    std::cout<<"File set size: "<<file_set_.size()<<std::endl;
    std::cout<<" Sift key: "<<randkey<<std::endl;
    {
        std::string dist_title(title+"_dist_mat.txt");
        std::string label_title(title+"_label_mat.txt");
        std::string file_title(title+"_file_mat.txt");

        std::ofstream my_file(dist_title.c_str());

        // Compute proximity graph depending on beta
        for (size_t a = 0; a < k; ++a)
        {
            my_file<<dists[randkey][a]<<std::endl;            
        }

        for (size_t b = 0; b < k; ++b)
        {
            // Grab knn neighbor
            int32_t* sj=index_int32_->getPoint(nn_index[randkey][b]);
          
            for (size_t c = b+1; c < k; ++c)
            {
                    // Get distance between two shapes
                    int32_t* sk=index_int32_->getPoint(nn_index[randkey][c]);
                    float djk=distance(sj,sk,data_dimensions_);
                    my_file<<djk<<std::endl;
            }

        }
        my_file.close();
   
        std::ofstream my_file2(label_title.c_str());
        //  std::ofstream my_file3(file_title.c_str());
        //std::set<std::string> knn_classes;
        for (size_t neighbor = 0; neighbor < k ; ++neighbor) 
        {
            // Find the category of this neighbor.
            int neighbor_index = nn_index[randkey][neighbor];
            
            std::string neighbor_class = class_vector_[neighbor_index];
            //  std::string file_id = file_set_[neighbor_index];
         
            my_file2<<neighbor_class<<std::endl;
            //  my_file3<<file_id<<std::endl;
        }
        my_file2.close();
        // my_file3.close();
    

    }

    delete[] batch_query.ptr();
    delete[] nn_index.ptr();
    delete[] dists.ptr();

    Result result2;
    result2.category = "emu";
    return result2;

  }

  Result Pg_Classify(const sjm::sift::DescriptorSet& descriptor_set,
                     const float subsample_percentage) {
    CHECK(index_built_) << "Must call .BuildIndex() before .Classify()";
    // We'll fetch background_index_ features for estimation, capped
    // at the data_size_.
    int b =
        std::min(data_size_, static_cast<uint64_t>(background_index_));
    // We'll use nearest_neighbors_ for foreground, capped at b - 1.
    int k =
        std::min(b - 1, nearest_neighbors_);
    // Set up the class distance accumulator.
    std::map<std::string, float> category_totals;
    for (std::set<std::string>::const_iterator it = class_set_.begin();
         it != class_set_.end();
         ++it) {
      category_totals[*it] = 0;
    }
    // Set up the data for the batch query.
    // First, create a temp array for up to as many descriptors as 100%.
    const size_t kTempSize =
        descriptor_set.sift_descriptor_size() * data_dimensions_;
    uint8_t* temp =
        new uint8_t[kTempSize];
    // Put a subsample of the data into the temp array.
    int next_matrix_index = 0;
    for (int i = 0; i < descriptor_set.sift_descriptor_size(); ++i) {
      if (std::rand() / static_cast<float>(RAND_MAX) < subsample_percentage) {
        sjm::sift::ConvertProtobufDescriptorToWeightedArray(
            descriptor_set.sift_descriptor(i),
            alpha_,
            temp + (next_matrix_index * data_dimensions_));
        ++next_matrix_index;
      }
    }
    // Move the actually used data from the temp array into one that
    // fits.  We don't need to delete this later because it's cleaned up
    // when we delete[] batch_query->data.
    const size_t kQuerySize =
        next_matrix_index * data_dimensions_;
    uint8_t* query_array =
        new uint8_t[kQuerySize];
    std::copy(temp, temp + (next_matrix_index * data_dimensions_),
              query_array);
    delete[] temp;

    // Set up the query for k + 1 nn.
    flann::Matrix<uint8_t> batch_query =
        flann::Matrix<uint8_t>(query_array,
                               next_matrix_index,
                               data_dimensions_);
    flann::Matrix<int> nn_index(new int[batch_query.rows * b],
                                batch_query.rows, b);
    flann::Matrix<float> dists(new float[batch_query.rows * b],
                               batch_query.rows, b);

    // Execute the query, getting indices and dists.
    index_->knnSearch(batch_query, nn_index, dists, b,
                      flann::SearchParams(checks_));
    std::map<size_t ,std::set<int> > pg_graph;

    /* { */
    /*     std::ofstream my_file("dist_mat.txt"); */

    /*     // Compute proximity graph depending on beta */
    /*     for (size_t a = 0; a < k; ++a) */
    /*     { */
    /*         my_file<<dists[0][a]<<std::endl; */
    /*     } */

    /*     for (size_t b = 0; b < k; ++b) */
    /*     { */
    /*         // Grab knn neighbor */
    /*         uint8_t* sj=index_->getPoint(nn_index[0][b]); */
          
    /*         for (size_t c = b+1; c < k; ++c) */
    /*         { */
    /*                 // Get distance between two shapes */
    /*                 uint8_t* sk=index_->getPoint(nn_index[0][c]); */
    /*                 float djk=distance(sj,sk,data_dimensions_); */
    /*                 my_file<<djk<<std::endl; */
    /*         } */

    /*     } */
    /*     my_file.close(); */
   
    /* } */
   
    bool flag=true;
    // Compute proximity graph depending on beta
    for (size_t ni = 0; ni < dists.rows; ++ni) 
    {
        // query element
        uint8_t* si=&query_array[ni*data_dimensions_];
        for (size_t nj = 0; nj < k; ++nj) 
        {
            // Reset flag for this neighbor
            flag=true;

            // Grab knn neighbor
            uint8_t* sj=index_->getPoint(nn_index[ni][nj]);
            float dij=dists[ni][nj];
          
            for (size_t nk = 0; nk < nj; ++nk)
            {
                if ( nk != nj )
                {

                    // Get distance between two shapes
                    uint8_t* sk=index_->getPoint(nn_index[ni][nk]);
             
                    float dik=dists[ni][nk];
                    float djk=distance(sj,sk,data_dimensions_);
                    
                          
                    if ( beta_ < 1)
                    {
                        // Beta_ less than 1 case
                        if (  (dij*dij)
                              > ((dik*dik) +
                                 (djk*djk) +
                                 2*std::sqrt(1-(beta_*beta_))*
                                 (dik*djk)  ))
                        {
                            flag=false;
                            break;
                        }
                    }
                    else
                    {
                        // Beta_ greater than 1 case
                        if ( dij*dij > std::max(
                                 ((dik*dik)*((2/beta_)-1))+(djk*djk)
                                 ,(dik*dik)+(((2/beta_)-1)*(djk*djk)) ))
                        {
                        
                            flag=false;
                            break;
                        
                        }
                    
                    }
                }
                
                
            }
            
            if(flag)
            {
                pg_graph[ni].insert(nn_index[ni][nj]);
            }
        }
    }

    std::map<size_t ,std::set<int> >::iterator it;
    std::vector<double> avg_neighbors;
    double avg_edges=0.0;
    for  ( it = pg_graph.begin() ; it != pg_graph.end() ; ++it)
    {
        std::set<int> pg_neighbors = (*it).second;
        avg_neighbors.push_back(pg_neighbors.size());
        avg_edges = avg_edges+pg_neighbors.size();
    }
    LOG(INFO) << " Factor= " <<factor_
              << " Beta= " <<beta_
              <<" stats, min edges: "<<*std::min_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" max edges: "<<*std::max_element(
                avg_neighbors.begin(), avg_neighbors.end())
              <<" avg edges: "<<avg_edges/avg_neighbors.size()<<std::endl;
    

    // For each row, make a category adjustment map
    for (size_t row = 0; row < dists.rows; ++row) {
      std::map<std::string, float> category_distances;
      // The k+1st neighbor is used for the background distance.
      float background_distance = dists[row][b - 1] / 16129.0;
      for (size_t neighbor = 0; neighbor < k; ++neighbor) {
        // Find the category of this neighbor.
        int neighbor_index = nn_index[row][neighbor];
        std::string neighbor_class = class_vector_[neighbor_index];
        float ndist=dists[row][neighbor];
        // If it's not already been seen,
        if (ndist< factor_*dists[row][0] && 
            (category_distances.find(neighbor_class) ==
             category_distances.end()) && pg_graph[row].count(neighbor_index)) {
          // Put its distance in the map, substracting the background
          // distance.

          // This scales down the distance to be as if the original values
          // had been in [0,1] instead of in [0,127]. Useful in order to
          // avoid overflow errors in some of the probability estimate
          // models. (16129 = 127 * 127
          float distance_squared = dists[row][neighbor] / 16129.0;
          category_distances[neighbor_class] =
              distance_squared - background_distance;
        }
      }
      // Now, adjust the distance totals.
      for (std::map<std::string, float>::const_iterator it =
               category_distances.begin();
           it != category_distances.end();
           ++it) {
        category_totals[it->first] += it->second;
      }
    }
    delete[] batch_query.ptr();
    delete[] nn_index.ptr();
    delete[] dists.ptr();

    // Get the result.
    std::string best_class = "";
    float smallest_distance = 99999999999;
    for (std::map<std::string, float>::const_iterator it =
             category_totals.begin();
         it != category_totals.end(); ++it) {
      if (it->second < smallest_distance) {
        best_class = it->first;
        smallest_distance = it->second;
      }
    }
    Result result;
    result.category = best_class;
    return result;
  }

  Result Pg_Classify_rad_search(const sjm::sift::DescriptorSet& descriptor_set,
                                const float subsample_percentage) {
    CHECK(index_built_) << "Must call .BuildIndex() before .Classify()";
    // We'll fetch background_index_ features for estimation, capped
    // at the data_size_.
    int b =
        std::min(data_size_, static_cast<uint64_t>(background_index_));
    // We'll use nearest_neighbors_ for foreground, capped at b - 1.
    int k =
        std::min(b - 1, nearest_neighbors_);
    // Set up the class distance accumulator.
    std::map<std::string, float> category_totals;
    for (std::set<std::string>::const_iterator it = class_set_.begin();
         it != class_set_.end();
         ++it) {
      category_totals[*it] = 0;
    }
    // Set up the data for the batch query.
    // First, create a temp array for up to as many descriptors as 100%.
    const size_t kTempSize =
        descriptor_set.sift_descriptor_size() * data_dimensions_;
    uint8_t* temp =
        new uint8_t[kTempSize];
    // Put a subsample of the data into the temp array.
    int next_matrix_index = 0;
    for (int i = 0; i < descriptor_set.sift_descriptor_size(); ++i) {
      if (std::rand() / static_cast<float>(RAND_MAX) < subsample_percentage) {
        sjm::sift::ConvertProtobufDescriptorToWeightedArray(
            descriptor_set.sift_descriptor(i),
            alpha_,
            temp + (next_matrix_index * data_dimensions_));
        ++next_matrix_index;
      }
    }
    // Move the actually used data from the temp array into one that
    // fits.  We don't need to delete this later because it's cleaned up
    // when we delete[] batch_query->data.
    const size_t kQuerySize =
        next_matrix_index * data_dimensions_;
    uint8_t* query_array =
        new uint8_t[kQuerySize];
    std::copy(temp, temp + (next_matrix_index * data_dimensions_),
              query_array);
    delete[] temp;

    // Set up the query for k + 1 nn.
    flann::Matrix<uint8_t> batch_query =
        flann::Matrix<uint8_t>(query_array,
                               next_matrix_index,
                               data_dimensions_);
    flann::Matrix<int> nn_index(new int[batch_query.rows],
                                batch_query.rows, 1);
    flann::Matrix<float> dists(new float[batch_query.rows],
                               batch_query.rows, 1);

    // Execute the query, getting indices and dists.
    index_->knnSearch(batch_query, nn_index, dists, 1,
                      flann::SearchParams(checks_));


    
    /* double max_radius=0.0; */
    /* double sum=0.0; */

    /* for (size_t row = 0; row < dists.rows; ++row)  */
    /* { */
    /*   double temp = dists[row][b - 1]; */
    /*   sum=sum+temp; */

    /*   if ( temp > max_radius) */
    /*   { */
    /*       max_radius=temp; */
    /*   } */

    /* } */


    /* double mean_radius = sum/dists.rows; */


    std::map<size_t ,std::set<int> > pg_graph;

    /* { */
    /*     std::ofstream my_file("dist_mat.txt"); */

    /*     // Compute proximity graph depending on beta */
    /*     for (size_t a = 0; a < dists_rad_search[0].size(); ++a) */
    /*     { */
    /*         my_file<<dists_rad_search[0][a]<<std::endl; */
    /*     } */

    /*     for (size_t b = 0; b < dists_rad_search[0].size(); ++b) */
    /*     { */
    /*         // Grab knn neighbor */
    /*         uint8_t* sj=index_->getPoint(nn_rad_search_indices[0][b]); */
          
    /*         for (size_t c = b+1; c < dists_rad_search[0].size(); ++c) */
    /*         { */
    /*                 // Get distance between two shapes */
    /*                 uint8_t* sk=index_->getPoint(nn_rad_search_indices[0][c]); */
    /*                 float djk=distance(sj,sk,data_dimensions_); */
    /*                 my_file<<djk<<std::endl; */
    /*         } */

    /*     } */
    /*     my_file.close(); */
   
    /* } */
    std::vector< std::vector<int> > nn_rad_search_indices;
    std::vector< std::vector<float> > dists_rad_search;
    std::vector< float > background_distances;

    bool flag=true;
    // Compute proximity graph depending on beta
    for (size_t ni = 0; ni < dists.rows ; ++ni) 
    {

        std::vector< std::vector<int> > local_nn_rad_search_indices;
        std::vector< std::vector<float> > local_dists_rad_search;

        uint8_t* si=&query_array[ni*data_dimensions_];

        // Set up the query for k + 1 nn.
        flann::Matrix<uint8_t> query =
            flann::Matrix<uint8_t>(si,
                                   1,
                                   data_dimensions_);
        double radius=dists[ni][0]*3.0;

        // Execute a radius search using backgound distance
        index_->radiusSearch(query, 
                             local_nn_rad_search_indices, 
                             local_dists_rad_search, 
                             radius,
                             flann::SearchParams(checks_));

        /* LOG(INFO)<<" Max radius search: "<<radius<<std::endl; */
        /* LOG(INFO)<<" Neighbors in hyper-sphere: " */
        /*          <<local_nn_rad_search_indices[0].size()<<std::endl; */

        nn_rad_search_indices.push_back(local_nn_rad_search_indices[0]);
        dists_rad_search.push_back(local_dists_rad_search[0]);

        background_distances.push_back( radius );
        for (size_t nj = 0; nj < local_nn_rad_search_indices[0].size() ; ++nj) 
        {
            // Reset flag for this neighbor
            flag=true;

            // Grab knn neighbor
            uint8_t* sj=index_->getPoint(local_nn_rad_search_indices[0][nj]);
            float dij=local_dists_rad_search[0][nj];
          
            for (size_t nk = 0; nk < nj ; ++nk)
            {
                if ( nk != nj )
                {

                    // Get distance between two shapes
                    uint8_t* sk=index_->getPoint(
                        local_nn_rad_search_indices[0][nk]);
             
                    float dik=local_dists_rad_search[0][nk];
                    float djk=distance(sj,sk,data_dimensions_);
                    
                          
                    if ( beta_ < 1)
                    {
                        // Beta_ less than 1 case
                        if (  (dij*dij)
                              > ((dik*dik) +
                                 (djk*djk) +
                                 2*std::sqrt(1-(beta_*beta_))*
                                 (dik*djk)  ))
                        {
                            flag=false;
                            break;
                        }
                    }
                    else
                    {
                        // Beta_ greater than 1 case
                        if ( dij*dij > std::max(
                                 ((dik*dik)*((2/beta_)-1))+(djk*djk)
                                 ,(dik*dik)+(((2/beta_)-1)*(djk*djk)) ))
                        {
                        
                            flag=false;
                            break;
                        
                        }
                    
                    }
                }
                
                
            }
            
            if(flag)
            {
                pg_graph[ni].insert(local_nn_rad_search_indices[0][nj]);
            }
        }
        /* LOG(INFO)<<" Neighbors in pg graph: " */
        /*          <<pg_graph[ni].size()<<std::endl; */

    }

    std::map<size_t ,std::set<int> >::iterator it;
    std::vector<double> avg_neighbors;
    double avg_edges=0.0;
    for  ( it = pg_graph.begin() ; it != pg_graph.end() ; ++it)
    {
        std::set<int> pg_neighbors = (*it).second;
        avg_neighbors.push_back(pg_neighbors.size());
        avg_edges = avg_edges+pg_neighbors.size();
    }

    LOG(INFO) << "Beta= " <<beta_
              <<" stats, min edges: "<<*std::min_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" max edges: "<<*std::max_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" avg edges: "<<avg_edges/avg_neighbors.size()<<std::endl;
    

    // For each row, make a category adjustment map
    for (size_t row = 0; row < dists_rad_search.size(); ++row) 
    {
        std::map<std::string, float> category_distances;
        // The k+1st neighbor is used for the background distance.
        float background_distance =background_distances[row];
        for (size_t neighbor = 0; neighbor < nn_rad_search_indices[row].size() 
                 ; ++neighbor) 
        {
            // Find the category of this neighbor.
            int neighbor_index = nn_rad_search_indices[row][neighbor];
            std::string neighbor_class = class_vector_[neighbor_index];
            // If it's not already been seen,
            if (category_distances.find(neighbor_class) ==
                category_distances.end() && 
                pg_graph[row].count(neighbor_index))
            {
                // Put its distance in the map, substracting the background
                // distance.
                
                // This scales down the distance to be as if the original values
                // had been in [0,1] instead of in [0,127]. Useful in order to
                // avoid overflow errors in some of the probability estimate
                // models. (16129 = 127 * 127
                float distance_squared= dists_rad_search[row][neighbor]/16129.0;
                category_distances[neighbor_class] =
                    distance_squared - background_distance;
            }
        }
        // Now, adjust the distance totals.
        for (std::map<std::string, float>::const_iterator it =
                 category_distances.begin();
             it != category_distances.end();
             ++it) {
            category_totals[it->first] += it->second;
        }
    }
    delete[] batch_query.ptr();
    delete[] nn_index.ptr();
    delete[] dists.ptr();
    dists_rad_search.clear();
    nn_rad_search_indices.clear();

    // Get the result.
    std::string best_class = "";
    float smallest_distance = 99999999999;
    for (std::map<std::string, float>::const_iterator it =
             category_totals.begin();
         it != category_totals.end(); ++it) {
      if (it->second < smallest_distance) {
        best_class = it->first;
        smallest_distance = it->second;
      }
    }
    Result result;
    result.category = best_class;
    return result;
  }

 Result Pg_Classify_MajVote(const sjm::sift::DescriptorSet& descriptor_set,
                            const float subsample_percentage) {
    CHECK(index_built_) << "Must call .BuildIndex() before .Classify()";
    // We'll fetch background_index_ features for estimation, capped
    // at the data_size_.
    int b =
        std::min(data_size_, static_cast<uint64_t>(background_index_));
    // We'll use nearest_neighbors_ for foreground, capped at b - 1.
    int k =
        std::min(b - 1, nearest_neighbors_);
    // Set up the class distance accumulator.
    std::map<std::string, float> category_totals;
    for (std::set<std::string>::const_iterator it = class_set_.begin();
         it != class_set_.end();
         ++it) {
      category_totals[*it] = 0;
    }
    // Set up the data for the batch query.
    // First, create a temp array for up to as many descriptors as 100%.
    const size_t kTempSize =
        descriptor_set.sift_descriptor_size() * data_dimensions_;
    uint8_t* temp =
        new uint8_t[kTempSize];
    // Put a subsample of the data into the temp array.
    int next_matrix_index = 0;
    for (int i = 0; i < descriptor_set.sift_descriptor_size(); ++i) {
      if (std::rand() / static_cast<float>(RAND_MAX) < subsample_percentage) {
        sjm::sift::ConvertProtobufDescriptorToWeightedArray(
            descriptor_set.sift_descriptor(i),
            alpha_,
            temp + (next_matrix_index * data_dimensions_));
        ++next_matrix_index;
      }
    }
    // Move the actually used data from the temp array into one that
    // fits.  We don't need to delete this later because it's cleaned up
    // when we delete[] batch_query->data.
    const size_t kQuerySize =
        next_matrix_index * data_dimensions_;
    uint8_t* query_array =
        new uint8_t[kQuerySize];
    std::copy(temp, temp + (next_matrix_index * data_dimensions_),
              query_array);
    delete[] temp;

    // Set up the query for k + 1 nn.
    flann::Matrix<uint8_t> batch_query =
        flann::Matrix<uint8_t>(query_array,
                               next_matrix_index,
                               data_dimensions_);
    flann::Matrix<int> nn_index(new int[batch_query.rows * b],
                                batch_query.rows, b);
    flann::Matrix<float> dists(new float[batch_query.rows * b],
                               batch_query.rows, b);

    // Execute the query, getting indices and dists.
    index_->knnSearch(batch_query, nn_index, dists, b,
                      flann::SearchParams(checks_));
    std::map<size_t ,std::set<int> > pg_graph;

    /* { */
    /*     std::ofstream my_file("dist_mat.txt"); */

    /*     // Compute proximity graph depending on beta */
    /*     for (size_t a = 0; a < k; ++a) */
    /*     { */
    /*         my_file<<dists[0][a]<<std::endl; */
    /*     } */

    /*     for (size_t b = 0; b < k; ++b) */
    /*     { */
    /*         // Grab knn neighbor */
    /*         uint8_t* sj=index_->getPoint(nn_index[0][b]); */
          
    /*         for (size_t c = b+1; c < k; ++c) */
    /*         { */
    /*                 // Get distance between two shapes */
    /*                 uint8_t* sk=index_->getPoint(nn_index[0][c]); */
    /*                 float djk=distance(sj,sk,data_dimensions_); */
    /*                 my_file<<djk<<std::endl; */
    /*         } */

    /*     } */
    /*     my_file.close(); */
   
    /* } */
    std::vector< std::map<std::string,std::vector<float> > > pg_map;

    bool flag=true;
    // Compute proximity graph depending on beta
    for (size_t ni = 0; ni < dists.rows ; ++ni) 
    {
        // query element
        uint8_t* si=&query_array[ni*data_dimensions_];

        std::map<std::string,std::vector<float> > local_map;
        for (size_t nj = 0; nj < k; ++nj) 
        {
            // Reset flag for this neighbor
            flag=true;

            // Grab knn neighbor
            uint8_t* sj=index_->getPoint(nn_index[ni][nj]);
            float dij=dists[ni][nj];
          
            for (size_t nk = 0; nk < nj; ++nk)
            {
                if ( nk != nj )
                {

                    // Get distance between two shapes
                    uint8_t* sk=index_->getPoint(nn_index[ni][nk]);
             
                    float dik=dists[ni][nk];
                    float djk=distance(sj,sk,data_dimensions_);
                    
                          
                    if ( beta_ < 1)
                    {
                        // Beta_ less than 1 case
                        if (  (dij*dij)
                              > ((dik*dik) +
                                 (djk*djk) +
                                 2*std::sqrt(1-(beta_*beta_))*
                                 (dik*djk)  ))
                        {
                            flag=false;
                            break;
                        }
                    }
                    else
                    {
                        // Beta_ greater than 1 case
                        if ( dij*dij > std::max(
                                 ((dik*dik)*((2/beta_)-1))+(djk*djk)
                                 ,(dik*dik)+(((2/beta_)-1)*(djk*djk)) ))
                        {
                        
                            flag=false;
                            break;
                        
                        }
                    
                    }
                }
                
                
            }
            
            if(flag)
            {
                pg_graph[ni].insert(nn_index[ni][nj]);
                std::string class_string = class_vector_[nn_index[ni][nj]];
                local_map[class_string].push_back(dij);

            }
        }
        pg_map.push_back(local_map);
    }

    std::map<size_t ,std::set<int> >::iterator it;
    std::vector<double> avg_neighbors;
    double avg_edges=0.0;
    for  ( it = pg_graph.begin() ; it != pg_graph.end() ; ++it)
    {
        std::set<int> pg_neighbors = (*it).second;
        avg_neighbors.push_back(pg_neighbors.size());
        avg_edges = avg_edges+pg_neighbors.size();
    }

    LOG(INFO) << "Maj Vote Beta= " <<beta_
              <<" stats, min edges: "<<*std::min_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" max edges: "<<*std::max_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" avg edges: "<<avg_edges/avg_neighbors.size()<<std::endl;
    
    std::vector<std::string> majority_classes;
    // Print map
    {
        for ( unsigned int i=0; i < pg_map.size() ; ++i)
        {
            std::map<std::string,std::vector<float> > local_map=
                pg_map[i];
            std::map<std::string,std::vector<float> >::iterator it;
            std::string majority_class="";
            unsigned int votes_class=0;

            for ( it = local_map.begin() ; it != local_map.end() ; ++it)
            {
                std::vector<float> local_dists=(*it).second;

                if ( local_dists.size() > votes_class )
                {
                    votes_class=local_dists.size();
                    majority_class=(*it).first;
                }


                //   std::cout<<(*it).first<<" :";
                /* for ( unsigned int j=0; j < local_dists.size() ; ++j) */
                /* { */
                /*     std::cout<<local_dists[j]<<" "; */
                /* } */
                /* std::cout<<std::endl;  */
                /* std::cout<<"Distance to class: "<<*std::min_element( */
                /* local_dists.begin(),local_dists.end())<<std::endl; */
            }
            /* std::cout<<std::endl; */
            /* std::cout<<"Majority class: "<<majority_class<<std::endl; */
            /* std::cout<<std::endl; */
            majority_classes.push_back(majority_class);
        }

    }

    // For each row, make a category adjustment map
    for (size_t row = 0; row < dists.rows ; ++row) 
    {
        std::string key=majority_classes[row];
        category_totals[key] += 1;
    }
    
    delete[] batch_query.ptr();
    delete[] nn_index.ptr();
    delete[] dists.ptr();

    // Get the result.
    std::string best_class = "";
    float smallest_distance = 0;
    for (std::map<std::string, float>::const_iterator it =
             category_totals.begin();
         it != category_totals.end(); ++it) {
        //   std::cout<<" Category: "<<it->first<<" votes "<<it->second<<std::endl;
      if (it->second > smallest_distance) {
        best_class = it->first;
        smallest_distance = it->second;
      }
    }
    Result result;
    result.category = best_class;
    return result;
  }

 Result Weighted_Vote(const sjm::sift::DescriptorSet& descriptor_set,
                      const float subsample_percentage) {
    CHECK(index_built_) << "Must call .BuildIndex() before .Classify()";
    // We'll fetch background_index_ features for estimation, capped
    // at the data_size_.
    int b =
        std::min(data_size_, static_cast<uint64_t>(background_index_));
    // We'll use nearest_neighbors_ for foreground, capped at b - 1.
    int k =
        std::min(b - 1, nearest_neighbors_);
    // Set up the class distance accumulator.
    std::map<std::string, float> category_totals;
    for (std::set<std::string>::const_iterator it = class_set_.begin();
         it != class_set_.end();
         ++it) {
      category_totals[*it] = 0;
    }
    // Set up the data for the batch query.
    // First, create a temp array for up to as many descriptors as 100%.
    const size_t kTempSize =
        descriptor_set.sift_descriptor_size() * data_dimensions_;
    uint8_t* temp =
        new uint8_t[kTempSize];
    // Put a subsample of the data into the temp array.
    int next_matrix_index = 0;
    for (int i = 0; i < descriptor_set.sift_descriptor_size(); ++i) {
      if (std::rand() / static_cast<float>(RAND_MAX) < subsample_percentage) {
        sjm::sift::ConvertProtobufDescriptorToWeightedArray(
            descriptor_set.sift_descriptor(i),
            alpha_,
            temp + (next_matrix_index * data_dimensions_));
        ++next_matrix_index;
      }
    }
    // Move the actually used data from the temp array into one that
    // fits.  We don't need to delete this later because it's cleaned up
    // when we delete[] batch_query->data.
    const size_t kQuerySize =
        next_matrix_index * data_dimensions_;
    uint8_t* query_array =
        new uint8_t[kQuerySize];
    std::copy(temp, temp + (next_matrix_index * data_dimensions_),
              query_array);
    delete[] temp;

    // Set up the query for k + 1 nn.
    flann::Matrix<uint8_t> batch_query =
        flann::Matrix<uint8_t>(query_array,
                               next_matrix_index,
                               data_dimensions_);
    flann::Matrix<int> nn_index(new int[batch_query.rows * b],
                                batch_query.rows, b);
    flann::Matrix<float> dists(new float[batch_query.rows * b],
                               batch_query.rows, b);

    // Execute the query, getting indices and dists.
    index_->knnSearch(batch_query, nn_index, dists, b,
                      flann::SearchParams(checks_));
    std::map<size_t ,std::set<int> > pg_graph;

    /* { */
    /*     std::ofstream my_file("dist_mat.txt"); */

    /*     // Compute proximity graph depending on beta */
    /*     for (size_t a = 0; a < k; ++a) */
    /*     { */
    /*         my_file<<dists[0][a]<<std::endl; */
    /*     } */

    /*     for (size_t b = 0; b < k; ++b) */
    /*     { */
    /*         // Grab knn neighbor */
    /*         uint8_t* sj=index_->getPoint(nn_index[0][b]); */
          
    /*         for (size_t c = b+1; c < k; ++c) */
    /*         { */
    /*                 // Get distance between two shapes */
    /*                 uint8_t* sk=index_->getPoint(nn_index[0][c]); */
    /*                 float djk=distance(sj,sk,data_dimensions_); */
    /*                 my_file<<djk<<std::endl; */
    /*         } */

    /*     } */
    /*     my_file.close(); */
   
    /* } */
    std::vector< std::map<std::string,std::vector<float> > > pg_map;

    bool flag=true;
    // Compute proximity graph depending on beta
    for (size_t ni = 0; ni < dists.rows ; ++ni) 
    {
        // query element
        uint8_t* si=&query_array[ni*data_dimensions_];

        std::map<std::string,std::vector<float> > local_map;
        for (size_t nj = 0; nj < k; ++nj) 
        {
            // Reset flag for this neighbor
            flag=true;

            // Grab knn neighbor
            uint8_t* sj=index_->getPoint(nn_index[ni][nj]);
            float dij=dists[ni][nj];
          
            for (size_t nk = 0; nk < nj; ++nk)
            {
                if ( nk != nj )
                {

                    // Get distance between two shapes
                    uint8_t* sk=index_->getPoint(nn_index[ni][nk]);
             
                    float dik=dists[ni][nk];
                    float djk=distance(sj,sk,data_dimensions_);
                    
                          
                    if ( beta_ < 1)
                    {
                        // Beta_ less than 1 case
                        if (  (dij*dij)
                              > ((dik*dik) +
                                 (djk*djk) +
                                 2*std::sqrt(1-(beta_*beta_))*
                                 (dik*djk)  ))
                        {
                            flag=false;
                            break;
                        }
                    }
                    else
                    {
                        // Beta_ greater than 1 case
                        if ( dij*dij > std::max(
                                 ((dik*dik)*((2/beta_)-1))+(djk*djk)
                                 ,(dik*dik)+(((2/beta_)-1)*(djk*djk)) ))
                        {
                        
                            flag=false;
                            break;
                        
                        }
                    
                    }
                }
                
                
            }
            
            if(flag)
            {
                pg_graph[ni].insert(nn_index[ni][nj]);
                std::string class_string = class_vector_[nn_index[ni][nj]];
                local_map[class_string].push_back(dij);

            }
        }
        pg_map.push_back(local_map);
    }

    std::map<size_t ,std::set<int> >::iterator it;
    std::vector<double> avg_neighbors;
    double avg_edges=0.0;
    for  ( it = pg_graph.begin() ; it != pg_graph.end() ; ++it)
    {
        std::set<int> pg_neighbors = (*it).second;
        avg_neighbors.push_back(pg_neighbors.size());
        avg_edges = avg_edges+pg_neighbors.size();
    }

    LOG(INFO) <<" Factor "<<factor_
              << " Weighted Vote Beta= " <<beta_
              <<" stats, min edges: "<<*std::min_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" max edges: "<<*std::max_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" avg edges: "<<avg_edges/avg_neighbors.size()<<std::endl;
    
    std::vector<std::string> majority_classes;
    // Print map
    /* { */
    /*     for ( unsigned int i=0; i < pg_map.size() ; ++i) */
    /*     { */
    /*         std::map<std::string,std::vector<float> > local_map= */
    /*             pg_map[i]; */
    /*         std::map<std::string,std::vector<float> >::iterator it; */
    /*         std::string majority_class=""; */
    /*         unsigned int votes_class=0; */

    /*         for ( it = local_map.begin() ; it != local_map.end() ; ++it) */
    /*         { */
    /*             std::vector<float> local_dists=(*it).second; */

    /*             if ( local_dists.size() > votes_class ) */
    /*             { */
    /*                 votes_class=local_dists.size(); */
    /*                 majority_class=(*it).first; */
    /*             } */


    /*               std::cout<<(*it).first<<" :"; */
    /*             for ( unsigned int j=0; j < local_dists.size() ; ++j) */
    /*             { */
    /*                 std::cout<<local_dists[j]<<" "; */
    /*             } */
    /*             std::cout<<std::endl; */
    /*             std::cout<<"Distance to class: "<<*std::min_element( */
    /*             local_dists.begin(),local_dists.end())<<std::endl; */
    /*         } */
    /*         std::cout<<std::endl; */
    /*         std::cout<<"Majority class: "<<majority_class<<std::endl; */
    /*         std::cout<<std::endl; */
    /*         majority_classes.push_back(majority_class); */
    /*     } */

    /* } */

    for ( unsigned int i=0; i < pg_map.size() ; ++i)
    {
        std::map<std::string,std::vector<float> > local_map=
            pg_map[i];
        std::map<std::string,std::vector<float> >::iterator it;
        
        for ( it = local_map.begin() ; it != local_map.end() ; ++it)
        {
            std::vector<float> local_dists=(*it).second;
            std::string key= (*it).first;
            for ( unsigned int j=0; j < local_dists.size() ; ++j)
            { 
                if (local_dists[j] < factor_*dists[i][0] )
                {
                    float distance_squared= local_dists[j]/16129.0;
                    category_totals[key] += 1.0/distance_squared;
                }
            } 

        }
    }
    
    delete[] batch_query.ptr();
    delete[] nn_index.ptr();
    delete[] dists.ptr();

    // Get the result.
    std::string best_class = "";
    float smallest_distance = 0;
    for (std::map<std::string, float>::const_iterator it =
             category_totals.begin();
         it != category_totals.end(); ++it) {
        //  std::cout<<" Category: "<<it->first<<" votes "<<it->second<<std::endl;
      if (it->second > smallest_distance) {
        best_class = it->first;
        smallest_distance = it->second;
      }
    }
    Result result;
    result.category = best_class;
    return result;
  }

 Result Pg_Classify_Hybrid(const sjm::sift::DescriptorSet& descriptor_set,
                           const float subsample_percentage) {
    CHECK(index_built_) << "Must call .BuildIndex() before .Classify()";
    // We'll fetch background_index_ features for estimation, capped
    // at the data_size_.
    int b =
        std::min(data_size_, static_cast<uint64_t>(background_index_));
    // We'll use nearest_neighbors_ for foreground, capped at b - 1.
    int k =
        std::min(b - 1, nearest_neighbors_);
    // Set up the class distance accumulator.
    std::map<std::string, float> category_totals;
    for (std::set<std::string>::const_iterator it = class_set_.begin();
         it != class_set_.end();
         ++it) {
      category_totals[*it] = 0;
    }
    // Set up the data for the batch query.
    // First, create a temp array for up to as many descriptors as 100%.
    const size_t kTempSize =
        descriptor_set.sift_descriptor_size() * data_dimensions_;
    uint8_t* temp =
        new uint8_t[kTempSize];
    // Put a subsample of the data into the temp array.
    int next_matrix_index = 0;
    for (int i = 0; i < descriptor_set.sift_descriptor_size(); ++i) {
      if (std::rand() / static_cast<float>(RAND_MAX) < subsample_percentage) {
        sjm::sift::ConvertProtobufDescriptorToWeightedArray(
            descriptor_set.sift_descriptor(i),
            alpha_,
            temp + (next_matrix_index * data_dimensions_));
        ++next_matrix_index;
      }
    }
    // Move the actually used data from the temp array into one that
    // fits.  We don't need to delete this later because it's cleaned up
    // when we delete[] batch_query->data.
    const size_t kQuerySize =
        next_matrix_index * data_dimensions_;
    uint8_t* query_array =
        new uint8_t[kQuerySize];
    std::copy(temp, temp + (next_matrix_index * data_dimensions_),
              query_array);
    delete[] temp;

    // Set up the query for k + 1 nn.
    flann::Matrix<uint8_t> batch_query =
        flann::Matrix<uint8_t>(query_array,
                               next_matrix_index,
                               data_dimensions_);
    flann::Matrix<int> nn_index(new int[batch_query.rows * b],
                                batch_query.rows, b);
    flann::Matrix<float> dists(new float[batch_query.rows * b],
                               batch_query.rows, b);

    // Execute the query, getting indices and dists.
    index_->knnSearch(batch_query, nn_index, dists, b,
                      flann::SearchParams(checks_));
    std::map<size_t ,std::set<int> > pg_graph;

    /* { */
    /*     std::ofstream my_file("dist_mat.txt"); */

    /*     // Compute proximity graph depending on beta */
    /*     for (size_t a = 0; a < k; ++a) */
    /*     { */
    /*         my_file<<dists[0][a]<<std::endl; */
    /*     } */

    /*     for (size_t b = 0; b < k; ++b) */
    /*     { */
    /*         // Grab knn neighbor */
    /*         uint8_t* sj=index_->getPoint(nn_index[0][b]); */
          
    /*         for (size_t c = b+1; c < k; ++c) */
    /*         { */
    /*                 // Get distance between two shapes */
    /*                 uint8_t* sk=index_->getPoint(nn_index[0][c]); */
    /*                 float djk=distance(sj,sk,data_dimensions_); */
    /*                 my_file<<djk<<std::endl; */
    /*         } */

    /*     } */
    /*     my_file.close(); */
   
    /* } */
    std::vector< std::map<std::string,std::vector<float> > > pg_map;

    bool flag=true;
    // Compute proximity graph depending on beta
    for (size_t ni = 0; ni < dists.rows ; ++ni) 
    {
        // query element
        uint8_t* si=&query_array[ni*data_dimensions_];

        std::map<std::string,std::vector<float> > local_map;
        for (size_t nj = 0; nj < k; ++nj) 
        {
            // Reset flag for this neighbor
            flag=true;

            // Grab knn neighbor
            uint8_t* sj=index_->getPoint(nn_index[ni][nj]);
            float dij=dists[ni][nj];
          
            for (size_t nk = 0; nk < nj; ++nk)
            {
                if ( nk != nj )
                {

                    // Get distance between two shapes
                    uint8_t* sk=index_->getPoint(nn_index[ni][nk]);
             
                    float dik=dists[ni][nk];
                    float djk=distance(sj,sk,data_dimensions_);
                    
                          
                    if ( beta_ < 1)
                    {
                        // Beta_ less than 1 case
                        if (  (dij*dij)
                              > ((dik*dik) +
                                 (djk*djk) +
                                 2*std::sqrt(1-(beta_*beta_))*
                                 (dik*djk)  ))
                        {
                            flag=false;
                            break;
                        }
                    }
                    else
                    {
                        // Beta_ greater than 1 case
                        if ( dij*dij > std::max(
                                 ((dik*dik)*((2/beta_)-1))+(djk*djk)
                                 ,(dik*dik)+(((2/beta_)-1)*(djk*djk)) ))
                        {
                        
                            flag=false;
                            break;
                        
                        }
                    
                    }
                }
                
                
            }
            
            if(flag)
            {
                pg_graph[ni].insert(nn_index[ni][nj]);
                std::string class_string = class_vector_[nn_index[ni][nj]];
                local_map[class_string].push_back(dij);

            }
        }
        pg_map.push_back(local_map);
    }

    std::map<size_t ,std::set<int> >::iterator it;
    std::vector<double> avg_neighbors;
    double avg_edges=0.0;
    for  ( it = pg_graph.begin() ; it != pg_graph.end() ; ++it)
    {
        std::set<int> pg_neighbors = (*it).second;
        avg_neighbors.push_back(pg_neighbors.size());
        avg_edges = avg_edges+pg_neighbors.size();
    }

    LOG(INFO) << "Hybrid Vote Beta= " <<beta_
              <<" stats, min edges: "<<*std::min_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" max edges: "<<*std::max_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" avg edges: "<<avg_edges/avg_neighbors.size()<<std::endl;
    
    std::vector<std::set<std::string> > knn_class_map;
    {
        for ( unsigned int k=0; k < dists.rows; ++k)
        {
            std::set<std::string> knn_classes;
            for (size_t neighbor = 0; neighbor < 20; ++neighbor) {
                // Find the category of this neighbor.
                int neighbor_index = nn_index[k][neighbor];
                std::string neighbor_class = class_vector_[neighbor_index];
                knn_classes.insert(neighbor_class);
            }

            knn_class_map.push_back(knn_classes);

            /* std::set<std::string>::iterator kit; */
            /* for ( kit =  knn_classes.begin() ; kit != knn_classes.end() ; ++kit) */
            /* { */
            /*     std::cout<<" Knn Class: "<<k<<" "<<(*kit)<<std::endl; */
            /* } */
            /* std::cout<<std::endl; */
        }
    }

    std::vector<std::string> majority_classes;
    // Print map
    {
        for ( unsigned int i=0; i < pg_map.size() ; ++i)
        {
            std::map<std::string,std::vector<float> > local_map=
                pg_map[i];
            std::map<std::string,std::vector<float> >::iterator it;
            std::string majority_class="";
            unsigned int votes_class=0;

            for ( it = local_map.begin() ; it != local_map.end() ; ++it)
            {
                std::vector<float> local_dists=(*it).second;

                if ( local_dists.size() > votes_class )
                {
                    votes_class=local_dists.size();
                    majority_class=(*it).first;
                }


                //   std::cout<<(*it).first<<" :";
                /* for ( unsigned int j=0; j < local_dists.size() ; ++j) */
                /* { */
                /*     std::cout<<local_dists[j]<<" "; */
                /* } */
                /* std::cout<<std::endl;  */
                /* std::cout<<"Distance to class: "<<*std::min_element( */
                /* local_dists.begin(),local_dists.end())<<std::endl; */
            }
            /* std::cout<<std::endl; */
            //   std::cout<<"Majority class: "<<majority_class<<std::endl; 
            /* std::cout<<std::endl; */
            majority_classes.push_back(majority_class);
        }

    }

    // For each row, make a category adjustment map
    for (size_t row = 0; row < dists.rows ; ++row) 
    {
        if ( knn_class_map[row].size() == 1 )
        {
            std::string knn_key=*(knn_class_map[row].begin());
            category_totals[knn_key] += 1;
        }
        else
        {
            std::string g_key=majority_classes[row];
            category_totals[g_key] += 1;
        }

    }
    
    delete[] batch_query.ptr();
    delete[] nn_index.ptr();
    delete[] dists.ptr();

    // Get the result.
    std::string best_class = "";
    float smallest_distance = 0;
    for (std::map<std::string, float>::const_iterator it =
             category_totals.begin();
         it != category_totals.end(); ++it) {
        //   std::cout<<" Category: "<<it->first<<" votes "<<it->second<<std::endl;
      if (it->second > smallest_distance) {
        best_class = it->first;
        smallest_distance = it->second;
      }
    }
    Result result;
    result.category = best_class;
    return result;
  }


  Result Pg_Classify_codebook(const sjm::sift::DescriptorSet& descriptor_set,
                              const float subsample_percentage) {
    CHECK(index_built_) << "Must call .BuildIndex() before .Classify()";
    // We'll fetch background_index_int32_ features for estimation, capped
    // at the data_size_.
    int b =
        std::min(data_size_, static_cast<uint64_t>(background_index_));
    // We'll use nearest_neighbors_ for foreground, capped at b - 1.
    int k =
        std::min(b - 1, nearest_neighbors_);
    // Set up the class distance accumulator.
    std::map<std::string, float> category_totals;
    for (std::set<std::string>::const_iterator it = class_set_.begin();
         it != class_set_.end();
         ++it) {
      category_totals[*it] = 0;
    }
    // Set up the data for the batch query.
    // First, create a temp array for up to as many descriptors as 100%.
    const size_t kTempSize =
        descriptor_set.sift_descriptor_size() * data_dimensions_;
    int32_t* temp =
        new int32_t[kTempSize];
    // Put a subsample of the data into the temp array.
    int next_matrix_index = 0;
    for (int i = 0; i < descriptor_set.sift_descriptor_size(); ++i) {
      if (std::rand() / static_cast<float>(RAND_MAX) < subsample_percentage) {
        sjm::sift::ConvertProtobufDescriptorToWeightedArray(
            descriptor_set.sift_descriptor(i),
            alpha_,
            temp + (next_matrix_index * data_dimensions_));
        ++next_matrix_index;
      }
    }
    // Move the actually used data from the temp array into one that
    // fits.  We don't need to delete this later because it's cleaned up
    // when we delete[] batch_query->data.
    const size_t kQuerySize =
        next_matrix_index * data_dimensions_;
    int32_t* query_array =
        new int32_t[kQuerySize];
    std::copy(temp, temp + (next_matrix_index * data_dimensions_),
              query_array);
    delete[] temp;

    // Set up the query for k + 1 nn.
    flann::Matrix<int32_t> batch_query =
        flann::Matrix<int32_t>(query_array,
                               next_matrix_index,
                               data_dimensions_);
    flann::Matrix<int> nn_index(new int[batch_query.rows * b],
                                batch_query.rows, b);
    flann::Matrix<float> dists(new float[batch_query.rows * b],
                               batch_query.rows, b);

    // Execute the query, getting indices and dists.
    index_int32_->knnSearch(batch_query, nn_index, dists, b,
                      flann::SearchParams(checks_));
    std::map<size_t ,std::set<int> > pg_graph;

    /* { */
    /*     std::ofstream my_file("dist_mat.txt"); */

    /*     // Compute proximity graph depending on beta */
    /*     for (size_t a = 0; a < k; ++a) */
    /*     { */
    /*         my_file<<dists[0][a]<<std::endl; */
    /*     } */

    /*     for (size_t b = 0; b < k; ++b) */
    /*     { */
    /*         // Grab knn neighbor */
    /*         int32_t* sj=index_int32_->getPoint(nn_index[0][b]); */
          
    /*         for (size_t c = b+1; c < k; ++c) */
    /*         { */
    /*                 // Get distance between two shapes */
    /*                 int32_t* sk=index_int32_->getPoint(nn_index[0][c]); */
    /*                 float djk=distance(sj,sk,data_dimensions_); */
    /*                 my_file<<djk<<std::endl; */
    /*         } */

    /*     } */
    /*     my_file.close(); */
   
    /* } */
   
    bool flag=true;
    // Compute proximity graph depending on beta
    for (size_t ni = 0; ni < dists.rows; ++ni) 
    {
        // query element
        int32_t* si=&query_array[ni*data_dimensions_];
        for (size_t nj = 0; nj < k; ++nj) 
        {
            // Reset flag for this neighbor
            flag=true;

            // Grab knn neighbor
            int32_t* sj=index_int32_->getPoint(nn_index[ni][nj]);
            float dij=dists[ni][nj];
          
            for (size_t nk = 0; nk < nj; ++nk)
            {
                if ( nk != nj )
                {

                    // Get distance between two shapes
                    int32_t* sk=index_int32_->getPoint(nn_index[ni][nk]);
             
                    float dik=dists[ni][nk];
                    float djk=distance(sj,sk,data_dimensions_);
                    
                          
                    if ( beta_ < 1)
                    {
                        // Beta_ less than 1 case
                        if (  (dij*dij)
                              > ((dik*dik) +
                                 (djk*djk) +
                                 2*std::sqrt(1-(beta_*beta_))*
                                 (dik*djk)  ))
                        {
                            flag=false;
                            break;
                        }
                    }
                    else
                    {
                        // Beta_ greater than 1 case
                        if ( dij*dij > std::max(
                                 ((dik*dik)*((2/beta_)-1))+(djk*djk)
                                 ,(dik*dik)+(((2/beta_)-1)*(djk*djk)) ))
                        {
                        
                            flag=false;
                            break;
                        
                        }
                    
                    }
                }
                
                
            }
            
            if(flag)
            {
                pg_graph[ni].insert(nn_index[ni][nj]);
            }
        }
    }

    std::map<size_t ,std::set<int> >::iterator it;
    std::vector<double> avg_neighbors;
    double avg_edges=0.0;
    for  ( it = pg_graph.begin() ; it != pg_graph.end() ; ++it)
    {
        std::set<int> pg_neighbors = (*it).second;
        avg_neighbors.push_back(pg_neighbors.size());
        avg_edges = avg_edges+pg_neighbors.size();
    }
    LOG(INFO) << " CodeBook, Factor= " <<factor_
              << " Beta= " <<beta_
              <<" stats, min edges: "<<*std::min_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" max edges: "<<*std::max_element(
                avg_neighbors.begin(), avg_neighbors.end())
              <<" avg edges: "<<avg_edges/avg_neighbors.size()<<std::endl;
    

    // For each row, make a category adjustment map
    for (size_t row = 0; row < dists.rows; ++row) {
      std::map<std::string, float> category_distances;
      // The k+1st neighbor is used for the background distance.
      float background_distance = dists[row][b - 1] / 16129.0;
      for (size_t neighbor = 0; neighbor < k; ++neighbor) {
        // Find the category of this neighbor.
        int neighbor_index = nn_index[row][neighbor];
        std::string neighbor_class = class_vector_[neighbor_index];
        float ndist=dists[row][neighbor];
        // If it's not already been seen,
        if ((category_distances.find(neighbor_class) ==
             category_distances.end()) && pg_graph[row].count(neighbor_index)) {
          // Put its distance in the map, substracting the background
          // distance.

          // This scales down the distance to be as if the original values
          // had been in [0,1] instead of in [0,127]. Useful in order to
          // avoid overflow errors in some of the probability estimate
          // models. (16129 = 127 * 127
          float distance_squared = dists[row][neighbor] / 16129.0;
          category_distances[neighbor_class] =
              distance_squared - background_distance;
        }
      }
      // Now, adjust the distance totals.
      for (std::map<std::string, float>::const_iterator it =
               category_distances.begin();
           it != category_distances.end();
           ++it) {
        category_totals[it->first] += it->second;
      }
    }
    delete[] batch_query.ptr();
    delete[] nn_index.ptr();
    delete[] dists.ptr();

    // Get the result.
    std::string best_class = "";
    float smallest_distance = 99999999999;
    for (std::map<std::string, float>::const_iterator it =
             category_totals.begin();
         it != category_totals.end(); ++it) {
      if (it->second < smallest_distance) {
        best_class = it->first;
        smallest_distance = it->second;
      }
    }
    Result result;
    result.category = best_class;
    return result;
  }


 Result Weighted_Vote_Codebook(const sjm::sift::DescriptorSet& descriptor_set,
                      const float subsample_percentage) {
    CHECK(index_built_) << "Must call .BuildIndex() before .Classify()";
    // We'll fetch background_index_ features for estimation, capped
    // at the data_size_.
    int b =
        std::min(data_size_, static_cast<uint64_t>(background_index_));
    // We'll use nearest_neighbors_ for foreground, capped at b - 1.
    int k =
        std::min(b - 1, nearest_neighbors_);
    // Set up the class distance accumulator.
    std::map<std::string, float> category_totals;
    for (std::set<std::string>::const_iterator it = class_set_.begin();
         it != class_set_.end();
         ++it) {
      category_totals[*it] = 0;
    }
    // Set up the data for the batch query.
    // First, create a temp array for up to as many descriptors as 100%.
    const size_t kTempSize =
        descriptor_set.sift_descriptor_size() * data_dimensions_;
    int32_t* temp =
        new int32_t[kTempSize];
    // Put a subsample of the data into the temp array.
    int next_matrix_index = 0;
    for (int i = 0; i < descriptor_set.sift_descriptor_size(); ++i) {
      if (std::rand() / static_cast<float>(RAND_MAX) < subsample_percentage) {
        sjm::sift::ConvertProtobufDescriptorToWeightedArray(
            descriptor_set.sift_descriptor(i),
            alpha_,
            temp + (next_matrix_index * data_dimensions_));
        ++next_matrix_index;
      }
    }
    // Move the actually used data from the temp array into one that
    // fits.  We don't need to delete this later because it's cleaned up
    // when we delete[] batch_query->data.
    const size_t kQuerySize =
        next_matrix_index * data_dimensions_;
    int32_t* query_array =
        new int32_t[kQuerySize];
    std::copy(temp, temp + (next_matrix_index * data_dimensions_),
              query_array);
    delete[] temp;

    // Set up the query for k + 1 nn.
    flann::Matrix<int32_t> batch_query =
        flann::Matrix<int32_t>(query_array,
                               next_matrix_index,
                               data_dimensions_);
    flann::Matrix<int> nn_index(new int[batch_query.rows * b],
                                batch_query.rows, b);
    flann::Matrix<float> dists(new float[batch_query.rows * b],
                               batch_query.rows, b);

    // Execute the query, getting indices and dists.
    index_int32_->knnSearch(batch_query, nn_index, dists, b,
                      flann::SearchParams(checks_));
    std::map<size_t ,std::set<int> > pg_graph;

    /* { */
    /*     std::ofstream my_file("dist_mat.txt"); */

    /*     // Compute proximity graph depending on beta */
    /*     for (size_t a = 0; a < k; ++a) */
    /*     { */
    /*         my_file<<dists[0][a]<<std::endl; */
    /*     } */

    /*     for (size_t b = 0; b < k; ++b) */
    /*     { */
    /*         // Grab knn neighbor */
    /*         int32_t* sj=index_->getPoint(nn_index[0][b]); */
          
    /*         for (size_t c = b+1; c < k; ++c) */
    /*         { */
    /*                 // Get distance between two shapes */
    /*                 int32_t* sk=index_->getPoint(nn_index[0][c]); */
    /*                 float djk=distance(sj,sk,data_dimensions_); */
    /*                 my_file<<djk<<std::endl; */
    /*         } */

    /*     } */
    /*     my_file.close(); */
   
    /* } */
    std::vector< std::map<std::string,std::vector<float> > > pg_map;

    bool flag=true;
    // Compute proximity graph depending on beta
    for (size_t ni = 0; ni < dists.rows ; ++ni) 
    {
        // query element
        int32_t* si=&query_array[ni*data_dimensions_];

        std::map<std::string,std::vector<float> > local_map;
        for (size_t nj = 0; nj < k; ++nj) 
        {
            // Reset flag for this neighbor
            flag=true;

            // Grab knn neighbor
            int32_t* sj=index_int32_->getPoint(nn_index[ni][nj]);
            float dij=dists[ni][nj];
          
            for (size_t nk = 0; nk < nj; ++nk)
            {
                if ( nk != nj )
                {

                    // Get distance between two shapes
                    int32_t* sk=index_int32_->getPoint(nn_index[ni][nk]);
             
                    float dik=dists[ni][nk];
                    float djk=distance(sj,sk,data_dimensions_);
                    
                          
                    if ( beta_ < 1)
                    {
                        // Beta_ less than 1 case
                        if (  (dij*dij)
                              > ((dik*dik) +
                                 (djk*djk) +
                                 2*std::sqrt(1-(beta_*beta_))*
                                 (dik*djk)  ))
                        {
                            flag=false;
                            break;
                        }
                    }
                    else
                    {
                        // Beta_ greater than 1 case
                        if ( dij*dij > std::max(
                                 ((dik*dik)*((2/beta_)-1))+(djk*djk)
                                 ,(dik*dik)+(((2/beta_)-1)*(djk*djk)) ))
                        {
                        
                            flag=false;
                            break;
                        
                        }
                    
                    }
                }
                
                
            }
            
            if(flag)
            {
                pg_graph[ni].insert(nn_index[ni][nj]);
                std::string class_string = class_vector_[nn_index[ni][nj]];
                local_map[class_string].push_back(dij);

            }
        }
        pg_map.push_back(local_map);
    }

    std::map<size_t ,std::set<int> >::iterator it;
    std::vector<double> avg_neighbors;
    double avg_edges=0.0;
    for  ( it = pg_graph.begin() ; it != pg_graph.end() ; ++it)
    {
        std::set<int> pg_neighbors = (*it).second;
        avg_neighbors.push_back(pg_neighbors.size());
        avg_edges = avg_edges+pg_neighbors.size();
    }

    LOG(INFO) <<" Codebook, Factor "<<factor_
              << " Weighted Vote Beta= " <<beta_
              <<" stats, min edges: "<<*std::min_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" max edges: "<<*std::max_element(
                  avg_neighbors.begin(), avg_neighbors.end())
              <<" avg edges: "<<avg_edges/avg_neighbors.size()<<std::endl;
    
    std::vector<std::string> majority_classes;
    // Print map
    /* { */
    /*     for ( unsigned int i=0; i < pg_map.size() ; ++i) */
    /*     { */
    /*         std::map<std::string,std::vector<float> > local_map= */
    /*             pg_map[i]; */
    /*         std::map<std::string,std::vector<float> >::iterator it; */
    /*         std::string majority_class=""; */
    /*         unsigned int votes_class=0; */

    /*         for ( it = local_map.begin() ; it != local_map.end() ; ++it) */
    /*         { */
    /*             std::vector<float> local_dists=(*it).second; */

    /*             if ( local_dists.size() > votes_class ) */
    /*             { */
    /*                 votes_class=local_dists.size(); */
    /*                 majority_class=(*it).first; */
    /*             } */


    /*               std::cout<<(*it).first<<" :"; */
    /*             for ( unsigned int j=0; j < local_dists.size() ; ++j) */
    /*             { */
    /*                 std::cout<<local_dists[j]<<" "; */
    /*             } */
    /*             std::cout<<std::endl; */
    /*             std::cout<<"Distance to class: "<<*std::min_element( */
    /*             local_dists.begin(),local_dists.end())<<std::endl; */
    /*         } */
    /*         std::cout<<std::endl; */
    /*         std::cout<<"Majority class: "<<majority_class<<std::endl; */
    /*         std::cout<<std::endl; */
    /*         majority_classes.push_back(majority_class); */
    /*     } */

    /* } */

    for ( unsigned int i=0; i < pg_map.size() ; ++i)
    {
        std::map<std::string,std::vector<float> > local_map=
            pg_map[i];
        std::map<std::string,std::vector<float> >::iterator it;
        
        for ( it = local_map.begin() ; it != local_map.end() ; ++it)
        {
            std::vector<float> local_dists=(*it).second;
            std::string key= (*it).first;
            for ( unsigned int j=0; j < local_dists.size() ; ++j)
            { 
                float distance_squared= local_dists[j]/16129.0;
                category_totals[key] += 1.0/distance_squared;
            } 

        }
    }
    
    delete[] batch_query.ptr();
    delete[] nn_index.ptr();
    delete[] dists.ptr();

    // Get the result.
    std::string best_class = "";
    float smallest_distance = 0;
    for (std::map<std::string, float>::const_iterator it =
             category_totals.begin();
         it != category_totals.end(); ++it) {
        //  std::cout<<" Category: "<<it->first<<" votes "<<it->second<<std::endl;
      if (it->second > smallest_distance) {
        best_class = it->first;
        smallest_distance = it->second;
      }
    }
    Result result;
    result.category = best_class;
    return result;
  }

 private:
  int nearest_neighbors_;
  int background_index_;
  float alpha_;
  int checks_;
  uint64_t data_size_;
  bool index_built_;
  flann::Matrix<uint8_t>* data_;
  flann::Matrix<int32_t>* dataset_;
  int data_dimensions_;
  flann::Index<flann::L2<uint8_t> >* index_;
  flann::Index<flann::L2<int32_t> >* index_int32_;
  bool params_set_;
  int trees_;
  std::vector<std::string> class_vector_;
  std::set<std::string> class_set_;
  std::vector<std::string> file_set_;
  double beta_;
  double factor_;
};
}}  // Namespace.

#endif  // NAIVE_BAYES_NEAREST_NEIGHBOR_MERGED_CLASSIFIER_H_
