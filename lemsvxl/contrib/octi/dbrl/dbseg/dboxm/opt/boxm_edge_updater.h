#ifndef boxm_edge_updater_h_
#define boxm_edge_updater_h_

#include <vector>
#include <string>

#include "boxm_aux_traits.h"
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene.h>

#include <boxm/boxm_edge_sample.h>

template<class T_loc, class APM, class AUX>
class boxm_edge_updater
{
public:

  boxm_edge_updater(boxm_scene<boct_tree<T_loc, boxm_edge_sample<APM> > > &scene, 
                        std::vector<std::string> const& image_ids);

  ~boxm_edge_updater(){}

  bool add_cells();

protected:

  std::vector<std::string> image_ids_;

  boxm_scene<boct_tree<T_loc, boxm_edge_sample<APM> > > &scene_;

  const float max_cell_P_;
  const float min_cell_P_;

};















#endif
