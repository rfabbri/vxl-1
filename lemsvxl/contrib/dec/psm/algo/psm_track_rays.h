#ifndef psm_track_rays_h_
#define psm_track_rays_h_

#include <vcl_vector.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_image_sample.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>

#include <hsds/hsds_fd_tree.h>

#include "psm_raytrace_function.h"



template <class S>
void psm_track_rays(psm_scene<S> &scene, vpgl_perspective_camera<double> const& cam, 
                    unsigned int start_i, unsigned int ni, unsigned int start_j, unsigned int nj, vcl_string fname)
{
  vcl_ofstream os(fname.c_str(), vcl_ios::binary);
  if (!os.good()) {
    vcl_cerr << "error opening " << fname << " for write! " << vcl_endl;
  }

  vcl_map<hsds_fd_tree_node_index<3>,unsigned int> index_map;
  hsds_fd_tree<S,3> &block = scene.get_block(vgl_point_3d<int>(0,0,0));
  hsds_fd_tree<S,3>::iterator cell_it = block.begin();
  for (unsigned int c=0; cell_it != block.end(); ++cell_it, ++c) {
    index_map.insert(vcl_make_pair<hsds_fd_tree_node_index<3>,unsigned int>(cell_it->first,c));
  }

  psm_scene<psm_null_aux_sample> aux_scene;
  psm_raytrace_function<psm_track_rays_functor<S>, S, psm_null_aux_sample> raytrace_fn(scene, cam, aux_scene);

  vcl_cout << "ni = " << ni << " nj = " << nj << " sizeof(unsigned int) = " << sizeof(unsigned int) << vcl_endl;
  os.write((char*)&ni,sizeof(unsigned int));
  os.write((char*)&nj,sizeof(unsigned int));


  for (unsigned int j=0; j<nj; ++j) {
    for (unsigned int i=0; i<ni; ++i) {
      psm_track_rays_functor<S> functor;
      raytrace_fn.run(i+start_i, j+start_j,functor);
      vcl_vector<hsds_fd_tree_node_index<3> > cells = functor.cells();
      vcl_vector<float> lengths = functor.lengths();
      unsigned int ncells = cells.size();
      vcl_cout << ncells << " ";
      os.write((char*)&ncells,sizeof(unsigned int));
      for (unsigned c=0; c<ncells; ++c) {
        unsigned int cell_idx = index_map[cells[c]];
        float len = lengths[c];
        os.write((char*)&cell_idx,sizeof(unsigned int));
        os.write((char*)&len,sizeof(unsigned int));
      }
    }
  }

  return; 
}

template <class S>
class psm_track_rays_functor
{
public:

  //: default constructor
  psm_track_rays_functor() {}

  //: accumulate 
  inline bool step_cell(vgl_point_3d<double> s0, vgl_point_3d<double> s1, S &cell_value, psm_null_aux_sample &aux_value, hsds_fd_tree_node_index<3> cell_index)
  {
    float cell_len = (float)(s1 - s0).length();
    cells_.push_back(cell_index);
    lens_.push_back(cell_len);

    return true;
  }

  vcl_vector<hsds_fd_tree_node_index<3> > cells() {return cells_;}
  vcl_vector<float> lengths() {return lens_;}

private:

  vcl_vector<hsds_fd_tree_node_index<3> > cells_;
  vcl_vector<float> lens_;
};





#endif

