// This is brcv/seg/dbcll/dbcll_rnn_agg_clustering.cxx
//:
// \file
// \brief A RNN Agglomerative Clustering
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 1/28/08
//
// This code is base on the algorithm described in:
//   B. Leibe, A. Leonardis, B. Schiele
//   Robust Object Detection with Interleaved Categorization and Segmentation
//   IJCV 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include "dbcll_rnn_agg_clustering.h"
#include <vcl_set.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>
#include "dbcll_cluster.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>


namespace{

template<class T>
double find_nn(T begin, T end, const dbcll_cluster& c, T& result)
{
  double sim = -vcl_numeric_limits<double>::infinity();
  for(T i=begin; i!=end; ++i){
    double s = c.similarity(**i);
    if( s > sim){
      sim = s;
      result = i;
    }
  }
  return sim;
}

bool greater_similarity(const vcl_pair<double,dbcll_cluster_sptr>& c1,
                        const vcl_pair<double,dbcll_cluster_sptr>& c2)
{
  return c1.first < c2.first;
}


};


//=============================================================================

//: Constructor
dbcll_remainder_heap::dbcll_remainder_heap(const citr& begin, const citr& end)
{
  seed_ = (*(end-1))->clone();
  for(citr c= begin; c!=end; ++c){
    double s = seed_->similarity(**c);
    heap_.push_back(sc_pair(s,*c));
  }
  heap_end_ = heap_.end()-1;
  vcl_make_heap(heap_.begin(), heap_end_, greater_similarity);
}


//: Reseed the heap
void dbcll_remainder_heap::reseed(const dbcll_cluster_sptr& c)
{
  seed_ = c->clone();
  for(heap_iterator hitr= heap_.begin(); hitr!=heap_.end(); ++hitr){
    hitr->first = seed_->similarity(*hitr->second);
  }
  vcl_make_heap(heap_.begin(), heap_.end(), greater_similarity);
  vcl_pop_heap(heap_.begin(), heap_.end(), greater_similarity);
  heap_end_ = last_nn_ = heap_.end()-1;
}


//: pop an element from the heap
dbcll_remainder_heap::sc_pair
dbcll_remainder_heap::pop_with_sim()
{
  if((heap_.end()-heap_end_)>10000){
    vcl_cout << "pop reseeding" << vcl_endl;
    if(heap_end_ != heap_.begin())
      --heap_end_;
    reseed(heap_end_->second);
  }
  if(heap_end_ == heap_.end()){
    vcl_pop_heap(heap_.begin(), heap_end_, greater_similarity);
    --heap_end_;
  }
  sc_pair p = heap_.back();
  heap_.pop_back();
  return p;
}

//: pop an element from the heap
dbcll_cluster_sptr
dbcll_remainder_heap::pop()
{
  return pop_with_sim().second;
}

void dbcll_remainder_heap::insert(const dbcll_cluster_sptr& c)
{
  double sim = seed_->similarity(*c);
  if(sim < heap_end_->first && heap_.begin() != heap_end_){
    heap_iterator i = heap_.insert(heap_end_, sc_pair(sim,c));
    vcl_push_heap(heap_.begin(), i, greater_similarity);
  }
  else{
    heap_iterator i = vcl_lower_bound(heap_end_, heap_.end(), sc_pair(sim,c), greater_similarity);
    heap_.insert(i, sc_pair(sim,c));
  }
}

//: Find the nearest neighbor with similarity greater than nn_sim
dbcll_remainder_heap::sc_pair
dbcll_remainder_heap::nearest_neighbor(const dbcll_cluster_sptr& query, double nn_sim)
{
  if(heap_.empty())
    return sc_pair(-vcl_numeric_limits<double>::infinity(),NULL);

   

  double query_thresh = seed_->similarity(*query);
  double best_sim = nn_sim;
  double ball_thresh = -vcl_numeric_limits<double>::infinity();
  double min_thresh = 0;
  if(vnl_math::isfinite(best_sim)){
    double srad = 2*vcl_sqrt(query_thresh*best_sim);
    ball_thresh = query_thresh + best_sim - srad;
    min_thresh = query_thresh + best_sim + srad;
  }

  heap_iterator hitr = last_nn_ = heap_.end();

  if(heap_end_ == heap_.end()){
    vcl_pop_heap(heap_.begin(), heap_end_, greater_similarity);
    --heap_end_;
  }
  if(heap_end_->first <= min_thresh){
    hitr = vcl_upper_bound(heap_end_, heap_.end(), sc_pair(min_thresh,NULL), greater_similarity);
    --hitr;
  }
  else{
    while(heap_end_->first >= min_thresh && heap_end_ != heap_.begin())
    {
      vcl_pop_heap(heap_.begin(), heap_end_, greater_similarity);
      --heap_end_;
    }
    hitr = heap_end_;
  }

  while(hitr->first >= ball_thresh)
  {
    double s = hitr->second->similarity(*query);
    if(s >= best_sim){
      best_sim = s;
      ball_thresh = query_thresh + best_sim - 2*vcl_sqrt(query_thresh*best_sim);
      last_nn_ = hitr;
    }
    if(hitr == heap_.begin())
      break;
    if(hitr == heap_end_){
      vcl_pop_heap(heap_.begin(), heap_end_, greater_similarity);
      --heap_end_;
    }
    --hitr;
  }
#if 0
  vcl_cout << " sorted size = "<< (heap_.end() - heap_end_)
          << " found at " << (heap_.end() - last_nn_)
          << " of " <<(heap_.end() - hitr)
          << " with theshold "<< ball_thresh <<vcl_endl;
#endif
#if 0
  if(last_nn_ != heap_.end() && (heap_.end()-heap_end_)>10000){
    vcl_cout << "reseeding" << vcl_endl;
    reseed(last_nn_->second);
    return nearest_neighbor(query, last_nn_->first);
    //return *last_nn_;
  }
#endif
  if(last_nn_ == heap_.end())
    return sc_pair(-vcl_numeric_limits<double>::infinity(),NULL);
  return sc_pair(best_sim,last_nn_->second);
}


void dbcll_remainder_heap::remove_last_nn()
{
  heap_.erase(last_nn_);
  last_nn_ = heap_.end();
}


void dbcll_remainder_heap::print_status() const
{
  vcl_cout << "remaining: "<<heap_.size()
            << " sorted: "<<(heap_.end() - heap_end_)
            << " range ("<<heap_end_->first<<", "<<heap_.back().first<<")" <<vcl_endl;
}


//=============================================================================

//: Constructor
dbcll_remainder_set::dbcll_remainder_set(const citr& begin, const citr& end)
  : clusters_(begin, end)
{
}

//: pop an element from the heap
dbcll_cluster_sptr
dbcll_remainder_set::pop()
{
  dbcll_cluster_sptr c = *(clusters_.begin());
  clusters_.erase(clusters_.begin());
  return c;
}


void dbcll_remainder_set::insert(const dbcll_cluster_sptr& c)
{
  clusters_.insert(c);
}

//: Find the nearest neighbor with similarity greater than nn_sim
dbcll_remainder_set::sc_pair
dbcll_remainder_set::nearest_neighbor(const dbcll_cluster_sptr& query, double nn_sim)
{
  if(clusters_.empty())
    return sc_pair(-vcl_numeric_limits<double>::infinity(),NULL);

  double sim = -vcl_numeric_limits<double>::infinity();
  last_nn_ = clusters_.end();
  typedef cluster_container::iterator citr;
  for(citr i=clusters_.begin(); i!=clusters_.end(); ++i){
    double s = query->similarity(**i);
    if( s > sim){
      sim = s;
      last_nn_ = i;
    }
  }

  if(last_nn_ == clusters_.end())
    return sc_pair(-vcl_numeric_limits<double>::infinity(),NULL);
  return sc_pair(sim,*last_nn_);
}


void dbcll_remainder_set::remove_last_nn()
{
  clusters_.erase(last_nn_);
  last_nn_ = clusters_.end();
}


void dbcll_remainder_set::print_status() const
{
  vcl_cout << "remaining: "<<clusters_.size() <<vcl_endl;
}
  

//=============================================================================

// Apply RNN Agglometerative clustering to these clusters
void dbcll_rnn_agg_clustering(dbcll_remainder& remain,
                              vcl_vector<dbcll_cluster_sptr>& clusters, 
                              double t)
{
  clusters.clear();
  unsigned init_size = remain.size();
  
  vcl_vector<dbcll_cluster_sptr> chain;
  vcl_vector<double> chain_sim;
  chain.push_back(remain.pop());
  chain_sim.push_back(-vcl_numeric_limits<double>::infinity());
  
  while(1<remain.size() || !chain.empty()){
    dbcll_cluster_sptr curr = chain.back();
    
    // Find the nearest neighbor, (only look at neighbors closer than the previous NN)
    vcl_pair<double,dbcll_cluster_sptr> nn = remain.nearest_neighbor(curr,chain_sim.back());
    
    if(nn.first > chain_sim.back()){
      // No RNNs, Add nn_itr to the chain
      chain.push_back(nn.second);
      chain_sim.push_back(nn.first);
      remain.remove_last_nn();
    }
    else{
      // Found RNNs, Agglomerate the last two chain links
      if(chain_sim.back() > t){
        dbcll_cluster_sptr p1 = chain.back();  chain.pop_back();
        dbcll_cluster_sptr p2 = chain.back();  chain.pop_back();
        chain_sim.pop_back();
        chain_sim.pop_back();
        p1->merge(*p2);
        remain.insert(p1);
      }
      else{
        // This chain is finished
        clusters.insert(clusters.end(), chain.begin(), chain.end());
        chain.clear();
        chain_sim.clear();
        remain.print_status();
        //vcl_cout << "chain finished: " << 100.0*remain.size() / double(init_size) << "% remaining"<<vcl_endl;
      }
    }
    if(chain.empty() && 1<remain.size()){
      chain.push_back(remain.pop());
      chain_sim.push_back(-vcl_numeric_limits<double>::infinity());
    }
  }
  if(remain.size() == 1)
    clusters.push_back(remain.pop());
}


// Apply complete RNN Agglometerative clustering and return the trace
void dbcll_rnn_agg_clustering(dbcll_remainder& remain, 
                              vcl_vector<dbcll_trace_pt>& trace)
{
  unsigned init_size = remain.size();

  vcl_vector<dbcll_cluster_sptr> chain;
  vcl_vector<double> chain_sim;
  chain.push_back(remain.pop());
  chain_sim.push_back(-vcl_numeric_limits<double>::infinity());

  while(1<remain.size() || !chain.empty()){
    dbcll_cluster_sptr curr = chain.back();

    // Find the nearest neighbor, (only look at neighbors closer than the previous NN)
    vcl_pair<double,dbcll_cluster_sptr> nn = remain.nearest_neighbor(curr,chain_sim.back());

    if(nn.first > chain_sim.back()){
      // No RNNs, Add nn_itr to the chain
      chain.push_back(nn.second);
      chain_sim.push_back(nn.first);
      remain.remove_last_nn();
    }
    else{
      // Found RNNs, Agglomerate the last two chain links
      dbcll_cluster_sptr p1 = chain.back();  chain.pop_back();
      dbcll_cluster_sptr p2 = chain.back();  chain.pop_back();
      double sim = chain_sim.back();
      chain_sim.pop_back();
      chain_sim.pop_back();
      unsigned c1 = p1->key_index();
      unsigned c2 = p2->key_index();
      p1->merge(*p2);
      trace.push_back(dbcll_trace_pt(c1,c2,sim,p1->var()));
      remain.insert(p1);
    }
    if(chain.empty() && 1<remain.size()){
      remain.print_status();
      chain.push_back(remain.pop());
      chain_sim.push_back(-vcl_numeric_limits<double>::infinity());
    }
  }
}



