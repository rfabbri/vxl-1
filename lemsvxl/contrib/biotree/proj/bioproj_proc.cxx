#include "bioproj_proc.h"
#include <vcl_fstream.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/algo/vpgl_project.h>
#include <vnl/vnl_math.h>
#include <vil/algo/vil_convolve_1d.h>
#include <vcl_cstdio.h>

bioproj_proc::bioproj_proc(bioproj_io *proj_io)
:proj_io_(proj_io)
{
  structures_created = false;
}

void bioproj_proc::execute(int view_filter, int spatial_filter, vcl_string outfile)
{
  view_filter_type_ = view_filter;
  spatial_filter_type_ = spatial_filter;

  // in case of computing multiple responses using the same proc class, we do not need to
  // do some computations again and again, also box enlargement should only be done once
  if(structures_created == false)
  {
    // compute all cameras here for efficiency
    for(unsigned viewno = 0; viewno < proj_io_->nviews_; viewno++)
      cameras_.push_back(proj_io_->scan_(viewno));
    
    // get the first camera and project the 3D box
    xmvg_perspective_camera<double> first_cam = cameras_[0];

    // extend the 3D box depending on the size of sigma_z
    int twice_extent_z = 2 * proj_io_->sigma_z_extent_;
    proj_io_->box_.set_depth(proj_io_->box_.depth() + twice_extent_z * proj_io_->sigma_z_ / 1000 );
  
    vgl_box_2d<double> box_2d = vpgl_project::project_bounding_box(first_cam, proj_io_->box_);
    // find the range of (xi,theta)-planes needed for the process
    min_plane_index_ = int(vcl_floor(box_2d.min_y())) - proj_io_->num_comp_planes_;
    max_plane_index_ = int(vcl_ceil(box_2d.max_y())) + proj_io_->num_comp_planes_;

    if(min_plane_index_ < 0 || max_plane_index_ >= proj_io_->sensor_dim_.get(1))
      print_error_message(1);
    if(int(vcl_floor(box_2d.min_x())) < 0 || int(vcl_ceil(box_2d.max_x())) >= proj_io_->sensor_dim_.get(0))
      print_error_message(2);

    // calculate the number of grid points
    grid_w_ = int(vcl_ceil(1000*proj_io_->box_.width() / proj_io_->resolution_));
    grid_h_ = int(vcl_ceil(1000*proj_io_->box_.height() / proj_io_->resolution_));
    grid_d_ = int(vcl_ceil(1000*proj_io_->box_.depth() / proj_io_->resolution_));

    coord_x_.resize(grid_w_, grid_h_, grid_d_);
    coord_y_.resize(grid_w_, grid_h_, grid_d_);
    coord_z_.resize(grid_w_, grid_h_, grid_d_);
    // populate the grid point coordinates
    for(int k = 0; k < grid_d_; k++)
    {
      for(int j = 0; j < grid_h_; j++)
      {
        for(int i = 0; i < grid_w_; i++)
        {
          double coord_x = proj_io_->box_.min_x() + i * proj_io_->resolution_ / 1000;
          double coord_y = proj_io_->box_.min_y() + j * proj_io_->resolution_ / 1000;
          double coord_z = proj_io_->box_.min_z() + k * proj_io_->resolution_ / 1000;
          coord_x_(i,j,k) = coord_x;
          coord_y_(i,j,k) = coord_y;
          coord_z_(i,j,k) = coord_z;
        }
      }
    }
  }

  // populate the data class (this has to be done every time since the object becomes local and
  // gets deleted if we out it inside the if statement above)
  bioproj_data proj_data(proj_io_->file_path_, min_plane_index_, max_plane_index_, 
    proj_io_->nviews_, proj_io_->sensor_dim_.get(0), proj_io_->sensor_dim_.get(1),
    grid_w_, grid_h_, grid_d_);
  proj_data_ = &proj_data;

  view_space_ops_with_fixed_magnification();
  
  // compute responses step-1
  for(int k = 0; k < grid_d_; k++)
  {
    for(int j = 0; j < grid_h_; j++)
    {
      for(int i = 0; i < grid_w_; i++)
      {
        double coord_x = coord_x_(i,j,k);
        double coord_y = coord_y_(i,j,k);
        double coord_z = coord_z_(i,j,k);
//        vcl_cout << coord_x << " " << coord_y << " " << coord_z << vcl_endl;
        double resp = response_at_point(vgl_point_3d<double>(coord_x, coord_y, coord_z));
        proj_data_->grid_(i,j,k) = resp;
      }
    }
  }
  // compute responses step-2
  apply_z_filtering();

  write_result(outfile);

  structures_created = true;
}

void bioproj_proc::view_space_ops_with_fixed_magnification()
{
  // since the magnification factor is fixed, we will convolve each xi-row and
  // store the result as the first step, hence get memory to store 
  // the convolution result
  proj_data_->convolved_.set_size(proj_data_->planes_width_, 
                                  proj_data_->planes_height_, 
                                  proj_data_->planes_depth_);
  // create the filters and do the convolution
  double m = proj_io_->source_sensor_dist_ / proj_io_->source_origin_dist_; //magnification factor
  double sigma = proj_io_->sigma_r_ * m / proj_io_->sensor_pix_size_;
  int extent = proj_io_->sigma_r_extent_;
  double theta = 0.0;
  double theta_step = proj_io_->rot_step_angle_ * vnl_math::pi / 180;

  double *xi_row;
  double *xi_row_convolved;
  xi_row = new double[proj_data_->planes_height_];
  xi_row_convolved = new double[proj_data_->planes_height_];

  // loop over views
  for(unsigned viewno = 0; viewno < proj_io_->nviews_; viewno++)
  {
    vcl_cout << viewno << " of " << proj_io_->nviews_-1 << vcl_endl;

    double *filter_;
    int kernel_center;
    // create filter
    if(view_filter_type_ == 0)
    {
      bioproj_nu_g_filter filter(sigma, extent, 1);
      kernel_center = filter.half_kernel_size_;
      filter_ = new double[2*kernel_center+1];
      vcl_memcpy(filter_, filter.filter_, sizeof(double)*filter.full_kernel_size_);
    }
    else if(view_filter_type_ == 1)
    {
      bioproj_nu_gx_filter filter(sigma, theta, extent, 1);
      kernel_center = filter.half_kernel_size_;
      filter_ = new double[2*kernel_center+1];
      vcl_memcpy(filter_, filter.filter_, sizeof(double)*filter.full_kernel_size_);
    }
    else if(view_filter_type_ == 2)
    {
      bioproj_nu_gy_filter filter(sigma, theta, extent, 1);
      kernel_center = filter.half_kernel_size_;
      filter_ = new double[2*kernel_center+1];
      vcl_memcpy(filter_, filter.filter_, sizeof(double)*filter.full_kernel_size_);
    }
    else if(view_filter_type_ == 3)
    {
      bioproj_nu_gxx_filter filter(sigma, theta, extent);
      kernel_center = filter.half_kernel_size_;
      filter_ = new double[2*kernel_center+1];
      vcl_memcpy(filter_, filter.filter_, sizeof(double)*filter.full_kernel_size_);
    }
    else if(view_filter_type_ == 4)
    {
      bioproj_nu_gxy_filter filter(sigma, theta, extent);
      kernel_center = filter.half_kernel_size_;
      filter_ = new double[2*kernel_center+1];
      vcl_memcpy(filter_, filter.filter_, sizeof(double)*filter.full_kernel_size_);
    }
    else if(view_filter_type_ == 5)
    {
      bioproj_nu_gyy_filter filter(sigma, theta, extent);
      kernel_center = filter.half_kernel_size_;
      filter_ = new double[2*kernel_center+1];
      vcl_memcpy(filter_, filter.filter_, sizeof(double)*filter.full_kernel_size_);
    }
    else
      vcl_cout << "FILTER NOT DEFINED!!!" << vcl_endl;

    // loop over view data rows
    for(int k = 0; k < proj_data_->planes_depth_; k++)
    {
      // get the row
      for(int j = 0; j < proj_data_->planes_height_; j++)
        xi_row[j] = double(proj_data_->planes_(viewno, j, k));
      // convolve it
      vil_convolve_1d<double,double,double,double>(&xi_row[0], proj_data_->planes_height_, 1, 
                                                   &xi_row_convolved[0], 1, 
                                                   &(filter_[kernel_center]), -kernel_center, kernel_center,
                                                   double(), vil_convolve_zero_extend, vil_convolve_zero_extend);
      // store the convolution result
      for(int j = 0; j < proj_data_->planes_height_; j++)
         proj_data_->convolved_(viewno, j, k) = xi_row_convolved[j];
    }
    theta += theta_step;
    delete []filter_;
  }
  delete []xi_row;
  delete []xi_row_convolved;
}

double bioproj_proc::response_at_point(vgl_point_3d<double> p)
{
//  FILE *fp = vcl_fopen("D:\\MyDocs\\Temp\\points.txt", "w");
  vgl_homg_point_3d<double> hp(p);
  double x0 = p.x(); double y0 = p.y(); double z0 = p.z();
  double theta = 0;
  double theta_step = proj_io_->rot_step_angle_ * vnl_math::pi / 180;
  int min_plane_index_ = proj_data_->min_plane_;
  double resp = 0;
  for(unsigned viewno = 0; viewno < proj_io_->nviews_; viewno++)
  {
    xmvg_perspective_camera<double> cam = cameras_[viewno];
    vgl_point_2d<double> pp(cam.project(hp));
//    vcl_fprintf(fp, "%f\n",pp.x());
    double xi = x0 * vcl_cos(theta) + y0 * vcl_sin(theta);
//    vcl_fprintf(fp, "%f\n", xi);
    resp += interpolate_convolved_data_at_point(vgl_point_2d<double>(pp.x(), pp.y()-min_plane_index_), viewno);
//    resp += interpolate_convolved_data_at_point(vgl_point_2d<double>(xi*1000/proj_io_->sensor_pix_size_+99, 
//                                                                     pp.y()-min_plane_index_), 
//                                                                     viewno);
    theta += theta_step;
  }
//  vcl_fclose(fp);
  return resp;
}

double bioproj_proc::interpolate_convolved_data_at_point(vgl_point_2d<double> p, int viewno)
{
  // linear interpolation in (theta, xi, z) coordinate system
  double xi = p.x(); double z = p.y();
  int floor_xi = int(vcl_floor(xi)); int ceil_xi = int(vcl_ceil(xi));
  int floor_z = int(vcl_floor(z)); int ceil_z = int(vcl_ceil(z));

  double d1 = xi - floor_xi;
  double d2 = z - floor_z;

  double val1 = proj_data_->convolved_(viewno, floor_xi, floor_z);
  double val2 = proj_data_->convolved_(viewno, ceil_xi, floor_z);
  double val3 = proj_data_->convolved_(viewno, floor_xi, ceil_z);
  double val4 = proj_data_->convolved_(viewno, ceil_xi, ceil_z);

  double temp1 = val1 * (1-d1) + val2 * d1;
  double temp2 = val3 * (1-d1) + val4 * d1;

  double fin = temp1 * (1-d2) + temp2 * d2;

  return fin;
}

void bioproj_proc::apply_z_filtering()
{
  int num_grid_points = proj_data_->grid_w_ * proj_data_->grid_h_;
  int index = 1;
  int extent = proj_io_->sigma_z_extent_;

  double *filter_;
  int kernel_center;

  if(spatial_filter_type_ == 0)
  {
    bioproj_g_filter z_filter(proj_io_->sigma_z_/proj_io_->resolution_, extent);
    kernel_center = z_filter.half_kernel_size_;
    filter_ = new double[2*kernel_center+1];
    vcl_memcpy(filter_, z_filter.filter_, sizeof(double)*z_filter.full_kernel_size_);
  }
  else if(spatial_filter_type_ == 1)
  {
    bioproj_gz_filter z_filter(proj_io_->sigma_z_/proj_io_->resolution_, extent);
    kernel_center = z_filter.half_kernel_size_;
    filter_ = new double[2*kernel_center+1];
    vcl_memcpy(filter_, z_filter.filter_, sizeof(double)*z_filter.full_kernel_size_);
  }
  else if(spatial_filter_type_ == 2)
  {
    bioproj_gzz_filter z_filter(proj_io_->sigma_z_/proj_io_->resolution_, extent);
    kernel_center = z_filter.half_kernel_size_;
    filter_ = new double[2*kernel_center+1];
    vcl_memcpy(filter_, z_filter.filter_, sizeof(double)*z_filter.full_kernel_size_);
  }
  else
  {
    vcl_cout << "NOT DEFINED!!!" << vcl_endl;
  }

  double *z_line;
  double *z_line_convolved;
  z_line = new double[proj_data_->grid_d_];
  z_line_convolved = new double[proj_data_->grid_d_];
  for(int j = 0; j < proj_data_->grid_h_; j++)
  {
    for(int i = 0; i < proj_data_->grid_w_; i++)
    {
      // collect the values
      for(int k = 0; k < proj_data_->grid_d_; k++)
        z_line[k] = proj_data_->grid_(i, j, k);

      // convolve it
      vil_convolve_1d<double,double,double,double>(&z_line[0], proj_data_->grid_d_, 1, 
                                                   &z_line_convolved[0], 1, 
                                                   &(filter_[kernel_center]), -kernel_center, kernel_center,
                                                   double(), vil_convolve_zero_extend, vil_convolve_zero_extend);

      // put the values back
      for(int k = 0; k < proj_data_->grid_d_; k++)
        proj_data_->grid_(i, j, k) = z_line_convolved[k];

      vcl_cout << index++ / double(num_grid_points) * 100.0 << "% of z convolution finished" << vcl_endl;
    }
  }
  delete [] z_line;
  delete [] z_line_convolved;
  delete [] filter_;
}

void bioproj_proc::write_result(vcl_string outfile)
{
  int grid_w = proj_data_->grid_w_;
  int grid_h = proj_data_->grid_h_;
  int grid_d = proj_data_->grid_d_;

  int num_z_cut = int(vcl_ceil(proj_io_->sigma_z_extent_ * proj_io_->sigma_z_ / proj_io_->resolution_));

  FILE *fp = vcl_fopen(outfile.c_str(), "w");
  vcl_fprintf(fp, "%d %d %d\n", grid_w, grid_h, grid_d - 2*num_z_cut);
  for(int k = num_z_cut; k < grid_d-num_z_cut; k++)
  {
    for(int j = 0; j < grid_h; j++)
    {
      for(int i = 0; i < grid_w; i++)
      {
        vcl_fprintf(fp, "%f ", proj_data_->grid_(i,j,k));
//        vcl_cout << coord_x_(i,j,k) << vcl_endl;
//        vcl_cout << coord_y_(i,j,k) << vcl_endl;
//        vcl_cout << coord_z_(i,j,k) << vcl_endl;
      }
      vcl_fprintf(fp, "\n");
    }
    vcl_fprintf(fp, "\n");
  }
  vcl_fclose(fp);
}

void bioproj_proc::print_error_message(int error_code)
{
  vcl_string error;
  if(error_code == 1)
  {
    error = error + "The specified 3D box is extended in the world z-direction. ";
    error = error + "The extended box is projected using the first camera of the scan. ";
    error = error + "The projected 2D box is extended in v-direction to compensate for ";
    error = error + "the wobble effect of the scan setup. Ideally, there would be no ";
    error = error + "wobble, and we would not need to extend the projected box. ";
    error = error + "If the extended 2D box boundaries in v-direction goes out of the sensor, ";
    error = error + "this error is encountered. There are three possible causes for this:\n";
    error = error + "1. The 3D box does not project completely on the sensor.\n";
    error = error + "2. The z filter sigma or extent or the combination of two is large.\n";
    error = error + "3. The 2D box extension value is large.\n";
  }
  else if(error_code == 2)
    error = error + "The projection of 3D box falls outside the sensor in u-direction of the sensor.\n";
  else
    error = error + "Unrecognized error code, please notify the author.\n";

  vcl_cout << error << vcl_endl;

  exit(-1);
}
