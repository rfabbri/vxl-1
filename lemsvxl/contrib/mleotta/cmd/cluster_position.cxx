// This is mleotta/cmd/cluster_position.cxx

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vbl/vbl_triple.h>
#include <vnl/vnl_double_3.h>

#include <modrec/modrec_feature_3d.h>
#include <modrec/io/modrec_io_desc_feature_3d.h>

#include <dbcll/dbcll_euclidean_cluster.h>
#include <dbcll/dbcll_rnn_agg_clustering.h>
#include <dbcll/dbcll_k_means.h>

#include "cluster_io.h"



vul_arg<vcl_string>  a_feat_file("-feat3d", "path to 3d features", "");
vul_arg<vcl_string>  a_out_file("-out", "path to output file (clusters indices or trace)", "");
vul_arg<double>      a_cthresh("-thresh", "cluster similarity theshold", 0.0);
vul_arg<unsigned>    a_dim("-dim", "feature descriptor dimension", 128);
vul_arg<double>      a_sthresh("-sthresh", "use K-Means first and use this threshold in each group", 0.0);



// sort by size followed by key index
bool less_num_members(const dbcll_cluster_sptr& c1, 
                      const dbcll_cluster_sptr& c2)
{
  if(c1->size() == c2->size())
    return c1->key_index() < c2->key_index();
  return c1->size() < c2->size();
}


void cluster_with_threshold(vcl_vector<dbcll_cluster_sptr>& clusters, double thresh)
{
  // Cluster the features based on appearance
  dbcll_remainder_heap remain(clusters.begin(), clusters.end());

  clusters.clear();
  dbcll_rnn_agg_clustering(remain, clusters, -thresh);

}


void cluster_for_trace(vcl_vector<dbcll_cluster_sptr>& clusters)
{
  // Cluster the features based on appearance
  dbcll_remainder_heap remain(clusters.begin(), clusters.end());

  vcl_vector<dbcll_trace_pt> trace;
  dbcll_rnn_agg_clustering(remain, trace);

  // write the trace file
  write_trace(a_out_file(),trace);
}


template <unsigned dim>
void cluster_features()
{
  vcl_vector<modrec_desc_feature_3d<dim> > features;
  typedef vbl_triple<unsigned,unsigned,unsigned> utriple;
  vcl_vector<utriple> idx_array;
  read_features(a_feat_file(), features, idx_array);

  vcl_cout << "read " << features.size() << vcl_endl;

  vcl_vector<dbcll_cluster_sptr> clusters;
  for(unsigned i=0; i<features.size(); ++i){
    const vgl_point_3d<double>& pt = features[i].position();
    clusters.push_back(new dbcll_euclidean_cluster<3>(vnl_double_3(pt.x(),pt.y(),pt.z()),i));
  }

  // do k-means first
  if(a_sthresh.set()){
    vcl_vector<vnl_vector_fixed<double,3> > points;
    for(unsigned i=0; i<features.size(); ++i){
      const vgl_point_3d<double>& pt = features[i].position();
      points.push_back(vnl_double_3(pt.x(),pt.y(),pt.z()));
    }

    unsigned k = points.size()/20000 + 1;
    vcl_cout << "init k-means with k = " << k << vcl_endl;
    vcl_vector<vnl_vector_fixed<double,3> > means = dbcll_init_k_means_d2<3>(points,k);

    vcl_cout << "starting k-means" << vcl_endl;
    vcl_vector<vcl_vector<unsigned> > groups;
    unsigned n = dbcll_fast_k_means<3>(points, groups, means, 25);

    vcl_cout << "finished k-means" << vcl_endl;

    vcl_vector<dbcll_cluster_sptr> sub_clusters, new_clusters;
    for(unsigned i=0; i<groups.size(); ++i){
      vcl_cout << "clustering group "<< i << vcl_endl;
      sub_clusters.clear();
      for(unsigned j=0; j<groups[i].size(); ++j)
        sub_clusters.push_back(clusters[groups[i][j]]);

      cluster_with_threshold(sub_clusters,a_sthresh());
      new_clusters.insert(new_clusters.end(),
                          sub_clusters.begin(), sub_clusters.end());
    }
    clusters.swap(new_clusters);
    vcl_cout << "clustering sub groups done" << vcl_endl;
  }

  if(a_cthresh.set()){
    cluster_with_threshold(clusters,a_cthresh());
    vcl_sort(clusters.begin(), clusters.end(), less_num_members);

    // write the cluster file
    write_clusters(a_out_file(),clusters);
  }
  else{
    cluster_for_trace(clusters);
  }
}


// The Main Function
int main(int argc, char** argv)
{
  vul_arg_parse(argc, argv);

  switch(a_dim()){
  case 64:
    cluster_features<64>();
    break;
  case 128:
    cluster_features<128>();
    break;
  default:
      vcl_cerr << "features with dimension "<<a_dim()<<" not supported" << vcl_endl;
  }

  return 0;
}
