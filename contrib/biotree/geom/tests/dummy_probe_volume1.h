#if !defined(GEOM_TEST_DUMMY_PROBE_VOLUME1_H_)
#define GEOM_TEST_DUMMY_PROBE_VOLUME1_H_

#include <geom/geom_voxel_enumerator.h>
#include <geom/geom_probe_volume.h>

class dummy_probe_volume1 : public geom_probe_volume
 {
 private:
   class enumerator : public geom_voxel_enumerator {
   private:
     int which_voxel_;
   public:
     enumerator(){which_voxel_ = 0;}
     bool has_next(){return which_voxel_ < 27;}
     geom_voxel_specifier next(){
       unsigned x = which_voxel_ % 3;
       unsigned y = (which_voxel_/3) % 3;
       unsigned z = which_voxel_ / 9;
       ++ which_voxel_;
       return geom_voxel_specifier(x, y, z);
     }
   };
 public:
   geom_voxel_enumerator_sptr grid_cells(const vgl_box_3d<double> & grid_box, double spacing){
     return new enumerator();
   }

  bool contains(worldpt pt){return true;}
 };
#endif
