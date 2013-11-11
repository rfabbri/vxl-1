#ifndef hsds_fd_tree_incremental_reader_h_
#define hsds_fd_tree_incremental_reader_h_

#include <vcl_string.h>

#include <vcl_ios.h>
#include <vcl_fstream.h>

#include <vbl/vbl_bounding_box.h>

#include "hsds_fd_tree_node_index.h"
#include "hsds_fd_tree.h"

template<class T, unsigned d>
class hsds_fd_tree_incremental_reader
{
public:
  hsds_fd_tree_incremental_reader(){};

  void init(vcl_string filename);

  ~hsds_fd_tree_incremental_reader();

  bool next(hsds_fd_tree_node_index<d> &idx, T &data);
 
  unsigned int nnodes(){return nnodes_;}
  unsigned int max_levels(){return max_levels_;}
  vbl_bounding_box<double,d> bbox(){return bbox_;}

protected:

   // copy constructor
  hsds_fd_tree_incremental_reader(hsds_fd_tree_incremental_reader<T,d> const& that);

  unsigned int nnodes_;
  vcl_ifstream ifs_idx_;
  vcl_ifstream ifs_data_;

  unsigned int max_levels_;
  vbl_bounding_box<double,d>  bbox_;

};


#endif

