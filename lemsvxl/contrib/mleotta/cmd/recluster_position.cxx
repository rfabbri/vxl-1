// This is mleotta/cmd/recluster_position.cxx

#include <vul/vul_arg.h>
#include <vbl/vbl_triple.h>

#include <modrec/modrec_feature_3d.h>

#include <dbcll/dbcll_euclidean_cluster.h>
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
    vcl_vector<dbcll_cluster_sptr> clusters;
    const vcl_vector<unsigned>& ci = clusters_idx[i];
    for(unsigned j=0; j<ci.size(); ++j){
      vgl_point_3d<double> pt = features[ci[j]].position();
      clusters.push_back(new dbcll_euclidean_cluster<3>(vnl_double_3(pt.x(),pt.y(),pt.z()),ci[j]));
    }


#if 0
    if(clusters_idx[i].size() < 7)
      continue;

    
    vcl_vector<unsigned> ci = clusters_idx[i];
    dbcll_euclidean_cluster<dim> c(features[ci[0]].descriptor(),ci[0]);
    for(unsigned j=1; j<ci.size(); ++j){
      c.merge(dbcll_euclidean_cluster<dim>(features[ci[j]].descriptor(),ci[j]));
    }

    vcl_cout << "size: "<< ci.size() <<" var: " << c.var();

    vcl_vector<dbcll_cluster_sptr> clusters;
    for(unsigned j=0; j<features.size(); ++j){
      double d = vnl_vector_ssd(features[j].descriptor(), c.mean());
      if( d < thresh){
        vgl_point_3d<double> pt = features[j].position();
        clusters.push_back(new dbcll_euclidean_cluster<3>(vnl_double_3(pt.x(),pt.y(),pt.z()),j));
      }
    }
#endif

    if(clusters.size() > 1){
      dbcll_remainder_heap remain(clusters.begin(), clusters.end());
      clusters.clear();
      dbcll_rnn_agg_clustering(remain, clusters,-.01);
      vcl_sort(clusters.begin(), clusters.end(), less_num_members);
    }
    for(unsigned j=0; j<clusters.size(); ++j){
      vcl_vector<unsigned> members = clusters[j]->members();
      vcl_vector<unsigned> new_match;
      for(unsigned k=0; k< members.size(); ++k){
        new_match.push_back(members[k]);
      }
#if 0
      dbcll_euclidean_cluster<3>* cl = static_cast<dbcll_euclidean_cluster<3>*>(clusters[j].ptr());
      vcl_vector<unsigned> members = cl->members();
      
      if(members.size() < 10) continue;
      
      vcl_vector<unsigned> new_match;
      for(unsigned k=0; k< members.size(); ++k){
        vgl_point_3d<double> pt = features[members[k]].position();
        double d2 = vnl_vector_ssd(cl->mean(), vnl_double_3(pt.x(),pt.y(),pt.z()));
        if(d2 < 0.01)
          new_match.push_back(members[k]);
      }
#endif
      new_clusters.push_back(new_match);
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
  vul_arg<double>      a_athresh("-athresh", "appearance threshold", .1);
  vul_arg<unsigned>    a_dim("-dim", "feature descriptor dimension", 128);
  vul_arg_parse(argc, argv);

  vcl_vector<vcl_vector<unsigned> > clusters_idx;
  read_clusters(a_clust_file(), clusters_idx);

  switch(a_dim()){
  case 64:
    {
      recluster<64>(a_feat_file(), clusters_idx, a_athresh());
    }
    break;
  case 128:
    {
      recluster<128>(a_feat_file(), clusters_idx, a_athresh());
    }
    break;
  default:
      vcl_cerr << "features with dimension "<<a_dim()<<" not supported" << vcl_endl;
  }

  // write the new cluster file
  write_clusters(a_clust_file_out(), clusters_idx);
  
  return 0;
}
