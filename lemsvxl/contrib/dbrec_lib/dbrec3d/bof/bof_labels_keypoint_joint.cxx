//:
// \file
// \author Isabel Restrepo
// \date 11-Apr-2011

#include "bof_labels_keypoint_joint.h"

#include <vul/vul_file.h>
#include <vcl_limits.h>

//: Return the maximum aposteriori class, given the object represented by classify_clusters
const char bof_labels_keypoint_joint::max_aposteriori(const vcl_vector< dbcll_euclidean_cluster_light<10> > classify_clusters, 
                                                      vcl_vector<double> &log_p_c_given_obj) const
{
  log_p_c_given_obj.clear();
  log_p_c_given_obj.resize(ncategories_, 0.0);
  double max_log_aposteriori = -1.0f * vcl_numeric_limits<double>::infinity();
  char max_class = -1;
  vcl_cout << "Number of categories= " << ncategories_ << "\n";

  for (unsigned ci=0; ci<ncategories_; ci++){
    double log_p_obj_given_c =0.0;
    for (unsigned xi=0; xi<nkeypoints_; xi++) {
      double log_p_x_given_c = (this->log_p_xc(xi,ci))- (this->log_p_c(ci));
      log_p_obj_given_c +=(log_p_x_given_c*classify_clusters[xi].size());
   }
    log_p_c_given_obj[ci] = log_p_obj_given_c;
    if (log_p_obj_given_c > max_log_aposteriori) {
      max_log_aposteriori = log_p_obj_given_c;
      max_class = ci;
    }
  }
  return max_class;
}

//: Merge the clusters in clusters2, with the clusters associated with this class
void bof_labels_keypoint_joint::merge_clusters(char class_id, const vcl_vector< dbcll_euclidean_cluster_light<10> > &class_cluster2)
{
  vcl_vector< dbcll_euclidean_cluster_light<10> > &class_cluster = all_class_clusters_[class_id];
  unsigned long total_size = 0;
  for (unsigned ci=0; ci<class_cluster.size(); ci++) {
    dbcll_euclidean_cluster_light<10> &keypoint_cluster = class_cluster[ci];
    keypoint_cluster.merge(class_cluster2[ci]);
  }
}

//: Read xml clsuter files and combine them
void bof_labels_keypoint_joint::xml_read_and_combine(vcl_string xml_scen_prfx, vcl_vector< vnl_vector_fixed<double, 10> > common_means, unsigned nscenes, unsigned ncategories)
{
  for (unsigned si=0; si< nscenes; si++) {
    for(unsigned ci=0; ci < ncategories; ci++){
      vcl_stringstream xml_file_ss;
      xml_file_ss << xml_scen_prfx << si << "/class_" << ci << "_distribution.xml";
      if (!vul_file::exists(xml_file_ss.str()))
        continue;
      vcl_vector< dbcll_euclidean_cluster_light<10> > scene_ith_clusters;
      dbcll_xml_read(scene_ith_clusters, common_means[ci], xml_file_ss.str());
      this->merge_clusters(ci, scene_ith_clusters);
    }
  }
}

//: Read the clusters in an xml file 
void bof_labels_keypoint_joint::xml_read(vcl_string xml_path, vcl_vector< vnl_vector_fixed<double, 10> > common_means)
{
  vcl_cout << "Number of categories: "<< ncategories_ << vcl_endl;
  for(unsigned ci=0; ci < ncategories_; ci++){
    vcl_stringstream xml_file_ss;
    xml_file_ss << xml_path << "/all_scenes/class_" << ci << "_distribution.xml";
    if (!vul_file::exists(xml_file_ss.str())){
      vcl_cerr << "File " << xml_file_ss.str() << "doesn't exist \n";
      continue;
    }
    vcl_vector< dbcll_euclidean_cluster_light<10> > class_ith_clusters;
    dbcll_xml_read(class_ith_clusters, common_means[ci], xml_file_ss.str());
    this->merge_clusters(ci, class_ith_clusters);
  }
  
}

//: Write this class' cluster to xml
void bof_labels_keypoint_joint::xml_write(vcl_string xml_path)
{
  for (unsigned i = 0 ; i < all_class_clusters_.size(); i++) {
    vcl_stringstream ss;
    ss << xml_path << "/class_" << i << "_distribution.xml";
    dbcll_xml_write(all_class_clusters_[i],ss.str());
  }
}

//: Save the clusters associated with this class as bsta histograms
void bof_labels_keypoint_joint::bsta_save_and_plot(vcl_string path_out)
{
  for (unsigned class_id = 0 ; class_id < all_class_clusters_.size(); class_id++) {
    
    const vcl_vector< dbcll_euclidean_cluster_light<10> > &class_clusters = all_class_clusters_[class_id];
    vcl_vector<double> counts(class_clusters.size(),0.0);
    for (unsigned cluster_idx=0; cluster_idx < class_clusters.size(); cluster_idx++) {
      counts[cluster_idx]=class_clusters[cluster_idx].size();
    }
    
    bsta_histogram<double> class_hist(1.0, (double)class_clusters.size(), counts);
    
    vcl_stringstream ss;
    ss << path_out << "/class_" << class_id;
    vcl_ofstream hist_ofs((ss.str() + "_bsta_hist.txt").c_str());
    vcl_ofstream hist_arrays_ofs((ss.str() + "_hist_plot.txt").c_str());
    
    class_hist.write(hist_ofs);
    class_hist.print_to_arrays(hist_arrays_ofs);
    
    hist_ofs.close();
    hist_arrays_ofs.close();
    
  }
  
}


/****************** BInary IO fo bof_labels_keypoint_joint**************************/
//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & , bof_labels_keypoint_joint const & )
{
  vcl_cerr << "Error: Trying to save but binary io not implemented\n";
  return;
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & , bof_labels_keypoint_joint & )
{
  vcl_cerr << "Error: Tryinbof_labels_keypoint_jointg to read but binary io not implemented\n";
  return;
}

void vsl_print_summary(vcl_ostream & , const bof_labels_keypoint_joint & )
{
  vcl_cerr << "Error: Trying to print but binary io not implemented\n";
  return;
}

void vsl_b_read(vsl_b_istream& is,bof_labels_keypoint_joint* p)
{
  delete p;
  vcl_cerr << "Error: Trying to read but binary io not implemented\n";
  return;
}

void vsl_b_write(vsl_b_ostream& os, const bof_labels_keypoint_joint* &p)
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

void vsl_print_summary(vcl_ostream& os, const bof_labels_keypoint_joint* &p)
{
  if (p==0)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}
