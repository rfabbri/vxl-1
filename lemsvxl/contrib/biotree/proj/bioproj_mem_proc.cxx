#include "bioproj_mem_proc.h"
#include <vcl_fstream.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/algo/vpgl_project.h>
#include <vnl/vnl_math.h>
#include <vil/algo/vil_convolve_1d.h>
#include <vcl_cstdio.h>
#include <vcl_ctime.h>
#include <vil/vil_image_resource_sptr.h>
#include <vul/vul_timer.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_resample_bilin.h>
#include <vil3d/vil3d_math.h>

bioproj_mem_proc::bioproj_mem_proc(bioproj_mem_io *proj_mem_io)
:proj_mem_io_(proj_mem_io)
{
  res_x_ = (proj_mem_io_->box_.max_x() - proj_mem_io_->box_.min_x()) / (proj_mem_io_->grid_w_ - 1);
  res_y_ = (proj_mem_io_->box_.max_y() - proj_mem_io_->box_.min_y()) / (proj_mem_io_->grid_h_ - 1);
  if(proj_mem_io_->grid_d_ != 1)
    res_z_ = (proj_mem_io_->box_.max_z() - proj_mem_io_->box_.min_z()) / (proj_mem_io_->grid_d_ - 1);
  else
    res_z_ = proj_mem_io_->box_.max_z() - proj_mem_io_->box_.min_z();
  
}

template<class T>
inline void invert_given_i_zero(vil_image_view<T>& imA, const float& i_zero)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  vcl_ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  T* planeA = imA.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA)
  {
     T* rowA   = planeA;
    for (unsigned j=0;j<nj;++j,rowA += jstepA)
    {
       T* pixelA = rowA;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA)
        *pixelA = T(i_zero)-T(*pixelA);
    }
  }
}


//: execute the algorithm
bool bioproj_mem_proc::execute(int view_filter, int spatial_filter, int resize_factor)
{
  view_filter_type_ = view_filter;
  spatial_filter_type_ = spatial_filter;
  resize_factor_ = resize_factor;
  assert(resize_factor_ == 1 || resize_factor_ == 2 || resize_factor_ == 4 || resize_factor_ == 8);

  // compute all cameras here for efficiency
  for(unsigned viewno = 0; viewno < proj_mem_io_->nviews_; viewno++)
    cameras_.push_back(proj_mem_io_->scan_(viewno));

  proj_mem_io_->grid_.fill(0);
  double theta_step = proj_mem_io_->rot_step_angle_ * vnl_math::pi / 180;
  // for each view
  vul_timer timer;
  for(unsigned viewno = 0; viewno < proj_mem_io_->nviews_; viewno++)
  {
    vcl_cout << "Processing " << viewno << vcl_endl;
    timer.mark();

    if(!proj_mem_io_->found_filenames()){
            vcl_cerr << "Error: Found no filenames\n";
            return 1;
    }
    // get the view
    vil_image_resource_sptr resc  = vil_load_image_resource(proj_mem_io_->filenames_[viewno].c_str());
    if(!resc){
            vcl_cerr << "unable to load image resource from proj_mem_io_->filenames_[viewno]\n";
            return 0;
    }
    if(resc->pixel_format() == VIL_PIXEL_FORMAT_UINT_16){
      vil_image_view<vxl_uint_16> temp_view = resc->get_view();
      vil_convert_cast(temp_view,temp_view_double_);
    }
    else if(resc->pixel_format() == VIL_PIXEL_FORMAT_FLOAT){
      vil_image_view<float> temp_view = resc->get_view();
      vil_convert_cast(temp_view,temp_view_double_);
    }
    else{
      vcl_cerr << "ERROR: Can't handle pixel format\n"; return 0;
    }
    invert_given_i_zero(temp_view_double_,proj_mem_io_->i_zero_);

    if(resize_factor_ != 1)
    {
      int sensor_w = proj_mem_io_->sensor_dim_.get(0);
      int sensor_h = proj_mem_io_->sensor_dim_.get(1);
      int new_sensor_w = sensor_w + (resize_factor_-1) * (sensor_w-1);
      int new_sensor_h = sensor_h + (resize_factor_-1) * (sensor_h-1);
      vil_resample_bilin(temp_view_double_, current_view_, new_sensor_w, new_sensor_h);
    }
    else{
      current_view_ = temp_view_double_;
    }
    // do the convolution on the view
    convolve_current_view(theta_step * viewno, viewno);
    // accumulate the responses for the grid points from this convolved view
    accumulate_responses_from_current_convolved_view(viewno);
    timer.print(vcl_cout);
  }
  // multiply accumulated values by the coefficent in front of the integral     
  vil3d_math_scale_and_offset_values(proj_mem_io_->grid_,float(1.0/(2*proj_mem_io_->nviews_)),0);
  apply_z_filtering();
  return 1;
}

void bioproj_mem_proc::convolve_current_view(double theta, int viewno)
{
  double increment = 1.0 / resize_factor_;

  double m = proj_mem_io_->source_sensor_dist_ / proj_mem_io_->source_origin_dist_; //magnification factor
  double sigma = proj_mem_io_->sigma_r_ * m / proj_mem_io_->sensor_pix_size_;
  int extent = proj_mem_io_->sigma_r_extent_;

  double *filter_;
  int kernel_center;

  double *xi_row;
  double *xi_row_convolved;
  xi_row = new double[current_view_.ni()];
  xi_row_convolved = new double[current_view_.ni()];

  if(view_filter_type_ == 0)
  {
    bioproj_nu_g_filter filter(sigma, extent, increment);
    kernel_center = filter.half_kernel_size_;
    filter_ = new double[2*kernel_center+1];
    vcl_memcpy(filter_, filter.filter_, sizeof(double)*filter.full_kernel_size_);
  }
  else if(view_filter_type_ == 1)
  {
    bioproj_nu_gx_filter filter(sigma, theta, extent, increment);
    kernel_center = filter.half_kernel_size_;
    filter_ = new double[2*kernel_center+1];
    vcl_memcpy(filter_, filter.filter_, sizeof(double)*filter.full_kernel_size_);
  }
  else if(view_filter_type_ == 2)
  {
    bioproj_nu_gy_filter filter(sigma, theta, extent, increment);
    kernel_center = filter.half_kernel_size_;
    filter_ = new double[2*kernel_center+1];
    vcl_memcpy(filter_, filter.filter_, sizeof(double)*filter.full_kernel_size_);
  }
  else
    vcl_cout << "PROJECTED FILTER NOT DEFINED!!!" << vcl_endl;

  // for time efficiency
  xmvg_perspective_camera<double> cam = cameras_[viewno];
  vgl_box_2d<double> box_2d = vpgl_project::project_bounding_box(cam, proj_mem_io_->box_);

  int u_min = int(vcl_floor(box_2d.min_x())); int u_max = int(vcl_ceil(box_2d.max_x()));
  int v_min = int(vcl_floor(box_2d.min_y())); int v_max = int(vcl_ceil(box_2d.max_y()));
  int u,v;
  if(resize_factor_ == 1)
  {
    u = proj_mem_io_->sensor_dim_.get(0); // sensor width
    v = proj_mem_io_->sensor_dim_.get(1); // sensor height
  
    if(v_min < 0) v_min = 0;
    if(v_max > v) v_max = v-1;
  }
  else
  {
    u = current_view_.ni(); // magnified sensor width
    v = current_view_.nj(); // magnified sensor height

    v_min *= resize_factor_;
    v_max *= resize_factor_;

    if(v_min < 0) v_min = 0;
    if(v_max > v) v_max = v-1;
  }

  current_view_convolved_.set_size(u, v);
  // loop over view data rows
  for(int j = v_min; j <= v_max; j++)
  {
    // get the row
    for(int i = 0; i < u; i++)
      xi_row[i] = double(current_view_(i, j));
    // convolve it
    vil_convolve_1d<double,double,double,double>(&xi_row[0], current_view_.ni(), 1, 
      &xi_row_convolved[0], 1, 
      &(filter_[kernel_center]), -kernel_center, kernel_center,
      double(), vil_convolve_zero_extend, vil_convolve_zero_extend);
    // store the convolution result
    for(int i = 0; i < u; i++)
      current_view_convolved_(i, j) = xi_row_convolved[i];
  }
  delete []xi_row;
  delete []xi_row_convolved;
  delete []filter_;
}

void bioproj_mem_proc::accumulate_responses_from_current_convolved_view(int viewno)
{
  int w = proj_mem_io_->grid_w_;
  int h = proj_mem_io_->grid_h_;
  int d = proj_mem_io_->grid_d_;
  xmvg_perspective_camera<double> cam = cameras_[viewno];

  for(int k=0; k < d; k++)
  {
    for(int j=0; j < h; j++)
    {
      for(int i=0; i < w; i++)
      {
        double px = proj_mem_io_->box_.min_x() + i * res_x_;
        double py = proj_mem_io_->box_.min_y() + j * res_y_;
        double pz = proj_mem_io_->box_.min_z() + k * res_z_;
        vgl_point_3d<double> point(px, py, pz);
        vgl_homg_point_3d<double> homg_point(point);
        vgl_point_2d<double> proj_point(cam.project(homg_point));
        if(resize_factor_ != 1)
        {
          proj_point.x() *= resize_factor_;
          proj_point.y() *= resize_factor_;
        }
        // interpolate and add to the accumulated result
        proj_mem_io_->grid_(i,j,k) += float(interpolate_convolved_data_at_point(proj_point));
      }
    }
  }
}

double bioproj_mem_proc::interpolate_convolved_data_at_point(vgl_point_2d<double> p)
{
  double x = p.x();
  double y = p.y();
  double val1 = 0;
  double val2 = 0;
  double val3 = 0;
  double val4 = 0;
  
  int floor_x = int(vcl_floor(x)); int ceil_x = int(vcl_ceil(x));
  int floor_y = int(vcl_floor(y)); int ceil_y = int(vcl_ceil(y));

  double d1 = x - floor_x;
  double d2 = y - floor_y;

  int width = current_view_convolved_.ni();
  int height = current_view_convolved_.nj();

  if(floor_x >= 0 && floor_y >= 0 && floor_x < width && floor_y < height)
    val1 = current_view_convolved_(floor_x, floor_y);
  if(ceil_x >= 0 && floor_y >= 0 && ceil_x < width && floor_y < height)
    val2 = current_view_convolved_(ceil_x, floor_y);
  if(floor_x >= 0 && ceil_y >= 0 && floor_x < width && ceil_y < height)
    val3 = current_view_convolved_(floor_x, ceil_y);
  if(ceil_x >= 0 && ceil_y >= 0 && ceil_x < width && ceil_y < height)
    val4 = current_view_convolved_(ceil_x, ceil_y);

  double temp1 = val1 * (1-d1) + val2 * d1;
  double temp2 = val3 * (1-d1) + val4 * d1;

  double fin = temp1 * (1-d2) + temp2 * d2;

  return fin;
}

void bioproj_mem_proc::apply_z_filtering()
{
  int num_grid_points = proj_mem_io_->grid_w_ * proj_mem_io_->grid_h_;
  int index = 1;
  int extent = proj_mem_io_->sigma_z_extent_;

  double *filter_;
  int kernel_center;

  if(spatial_filter_type_ == 0)
  {
    bioproj_g_filter z_filter(proj_mem_io_->sigma_z_/(1000*res_z_), extent);
    kernel_center = z_filter.half_kernel_size_;
    filter_ = new double[2*kernel_center+1];
    vcl_memcpy(filter_, z_filter.filter_, sizeof(double)*z_filter.full_kernel_size_);
  }
  else if(spatial_filter_type_ == 1)
  {
    bioproj_gz_filter z_filter(proj_mem_io_->sigma_z_/(1000*res_z_), extent);
    kernel_center = z_filter.half_kernel_size_;
    filter_ = new double[2*kernel_center+1];
    vcl_memcpy(filter_, z_filter.filter_, sizeof(double)*z_filter.full_kernel_size_);
  }
  else if(spatial_filter_type_ == 2)
  {
    bioproj_gzz_filter z_filter(proj_mem_io_->sigma_z_/(1000*res_z_), extent);
    kernel_center = z_filter.half_kernel_size_;
    filter_ = new double[2*kernel_center+1];
    vcl_memcpy(filter_, z_filter.filter_, sizeof(double)*z_filter.full_kernel_size_);
  }
  else
    vcl_cout << "SPATIAL FILTER NOT DEFINED!!!" << vcl_endl;

  double *z_line;
  double *z_line_convolved;
  z_line = new double[proj_mem_io_->grid_d_];
  z_line_convolved = new double[proj_mem_io_->grid_d_];
  for(int j = 0; j < proj_mem_io_->grid_h_; j++)
  {
    for(int i = 0; i < proj_mem_io_->grid_w_; i++)
    {
      // collect the values
      for(int k = 0; k < proj_mem_io_->grid_d_; k++)
        z_line[k] = proj_mem_io_->grid_(i, j, k);

      // convolve it
      vil_convolve_1d<double,double,double,double>(&z_line[0], proj_mem_io_->grid_d_, 1, 
        &z_line_convolved[0], 1, 
        &(filter_[kernel_center]), -kernel_center, kernel_center,
        double(), vil_convolve_zero_extend, vil_convolve_zero_extend);

      // put the values back
      for(int k = 0; k < proj_mem_io_->grid_d_; k++)
        proj_mem_io_->grid_(i, j, k) = float(z_line_convolved[k]);

    }
      vcl_cout << index++ / double(num_grid_points) * 100.0 << "% of z convolution finished" << vcl_endl;
  }
  delete [] z_line;
  delete [] z_line_convolved;
  delete [] filter_;
}

void bioproj_mem_proc::write_result(vcl_string outfile)
{
        vcl_cerr << "write_result deprecated; access proj_mem_io_->grid in calling function\n";
}
