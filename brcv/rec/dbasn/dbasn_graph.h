//---------------------------------------------------------------------
// This is brcv/rec/dbasn/dbasn_graph.h
//:
// \file
// \brief a simple graph structure
//
// \author
//  O.C. Ozcanli - January 21, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef dbasn_graph_h_
#define dbasn_graph_h_

#include <vcl_utility.h>
#include <vcl_cstdio.h>
#include <vcl_vector.h>
#include <vcl_string.h>

#include <dbasn/dbasn_node.h>

//: Graduated Assignment graph structure
class dbasn_graph
{
protected:
  //: List of grad. assignment graph nodes.
  vcl_vector<dbasn_node*> nodes_;
  
  //: adjacency matrix of this graph, nN * nN. 
  //  Value represents the cost of each link.
  float     **links_;

  //: number of links (non-zero entries in the upper matrix of links_[][].
  int       n_links_;

  bool      b_1st_graph_;

public:  
  //####### Constructor/Destructor #######
  dbasn_graph () {
    nodes_.clear();
    links_ = NULL;
    n_links_ = 0;
  }
  virtual ~dbasn_graph () {
    if (links_ != NULL) {
      assert (nodes_.size() != 0);
      for (unsigned int i=0; i<nodes_.size(); i++)
        delete []links_[i];
      delete []links_;
    }
  }

  //####### Data access functions ####### 
  const vcl_vector<dbasn_node*>& nodes () const {
    return nodes_;
  }
  const dbasn_node* nodes (const unsigned int i) const {
    return nodes_[i];
  }
  dbasn_node* nodes (const unsigned int i) {
    return nodes_[i];
  }
  void _add_node (const dbasn_node* N) {
    nodes_.push_back ((dbasn_node*) N);
  }

  const unsigned int n_nodes() const {
    return nodes_.size();
  }

  //: allocate the links_[][] array according to the size of nodes_[].
  void alloc_links ();

  float** links() const { 
    return links_; 
  }
  float links (const unsigned int i, const unsigned int j) const {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    return links_[i][j];
  }
  void add_link (const unsigned int i, const unsigned int j, const float cost) {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    links_[i][j] = cost;
    links_[j][i] = cost;
    nodes_[i]->_add_conn_nid (j);
    nodes_[j]->_add_conn_nid (i);
    n_links_++;
  }
  int n_links() {
    return n_links_;
  }

  void set_1st_graph (const bool b) {
    b_1st_graph_ = b;
  }  

  //####### Graph connectivity query functions #######
  void get_node_cost_max_min (float& max, float& min) const;
  void get_link_cost_max_min (float& max, float& min) const;

  ///const int N_conn (unsigned int nid);

  //####### Graph connectivity modification functions #######
  void normalize_node_cost (const double max, const double min, const int verbose = 1);
  void normalize_link_cost (const double max, const double min, const int verbose = 1);
 
  //####### Other Functions #######
  void print_node_cost (void);
  void print_link_cost ();
};

//: Medial scaffold shock curve type
#define AL_TYPE             char
#define AL_TYPE_BOGUS       '?'
#define AL_TYPE_A3_RIB      'R'
#define AL_TYPE_A13_AXIAL   'A'
#define AL_TYPE_DEGE_AXIAL  'D'
#define AL_TYPE_VIRTUAL     'V'

//: The augmented graph with link type info.
class dbasn_graph_aug : public dbasn_graph
{
protected:
  AL_TYPE** links_type_;

public:
  //###### Constructor/Destructor ######
  dbasn_graph_aug () : dbasn_graph () {
    links_type_ = NULL;
  }
  virtual ~dbasn_graph_aug () {
    if (links_type_ != NULL) {
      assert (nodes_.size() != 0);
      for (unsigned int i=0; i<nodes_.size(); i++)
        delete []links_type_[i];
      delete []links_type_;
    }
  }

  //###### Data access functions ######
  AL_TYPE** links_type () const {
    return links_type_;
  }
  AL_TYPE link_type (const unsigned int i, const unsigned int j) const {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    return links_type_[i][j];
  }

  //: allocate the links_type_[][] array according to the size of nodes_[].
  void alloc_links_type () {
    const unsigned int nN = nodes_.size();
    links_type_ = new AL_TYPE*[nN];
    for (unsigned int i = 0; i<nN; i++)
      links_type_[i] = new AL_TYPE[nN];

    for (unsigned int i = 0; i<nN; i++)
      for (unsigned int j = 0; j<nN; j++)
        links_type_[i][j] = AL_TYPE_BOGUS;
  }

  void add_link_type (const unsigned int i, const unsigned int j, const AL_TYPE type) {
    assert (i < nodes_.size());
    assert (j < nodes_.size());
    links_type_[i][j] = type;
    links_type_[j][i] = type;
  }
};

#endif
