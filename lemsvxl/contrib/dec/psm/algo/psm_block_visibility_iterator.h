#ifndef psm_block_visibility_iterator_h_
#define psm_block_visibility_iterator_h_

#include <vcl_vector.h>
#include <vcl_map.h>

#include <vbl/vbl_bounding_box.h>

#include <vpgl/vpgl_perspective_camera.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_scene.h>


class psm_block_vis_graph_node;
typedef vcl_map<vgl_point_3d<int>, psm_block_vis_graph_node, vgl_point_3d_cmp<int> >  psm_block_vis_graph_type;

//: nodes in the visibility graph
class psm_block_vis_graph_node 
{
public:
  psm_block_vis_graph_node() : incoming_count(0) {}

  unsigned int incoming_count;
  vcl_vector<psm_block_vis_graph_type::iterator> outgoing_links;
};


template<psm_apm_type APM>
class psm_block_visibility_iterator
{
public:
  psm_block_visibility_iterator(psm_scene<APM> &scene, vpgl_camera<double> const* cam, unsigned int i0, unsigned int j0, unsigned int cam_ni, unsigned int cam_nj, bool reverse_iterator = false); 

  bool next();

  void current_blocks(vcl_vector<vgl_point_3d<int> > &blocks);

private:
  psm_scene<APM> &scene_;
  vpgl_camera<double> const* cam_;


  vcl_vector<psm_block_vis_graph_type::iterator> curr_blocks_;
  psm_block_vis_graph_type vis_graph_;

  vcl_vector<vgl_point_3d<int> > to_process_indices_;

  unsigned int img_ni_;
  unsigned int img_nj_;

  bool reverse_it_;

};

#endif

