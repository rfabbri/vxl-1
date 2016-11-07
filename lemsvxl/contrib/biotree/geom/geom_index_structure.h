#if !defined(GEOM_INDEX_STRUCTURE_H_)
#define GEOM_INDEX_STRUCTURE_H_

#include <biob/biob_grid_worldpt_roster.h>
#include "geom_voxel_enumerator_sptr.h"
#include "geom_probe_volume_sptr.h"
#include <biob/biob_worldpt_index_enumerator_sptr.h>
#include <biob/biob_worldpt_roster_sptr.h>

class geom_index_structure {
 private:
  vgl_box_3d<double> grid_box_;
  double resolution_;//spacing of the grid
  biob_grid_worldpt_roster grid_;
  biob_worldpt_roster_sptr roster_;
  typedef vcl_list<biob_worldpt_index> voxel_inhabitants_t;
  typedef vcl_vector<voxel_inhabitants_t> voxel_to_inhabitants_t;
  voxel_to_inhabitants_t voxel_to_inhabitants_;
  class enumerator : public biob_enumerator<biob_worldpt_index> {
  private:
    biob_worldpt_index next_to_return_;
    bool has_next_;
    geom_voxel_enumerator_sptr voxel_enumerator_;
    biob_worldpt_index grid_index_;
    voxel_inhabitants_t::const_iterator inhabitants_iterator_;
    biob_worldpt_roster_sptr roster_;
    const biob_grid_worldpt_roster & grid_;
    const voxel_to_inhabitants_t & voxel_to_inhabitants_;
    const geom_probe_volume_sptr probe_volume_;
    bool done_with_this_voxel();
    void advance_point();
    void advance_voxel();
  public:
    bool has_next();
    biob_worldpt_index next();
    enumerator(biob_worldpt_roster_sptr roster,
               const biob_grid_worldpt_roster & grid,
               const voxel_to_inhabitants_t& voxel_to_inhabitants,
               geom_probe_volume_sptr probe_volume,
               geom_voxel_enumerator_sptr voxel_enumerator);               
};

 public:
  geom_index_structure(const biob_worldpt_roster_sptr roster, double resolution);
  biob_worldpt_index_enumerator_sptr  enclosed_by(geom_probe_volume_sptr volume);
};

#endif
