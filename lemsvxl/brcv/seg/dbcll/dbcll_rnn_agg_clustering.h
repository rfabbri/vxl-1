// This is brcv/seg/dbcll/dbcll_rnn_agg_clustering.h
#ifndef dbcll_rnn_agg_clustering_h_
#define dbcll_rnn_agg_clustering_h_
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

#include "dbcll_cluster_sptr.h"
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_cstddef.h>
#include <vcl_utility.h>

//: represents a point in the cluster process when c1 merges with c2
struct dbcll_trace_pt
{
  dbcll_trace_pt(): c1(0), c2(0), sim(0.0), var(0.0)  {}
  dbcll_trace_pt(unsigned cl1, unsigned cl2, double s, double v)
  : c1(cl1), c2(cl2), sim(s), var(v) {}
  //: indices of the clusters to merge
  unsigned c1,c2;
  //: similarity of the clusters before merge
  double sim;
  //: variance/size of the cluster after merge 
  double var;
};

//: A class to efficiently find remaining nearest neighbors
class dbcll_remainder
{
public:
  // define a similarity, cluster pair
  typedef vcl_pair<double,dbcll_cluster_sptr> sc_pair;

  //: return true if the heap is empty
  virtual bool empty() const = 0;

  virtual vcl_size_t size() const = 0;

  virtual dbcll_cluster_sptr pop() = 0;

  virtual void insert(const dbcll_cluster_sptr& c) = 0;

  //: Find the nearest neighbor with similarity greater than nn_sim
  virtual sc_pair nearest_neighbor(const dbcll_cluster_sptr& query, double nn_sim) = 0;

  virtual void remove_last_nn() = 0;

  virtual void print_status() const = 0;

};

//: A class to efficiently find remaining nearest neighbors
class dbcll_remainder_heap : public dbcll_remainder
{
public:
  typedef vcl_vector<sc_pair> heap_container;
  typedef heap_container::iterator heap_iterator;
  typedef vcl_vector<dbcll_cluster_sptr>::const_iterator citr;

  //: Constructor
  dbcll_remainder_heap(const citr& begin, const citr& end);

  //: return true if the heap is empty
  bool empty() const { return heap_.empty(); }

  vcl_size_t size() const { return heap_.size(); }

  void reseed(const dbcll_cluster_sptr& c);

  dbcll_cluster_sptr pop();
  sc_pair pop_with_sim();

  void insert(const dbcll_cluster_sptr& c);

  //: Find the nearest neighbor with similarity greater than nn_sim
  sc_pair nearest_neighbor(const dbcll_cluster_sptr& query, double nn_sim);

  void remove_last_nn();

  void print_status() const;


private:
  heap_container heap_;
  heap_iterator heap_end_;
  heap_iterator last_nn_;
  dbcll_cluster_sptr seed_;
};


//: A class to find remaining nearest neighbors by brute force
class dbcll_remainder_set : public dbcll_remainder
{
public:
  typedef vcl_set<dbcll_cluster_sptr> cluster_container;
  typedef vcl_vector<dbcll_cluster_sptr>::const_iterator citr;

  //: Constructor
  dbcll_remainder_set(const citr& begin, const citr& end);

  //: return true if the heap is empty
  bool empty() const { return clusters_.empty(); }

  vcl_size_t size() const { return clusters_.size(); }

  dbcll_cluster_sptr pop();

  void insert(const dbcll_cluster_sptr& c);

  //: Find the nearest neighbor with similarity greater than nn_sim
  sc_pair nearest_neighbor(const dbcll_cluster_sptr& query, double nn_sim);

  void remove_last_nn();

  void print_status() const;

private:
  cluster_container clusters_;
  cluster_container::iterator last_nn_;
};


//: Apply RNN Agglometerative clustering to these clusters up to threshold t
void dbcll_rnn_agg_clustering(dbcll_remainder& remain, 
                              vcl_vector<dbcll_cluster_sptr>& clusters,
                              double t);

//: Apply complete RNN Agglometerative clustering and return the trace
void dbcll_rnn_agg_clustering(dbcll_remainder& remain, 
                              vcl_vector<dbcll_trace_pt>& trace);



#endif // dbcll_rnn_agg_clustering_h_
