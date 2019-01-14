#ifndef hsds_fd_tree_incremental_reader_txx_
#define hsds_fd_tree_incremental_reader_txx_

#include <string>

#include <fstream>
#include <ios>

#include <vbl/vbl_bounding_box.h>

#include "hsds_fd_tree_node_index.h"
#include "hsds_fd_tree.h"

#include "hsds_fd_tree_incremental_reader.h"


template<class T, unsigned d>
void hsds_fd_tree_incremental_reader<T,d>::init(std::string filename) 
{
  ifs_idx_.open(filename.c_str(), std::ios::binary);
  ifs_data_.open(filename.c_str(), std::ios::binary);

  if (!ifs_idx_.good()) {
    std::cerr << "error initializing incremental read! (idx stream) filename = " << filename << std::endl;
    return;
  }
  if (!ifs_data_.good()) {
    std::cerr << "error initializing incremental read! (data stream) filename = " << filename << std::endl;
    return;
  }
  hsds_fd_tree_header<T,d> header;
  ifs_idx_.read(reinterpret_cast<char*>(&header),sizeof(header));
  // check that header matches what we are expecting
  if (header.dimension != d) {
    std::cerr << "error: dimension in header " << header.dimension << " does not match expected value " << d << std::endl;
    return;
  }
  if (header.data_size != sizeof(T)) {
    std::cerr << "error: data size in header " << header.data_size << " does not match expected value " << sizeof(T) << std::endl;
    return;
  }
  ifs_data_.seekg(sizeof(header) + sizeof(hsds_fd_tree_node_index<3>)*header.nnodes);

  max_levels_ = header.max_levels;
  bbox_.reset();
  bbox_.update(header.corner_low.data_block());
  bbox_.update(header.corner_high.data_block());
}

template<class T, unsigned d>
hsds_fd_tree_incremental_reader<T,d>::~hsds_fd_tree_incremental_reader()
{
}

// copy constructor
template<class T, unsigned d>
hsds_fd_tree_incremental_reader<T,d>::hsds_fd_tree_incremental_reader(hsds_fd_tree_incremental_reader<T,d> const& that)
: ifs_idx_(), ifs_data_()
{}


template<class T, unsigned d>
bool hsds_fd_tree_incremental_reader<T,d>::next(hsds_fd_tree_node_index<d> &idx, T &data)
{
  if (!ifs_idx_.good()) {
    std::cerr << "error: hsds_fd_tree_incremental_reader::next : ifx_idx_.good() returned false." << std::endl;
    return false;
  }
  if (!ifs_data_.good()) {
    std::cerr << "error: hsds_fd_tree_incremental_reader::next : ifx_data_.good() returned false." << std::endl;
    return false;
  }
  ifs_idx_.read(reinterpret_cast<char*>(&idx),sizeof(hsds_fd_tree_node_index<d>));
  ifs_data_.read(reinterpret_cast<char*>(&data),sizeof(T));

  return true;
}


#define HSDS_FD_TREE_INCREMENTAL_READER_INSTANTIATE(T,d) \
  template class hsds_fd_tree_incremental_reader<T,d>


#endif

