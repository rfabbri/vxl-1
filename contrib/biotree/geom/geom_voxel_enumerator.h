#if !defined(GEOM_VOXEL_ENUMERATOR_H_)
#define GEOM_VOXEL_ENUMERATOR_H_

#include <biob/biob_enumerator.h>

struct geom_voxel_specifier {
  unsigned long x_index_, y_index_,z_index_;
  geom_voxel_specifier(unsigned long x_index, unsigned long y_index, unsigned long z_index)
    : x_index_(x_index), y_index_(y_index), z_index_(z_index) {}
};

typedef biob_enumerator<geom_voxel_specifier> geom_voxel_enumerator;

#endif
