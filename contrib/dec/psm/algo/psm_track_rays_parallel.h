#ifndef psm_track_rays_parallel_h_
#define psm_track_rays_parallel_h_


#include <vcl_vector.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_image_sample.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>

#include <hsds/hsds_fd_tree.h>

#include "psm_parallel_raytrace_function.h"



template <psm_apm_type APM>
void psm_track_rays_parallel(psm_scene<APM> &scene, vpgl_perspective_camera<double> const& cam, unsigned int ni, unsigned int nj, vcl_string fname)
{
  vcl_ofstream os(fname.c_str());
  if (!os.good()) {
    vcl_cerr << "error opening " << fname << " for write! " << vcl_endl;
  }
  
  psm_parallel_raytrace_function<psm_track_rays_parallel_functor<psm_sample<APM> >, APM > raytrace_fn(scene, cam, ni, nj);

  psm_track_rays_parallel_functor<APM> functor(os);
  raytrace_fn.run(functor);




  return; 
}

template <class S>
class psm_track_rays_parallel_functor
{
public:

  //: default constructor
  psm_track_rays_parallel_functor(vcl_ofstream &os) : os_(os) {}

  //: accumulate 
  inline bool step_cells(hsds_fd_tree<psm_sample<APM>,3> &block, hsds_fd_tree<psm_aux_traits<PSM_AUX_NULL>::sample_datatype,3> &aux_block, vcl_vector<hsds_fd_tree_node_index<3> > &cells)
  {
    vcl_cout << ".";
    os_ << cells.size() << vcl_endl;
    for (unsigned i=0; i < cells.size(); ++i) {
      os_ << cells[i].idx << " ";
    }
    os_ << vcl_endl;
   
    return true;
  }


private:
  vcl_ofstream& os_;

};








#endif
