// This is bof_util.h
#ifndef bof_util_h
#define bof_util_h

//:
// \file
// \brief A class containing utility functions used in bag of featues. 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  1-Apr-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bof/bof_info.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_vector.h>

#include <vbl/vbl_ref_count.h>

class bof_util{
  
public:
  
  bof_util(vcl_string bof_info_path):info_(bof_info(bof_info_path)){}
  
  //: Labels for training a random sample of the specified block
  void random_label_for_training(int scene_id, int block_i, int block_j, int block_k, double fraction); 
  
  //: Samples randomnly 10-d features (from those labeled as training). 
  // The randomly selected features are returned on a vector
  void sample_from_train(int scene_id, int block_i, int block_j, int block_k, double fraction,
                         vcl_vector<vnl_vector_fixed<double,10> > &features); 
  
  static double parse_variance_from_xml_cluster(vcl_string xml_file);
  
protected:
  
  bof_info info_;
};



/****************** bof_feature_vector **************************/


//: A simple class to hold a vector on vnl_vector_fixed
class bof_feature_vector : public vbl_ref_count
{
public:
  vcl_vector<vnl_vector_fixed<double,10> > features_;
};

typedef vbl_smart_ptr<bof_feature_vector> bof_feature_vector_sptr;


/// Binary IO

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bof_feature_vector const &v);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bof_feature_vector &v);


void vsl_print_summary(vcl_ostream &os, const bof_feature_vector &v);


void vsl_b_read(vsl_b_istream& is, bof_feature_vector* v);


void vsl_b_write(vsl_b_ostream& os, const bof_feature_vector* &v);

void vsl_print_summary(vcl_ostream& os, const bof_feature_vector* &v);


#endif
