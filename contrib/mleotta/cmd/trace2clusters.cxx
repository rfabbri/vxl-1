// This is mleotta/cmd/trace2clusters.cxx

#include <vul/vul_arg.h>
#include <vbl/vbl_triple.h>

#include <modrec/modrec_feature_3d.h>

#include <dbcll/dbcll_euclidean_cluster.h>
#include <dbcll/dbcll_rnn_agg_clustering.h>

#include <vnl/vnl_double_3.h>

#include "cluster_io.h"


void merge(vcl_vector<unsigned>& c1, vcl_vector<unsigned>& c2)
{
  if(c1.empty() || c2.empty()){
    vcl_cout << "empty cluster" << vcl_endl;
    return;
  }
  vcl_vector<unsigned> tmp;
  vcl_merge(c1.begin(), c1.end(),
            c2.begin(), c2.end(),
            vcl_back_inserter(tmp));

  if(tmp.front() == c1.front()){
    c1.swap(tmp);
    c2.clear();
  }else{
    c2.swap(tmp);
    c1.clear();
  }
}

bool less_size(const vcl_vector<unsigned>& v1,
               const vcl_vector<unsigned>& v2)
{
  if(v1.size() == v2.size())
    return v1.front() < v2.front();
  return v1.size() < v2.size();
}

bool trace_sim_greater(const dbcll_trace_pt& t1,
                    const dbcll_trace_pt& t2)
{
  return t1.sim > t2.sim;
}

// The Main Function
int main(int argc, char** argv)
{
  vul_arg<vcl_string>  a_trace_file("-trace", "path to trace file", "");
  vul_arg<vcl_string>  a_cluster_out("-out", "path to output clusters index file", "");
  vul_arg<double>      a_thresh("-thresh", "similarity theshold", .1);
  vul_arg_parse(argc, argv);

  vcl_vector<dbcll_trace_pt> trace;
  read_trace(a_trace_file(),trace);

  unsigned max_idx=0;
  for(unsigned i=0; i<trace.size(); ++i){
    if(trace[i].c1 > max_idx) max_idx = trace[i].c1;
    if(trace[i].c2 > max_idx) max_idx = trace[i].c2;
  }

  // construct the initial clusters
  vcl_vector<vcl_vector<unsigned> > clusters(max_idx+1,vcl_vector<unsigned>(1,0));
  for(unsigned i=0; i<=max_idx; ++i)
    clusters[i][0] = i;

  
  vcl_cout << "initial cluster count: " << clusters.size() << vcl_endl;

  vcl_stable_sort(trace.begin(), trace.end(), trace_sim_greater);

  for(unsigned i=0; i<trace.size(); ++i){
    if(trace[i].sim >= -a_thresh())
    {
      merge(clusters[trace[i].c1], clusters[trace[i].c2]);
    }
  }


  vcl_vector<vcl_vector<unsigned> > valid_clusters;
  for(unsigned i=0; i<=max_idx; ++i)
    if(!clusters[i].empty())
      valid_clusters.push_back(clusters[i]);

  
  vcl_cout << "final cluster count: " << valid_clusters.size() << vcl_endl;

  vcl_sort(valid_clusters.begin(), valid_clusters.end(), less_size);

  write_clusters(a_cluster_out(), valid_clusters);

  return 0;
}
