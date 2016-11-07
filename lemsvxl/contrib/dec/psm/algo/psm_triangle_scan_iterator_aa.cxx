#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vgl/vgl_triangle_scan_iterator.h>

#include "psm_triangle_scan_iterator_aa.h"


//: constructor
psm_triangle_scan_iterator_aa::psm_triangle_scan_iterator_aa(double *verts_x, double *verts_y, unsigned int v0, unsigned int v1, unsigned int v2)
: super_tri_it_()
{
  super_tri_it_.a.x = verts_x[v0] * supersample_ratio_ - 0.5;
  super_tri_it_.a.y = verts_y[v0] * supersample_ratio_ - 0.5;
  super_tri_it_.b.x = verts_x[v1] * supersample_ratio_ - 0.5;
  super_tri_it_.b.y = verts_y[v1] * supersample_ratio_ - 0.5;
  super_tri_it_.c.x = verts_x[v2] * supersample_ratio_ - 0.5;
  super_tri_it_.c.y = verts_y[v2] * supersample_ratio_ - 0.5;

  tri_bb_.update(verts_x[v0], verts_y[v0]);
  tri_bb_.update(verts_x[v1], verts_y[v1]);
  tri_bb_.update(verts_x[v2], verts_y[v2]);

  int tri_xmin = (int)vcl_floor(tri_bb_.xmin());
  int tri_xmax = (int)vcl_floor(tri_bb_.xmax()) + 1; 
  tri_diameter_x_ = (tri_xmax - tri_xmin) + 1;
  aa_vals_.resize(tri_diameter_x_);
  aa_vals_offset_ = -tri_xmin;
  next_return_ = false;
}


//: Resets the scan iterator to before the first scan line
//  After calling this function, next() needs to be called before
//  startx() and endx() form a valid scan line.
void psm_triangle_scan_iterator_aa::reset()
{
  super_tri_it_.reset();
  next_return_ = super_tri_it_.next();
  while(next_return_ && super_tri_it_.scany() < 0) {
    next_return_ = super_tri_it_.next();
  }
}

//: Tries to move to the next scan line.
//  Returns false if there are no more scan lines.
bool psm_triangle_scan_iterator_aa::next()
{
  if (!next_return_) {
    return false;
  }
  int super_scany = super_tri_it_.scany();
  scany_ = super_scany / supersample_ratio_; // super_scany should always be >= 0

  // initialize startx_ and endx_
  startx_ = int(tri_bb_.xmax()) + 1;
  endx_ =  int(tri_bb_.xmin()) - 1;

  // compute antialiasing values for each pixel in scanline
  vcl_fill(aa_vals_.begin(), aa_vals_.end(), 0.0f);
 
  int super_scany_end = (scany_ + 1)*supersample_ratio_;
  static const float increment = 1.0f / (supersample_ratio_*supersample_ratio_);
  static const float full_increment = 1.0f / (supersample_ratio_); 

  while ( (super_scany < super_scany_end) && next_return_ ) {
    int super_startx = super_tri_it_.startx();
    int super_endx = super_tri_it_.endx() + 1;

    super_startx = vcl_max(0, super_startx);
    super_endx = vcl_max(0, super_endx);
    // make sure super_startx < super_endx
    if (super_endx > super_startx) {
      int scanline_startx = super_startx / supersample_ratio_;
      int scanline_endx = ((super_endx - 1)/ supersample_ratio_) + 1;
      // update startx and endx
      if (scanline_startx < startx_) {
        startx_ = scanline_startx;
      }
      if (scanline_endx > endx_) {
        endx_ = scanline_endx;
      }
      // case 1: run covers 1 superpixel only
      if (scanline_startx + 1 == scanline_endx) {
        aa_vals_[aa_vals_offset_ + scanline_startx] += increment*(super_endx - super_startx);
      }
      // case 2: startx is less than endx by more than 1 
      else {
        // partial coverage at the beginning of scanline
        aa_vals_[aa_vals_offset_ + scanline_startx] += increment*(supersample_ratio_ - (super_startx % supersample_ratio_));

        // full coverage in middle of scanline
        for (int x = scanline_startx + 1; x < scanline_endx-1; ++x) {
          aa_vals_[aa_vals_offset_ + x] += full_increment;
        }
        // partial coverage at end of scanline
        aa_vals_[aa_vals_offset_ + scanline_endx - 1] += increment*((super_endx - 1)% supersample_ratio_ + 1);
      }
    }
    next_return_ = super_tri_it_.next();
    super_scany = super_tri_it_.scany();
  }
  return true;
}

//: y-coordinate of the current scan line.
int psm_triangle_scan_iterator_aa::scany() const
{
  return scany_;
}

//: Returns starting x-value of the current scan line.
//  startx() should be smaller than endx(), unless the scan line is empty
int psm_triangle_scan_iterator_aa::startx() const
{
  return startx_;
}

//: Returns ending x-value of the current scan line.
//  endx() should be larger than startx(), unless the scan line is empty
int  psm_triangle_scan_iterator_aa::endx() const
{
  return endx_;
}

//: returns the amount of pixel at location x in the current scanline covered by the triangle
float psm_triangle_scan_iterator_aa::pix_coverage(int x)
{
  return aa_vals_[aa_vals_offset_ + x];
}
