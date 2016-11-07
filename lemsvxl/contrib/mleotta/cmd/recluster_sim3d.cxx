// This is mleotta/cmd/recluster_sim3d.cxx

#include <vul/vul_arg.h>
#include <vbl/vbl_triple.h>

#include <modrec/modrec_feature_3d.h>

#include <dbgl/algo/dbgl_similarity_3d.h>
#include <dbcll/dbcll_sim3d_cluster.h>
#include <dbcll/dbcll_similarity_matrix.h>
#include <dbcll/dbcll_rnn_agg_clustering.h>

#include <vnl/vnl_double_3.h>

#include "cluster_io.h"


bool less_num_members(const dbcll_cluster_sptr& c1, 
                      const dbcll_cluster_sptr& c2)
{
  if(c1->size() == c2->size())
    return c1->key_index() < c2->key_index();
  return c1->size() < c2->size();
}

bool less_size(const vcl_vector<unsigned>& v1,
               const vcl_vector<unsigned>& v2)
{
  if(v1.size() == v2.size())
    return v1.front() < v2.front();
  return v1.size() < v2.size();
}


template <unsigned dim>
void recluster(const vcl_string& feat_file,
               vcl_vector<vcl_vector<unsigned> >& clusters_idx,
               double thresh)
{
  vcl_vector<modrec_desc_feature_3d<dim> > features;
  typedef vbl_triple<unsigned,unsigned,unsigned> utriple;
  vcl_vector<utriple> idx_array;  
  read_features(feat_file, features, idx_array);

  vcl_cout << "read " << features.size() << vcl_endl;


  vcl_vector<vcl_vector<unsigned> > new_clusters;

  for(unsigned i=0; i<clusters_idx.size(); ++i){

    vcl_vector<dbgl_similarity_3d<double> > sims;
    vcl_vector<unsigned> sim_idx;
    vcl_vector<unsigned> ci = clusters_idx[i];
    for(unsigned j=0; j<ci.size(); ++j){
      const modrec_feature_3d& f = features[ci[j]];
      dbgl_similarity_3d<double> sim(f.scale(),f.orientation(),f.position()-vgl_point_3d<double>(0,0,0));
      sims.push_back(sim);
      sim_idx.push_back(ci[j]);
    }
    vcl_vector<dbcll_cluster_sptr> clusters;
    for(unsigned j=0; j<sims.size(); ++j){
      clusters.push_back(new dbcll_sim3d_cluster(sims,j));
    }

    // precompute distances
    vcl_vector<dbcll_cluster_sptr> clusters2 = dbcll_precompute_similarity(clusters);
    dbcll_remainder_set remain(clusters2.begin(), clusters2.end());
    clusters2.clear();
    dbcll_rnn_agg_clustering(remain, clusters2,-thresh);
    vcl_sort(clusters2.begin(), clusters2.end(), less_num_members);
    for(unsigned j=0; j<clusters2.size(); ++j){
      vcl_vector<unsigned> members = clusters2[j]->members();
      vcl_vector<unsigned> new_members(members.size());
      for(unsigned k=0; k<members.size(); ++k){
        new_members[k] = sim_idx[members[k]];
      }
      new_clusters.push_back(new_members);
    }
  }

  sort(new_clusters.begin(), new_clusters.end(), less_size);
  clusters_idx.swap(new_clusters);
}

// The Main Function
int main(int argc, char** argv)
{
  vul_arg<vcl_string>  a_feat_file("-feat3d", "path to 3d features", "");
  vul_arg<vcl_string>  a_clust_file("-in", "path to input clusters index file", "");
  vul_arg<vcl_string>  a_clust_file_out("-out", "path to output clusters index file", "");
  vul_arg<double>      a_thresh("-thresh", "sim3d threshold", .1);
  vul_arg<unsigned>    a_dim("-dim", "feature descriptor dimension", 128);
  vul_arg_parse(argc, argv);

  vcl_vector<vcl_vector<unsigned> > clusters_idx;
  read_clusters(a_clust_file(), clusters_idx);

  switch(a_dim()){
  case 64:
    {
      recluster<64>(a_feat_file(), clusters_idx, a_thresh());
    }
    break;
  case 128:
    {
      recluster<128>(a_feat_file(), clusters_idx, a_thresh());
    }
    break;
  default:
      vcl_cerr << "features with dimension "<<a_dim()<<" not supported" << vcl_endl;
  }

  // write the new cluster file
  write_clusters(a_clust_file_out(), clusters_idx);
  
  return 0;
}
