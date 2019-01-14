// This is mleotta/cmd/trace2clusters.cxx

#include <vul/vul_arg.h>
#include <vbl/vbl_triple.h>

#include <modrec/modrec_feature_3d.h>

#include <dbcll/dbcll_euclidean_cluster.h>
#include <dbcll/dbcll_rnn_agg_clustering.h>

#include <vnl/vnl_double_3.h>

#include "cluster_io.h"


void merge(std::vector<unsigned>& c1, std::vector<unsigned>& c2)
{
  if(c1.empty() || c2.empty()){
    std::cout << "empty cluster" << std::endl;
    return;
  }
  std::vector<unsigned> tmp;
  std::merge(c1.begin(), c1.end(),
            c2.begin(), c2.end(),
            std::back_inserter(tmp));

  if(tmp.front() == c1.front()){
    c1.swap(tmp);
    c2.clear();
  }else{
    c2.swap(tmp);
    c1.clear();
  }
}

bool less_size(const std::vector<unsigned>& v1,
               const std::vector<unsigned>& v2)
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
  vul_arg<std::string>  a_trace_file("-trace", "path to trace file", "");
  vul_arg<std::string>  a_cluster_out("-out", "path to output clusters index file", "");
  vul_arg<double>      a_thresh("-thresh", "similarity theshold", .1);
  vul_arg_parse(argc, argv);

  std::vector<dbcll_trace_pt> trace;
  read_trace(a_trace_file(),trace);

  unsigned max_idx=0;
  for(unsigned i=0; i<trace.size(); ++i){
    if(trace[i].c1 > max_idx) max_idx = trace[i].c1;
    if(trace[i].c2 > max_idx) max_idx = trace[i].c2;
  }

  // construct the initial clusters
  std::vector<std::vector<unsigned> > clusters(max_idx+1,std::vector<unsigned>(1,0));
  for(unsigned i=0; i<=max_idx; ++i)
    clusters[i][0] = i;

  
  std::cout << "initial cluster count: " << clusters.size() << std::endl;

  std::stable_sort(trace.begin(), trace.end(), trace_sim_greater);

  for(unsigned i=0; i<trace.size(); ++i){
    if(trace[i].sim >= -a_thresh())
    {
      merge(clusters[trace[i].c1], clusters[trace[i].c2]);
    }
  }


  std::vector<std::vector<unsigned> > valid_clusters;
  for(unsigned i=0; i<=max_idx; ++i)
    if(!clusters[i].empty())
      valid_clusters.push_back(clusters[i]);

  
  std::cout << "final cluster count: " << valid_clusters.size() << std::endl;

  std::sort(valid_clusters.begin(), valid_clusters.end(), less_size);

  write_clusters(a_cluster_out(), valid_clusters);

  return 0;
}
