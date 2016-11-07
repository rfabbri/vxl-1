#ifndef hsds_fd_tree_txx_
#define hsds_fd_tree_txx_

#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vcl_cassert.h>
#include <vcl_iomanip.h>
#include <vcl_utility.h>
#include <vcl_vector.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_bounding_box.h>

#include "hsds_fd_tree_node_index.h"
#include "hsds_fd_tree.h"

//: constructor taking the bounding box of the area to represent
template<class T, unsigned d>
hsds_fd_tree<T,d>::hsds_fd_tree(vbl_bounding_box<double,d> bbox, unsigned int max_levels) 
: bbox_(bbox), max_levels_(max_levels)
{
  // create root node
  hsds_fd_tree_node_index<d> index(0,0);
  nodes_.insert(vcl_make_pair<hsds_fd_tree_node_index<d>,T >(index, T()));
}

//: constructor taking the bounding box, initial level of subdivision, and init value for nodes.
template<class T, unsigned d>
hsds_fd_tree<T,d>::hsds_fd_tree(vbl_bounding_box<double,d> bbox, unsigned char init_level, T const& init_data, unsigned int max_levels)
: bbox_(bbox), max_levels_(max_levels)
{
  unsigned int nnodes = 1 << (d*init_level);
  unsigned int shift = hsds_fd_tree_node_index<d>::level_shift(init_level);
  for (unsigned i=0; i < nnodes; ++i) {
    hsds_fd_tree_node_index<d> index(i << shift, init_level);
    nodes_.insert(vcl_make_pair<hsds_fd_tree_node_index<d>,T >(index, init_data));
  }
}

//: constructor taking all data
template<class T, unsigned d>
hsds_fd_tree<T,d>::hsds_fd_tree(vbl_bounding_box<double,d> bbox, vcl_map<hsds_fd_tree_node_index<d>,T > nodes, unsigned int max_levels)
: bbox_(bbox), nodes_(nodes), max_levels_(max_levels) {}


//: return true if the point is located with the bounding box of the tree
template<class T, unsigned d>
bool hsds_fd_tree<T,d>::contains(vnl_vector<double> const& pt) const
{
  return bbox_.inside(pt.data_block());
}

 //: return the bounding box of a cell
template<class T, unsigned d>
vbl_bounding_box<double,d> hsds_fd_tree<T,d>::cell_bounding_box(hsds_fd_tree_node_index<d> const& cell_index) const
{
  return hsds_fd_tree<T,d>::cell_bounding_box(bbox_, cell_index);
}

//: return the bounding box of a cell given a block bounding box
template<class T, unsigned d>
vbl_bounding_box<double,d> hsds_fd_tree<T,d>::cell_bounding_box(vbl_bounding_box<double,d> const& block_bb, hsds_fd_tree_node_index<d> const& cell_index)
{
  vnl_vector_fixed<double,d> cell_origin(block_bb.min());
  vnl_vector_fixed<double,d> cell_len = vnl_vector_fixed<double,d>(block_bb.max()) - cell_origin;

  const unsigned int top_bit = 1 << (sizeof(unsigned int)*8 - 1);
  unsigned int idx = cell_index.idx;

  for (unsigned int lvl = 1; lvl <= cell_index.lvl; ++lvl) {
    cell_len *= 0.5;
    double *origin_ptr = cell_origin.data_block() + (d-1), *len_ptr = cell_len.data_block() + (d - 1);
    for (unsigned int dim=0; dim < d; ++dim,  --origin_ptr, --len_ptr) {
      if (idx & top_bit) {
        *origin_ptr += *len_ptr;
      }
      // shift top bit to next dimension/level
      idx = idx << 1;
    }
  }
  vbl_bounding_box<double,d> cell_bb;
  cell_bb.update(cell_origin.data_block());
  cell_bb.update((cell_origin + cell_len).data_block());

  return cell_bb;

  
}

//: fill the tree with the specified data
template<class T, unsigned d>
void hsds_fd_tree<T,d>::fill(T const& data)
{
  typename vcl_map<hsds_fd_tree_node_index<d>,T >::iterator it = nodes_.begin();
  for (; it != nodes_.end(); ++it) {
    it->second = data;
  }
  return;
}

//: return the node index which contains the point pt
template<class T, unsigned d>
hsds_fd_tree_node_index<d>  hsds_fd_tree<T,d>::index_at(vnl_vector<double> const& pt)
{

  vnl_vector_fixed<double,d> bbox_min(bbox_.min());
  vnl_vector_fixed<double,d> bbox_max(bbox_.max());

  vnl_vector_fixed<double,d> rel_pt = element_quotient((pt - bbox_min),(bbox_max - bbox_min));
  //if ( (rel_pt.min_value() < 0.0) || (rel_pt.max_value() > 1.0) ) {
    //vcl_cerr << "error: point " << pt << " is not in bounding box of tree!";
    // what to do here? just return dummy index for now.
    //return hsds_fd_tree_node_index<d>(0,0);
  //}

  // initialize bitloc as the most significant bit.
  unsigned int node_code = 0;

  for (unsigned level=0; level<=max_levels_; ++level) {
    // check for existance of node at desired level
    hsds_fd_tree_node_index<d> node_index(node_code,level);
    typename vcl_map<hsds_fd_tree_node_index<d>, T >::const_iterator nit(nodes_.find(node_index));
    if (nit == nodes_.end()) {
      vcl_cerr << "error: desired node not in tree." << vcl_endl;
      assert(false);
      return hsds_fd_tree_node_index<d>(0,0);
    }
    if ((*nit).first.lvl == level)  {
      // node with desired index found
      return node_index;
    }
    // else drop down one level
    for(unsigned dim=0; dim<d; ++dim) {
      if (rel_pt[dim] > 0.5) {
        node_code |= hsds_fd_tree_node_index<d>::bit_mask(level+1,dim);
        rel_pt[dim] -= 0.5;
      }
      rel_pt[dim] *= 2.0;
    }
  }
  // this should not be reached
  vcl_cerr << "error: node not found in tree: should not happen" << vcl_endl;
  assert(false);

  // return root node index as dummy
  return hsds_fd_tree_node_index<d>(0,0);
}

//: return full node code corresponding to pt, regardless of level at which pt is actually stored
template<class T, unsigned d>
hsds_fd_tree_node_index<d> hsds_fd_tree<T,d>::full_index_at(vnl_vector<double> const& pt)
{
  vnl_vector_fixed<double,d> bbox_min(bbox_.min());
  vnl_vector_fixed<double,d> bbox_max(bbox_.max());

  vnl_vector_fixed<double,d> rel_pt = element_quotient((pt - bbox_min),(bbox_max - bbox_min));
 
  //if ( (rel_pt.min_value() < 0.0) || (rel_pt.max_value() > 1.0) ) {
  //  vcl_cerr << "error: point " << pt << " is not in bounding box of tree!";
    // what to do here? just return dummy index for now.
  //  return hsds_fd_tree_node_index<d>(0,0);
  //}

  // initialize bitloc as the most significant bit.
  unsigned int node_code = 0;

  for (unsigned level=1; level<=max_levels_; ++level) {
    // drop down one level
    for(unsigned dim=0; dim<d; ++dim) {
      if (rel_pt[dim] > 0.5) {
        node_code |= hsds_fd_tree_node_index<d>::bit_mask(level,dim);
        rel_pt[dim] -= 0.5;
      }
      rel_pt[dim] *= 2.0;
    }
  }
  return hsds_fd_tree_node_index<d>(node_code,max_levels_);
}

//: return full node code corresponding to pt, regardless of level at which pt is actually stored
template<class T, unsigned d>
hsds_fd_tree_node_index<d> hsds_fd_tree<T,d>::full_index_at(hsds_fd_tree_node_index<d> start_index, vnl_vector<double> const& pt)
{
  vnl_vector_fixed<double,d> bbox_min(bbox_.min());
  vnl_vector_fixed<double,d> bbox_max(bbox_.max());

  // initialize bitloc as the most significant bit.
  unsigned int node_code = start_index.idx;

  vnl_vector_fixed<double,d> rel_pt = element_quotient((pt - bbox_min),(bbox_max - bbox_min));
  for (unsigned int level = 1; level <= start_index.lvl; ++level) {
    // drop down one level
    for (unsigned int dim=0; dim<d; ++dim) {
      if ( node_code & hsds_fd_tree_node_index<d>::bit_mask(level,dim) ) {
        rel_pt[dim] -= 0.5;
      }
      rel_pt[dim] *= 2.0;
    }
  }

  for (unsigned int level=start_index.lvl+1; level<=max_levels_; ++level) {
    // drop down one level
    for(unsigned int dim=0; dim<d; ++dim) {
      if (rel_pt[dim] > 0.5) {
        node_code |= hsds_fd_tree_node_index<d>::bit_mask(level,dim);
        rel_pt[dim] -= 0.5;
      }
      rel_pt[dim] *= 2.0;
    }
  }
  return hsds_fd_tree_node_index<d>(node_code,max_levels_);
}

//: return the neighboring cell of index
//: returned cell will be at level of index or lower
template<class T, unsigned d>
bool hsds_fd_tree<T,d>::neighbor_cell(hsds_fd_tree_node_index<d> index, unsigned int dim, bool positive_neighbor, hsds_fd_tree_node_index<d> &neighbor) const
{
  // step to next index at same level
  unsigned int lvl_coord = index.get_node_coordinate(dim);
  if (positive_neighbor) {
    // step in the positive direction
    if (lvl_coord == (1 << index.lvl) - 1) {
      // we are at the edge of the block - no neighbor in this direction
      return false;
    }
    ++lvl_coord;
  } else {
    // step in the negative direction
    if (lvl_coord == 0) {
      // we are at the edge of the block - no neighbor in this direction
      return false;

    }
    --lvl_coord;
  }
  hsds_fd_tree_node_index<d> step_index = index;
  step_index.set_node_coordinate(dim,lvl_coord);
  // now search for closest index less than or equal to step_index
  typename vcl_map<hsds_fd_tree_node_index<d>,T >::const_iterator nit = nodes_.upper_bound(step_index);
  // nit now points to an index equal to or greater than step_index
  if ( (nit == nodes_.end()) || ( (*nit).first != step_index) ) {
    // node not found. this means that the neighboring cell is lower depth than step_index
    --nit;
    // nit now points to the index closest to step_index without going over.
  }
  neighbor = nit->first;

  return true;
}

 //: return a set of cells bordering on the specified face of another cell
template<class T, unsigned d>
bool hsds_fd_tree<T,d>::neighbor_cells(hsds_fd_tree_node_index<d> index, unsigned int dim, bool positive_neighbor, vcl_vector<hsds_fd_tree_node_index<d> > &neighbors) const
{
  neighbors.clear();
    // step to next index at same level
  unsigned int lvl_coord = index.get_node_coordinate(dim);
  if (positive_neighbor) {
    // step in the positive direction
    if (lvl_coord == (1 << index.lvl) - 1) {
      // we are at the edge of the block - no neighbor in this direction
      return false;
    }
    ++lvl_coord;
  } else {
    // step in the negative direction
    if (lvl_coord == 0) {
      // we are at the edge of the block - no neighbor in this direction
      return false;

    }
    --lvl_coord;
  }
  hsds_fd_tree_node_index<d> neighbor_index = index;
  neighbor_index.set_node_coordinate(dim,lvl_coord);
  
  // now search for closest index less than or equal to step_index
  typename vcl_map<hsds_fd_tree_node_index<d>,T >::const_iterator nit = nodes_.upper_bound(neighbor_index);
  // nit now points to an index equal to or greater than step_index
  if ( (nit == nodes_.end()) || ( (*nit).first != neighbor_index) ) {
    // node not found. this means that the neighboring cell is shallower depth than step_index
    --nit;
    // nit now points to the index closest to step_index without going over.
    //neighbors.push_back(nit->first);
    // there can only be one neighboring cell if the neighbor is at shallower or equal depth
    if (nit->first.lvl <= neighbor_index.lvl) {
      neighbors.push_back(nit->first);
      return true;
    }
  }
  // create masks indicating relevant bits for neighboring nodes adjacent to desired face
  unsigned int level_mask = 0;
  level_mask = hsds_fd_tree_node_index<d>::full_mask(neighbor_index.lvl);

  unsigned int neighbor_mask = 0;
  // add bits for dimension of face we are interested in
  for (unsigned int l = neighbor_index.lvl+1; l <= max_levels_; ++l) {
    neighbor_mask |= hsds_fd_tree_node_index<d>::bit_mask(l,dim);
  }
  unsigned int neighbor_val = 0;
  // set bits indicating cells on appropriate face of neighbor
  if (!positive_neighbor) {
    // we want positive-most cells for a negative neighbor
    neighbor_val |= neighbor_mask;
  }
  // iterate through cells, keeping all touching desired face 
  for (; nit != nodes_.end() && ((nit->first.idx & level_mask ) == neighbor_index.idx); ++nit) {
    unsigned int node_level_mask = hsds_fd_tree_node_index<d>::full_mask(nit->first.lvl);
    if ( (nit->first.idx & neighbor_mask) == (neighbor_val & node_level_mask) ) {
      neighbors.push_back(nit->first);
    } 
  }
  if (neighbors.size() == 0) {
    vcl_cout << " neighbor_cells() : error! did not find any neighbors " << vcl_endl;
  }
  return true;
}


template<class T, unsigned d>
void hsds_fd_tree<T,d>::merge(hsds_fd_tree_node_index<d> index, T const& new_data)
{
  // check for degenerate case of total tree collapse (bit shifts of sizeof(int) dont seem to work)
  if (index.lvl == 0) {
    nodes_.clear();
    nodes_.insert(vcl_make_pair<hsds_fd_tree_node_index<d>,T >(index, new_data));
    return;
  }

  // retrieve first node 
  typename vcl_map<hsds_fd_tree_node_index<d>,T >::iterator nit = nodes_.find(index);
  if ( (nit == nodes_.end()) || ((*nit).first.lvl <= index.lvl) ) {
    vcl_cerr << "error: tree does not contain any ancestors of " << index << vcl_endl;
    return;
  }

  // zero out old level to get new index
  //unsigned int new_idx = index.idx & ~hsds_fd_tree_node_index<d>::level_mask(index.lvl);

  // all ancestors must have same idx modulo least significant nshift bits.
  unsigned int nshift = hsds_fd_tree_node_index<d>::level_shift(index.lvl);
  unsigned int shifted_idx = index.idx >> nshift;

  // remove all ancestors of node
  typename vcl_map<hsds_fd_tree_node_index<d>,T >::iterator first_nit = nit;
  while ( (nit != nodes_.end()) && ((*nit).first.idx >> nshift == shifted_idx) ) {
    ++nit;
    //vcl_cout << "non-shifted idx = 0x" << vcl_hex << (*nit).first.idx << vcl_endl;
    //vcl_cout << "idx shifted by " << vcl_dec << nshift << "= 0x" << vcl_hex << ((*nit).first.idx >> nshift) << vcl_endl;
  }
  nodes_.erase(first_nit, nit);

  // insert new node in tree
  nodes_.insert(vcl_make_pair<hsds_fd_tree_node_index<d>,T >(index, new_data));
  
  return;
}

template<class T, unsigned d>
void hsds_fd_tree<T,d>::split(hsds_fd_tree_node_index<d> index, T const& new_data)
{
  // retrieve old node 
  typename vcl_map<hsds_fd_tree_node_index<d>,T >::iterator nit = nodes_.find(index);
  if (nit == nodes_.end()) {
    vcl_cerr << "error: tree does not contain node with index " << index << vcl_endl;
    return;
  }
  if ((*nit).first.lvl != index.lvl) {
    vcl_cerr << "error: tree node with index " << (*nit).first << " does not match levels with index " << index << vcl_endl;
    return;
  }
  // remove old node from tree
  //T old_data = (*nit).second;
  unsigned char new_lvl = index.lvl + 1;
  if (new_lvl > this->max_levels_) {
    //vcl_cerr << "warning: cannot split node: max level (" << this->max_levels_ << ") reached." << vcl_endl;
    return;
  }

  nodes_.erase(nit);

  // create 2^d new nodes at next level, each with same value as old node.
  unsigned int nnodes = 1 << d;
  for (unsigned int i=0; i<nnodes; ++i) {
    unsigned int new_idx = index.idx + (i << hsds_fd_tree_node_index<d>::level_shift(new_lvl));
    hsds_fd_tree_node_index<d> new_index(new_idx,new_lvl);
    nodes_.insert(vcl_make_pair<hsds_fd_tree_node_index<d>,T >(new_index, new_data));
  }

  return;
}

// IO functions
//: write index to stream (for debugging)
template <unsigned d>
vcl_ostream&  operator<<(vcl_ostream& s, hsds_fd_tree_node_index<d> const& index)
{
  s << "0x" << vcl_setw(sizeof(index.idx)*2) << vcl_setfill('0') << vcl_hex << index.idx << ", " << vcl_dec << (int)index.lvl;
  return s;
}


template <class T, unsigned d>
void hsds_fd_tree<T,d>::b_read(vcl_istream &is)
{
  // check stream
  if (!is.good()) {
    vcl_cerr << "error: bad stream passed to b_read " << vcl_endl;
    return;
  }
  hsds_fd_tree_header<T,d> header;
  is.read(reinterpret_cast<char*>(&header),sizeof(header));
  // check that header matches what we are expecting
  if (header.dimension != d) {
    vcl_cerr << "error: dimension in header " << header.dimension << " does not match expected value " << d << vcl_endl;
    return;
  }
  if (header.data_size != sizeof(T)) {
    vcl_cerr << "error: data size in header " << header.data_size << " does not match expected value " << sizeof(T) << vcl_endl;
    return;
  }
  max_levels_ = header.max_levels;
  bbox_.reset();
  bbox_.update(header.corner_low.data_block());
  bbox_.update(header.corner_high.data_block());
  // read nodes
  nodes_.clear();
  // allocate block for reading in tree structure
  unsigned int idx_block_size = sizeof(hsds_fd_tree_node_index<d>)*header.nnodes;
  hsds_fd_tree_node_index<d>* indices = new hsds_fd_tree_node_index<d>[header.nnodes];
  is.read(reinterpret_cast<char*>(indices),idx_block_size);

  unsigned int ndata_block = idx_block_size / sizeof(T);
  if (ndata_block < 1)
    ndata_block = 1;


  T* data = new T[ndata_block];
  unsigned int data_read = 0;
  
  while (data_read < header.nnodes) {
    unsigned int data_remaining = header.nnodes - data_read;
    unsigned int ndata_to_read = ndata_block;
    if (ndata_to_read > data_remaining) {
      ndata_to_read = data_remaining;
    }
    is.read(reinterpret_cast<char*>(data),sizeof(T)*ndata_to_read);
    for (unsigned int i=0; i < ndata_to_read; ++i) {
      nodes_.insert(vcl_make_pair<hsds_fd_tree_node_index<d>,T >(indices[data_read + i],data[i]));
    }
    data_read += ndata_to_read;
  }
  delete[] indices;
  delete[] data;
}


template <class T, unsigned d>
void hsds_fd_tree<T,d>::b_write(vcl_ostream &os) const
{
  // check stream
  if (!os.good()) {
    vcl_cerr << "error: bad stream passed to b_write " << vcl_endl;
    return;
  }
  hsds_fd_tree_header<T,d> header(bbox_, nodes_.size(), max_levels_);
  os.write(reinterpret_cast<char*>(&header),sizeof(header));
  //vcl_cout << "sizeof(header) = " << sizeof(header) << vcl_endl;
  //vcl_cout << "data_size = " << header.data_size << vcl_endl;
  //vcl_cout << "nnodes = " << header.nnodes << vcl_endl;
  // allocate block for writing tree structure
  hsds_fd_tree_node_index<d>* indices = new hsds_fd_tree_node_index<d>[nodes_.size()];
  // fill in block
  typename hsds_fd_tree<T,d>::const_iterator tree_it = this->begin();
  hsds_fd_tree_node_index<d>* index = indices;
  for (; tree_it != this->end(); ++tree_it, ++index) {
    *index = tree_it->first;
  }
  // write tree structure
  unsigned int idx_block_size = sizeof(hsds_fd_tree_node_index<d>)*nodes_.size();
  //vcl_cout << "sizeof(node_index) = " << sizeof(hsds_fd_tree_node_index<d>) << vcl_endl;
  os.write(reinterpret_cast<char*>(indices),idx_block_size);
  delete[] indices;

  // allocate block for writing data
  unsigned int ndata_block = idx_block_size / sizeof(T);
  if (ndata_block < 1)
    ndata_block = 1;

  T* data = new T[ndata_block];
  unsigned int data_written = 0;
  
  tree_it = this->begin();
  while (data_written < nodes_.size()) {
    unsigned int data_remaining = nodes_.size() - data_written;
    unsigned int ndata_to_write = ndata_block;

    if (ndata_to_write > data_remaining) {
      ndata_to_write = data_remaining;
    }
    T* data_ptr = data;
    for(unsigned int n=0; n<ndata_to_write; ++n, ++tree_it, ++data_ptr) {
      *data_ptr = tree_it->second;
    }
    os.write(reinterpret_cast<char*>(data),sizeof(T)*ndata_to_write);
    data_written += ndata_to_write;
  }
  delete[] data;
}



#define HSDS_FD_TREE_INSTANTIATE(T,d) \
template class hsds_fd_tree<T,d>; \
template vcl_ostream& operator<<(vcl_ostream&, const hsds_fd_tree_node_index<d>&) \


#endif
