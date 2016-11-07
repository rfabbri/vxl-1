// This is bof_labels_keypoint_joint.h
#ifndef bof_labels_keypoint_joint_h
#define bof_labels_keypoint_joint_h

//:
// \file
// \brief A class to hold a joint probability P(X,C) between class labels and keypoints
// \author Isabel Restrepo mir@lems.brown.edu
// \date  11-Apr-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <dbcll/dbcll_euclidean_cluster_light.h>

#include <bsta/bsta_histogram.h>

#include <vul/vul_file.h>
#include <vbl/vbl_ref_count.h>

//: A joint probability P(X,C), quantizing number of times this keypoint "X" (10-d feature) occurs in all objects of the current label C
class bof_labels_keypoint_joint: public vbl_ref_count 
{
  
public:
  bof_labels_keypoint_joint(unsigned num_classes, unsigned num_keypoints):ncategories_(num_classes), nkeypoints_(num_keypoints) 
  {
    for (unsigned ci = 0 ; ci < num_classes; ci++) {
      all_class_clusters_.push_back(vcl_vector< dbcll_euclidean_cluster_light<10> >(num_keypoints));
    }
    
  }
  
  //: Return the maximum aposteriori class, given the object represented by classify_clusters
  const char max_aposteriori(const vcl_vector< dbcll_euclidean_cluster_light<10> > classify_clusters, 
                             vcl_vector<double> &log_p_c_given_obj) const;
  
  //: P(X,C) return the number of times feature X happens in all objects of class referred by class_i  
  const double p_xc(unsigned x_idx, unsigned class_id) const
  {
    return (double) (all_class_clusters_[class_id][x_idx].size());
  }
  
  inline const double log_p_xc(unsigned x_idx, unsigned class_id) const { return vcl_log(p_xc(x_idx,class_id)); }
  
  //: P(C) return the number of times all features happen in all objects of class referred by class_i  
  const double p_c(unsigned class_id) const
  {
    double p_c = 0;
    for (unsigned xi=0; xi<nkeypoints_; xi++) {
      p_c += all_class_clusters_[class_id][xi].size();
    }
    return (double) (p_c);
  }
  
  inline const double log_p_c(unsigned class_id) const { return vcl_log(p_c(class_id)); } 
  
  //: Merge the clusters in clusters2, with the clusters associated with this class
  void merge_clusters(char class_id, const vcl_vector< dbcll_euclidean_cluster_light<10> > &class_cluster2);
  
  //: Read xml clsuter files and combine them
  void xml_read_and_combine(vcl_string xml_scen_prfx, vcl_vector< vnl_vector_fixed<double, 10> > common_means,
                            unsigned nscenes, unsigned ncategories);
  
  //: Read the clusters in an xml file 
  void xml_read(vcl_string xml_path, vcl_vector< vnl_vector_fixed<double, 10> > common_means);
  
  //: Write this class' cluster to xml
  void xml_write(vcl_string xml_path);
  
  //: Save the clusters associated with this class as bsta histograms
  void bsta_save_and_plot(vcl_string path_out);
  
protected:
  //: quantizing number of times this keypoint "X" (10-d feature) occurs in all objects of the current label C
  //  outer vector indexed by class label e.g tree building, inner vecor by features/cluster-means
  vcl_vector<vcl_vector< dbcll_euclidean_cluster_light<10> > > all_class_clusters_;
  
  unsigned ncategories_;
  unsigned nkeypoints_;
  
}; 

typedef vbl_smart_ptr<bof_labels_keypoint_joint> bof_p_xc_sptr;

/**************** Binary IO *********************/

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bof_labels_keypoint_joint const &v);


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bof_labels_keypoint_joint &v);


void vsl_print_summary(vcl_ostream &os, const bof_labels_keypoint_joint &v);


void vsl_b_read(vsl_b_istream& is, bof_labels_keypoint_joint* v);


void vsl_b_write(vsl_b_ostream& os, const bof_labels_keypoint_joint* &v);

void vsl_print_summary(vcl_ostream& os, const bof_labels_keypoint_joint* &v);


#endif
