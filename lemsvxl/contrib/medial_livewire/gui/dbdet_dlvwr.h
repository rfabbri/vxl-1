#ifndef dbdet_dlvwr_h
#define dbdet_dlvwr_h
//:
//  \file
//  \brief Livewire Manual Segmentation Tool classes
//  \author Nhon Trinh (ntrinh@lems.brown.edu)
//  \date 11/07/2005
//  \verbatim
//  Modifications:
//    Nhon Trinh (ntrinh) 11/07/2005
//  \endverbatim


#include "dbdet_dlvwr_params.h"
#include <vcl_set.h>

#include <vil/vil_image_view.h>
#include <osl/osl_canny_ox_params.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>

//: A class to hold total cost from the seed to an individual pixel 
class dbdet_dlvwr_pixel : public vgl_point_2d<int >
{
protected:
  float accum_cost_; // cost of path from seed poin to `this' pixel
  //float path_length; // length of path from seed point to `this' pixel

public:
  //: constructor
  dbdet_dlvwr_pixel(): vgl_point_2d<int >(), accum_cost_(0) {}

  //: constructor
  dbdet_dlvwr_pixel(int new_x, int new_y, float new_cost = 0): 
  vgl_point_2d<int >(new_x, new_y), accum_cost_(new_cost){}

  //: destructor
  ~dbdet_dlvwr_pixel(){};

  //: Return accumulated cost of path from seed point
  float accum_cost() const { return this->accum_cost_; }

  //: Set pixel's accumulated cost
  void set_accum_cost(float new_cost){ this->accum_cost_ = new_cost;}

  //: operators
  bool operator== (const dbdet_dlvwr_pixel& rhs)
  {return this->x()==rhs.x() && this->y()==rhs.y(); };

};


//: operator to compare two pixels
struct dbdet_dlvwr_pixel_less_than
{
  bool operator()(const dbdet_dlvwr_pixel& lhs, const dbdet_dlvwr_pixel& rhs) const
  {
    return (lhs.accum_cost() < rhs.accum_cost());
  }
};

//: set of pixels equipped with a comparison operator
typedef vcl_set<dbdet_dlvwr_pixel, dbdet_dlvwr_pixel_less_than > dbdet_dlvwr_pixel_set;


//: A dbdet_dlvwr object stores the internal buffers
class dbdet_dlvwr 
{
public:
  //: Constructor - default
  dbdet_dlvwr() : params_(), canny_params_(){}

  //: Constructor 2
//  dbdet_dlvwr(dbdet_dlvwr_params const& params, osl_canny_ox_params const& canny_params):
//    image_w(0), image_h(0), params_(params), canny_params_(canny_params) { }
  // Destructor 
  ~dbdet_dlvwr() {}

  // =======================
  // Data access
  // =======================

  dbdet_dlvwr_params params() const {return this->params_;}

  //: set parameters of `this' live-wire
  void set_params(const dbdet_dlvwr_params& params) { this->params_=params; }
  
  //: set parameters of canny edge detector
  void set_canny_params(osl_canny_ox_params const & canny_params)
  { this->canny_params_ = canny_params; }

  //: Return seed point
  vgl_point_2d<int > seed() const { return this->seed_; }

  //: Return region (rectanglar box) which optimal paths have been computed
  vgl_box_2d<int > explored_region();

  //: set seed point
  bool set_seed(int seed_x, int seed_y);

  //: Return width of the image
  int image_w(){ return this->image_.ni(); }

  //: Return height of the image
  int image_h(){ return this->image_.nj(); }

  //: set the image
  void set_image(const vil_image_view<float >& image);

  //// set edge costs
  //void set_edge_cost(const vil_image_view<float >& edge_cost)
  //{this->edge_cost_ = edge_cost; }

  //: set gradient
  void set_grad_xy(const vil_image_view<float >& grad_x, 
    const vil_image_view<float >& grad_y)
  { this->grad_x_ = grad_x; this->grad_y_ = grad_y; }

  //: Return gradient vector at a pixel
  vgl_vector_2d<float > grad_at(int x, int y) const
  {return vgl_vector_2d<float >(this->grad_x_(x, y), this->grad_y_(x, y)); }



  //: Return gradient magnitude cost at a pixel
  float grad_mag_cost(int x, int y){ return this->grad_mag_cost_(x, y); }

  
  //: set gradient magnitude
  void set_grad_mag_cost(vil_image_view<float >& grad_mag_cost )
  { this->grad_mag_cost_ = grad_mag_cost; }


  //: Return LoG cost at a pixel
  float log_cost(int x, int y){ return this->log_cost_(x, y); }

  //: Set cost from Laplacian of Gaussian
  void set_log_cost(vil_image_view<float >& log_cost)
  { this->log_cost_ = log_cost; }
  

  //: Copy all parameters and cost images from `other'
  void copy(const dbdet_dlvwr& other);
  
  // =============================================================
  // Utilities
  // =============================================================
  //: Compute all static components of cost function
  // assuming only `image_' is present
  void compute_static_cost_components();

  //: compute shortest path from seed point to any point in its neighborhood
  // (a rectangle) 
  void compute_optimal_paths(int seed_x, int seed_y);

  // Return best edge point in neighborhood of a pixel
  vgl_point_2d<int > cursor_snap(int mouse_x, int mouse_y);

  

  //: Return neighbor of a pixel given relative coded direction 
  // assuming `dir' takes on value 0->(2*half_side+1)^2-1
  vgl_point_2d<int > neighbor_of(vgl_point_2d<int > pt, int dir, int half_side = 1);
  
  //: Return the optimum path from the free point to the seed point
  bool get_path(int free_x, int free_y,
          vcl_vector<vgl_point_2d<int > >& path);

  //: Return accumulated cost of path from seed point to each pixel
  float accum_cost(int x, int y){ return this->accum_cost_(x, y); }

  //: link cost between pixel p and pixel q
  float linked_cost(vgl_point_2d<int > p, vgl_point_2d<int > q);




  // -----------------------------------------------------------------
  // The following functions recompute the components from the image
  // regardless of what has been set to the components

  //: Compute gradient
  void compute_gradient();

  //: Compute gradient magnitude cost
  void compute_grad_mag_cost();

  //: Compute laplacian
  void compute_log_cost();

  //: Force static cost (gradient magnitude and Laplacian of Gaussian) to some
  // const value at selected points
  void force_static_cost(vcl_vector<vgl_point_2d<int > >& pts, float val = 0);

  ////: Compute `cursor_snap_' image, which specifies the closest edge point to each pixel in a neighborhood of the pixel
  //void compute_cursor_snap();


protected:
  // =====================================================================
  // Member variables
  // =====================================================================

  // live-wire parameters
  dbdet_dlvwr_params params_;

  // canny edge-detector parameters
  osl_canny_ox_params canny_params_;
  
  // coordinate of seed point
  vgl_point_2d<int > seed_;

  // static `images' - data associated each pixel statically
  // internal data of these `images' can be shared by multiple livewires

  // the input image
  vil_image_view<float > image_;

  // contour snap
  vil_image_view<vxl_byte > cursor_snap_;
  
  // gradient bitmaps
  vil_image_view<float > grad_x_;
  vil_image_view<float > grad_y_;

  vil_image_view<float > grad_mag_cost_;

  // laplacian of gaussian
  vil_image_view<float > log_cost_;

  // ---------------------------------
  // These `image' data are computed dynamically as seed point changes
  // -----------------------------------

  // parameters for dynamic programming graph-search
  // mask indicating how far wavefront has been expaned
  vil_image_view<bool > expanded_;
  
  // cost of path from seed point to each pixel in the image
  vil_image_view<float > accum_cost_;

  // pointer to neighbor that results in lowest cost
  // The value is coded to indicated which neighbor (there are 8)
  vil_image_view<vxl_byte > pointed_neighbor_;

  // length of path from seed point
  //vil_image_view<float >path_length_;
//  //bool get_path(int free_x, int free_y,
//  //        vcl_vector<vdgl_edgel> edgels);
//  float get_global_cost(int x, int y);
//
//  void compute_costs_from_edges(vcl_list<osl_edge*> canny_edges);

};
#endif // dbdet_dlvwr_h


