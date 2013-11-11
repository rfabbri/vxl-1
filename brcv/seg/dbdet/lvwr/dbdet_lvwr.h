#ifndef dbdet_lvwr_h
#define dbdet_lvwr_h
//:
//  \file
//  \brief Livewire Manual Segmentation Tool classes
//  \par Parameters :
//  - weight_z  - the weight of Laplacian Zero Crossing in local cost function
//  - weight_g  - the weight of Gradient Magnitude in local cost function
//  - weight_d  - the weight of Gradient Direction in local cost function
//  - weight_l  - the weight of cost due to path length 
//  \author
//      Ozge Can Ozcanli, LEMS, Brown University (ozge@lems.brown.edu)
//  \date 22.08.03
//  \verbatim
//  Modifications:
//    Modified to work on canny edgels by giving edge pixels 0 cost
//    Shannon Telesco -- added Compute method not to repeat edge detection 
//  \endverbatim

#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vil1/vil1_memory_image_of.h>

#include <dbdet/lvwr/dbdet_lvwr_params.h>

#include <osl/osl_roi_window.h>
#include <osl/osl_canny_ox_params.h>
#include <osl/osl_canny_ox.h>

class osl_edge;

//: my_pixel is a class to hold total cost and path length from the seed to an individual pixel 
class my_pixel {
public:
  my_pixel() : row(0), col(0), total_cost(0), path_length(0) { }
  my_pixel(int row_, int col_) :
    row(row_), col(col_), total_cost(0), path_length(0) { }

  my_pixel & operator= (const my_pixel &rhs)
  {
    row = rhs.row;
    col = rhs.col;
    total_cost = rhs.total_cost;
    return *this;
  }

  int row,col;
  float total_cost;
  int path_length;
};

//: A dbdet_lvwr object stores the internal buffers
class dbdet_lvwr {
public:
  dbdet_lvwr() :
    image_w(0), image_h(0), params_(), canny_params_() { }

  dbdet_lvwr(dbdet_lvwr_params const & params, osl_canny_ox_params const & canny_params) :
    image_w(0), image_h(0), params_(params), canny_params_(canny_params) { }
   ~dbdet_lvwr() {
  //  free_buffers();
   }

  void compute(vil1_image const &image, int seed_x, int seed_y) {

    prepare_buffers(image.width(), image.height(), seed_x, seed_y);

    if (params_.canny) {
      osl_canny_ox cox(canny_params_);

      static vcl_list<osl_edge*>    canny_edges;
      cox.detect_edges(image, &canny_edges);
      compute_costs_from_edges(canny_edges);

    } else if(params_.use_given_image)  {
      set_costs_from_image(image);
    } else {
      compute_gradients(image);

    }

    compute_directions(seed_x, seed_y);
  }


   void Compute(vil1_image const &image, int /*seed_x*/, int /*seed_y*/, dbdet_lvwr intsciss2) {

    //prepare_buffers(image.width(), image.height(), seed_x, seed_y);

    if (params_.canny) {
      osl_canny_ox cox(canny_params_);

      static vcl_list<osl_edge*>    canny_edges;
      cox.detect_edges(image, &canny_edges);
      compute_costs_from_edges(canny_edges);
    intsciss2.compute_costs_from_edges(canny_edges);
    } else {
      compute_gradients(image);
    intsciss2.compute_gradients(image);
    }
   }

  //void get_gradients() const;

  int image_w, image_h;
  int seed_x, seed_y;

  vil1_memory_image_of<float>    image_edgecosts_buf;

  // the input image
  vil1_memory_image_of<unsigned char> image_buf;

  // gradient bitmaps
  vil1_memory_image_of<int>    image_gradx_buf;
  vil1_memory_image_of<int>    image_grady_buf;
  vil1_memory_image_of<float>    image_gradmag_buf;

  // second moment matrix of the gradient vector.
  vil1_memory_image_of<float>    image_fxx_buf;
  vil1_memory_image_of<float>    image_fxy_buf;
  vil1_memory_image_of<float>    image_fyy_buf;

  // laplacian
  vil1_memory_image_of<float>    image_laplacian_buf;

  bool **expanded;
  float **global_costs;
  vcl_pair<float, float> **pointed_neighbours;
  int **path_length;

  // region of interest
  osl_roi_window window_str;

  // The stages of the algorithm
  void prepare_buffers(int w, int h, int sx, int sy);
  void free_buffers();
  void compute_gradients(vil1_image const &);
  void compute_directions(int sx, int sy);
  vil1_image getgradmag(void);

  // return the optimum path from the free point to the seed point
  bool get_path(int free_x, int free_y,
          vcl_vector<vcl_pair<int, int> > &cor);
  bool get_processed_path(int free_x, int free_y,
          vcl_vector<vcl_pair<int, int> > &cor);
  //bool get_path(int free_x, int free_y,
  //        vcl_vector<vdgl_edgel> edgels);
  float get_global_cost(int x, int y);

  void compute_costs_from_edges(vcl_list<osl_edge*> canny_edges);
  void set_costs_from_image(vil1_image const &image);
  void set_params(dbdet_lvwr_params const & params) {
    params_ = params;
  }
  void set_canny_params(osl_canny_ox_params const & canny_params) {
    canny_params_ = canny_params;
  }

//protected:
  dbdet_lvwr_params params_;
  osl_canny_ox_params canny_params_;

private:
  void compute_gradmag (osl_roi_window      *window_str,
                      vil1_memory_image_of<float>    *image_gradmag_ptr,

                      vil1_memory_image_of<int>   *image_gradx_ptr,
                      vil1_memory_image_of<int>   *image_grady_ptr);

  void compute_laplacian(osl_roi_window      *window_str,
                      vil1_memory_image_of<float>    *image_laplacian_ptr,
                      vil1_memory_image_of<float>   *image_fxx_ptr,
                      vil1_memory_image_of<float>   *image_fyy_ptr);

  bool contains(vcl_multiset<my_pixel> *active_pixels, my_pixel p1);
  float edge_direction_cost(my_pixel p, my_pixel p1, float *dp);
  float my_get_factor(int i, int j);

};
#endif // dbdet_lvwr_h


