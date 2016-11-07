#ifndef psm_cell_visibility_iterator_h_
#define psm_cell_visibility_iterator_h_

#include <vcl_vector.h>
#include <vcl_set.h>

#include <vbl/vbl_bounding_box.h>

#include <vpgl/vpgl_perspective_camera.h>

#include <hsds/hsds_fd_tree.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_sample.h>

//: nodes in the visibility graph
class psm_cell_vis_graph_node 
{
public:
  psm_cell_vis_graph_node() : incoming_count(0), visible(true) {}
  unsigned int incoming_count;
  bool visible;
  vcl_vector<hsds_fd_tree<psm_cell_vis_graph_node,3>::iterator> outgoing_links;
};


template<psm_apm_type APM>
class psm_cell_visibility_iterator
{
public:
  psm_cell_visibility_iterator(hsds_fd_tree<psm_sample<APM>,3> const& block, vpgl_perspective_camera<double> const& cam, unsigned int img_ni, unsigned int img_nj, bool full_vis_needed = false, bool reverse_iterator = false); 

  bool next();

  void current_cells(vcl_vector<hsds_fd_tree_node_index<3> > &cells);

private:
  hsds_fd_tree<psm_sample<APM>,3> const& block_;
  vpgl_perspective_camera<double> const& cam_;

  vcl_vector<hsds_fd_tree<psm_cell_vis_graph_node,3>::iterator > curr_cells_;
  hsds_fd_tree<psm_cell_vis_graph_node,3> vis_graph_;

  vcl_vector<hsds_fd_tree_node_index<3> > to_process_indices_;

  int cell_count_;
  unsigned int img_ni_;
  unsigned int img_nj_;

  bool reverse_it_;
  bool full_vis_needed_;

};

#endif

