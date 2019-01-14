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



vul_arg<std::string>  a_feat_file("-feat3d", "path to 3d features", "");
vul_arg<std::string>  a_out_file("-out", "path to output file (clusters indices or trace)", "");
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


void cluster_with_threshold(std::vector<dbcll_cluster_sptr>& clusters, double thresh)
{
  // Cluster the features based on appearance
  dbcll_remainder_heap remain(clusters.begin(), clusters.end());

  clusters.clear();
  dbcll_rnn_agg_clustering(remain, clusters, -thresh);

}


void cluster_for_trace(std::vector<dbcll_cluster_sptr>& clusters)
{
  // Cluster the features based on appearance
  dbcll_remainder_heap remain(clusters.begin(), clusters.end());

  std::vector<dbcll_trace_pt> trace;
  dbcll_rnn_agg_clustering(remain, trace);

  // write the trace file
  write_trace(a_out_file(),trace);
}


template <unsigned dim>
void cluster_features()
{
  std::vector<modrec_desc_feature_3d<dim> > features;
  typedef vbl_triple<unsigned,unsigned,unsigned> utriple;
  std::vector<utriple> idx_array;
  read_features(a_feat_file(), features, idx_array);

  std::cout << "read " << features.size() << std::endl;

  std::vector<dbcll_cluster_sptr> clusters;
  for(unsigned i=0; i<features.size(); ++i){
    const vgl_point_3d<double>& pt = features[i].position();
    clusters.push_back(new dbcll_euclidean_cluster<3>(vnl_double_3(pt.x(),pt.y(),pt.z()),i));
  }

  // do k-means first
  if(a_sthresh.set()){
    std::vector<vnl_vector_fixed<double,3> > points;
    for(unsigned i=0; i<features.size(); ++i){
      const vgl_point_3d<double>& pt = features[i].position();
      points.push_back(vnl_double_3(pt.x(),pt.y(),pt.z()));
    }

    unsigned k = points.size()/20000 + 1;
    std::cout << "init k-means with k = " << k << std::endl;
    std::vector<vnl_vector_fixed<double,3> > means = dbcll_init_k_means_d2<3>(points,k);

    std::cout << "starting k-means" << std::endl;
    std::vector<std::vector<unsigned> > groups;
    unsigned n = dbcll_fast_k_means<3>(points, groups, means, 25);

    std::cout << "finished k-means" << std::endl;

    std::vector<dbcll_cluster_sptr> sub_clusters, new_clusters;
    for(unsigned i=0; i<groups.size(); ++i){
      std::cout << "clustering group "<< i << std::endl;
      sub_clusters.clear();
      for(unsigned j=0; j<groups[i].size(); ++j)
        sub_clusters.push_back(clusters[groups[i][j]]);

      cluster_with_threshold(sub_clusters,a_sthresh());
      new_clusters.insert(new_clusters.end(),
                          sub_clusters.begin(), sub_clusters.end());
    }
    clusters.swap(new_clusters);
    std::cout << "clustering sub groups done" << std::endl;
  }

  if(a_cthresh.set()){
    cluster_with_threshold(clusters,a_cthresh());
    std::sort(clusters.begin(), clusters.end(), less_num_members);

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
      std::cerr << "features with dimension "<<a_dim()<<" not supported" << std::endl;
  }

  return 0;
}
