#ifndef psm_triangle_interpolation_iterator_aa_h_
#define psm_triangle_interpolation_iterator_aa_h_

#include <vcl_vector.h>

#include <vgl/vgl_region_scan_iterator.h>
#include <vgl/vgl_triangle_scan_iterator.h>
#include <vbl/vbl_bounding_box.h>

template<class T>
class psm_triangle_interpolation_iterator_aa : public vgl_region_scan_iterator
{
public:
  //: constructor
  psm_triangle_interpolation_iterator_aa(double *verts_x, double *verts_y, T *values, unsigned int v0 = 0, unsigned int v1 = 1, unsigned int v2 = 2);

  //: Resets the scan iterator to before the first scan line
  //  After calling this function, next() needs to be called before
  //  startx() and endx() form a valid scan line.
  virtual void reset();

  //: Tries to move to the next scan line.
  //  Returns false if there are no more scan lines.
  virtual bool next();

  //: y-coordinate of the current scan line.
  virtual int  scany() const;

  //: Returns starting x-value of the current scan line.
  //  startx() should be smaller than endx(), unless the scan line is empty
  virtual int  startx() const;

  //: Returns ending x-value of the current scan line.
  //  endx() should be larger than startx(), unless the scan line is empty
  virtual int  endx() const;

  //: returns the amount of pixel at location x in the current scanline covered by the triangle
  float pix_coverage(int x);

  //: returns the interpolated value at location x in the current scanline
  T value_at(int x);

protected:
  static const unsigned int supersample_ratio_ = 4;
  vgl_triangle_scan_iterator<double> super_tri_it_;

  int startx_;
  int endx_;
  int scany_;
  bool next_return_;

  unsigned int tri_diameter_x_;
  vcl_vector<float> aa_vals_;
  vcl_vector<T> interp_vals_;
  int vec_offset_;

  vbl_bounding_box<double,2> tri_bb_;

  double s0_;
  double s1_;
  double s2_;

  // start and end of pixels containing any of part triangle 
  //vcl_vector<int> border_x_start_;
  //vcl_vector<int> border_x_end_;
  // start and end of pixels completely enclosed by the triangle
  //vcl_vector<int> interior_x_start_;
  //vcl_vector<int> interior_x_end_;
};




#endif

