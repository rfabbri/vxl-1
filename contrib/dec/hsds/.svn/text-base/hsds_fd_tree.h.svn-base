#ifndef hsds_fd_tree_h_
#define hsds_fd_tree_h_

#include <vcl_map.h>
#include <vcl_utility.h>

#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_bounding_box.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_3d.h>

#include "hsds_fd_tree_node_index.h"

template<class T, unsigned d>
class hsds_fd_tree : vbl_ref_count {

public:
  //: default constructor - empty tree
  hsds_fd_tree() : max_levels_(hsds_fd_tree_node_index<d>::MAX_LEVELS) {}

  //: constructor taking the bounding box of the area to represent
  hsds_fd_tree(vbl_bounding_box<double,d> bbox, unsigned int max_levels = hsds_fd_tree_node_index<d>::MAX_LEVELS);

  //: constructor taking the bounding box, initial level of subdivision, and init value for nodes.
  hsds_fd_tree(vbl_bounding_box<double,d> bbox, unsigned char init_level, T const& init_data, unsigned int max_levels = hsds_fd_tree_node_index<d>::MAX_LEVELS);

  //: constructor taking all data
  hsds_fd_tree(vbl_bounding_box<double,d> bbox, vcl_map<hsds_fd_tree_node_index<d>,T > nodes, unsigned int max_levels = hsds_fd_tree_node_index<d>::MAX_LEVELS);

  //: returns the bounding box
  vbl_bounding_box<double,d> bounding_box() const {return bbox_;}

  //: return the bounding box of a cell
  vbl_bounding_box<double,d> cell_bounding_box(hsds_fd_tree_node_index<d> const& cell_index) const; 
  //: return the bounding box of a cell given a block bounding box 
  static vbl_bounding_box<double,d> cell_bounding_box(vbl_bounding_box<double,d> const& block_bb, hsds_fd_tree_node_index<d> const& cell_index);

  //: fill the block with the specified data
  void fill(T const& data);

  //: returns the maximum subdivision level
  unsigned int max_level() const {return max_levels_;}

  bool contains(vnl_vector<double> const& pt) const;

  //: return the node which contains the point pt
  hsds_fd_tree_node_index<d> index_at(vnl_vector<double> const& pt);
  //: convienence method for 3-d case
  hsds_fd_tree_node_index<d> index_at(double x, double y, double z){return index_at(vnl_vector_fixed<double,3>(x,y,z));}

  //: return full node code corresponding to pt, regardless of level at which pt is actually stored
  hsds_fd_tree_node_index<d> full_index_at(vnl_vector<double> const& pt);

  //: return full node code corresponding to pt, regardless of level at which pt is actually stored
  //: the search starts at start_index (returned value will be a child of start_index)
  hsds_fd_tree_node_index<d> full_index_at(hsds_fd_tree_node_index<d> start_index, vnl_vector<double> const& pt);

  //: return the data value at the point pt
  T& data_at(vnl_vector<double> pt) {return nodes_[index_at(pt)];}
  //: convienence method for 3-d case
  T& data_at(double x, double y, double z) {return data_at(vnl_vector_fixed<double,3>(x,y,z));}

  //: return the neighboring cell of index at shallower or equal level
  bool neighbor_cell(hsds_fd_tree_node_index<d> index, unsigned int dim, bool positive_neighbor, hsds_fd_tree_node_index<d> &neighbor) const;

  //: return a set of cells bordering on the specified face of another cell
  bool neighbor_cells(hsds_fd_tree_node_index<d> index, unsigned int dim, bool positive_neighbor, vcl_vector<hsds_fd_tree_node_index<d> > &neighbors) const;

  //: subdivide the given node
  void split(hsds_fd_tree_node_index<d> index, T const& new_data);

  //: merge the given node with its siblings
  void merge(hsds_fd_tree_node_index<d> index, T const& new_data);

  //: return the number of leaf nodes in the tree
  unsigned int size() const { return nodes_.size();}

  //: iterator through nodes
  typedef typename vcl_map<hsds_fd_tree_node_index<d>, T >::iterator iterator;
  //: const iterator through nodes
  typedef typename vcl_map<hsds_fd_tree_node_index<d>, T >::const_iterator const_iterator;

  //: iterator to first node
  iterator begin(){return nodes_.begin();}
  const_iterator begin() const {return nodes_.begin();}
  //: iterator to upper bound given by node
  const_iterator upper_bound(hsds_fd_tree_node_index<d> idx) const {return nodes_.upper_bound(idx);}
  //: iterator to lower bound given by node
  const_iterator lower_bound(hsds_fd_tree_node_index<d> idx) const {return nodes_.lower_bound(idx);}
  //: iterator to node
  const_iterator find(hsds_fd_tree_node_index<d> idx) const {return nodes_.find(idx);}
  //: iterator one past last node
  iterator end() {return nodes_.end();}
  const_iterator end() const {return nodes_.end();}

  //: operator returning data at specifed index
  //: Note: level is ignored
  T& operator [] (hsds_fd_tree_node_index<d> index){return nodes_[index];}

  //: create a new tree with identical structure of this tree, but with a new datatype
  template<class T2>
  hsds_fd_tree<T2,d> twin_tree(T2 init_val) const {
    vcl_map<hsds_fd_tree_node_index<d>,T2 > new_nodes;
    typename vcl_map<hsds_fd_tree_node_index<d>,T >::const_iterator old_it;
    for (old_it = nodes_.begin(); old_it != nodes_.end(); ++old_it) {
      new_nodes.insert(vcl_make_pair<hsds_fd_tree_node_index<d>,T2 >(old_it->first,init_val));
    }
    hsds_fd_tree<T2,d> twin(this->bbox_,new_nodes,this->max_levels_);
    return twin;
  }

  //: binary write tree to stream
  void b_write(vcl_ostream &os) const;

  //: binary read tree from stream
  void b_read(vcl_istream &is);


private:
  unsigned int max_levels_;

  vbl_bounding_box<double,d> bbox_;

  vcl_map<hsds_fd_tree_node_index<d>,T > nodes_;

};

// IO functions
//: write index to stream for debugging
template <unsigned d>
vcl_ostream&  operator<<(vcl_ostream& s, hsds_fd_tree_node_index<d> const& index);

//: header class for storing tree in file
template <class T, unsigned d>
class hsds_fd_tree_header
{
public:
  //: default constructor
  hsds_fd_tree_header(){}

  //: constructor
  hsds_fd_tree_header(vbl_bounding_box<double,d> const& bbox, unsigned int num_nodes, unsigned int max_sub_levels) 
    : dimension(d), data_size(sizeof(T)), corner_low(bbox.min()), corner_high(bbox.max()), nnodes(num_nodes), max_levels(max_sub_levels) {}

  unsigned int dimension;
  unsigned int data_size;
  unsigned int nnodes;
  unsigned int max_levels;
  vnl_vector_fixed<double,d> corner_low;
  vnl_vector_fixed<double,d> corner_high;
};




#endif
