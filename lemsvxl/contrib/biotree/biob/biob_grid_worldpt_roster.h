#if !defined(BIOB_GRID_WORLDPT_ROSTER_H_)
#define BIOB_GRID_WORLDPT_ROSTER_H_

#include "biob_worldpt_roster.h"
#include "biob_worldpt_box.h"
#include <vcl_list.h>

//:
// \file
// \brief a worldpt_roster in which the points represented are grid-points in a box
// \author P. N. Klein
//
// \verbatim
//    K.  Kang  April 1st, 2005
//    An convertion function of 1D to 3D position index is added
//    An point index by 3d grid is added
//
// \endverbatim


class biob_grid_worldpt_roster : public biob_worldpt_roster {
  private:
    biob_worldpt_box bounding_box_;
    double spacing_;
    unsigned long int num_points_, num_points_per_z_, num_points_per_y_;

    //: number of points per axis
    unsigned long num_points_x_;
    unsigned long num_points_y_;
    unsigned long num_points_z_;
 
    //: starting point
    double x0_, y0_, z0_;

  public:
    //: points are those inside the box, spaced at 'spacing' in all three dimensions
    biob_grid_worldpt_roster(biob_worldpt_box bounding_box, double spacing);
  
    biob_grid_worldpt_roster(unsigned long num_points_x,
      unsigned long num_points_y,
      unsigned long num_points_z,
      double x0, double y0, double z0);

    vcl_string class_id() { return "biob_grid_worldpt_roster"; }

    //number of worldpts stored
    unsigned long int num_points() const;
    
    //: derive the point, given the index
    worldpt point(biob_worldpt_index pti) const;

    //: grid index
    worldpt grid_3d(unsigned i, unsigned j, unsigned k) const;

    //: size of each dimension
    unsigned const nx() { return num_points_x_; }
    unsigned const ny() { return num_points_y_; }
    unsigned const nz() { return num_points_z_; }

    biob_worldpt_box bounding_box() { return bounding_box_; }
    double spacing() { return spacing_; }

    //:  3d index to 1d index convertion
    biob_worldpt_index index_3d_2_1d(unsigned i, unsigned j, unsigned k) const;
    bool conditional_index_3d_2_1d(long i, long j, long k, biob_worldpt_index & pti) const;

    //:return the list consisting of given index and indices of neighboring voxels
    vcl_list<biob_worldpt_index> neighboring_voxels(biob_worldpt_index pti) const;

    //: if given worldpt is within a voxel, returns true and sets  the index of the containing voxel (else returns false)
    bool voxel(worldpt pt, biob_worldpt_index & pti) const;

    void x_write_this(vcl_ostream& os);
};

#endif
