// This is brcv/seg/dbdet/lvwr/dbdet_dlvwr.cxx
//:
//  \file

#include "dbdet_dlvwr.h"

#include <vnl/vnl_math.h>

#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_correlate_2d.h>
#include <vil/vil_copy.h>

// ------------------------------------------------------------------
// set seed point
bool dbdet_dlvwr::
set_seed(int seed_x, int seed_y)
{ 
  // make sure new seed point is INSIDE the image
  if (seed_x < 3 || seed_x > (int)this->image_.ni()-3 ||
    seed_y < 3 || seed_y > (int)this->image_.nj()-3)
  {
    return false;
  }
  this->seed_.set(seed_x, seed_y);
  return true;
}



// ----------------------------------------------------------------------
//: Return region (rectanglar box) which optimal paths have been computed
vgl_box_2d<int > dbdet_dlvwr::
explored_region()
{
  int w_over_2 = this->params_.window_w_over_2;
  int h_over_2 = this->params_.window_h_over_2;
  int min_x = vnl_math_max(this->seed().x()-w_over_2, 3);
  int min_y = vnl_math_max(this->seed().y()-h_over_2, 3);
  int max_x = vnl_math_min(this->seed().x()+w_over_2, this->image_w()-4);
  int max_y = vnl_math_min(this->seed().y()+h_over_2, this->image_h()-4);
  return vgl_box_2d<int >(min_x, max_x, min_y, max_y);
}


//: set the image
void dbdet_dlvwr::
set_image(const vil_image_view<float >& image) 
{
  assert(image);
  this->image_ = image; 
  this->expanded_.set_size(image.ni(), image.nj());
  this->accum_cost_.set_size(image.ni(), image.nj());
  this->pointed_neighbor_.set_size(image.ni(), image.nj());
}


// -----------------------------------------------------------------------
//: Compute all static components of cost function
void dbdet_dlvwr::
compute_static_cost_components()
{
  assert(this->image_);
  this->compute_gradient();
  this->compute_grad_mag_cost();
  this->compute_log_cost();
  return;
}

// -----------------------------------------------------------------------
//: Compute gradient
// \TODO: use multi-scale gradient kernel
void dbdet_dlvwr::
compute_gradient()
{
  // preliminary check
  if (!this->image_) return;

  // smooth image
  vil_image_view<float > gauss_image;
  vil_gauss_filter_5tap<float, float >(this->image_,
    gauss_image, vil_gauss_filter_5tap_params(0.5));
  // compute gradient using sobel kernel
  vil_sobel_3x3<float, float >(gauss_image, this->grad_x_, this->grad_y_);

  return;
}


// -----------------------------------------------------------------------
//: Compute gradient magnitude cost
void dbdet_dlvwr::
compute_grad_mag_cost()
{
  if (!this->grad_x_ || !this->grad_y_) return;

  // require
  assert(this->grad_x_.ni()==this->grad_y_.ni() && 
    this->grad_x_.nj()==this->grad_y_.nj());

  // gradient magnitude
  vil_math_image_vector_mag(this->grad_x_, this->grad_y_, this->grad_mag_cost_);
  
  // transform gradient magnitude by an inverse linear ramp to get cost
  float grad_max, grad_min;
  vil_math_value_range(this->grad_mag_cost_, grad_min, grad_max);
  
  // rarely happens !!!!!!!!!
  if (grad_min >= grad_max)
  {
    this->grad_mag_cost_.set_size(this->image_.ni(), this->image_.nj());
    return;
  }

  
  //float scale = -1/(grad_max - grad_min);
  //float offset = grad_max / (grad_max - grad_min);
  //vil_math_scale_and_offset_values(this->grad_mag_cost_, scale, offset);

  // alternative method: user inverse quadratic instead of linear
  // f(x) = 1 + a (x-b)^2; such that
  // f(xmin) = 1;
  // f(xmax) = 0;
  // f'(xmin) = 0; --> to suppress small gradient
  float b = grad_min;
  float a = -1/((grad_max-grad_min)*(grad_max-grad_min));
  
  // transform gradient cost
  for (unsigned int i=0; i<this->grad_mag_cost_.ni(); ++i)
  {
    for (unsigned int j=0; j<this->grad_mag_cost_.nj(); ++j)
    {
      this->grad_mag_cost_(i,j) = 
        1 + a*vnl_math_sqr(this->grad_mag_cost_(i, j)-b);
    }
  }
  return;
}




// -----------------------------------------------------------------------
//: Compute laplacian
// \TODO rewrite to use 2 1D filter instead of 2D filter
// see http://www.cs.ubc.ca/~woodham/cpsc505/solutions/s05.html
void dbdet_dlvwr::
compute_log_cost()
{
  if (!this->image_) return;

  // Laplacian of Gaussian filter with size = 5x5, sigma = 0.5
  // the matrix is got from Matlab fspecial('log');
  vil_image_view<float > h(5, 5);
  h(0,0)=0.0448f; h(0,1)=0.0468f; h(0,2)=0.0564f; h(0,3)=0.0468f; h(0,4)=0.0448f;
  h(1,0)=0.0468f; h(1,1)=0.3167f; h(1,2)=0.7146f; h(1,3)=0.3167f; h(1,4)=0.0468f;
  h(2,0)=0.0564f; h(2,1)=0.7146f; h(2,2)=-4.9048f; h(2,3)=0.7146f; h(2,4)=0.0564f;
  h(3,0)=0.0468f; h(3,1)=0.3167f; h(3,2)=0.7146f; h(3,3)=0.3167f; h(3,4)=0.0468f;
  h(4,0)=0.0448f; h(4,1)=0.0468f; h(4,2)=0.0564f; h(4,3)=0.0468f; h(4,4)=0.0448f;

  // the laplacian of gaussian (log) is only computed for a window inside
  // inside the image (due to limitation of current convolve_2d code) 
  int offset_x = 2;
  int offset_y = 2;
  
  vil_image_view<float > image_log(this->image_.ni(), this->image_.nj());
  // image_log_temp is not necessary any more after 
  // data has been copied to image_log
  {
    vil_image_view<float > image_log_temp;
    vil_correlate_2d(this->image_, image_log_temp, h, double());
    vil_copy_to_window(image_log_temp, image_log, offset_x, offset_y);
  }

  // laplacian cost
  this->log_cost_.set_size(this->image_.ni(), this->image_.nj());
  this->log_cost_.fill(1);
  // scan all pixels in window where log is computed
  for (unsigned int i=offset_x; i<image_log.ni()-offset_x; ++i)
  {
    for (unsigned int j=offset_x; j<image_log.nj()-offset_x; ++j)
    {
      vgl_point_2d<int > cur_pt(i, j);
      float cur_log = image_log(i, j);

      // scan all neighbors
      for (int dir=0; dir<9; ++dir)
      {
        // do not compare against itself
        if (dir==5) continue;

        vgl_point_2d<int > neighbor = this->neighbor_of(cur_pt, dir);
        float neighbor_log = image_log(neighbor.x(), neighbor.y());
        if (cur_log*neighbor_log <= 0 && 
          vnl_math_abs(cur_log) <= vnl_math_abs(neighbor_log))
        {
          this->log_cost_(i, j) = 0;
          break;
        }
      }
    }
  }
}



// -----------------------------------------------------------------------
//: Force static cost (gradient magnitude and Laplacian of Gaussian) to some
// const value at selected points
void dbdet_dlvwr::
force_static_cost(vcl_vector<vgl_point_2d<int > >& pts, float val)
{
  for (unsigned int m=0; m<pts.size(); ++m)
  {
    this->grad_mag_cost_(pts[m].x(), pts[m].y()) = val;
    this->log_cost_(pts[m].x(), pts[m].y()) = val;
  }
  return;
}



////: Compute `cursor_snap_' image, which specifies the closest edge point to each pixel in a neighborhood of the pixel
//// TODO: take care of the boundary of the image
//void dbdet_dlvwr::
//compute_cursor_snap()
//{
//  this->cursor_snap_.set_size(this->image_.ni(), this->image_.nj());
//  unsigned char half_side = this->params_.cursor_snap_side_over_2;
//  int max_dir = (2*half_side + 1)*(2*half_side + 1);
//  for (int i=half_side+1; i<this->image_w()-half_side-1; ++i)
//  {
//    for (int j=half_side+1; j<this->image_h()-half_side-1; ++j)
//    {
//      vgl_point_2d<int > cur_pt(i, j);
//      // do a brute force search in the neighborhood
//      float min_cost = 10000;
//      int min_dir = -1;
//      for (int dir=0; dir < max_dir; ++dir)
//      {
//        vgl_point_2d<int > neighbor=this->neighbor_of(cur_pt, dir, half_side);
//        if (this->grad_mag_cost_(neighbor.x(), neighbor.y()) < min_cost)
//        {
//          min_dir = dir;
//          min_cost = this->grad_mag_cost_(neighbor.x(), neighbor.y());
//        }
//      }
//      //////////////////////////////////////////
//      this->cursor_snap_(cur_pt.x(), cur_pt.y()) = (vxl_byte)min_dir;
//    }
//  }
//}




// ----------------------------------------------------------------------
//: Return best edge point in neighborhood of a pixel
vgl_point_2d<int > dbdet_dlvwr::
cursor_snap(int mouse_x, int mouse_y)
{
  // easy case
  if (this->params().cursor_snap_side_over_2==0)
    return vgl_point_2d<int >(mouse_x, mouse_y);

  // do a brute force search in the neighborhood

  int search_r = this->params().cursor_snap_side_over_2;
  float cur_cost = 0;
  float min_cost = 10000;
  int min_x = 0;
  int min_y = 0;
  
  for (int i = -search_r; i<=search_r; ++i)
  {
    for (int j = -search_r; j<=search_r; ++j)
    {
      cur_cost = this->grad_mag_cost(mouse_x+i, mouse_y+j);
      // order of assigment matters !!! (min_x and min_y BEFORE min_cost)
      min_x = (cur_cost < min_cost) ? mouse_x+i : min_x;
      min_y = (cur_cost < min_cost) ? mouse_y+j : min_y;
      min_cost = (cur_cost < min_cost) ? cur_cost : min_cost;
      
    }
  }

  // return neighboring point with lowest gradient cost
  return vgl_point_2d<int >(min_x, min_y);
}




//: Copy all parameters and cost images from `other'
void dbdet_dlvwr::
copy(const dbdet_dlvwr& other)
{
  this->params_ = other.params_;
  this->canny_params_ = other.canny_params_;
  this->set_image(other.image_);
  this->grad_x_ = other.grad_x_;
  this->grad_y_ = other.grad_y_;
  this->grad_mag_cost_ = other.grad_mag_cost_;
  this->log_cost_ = other.log_cost_;
  this->cursor_snap_ = other.cursor_snap_;
  return;
}








// -----------------------------------------------------------------------
//: linked cost of of edge from pixel `p' to pixel `q'
float dbdet_dlvwr::
linked_cost(vgl_point_2d<int > p, vgl_point_2d<int > q)
{
  // static cost
  float reliability_coeff = 1-this->grad_mag_cost_(q.x(), q.y());
  float wn = (p.x()==q.x()|| p.y()==q.y()) ? 1.0f/vcl_sqrt(2.0f) : 1.0f;
  float static_cost = 
    this->params_.weight_z*this->log_cost_(q.x(), q.y())+ 
    wn *this->params_.weight_g * this->grad_mag_cost_(q.x(), q.y());

  // dynamic cost
  
  vgl_vector_2d<float > dprime_p(this->grad_y_(p.x(), p.y()), 
    -this->grad_x_(p.x(), p.y()));
  normalize(dprime_p);

  vgl_vector_2d<float > dprime_q(this->grad_y_(q.x(), q.y()), 
    -this->grad_x_(q.x(), q.y()));
  normalize(dprime_q);

  // linked vector
  vgl_vector_2d<float > link_pq((float)(q.x()-p.x()), (float)(q.y()-p.y()));
  normalize(link_pq);
  float dp = dot_product<float >(dprime_p, link_pq);
  if (dp < 0)
  {
    dp = -dp;
    link_pq = -link_pq;
  }
  float dq = dot_product<float >(link_pq, dprime_q);

  // gradient direction cost
  float fd = (float)((2.0/3*vnl_math::pi) * (vcl_acos(dp) + vcl_acos(dq)));
  float dynamic_cost = this->params_.weight_d * reliability_coeff *fd;

  // sum up
  return static_cost + dynamic_cost;
}




////: just give zero cost to edge pixels and 1 cost to the remaining pixels
//void dbdet_dlvwr::compute_costs_from_edges(vcl_list<osl_edge*> canny_edges) {
//
//  vcl_cout << image_edgecosts_buf.width() << vcl_endl;
//  vcl_cout << image_edgecosts_buf.height() << vcl_endl;
//
//  for (int row = 0; row < image_h; row++)
//   for (int col = 0; col < image_w; col++) {
//    image_edgecosts_buf[row][col] = 1;
//    }
//
//  vcl_list<osl_edge*>::const_iterator i;
//  for (i = canny_edges.begin(); i != canny_edges.end(); ++i)
//  {
//    osl_edge const* e = *i;
//    float *x = e->GetY(),*y = e->GetX(); // note x-y confusion.
//
//    for (unsigned int j = 0;j<e->size();j++)
//    {
//      image_edgecosts_buf[(int)(y[j])][(int)(x[j])] = 0;
//      //easy_tab->add_point(x[j], y[j]);
//    }
//  }
//
//}


//vcl_ostream& operator<< (vcl_ostream& Out, const my_pixel &p1)
//{
//  Out << "my pixel object row: " << p1.row << " col: " << p1.col;
//  Out << " total_cost: " << p1.total_cost /*<< " points to: << p1.point_to */<< "\n";
//  return Out;
//}



  



// compute shortest path from seed point to any point in its neighborhood
// (a rectangle) 
void dbdet_dlvwr::compute_optimal_paths(int seed_x, int seed_y)
{
  if (!this->set_seed(seed_x, seed_y))
    return;

  vgl_box_2d<int > box = this->explored_region();



  //  // --------------------------------
//  // these steps may not be necessary
//  this->expanded_.fill(false);
//  this->path_length_.fill(0);
//  this->global_cost_.fill(0);
//  this->pointed_neighbor_.fill(vgl_point_2d<int >(-1, -1));
//  // ---------------------------------


  // do not explore points outside the box
  for (int i=box.min_x()-1; i<=box.max_x()+1; ++i)
    for (int j=box.min_y()-1; j<=box.max_y()+1; ++j)
    {
      this->expanded_(i, j) = true;
      this->accum_cost_(i, j) = 1e8;
    }

  for (int i=box.min_x(); i<=box.max_x(); ++i)
    for (int j=box.min_y(); j<=box.max_y(); ++j)
      this->expanded_(i, j) = false;

  dbdet_dlvwr_pixel s(seed_x, seed_y);
  s.set_accum_cost(0);
  this->accum_cost_(s.x(), s.y()) = 0;
  this->expanded_(s.x(), s.y()) = true;

  dbdet_dlvwr_pixel_set active_list;
  active_list.insert(s);

  while(!active_list.empty())
  {
    dbdet_dlvwr_pixel_set::iterator it = active_list.begin();
    dbdet_dlvwr_pixel p = *it;
    active_list.erase(it);
    this->expanded_(p.x(), p.y()) = true;
    for (int dir = 0; dir < 9; ++dir)
    {
      vgl_point_2d<int > q = this->neighbor_of(p, dir);
      if (this->expanded_(q.x(), q.y()))
        continue;
      float gtmp = this->accum_cost_(p.x(), p.y()) + this->linked_cost(p, q);
      bool q_in_active_list = false;
      dbdet_dlvwr_pixel_set::iterator pit = active_list.begin();
      for (;pit != active_list.end(); ++pit)
      {
        if ((*pit).x()==q.x() && (*pit).y()==q.y())
        {
          q_in_active_list = true;
          break;
        }
      }

      // `q' is in `active_list'
      if (q_in_active_list)
      {
        // new cost is lower than cost from previous path
        float old_cost = (*pit).accum_cost();
        if (gtmp < old_cost)
        {
          active_list.erase(pit);
        }
      }
      // `q' is not in `active_list'
      else
      {
        dbdet_dlvwr_pixel qq(q.x(), q.y());
        qq.set_accum_cost(gtmp);
        active_list.insert(qq);
        this->pointed_neighbor_(q.x(), q.y()) = 8-dir;        
        this->accum_cost_(q.x(), q.y()) = gtmp;
      }
    }
  
  }
}


//: Return neighbor of a pixel given relative coded direction 
vgl_point_2d<int > dbdet_dlvwr::
neighbor_of(vgl_point_2d<int > pt, int dir, int half_side)
{
  return vgl_point_2d<int >(pt.x()+ dir%(2*half_side+1)-half_side, 
    pt.y()+ dir/(2*half_side+1)-half_side);
}


// ----------------------------------------------------------------------
// return the optimum path from the free point to the seed point
bool dbdet_dlvwr::
get_path(int free_x, int free_y, 
         vcl_vector<vgl_point_2d<int > >& path)
{
  path.clear();
  if (!this->explored_region().contains(free_x, free_y))
  {
    return false;
  }

  // trace back from the free point
  // since the free point is within explored region, the path
  // is guaranteed to pass seed point
  vgl_point_2d<int > current_pt(free_x, free_y);
  while (current_pt != this->seed_)
  {
    path.push_back(current_pt);
    
    current_pt = this->neighbor_of(current_pt, 
      this->pointed_neighbor_(current_pt.x(), current_pt.y()));
  }
  //path.push_back(current_pt);
  return true;
}



