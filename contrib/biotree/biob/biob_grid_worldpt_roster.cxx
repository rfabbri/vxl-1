#include "biob_grid_worldpt_roster.h"
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_box_3d.h>

biob_grid_worldpt_roster::biob_grid_worldpt_roster(biob_worldpt_box box, double spacing)
   : bounding_box_(box), spacing_(spacing)
{
  num_points_x_ = static_cast<unsigned long>((box.max_x() - box.min_x()) / spacing_ + 1);
  num_points_y_ = static_cast<unsigned long>((box.max_y() - box.min_y()) / spacing_ + 1);
  num_points_z_ = static_cast<unsigned long>((box.max_z() - box.min_z()) / spacing_ + 1);
  num_points_ = num_points_x_ * num_points_y_ * num_points_z_;

  num_points_per_z_ = num_points_y_ * num_points_x_;
  num_points_per_y_ = num_points_x_;

  x0_ = box.min_x();
  y0_ = box.min_y();
  z0_ = box.min_z();
}

biob_grid_worldpt_roster::biob_grid_worldpt_roster(unsigned long num_points_x,
    unsigned long num_points_y,
    unsigned long num_points_z,
    double x0, double y0, double z0)
    :spacing_(1),num_points_x_(num_points_x), num_points_y_(num_points_y), num_points_z_(num_points_z),
    x0_(x0), y0_(y0), z0_(z0)
{
  num_points_ = num_points_x_ * num_points_y_ * num_points_z_;
  num_points_per_z_ = num_points_y_ * num_points_x_;
  num_points_per_y_ = num_points_x_;
  bounding_box_.set_min_point(vgl_point_3d<double> (x0, y0, z0));
  bounding_box_.set_max_point(vgl_point_3d<double> (x0+num_points_x-1, y0+num_points_y-1, z0+num_points_z-1));
}
worldpt biob_grid_worldpt_roster::point(biob_worldpt_index pti) const 
{
  unsigned long int i = pti.index();
  unsigned long int iz = i / num_points_per_z_;
  unsigned long int iy = (i - iz* num_points_per_z_)/ num_points_per_y_;
  unsigned long int ix = (i - iz* num_points_per_z_ - iy*num_points_per_y_) % num_points_per_y_;
  return worldpt(ix * spacing_ + x0_ , iy * spacing_ + y0_, iz * spacing_ + z0_);
}

unsigned long int biob_grid_worldpt_roster::num_points() const
{
 return num_points_;
}

biob_worldpt_index  
biob_grid_worldpt_roster::index_3d_2_1d(unsigned i, unsigned j, unsigned k) const
{
  return biob_worldpt_index(num_points_per_z_ * k + num_points_per_y_* j + i);
}

worldpt biob_grid_worldpt_roster::grid_3d(unsigned i, unsigned j, unsigned k) const
{
  return worldpt(i*spacing_ + x0_, j * spacing_ + y0_, k *spacing_ + z0_);
}

//: returns true if pt is within the grid
bool
biob_grid_worldpt_roster::voxel(worldpt pt, biob_worldpt_index & pti) const {
 unsigned i = static_cast<unsigned>((pt.x()-x0_)/spacing_+.5);
 unsigned j = static_cast<unsigned>((pt.y()-y0_)/spacing_+.5);
 unsigned k = static_cast<unsigned>((pt.z()-z0_)/spacing_+.5);
 if (0 <= i && i < num_points_x_
     && 0 <= j && j < num_points_y_
     && 0 <= k && k < num_points_z_){
   pti = index_3d_2_1d(i,j,k);
   return true;
 }
 else {
   return false;
 }
}

bool biob_grid_worldpt_roster::conditional_index_3d_2_1d(long i, long j, long k, biob_worldpt_index & pti) const {
  if (i >= 0 && j >= 0 && k >= 0 && i < static_cast<long>(num_points_x_) && j < static_cast<long>(num_points_y_) && k < static_cast<long>(num_points_z_)){
    pti = index_3d_2_1d(i, j, k);
    return true;
  }
  return false;
}


vcl_list<biob_worldpt_index> biob_grid_worldpt_roster::neighboring_voxels(biob_worldpt_index pti) const{
  vcl_list<biob_worldpt_index> neighbors;
  long int i = pti.index();
  long int iz = i / num_points_per_z_;
  long int iy = (i - iz* num_points_per_z_)/ num_points_per_y_;
  long int ix = (i - iz* num_points_per_z_ - iy*num_points_per_y_) % num_points_per_y_;
  for (int deltax = -1; deltax <= 1; ++deltax){
    for (int deltay = -1; deltay <= 1; ++deltay){
      for (int deltaz = -1; deltaz <= 1; ++deltaz){
        biob_worldpt_index pti;
        bool okay = conditional_index_3d_2_1d(ix+deltax, iy+deltay, iz+deltaz, pti);
        if (okay){
          neighbors.push_back(pti);
        }
      }
    }
  }
  return neighbors;
}

void biob_grid_worldpt_roster::x_write_this(vcl_ostream& os)
{
  vsl_basic_xml_element element("biob_grid_worldpt_roster");

  // take care of unsigned long int in bxml later --Gamze
  element.add_attribute("spacing", (double) spacing());
  element.add_attribute("num_points",  (int)num_points());
  element.add_attribute("num_points_x", (int) nx());
  element.add_attribute("num_points_y",  (int)ny());
  element.add_attribute("num_points_z",  (int) nz());
  element.x_write_open(os);
  vcl_string str("roster_bounding_box");
  x_write(os, bounding_box(), str);
  element.x_write_close(os);
}
