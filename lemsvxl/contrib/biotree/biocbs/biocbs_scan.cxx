#include "biocbs_scan.h"
#include <vcl_cstdio.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

biocbs_scan::biocbs_scan(double sensor_pix_size,
                         double source_origin_dist, double source_sensor_dist,
                         vnl_int_2 sensor_dim, vnl_double_2 principal_point,
                         vgl_point_3d<double> init_sorce_pos, 
                         vnl_double_3 rot_axis, double rot_step_angle, 
                         int number_of_views,
                         double grid_resolution,
                         vbl_array_3d<unsigned short> *vol, 
                         vgl_point_3d<double> vol_center_pos,
                         int interp_type) :
sensor_pix_size_(sensor_pix_size), 
source_origin_dist_(source_origin_dist), source_sensor_dist_(source_sensor_dist),
sensor_dim_(sensor_dim), principal_point_(principal_point),
init_sorce_pos_(init_sorce_pos), 
rot_axis_(rot_axis), rot_step_angle_(rot_step_angle),
number_of_views_(number_of_views),
grid_resolution_(grid_resolution),
vol_center_pos_(vol_center_pos),
interp_type_(interp_type)
{
  xscan_dummy_scan scan(sensor_pix_size, source_sensor_dist, source_sensor_dist,
    sensor_dim, principal_point, init_sorce_pos, rot_axis, rot_step_angle,
    number_of_views);
  scan_ = scan;

  vol_ = vol;
  vol_dim_(0) = vol->get_row1_count();
  vol_dim_(1) = vol->get_row2_count();
  vol_dim_(2) = vol->get_row3_count();
  // compute the volume center voxel in voxel units
  vol_center_(0) = int(vcl_floor((vol_dim_(0)-1)/2.0));
  vol_center_(1) = int(vcl_floor((vol_dim_(1)-1)/2.0));
  vol_center_(2) = int(vcl_floor((vol_dim_(2)-1)/2.0));

  // compute all cameras here for efficiency
  for(int viewno = 0; viewno < number_of_views_; viewno++)
    cameras_.push_back(scan_(viewno));

  proj_data_.resize(sensor_dim_.get(0), sensor_dim_.get(1), number_of_views_);

  compute_3D_box_of_interest();
}

void biocbs_scan::project_volume(int viewno)
{
  vgl_homg_line_3d_2_points<double> line;
  vgl_point_3d<double> ray_start;
  vgl_vector_3d<double> ray_direction;
  // from ray direction to principal axis angle
  double rd_to_pa_angle;

//  viewno = 1;

//  double fill_value = 0;
//  vbl_array_2d<double> view(sensor_dim_.get(0), sensor_dim_.get(1), fill_value);

  xmvg_perspective_camera<double> cam(cameras_[viewno]);

  // the ray start point is the same (camera center) for all rays
  // hence compute it once here for efficiency
  line = cam.xmvg_backproject(vgl_homg_point_2d<double> (0.0, 0.0, 1.0));
  ray_start = (line.point_finite());
  
  for(int v = 0; v < sensor_dim_.get(1); v++)
  {
    for(int u = 0; u < sensor_dim_.get(0); u++)
    {
//      u = 100;
//      v = 100;
      line = cam.xmvg_backproject(vgl_homg_point_2d<double> (double(u), double(v), 1.0));
      ray_direction.set( line.point_infinite().x(), 
                         line.point_infinite().y(), 
                         line.point_infinite().z() );
      // if the ray direction is given in the wrong direction, 
      // i.e. not towards the image plane, but away from it
      // then it should be corrected
      double val =  dot_product(ray_direction, cam.principal_axis()) 
        / (ray_direction.length() * cam.principal_axis().length());
      if(val < -1.0)
        val = -1.0;
      else if(val > 1.0)
        val = 1.0;
      rd_to_pa_angle = vcl_acos(val);
      if(rd_to_pa_angle > vnl_math::pi_over_2)
        ray_direction = -ray_direction;

      vgl_point_3d <double> point;
      bool retval = intersect_line_and_box(box_, ray_start, ray_direction, point);
      if(retval == true)
        proj_data_(u, v, viewno) = discrete_line_integral(point, ray_direction);
    }
  }
//  write_view(view);
}

vgl_point_3d<double> biocbs_scan::grid_to_world(vnl_double_3 gp)
{
  double wp_x = vol_center_pos_.x() + (gp.get(0) - vol_center_.get(0)) * grid_resolution_ / 1000;
  double wp_y = vol_center_pos_.y() + (gp.get(1) - vol_center_.get(1)) * grid_resolution_ / 1000;
  double wp_z = vol_center_pos_.z() + (gp.get(2) - vol_center_.get(2)) * grid_resolution_ / 1000;
  return vgl_point_3d<double> (wp_x, wp_y, wp_z);
}

vnl_double_3 biocbs_scan::world_to_grid(vgl_point_3d<double> wp)
{
  double gp_x = vol_center_.get(0) + (wp.x() - vol_center_pos_.x()) * 1000 / grid_resolution_;
  double gp_y = vol_center_.get(1) + (wp.y() - vol_center_pos_.y()) * 1000 / grid_resolution_;
  double gp_z = vol_center_.get(2) + (wp.z() - vol_center_pos_.z()) * 1000 / grid_resolution_;
  return vnl_double_3(gp_x, gp_y, gp_z);
}

bool biocbs_scan::intersect_line_and_box(vgl_box_3d<double> box, vgl_point_3d<double> ray_start, 
                                         vgl_vector_3d<double> ray_direction, 
                                         vgl_point_3d <double> &point)
{
  vcl_vector< vgl_point_3d <double> > intersection_points;
  vgl_point_3d<double> intersection;
  double tol = 1e-8;
  double x0 = ray_start.x(); double y0 = ray_start.y(); double z0 = ray_start.z();
  double x1 = ray_direction.x(); double y1 = ray_direction.y(); double z1 = ray_direction.z();
  double xmin = box.min_x(); double xmax = box.max_x();
  double ymin = box.min_y(); double ymax = box.max_y();
  double zmin = box.min_z(); double zmax = box.max_z();
  // fake box is needed to check if a point is inside the box with some tolerance, so fake box
  // is "tol" longer on every side
  vgl_box_3d<double> fake_box(xmin-tol, ymin-tol, zmin-tol, xmax+tol, ymax+tol, zmax+tol);
  //find intersection with xmin-plane
  if(vcl_fabs(x1) > tol) //otherwise line an plane are parallel
  {
    double k = (xmin - x0) / x1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_box.contains(intersection))
      intersection_points.push_back(intersection);
  }
  //find intersection with ymin-plane
  if(vcl_fabs(y1) > tol) //otherwise line an plane are parallel
  {
    double k = (ymin - y0) / y1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_box.contains(intersection))
      intersection_points.push_back(intersection);
  }
  //find intersection with zmin-plane
  if(vcl_fabs(z1) > tol) //otherwise line an plane are parallel
  {
    double k = (zmin - z0) / z1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_box.contains(intersection))
      intersection_points.push_back(intersection);
  }
  //find intersection with xmax-plane
  if(vcl_fabs(x1) > tol) //otherwise line an plane are parallel
  {
    double k = (xmax - x0) / x1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_box.contains(intersection))
      intersection_points.push_back(intersection);
  }
  //find intersection with ymax-plane
  if(vcl_fabs(y1) > tol) //otherwise line an plane are parallel
  {
    double k = (ymax - y0) / y1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_box.contains(intersection))
      intersection_points.push_back(intersection);
  }
  //find intersection with zmax-plane
  if(vcl_fabs(z1) > tol) //otherwise line an plane are parallel
  {
    double k = (zmax - z0) / z1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_box.contains(intersection))
      intersection_points.push_back(intersection);
  }
  if(intersection_points.size() > 1)
  {
    double min_diff = 10000000000000.0;
    for(unsigned p=0; p < intersection_points.size(); p++)
    {
      vgl_vector_3d<double> diff = intersection_points[p] - ray_start;
      if(diff.length() < min_diff)
      {
        point = intersection_points[p];
        min_diff = diff.length();
      }
    }
    return true;
  }
  else
    return false;
}

void biocbs_scan::compute_3D_box_of_interest()
{
  box_.set_min_x(vol_center_pos_.x() - vol_center_.get(0) * grid_resolution_ / 1000);
  box_.set_min_y(vol_center_pos_.y() - vol_center_.get(1) * grid_resolution_ / 1000);
  box_.set_min_z(vol_center_pos_.z() - vol_center_.get(2) * grid_resolution_ / 1000);

  box_.set_max_x(vol_center_pos_.x() + (vol_dim_.get(0) - 1 - vol_center_.get(0)) * grid_resolution_ / 1000);
  box_.set_max_y(vol_center_pos_.y() + (vol_dim_.get(1) - 1 - vol_center_.get(1)) * grid_resolution_ / 1000);
  box_.set_max_z(vol_center_pos_.z() + (vol_dim_.get(2) - 1 - vol_center_.get(2)) * grid_resolution_ / 1000);
}

double biocbs_scan::discrete_line_integral(vgl_point_3d<double> point,
                                           vgl_vector_3d<double> ray_direction)
{
  double val = 0;
  double step_size = (grid_resolution_ / 1000) / 2;
  vgl_point_3d<double> sample_point = point;
  bool first_entrance = true;
  while(1)
  {
    vnl_double_3 sample_point_grid = world_to_grid(sample_point);
    if(interp_type_ == NO_INTERP)
    {
      int x = int(sample_point_grid.get(0)+0.5);
      int y = int(sample_point_grid.get(1)+0.5);
      int z = int(sample_point_grid.get(2)+0.5);
      if(x > vol_dim_.get(0)-1 || x < 0 ||
        y > vol_dim_.get(1)-1 || y < 0 ||
        z > vol_dim_.get(2)-1 || z < 0 )
        break;
      else
        val += vol_val(x, y, z);
    }
    else if(interp_type_ == LINEAR_INTERP)
    {
      double x = sample_point_grid.get(0);
      double y = sample_point_grid.get(1);
      double z = sample_point_grid.get(2);
      if(x > vol_dim_.get(0)-1 || x < 0 ||
        y > vol_dim_.get(1)-1 || y < 0 ||
        z > vol_dim_.get(2)-1 || z < 0 )
      {
        if(first_entrance == false)
          break;
        else
          first_entrance = false;
      }
      else
      {
        val += interpolate_subvoxel_value(vgl_point_3d<double> (x,y,z));
        first_entrance = false;
      }
    }
    else
    {
      vcl_cout << "There is no interpolation technique with the given name" << vcl_endl;
      exit(-1);
    }

    sample_point += step_size * ray_direction;
  }
  return val;
}

double biocbs_scan::interpolate_subvoxel_value(vgl_point_3d<double> subvox)
{
  double x = subvox.x();
  double y = subvox.y();
  double z = subvox.z();

  int fx = int(vcl_floor(x));
  int fy = int(vcl_floor(y));
  int fz = int(vcl_floor(z));

  int cx = int(vcl_ceil(x));
  int cy = int(vcl_ceil(y));
  int cz = int(vcl_ceil(z));

  // interpolate along x on the bottom slice
  double d1 = x - fx;
  double val1 = vol_val(fx, fy, fz) * (1-d1) + vol_val(cx, fy, fz) * d1;
  double val2 = vol_val(fx, cy, fz) * (1-d1) + vol_val(cx, cy, fz) * d1;
  // interpolate along y on the bottom slice
  double d2 = y - fy;
  double val3 = val1 * (1-d2) + val2 * d2;

  // interpolate along x on the top slice
  double val4 = vol_val(fx, fy, cz) * (1-d1) + vol_val(cx, fy, cz) * d1;
  double val5 = vol_val(fx, cy, cz) * (1-d1) + vol_val(cx, cy, cz) * d1;
  // interpolate along y on the bottom slice
  double val6 = val4 * (1-d2) + val5 * d2;

  // interpolate along the top and bottom slices
  double d3 = z - fz;
  double val7 = val3 * (1-d3) + val6 * d3;

  return val7;
}

void biocbs_scan::adjust_data_range(double adjust_val)
{
  // find maximum value of data
  double max_val = 0;
  for(int i=0; i<sensor_dim_.get(0); i++)
  {
    for(int j=0; j<sensor_dim_.get(1); j++)
    {
      for(int k=0; k<number_of_views_; k++)
      {
        double val = proj_data_(i,j,k);
        if(val > max_val)
          max_val = val;
      }
    }
  }
  // adjust the data with max value to be the given value
  double multiplier = adjust_val/max_val;
  for(int i=0; i<sensor_dim_.get(0); i++)
  {
    for(int j=0; j<sensor_dim_.get(1); j++)
    {
      for(int k=0; k<number_of_views_; k++)
      {
        proj_data_(i,j,k) *= multiplier;
      }
    }
  }
}

void biocbs_scan::write_data_3d(vcl_string fname)
{
  FILE *fp;
  fp = vcl_fopen(fname.c_str(), "w");
  vcl_fprintf(fp, "%d %d %d\n", sensor_dim_.get(0), sensor_dim_.get(1), number_of_views_);
  
  for(int k=0; k < number_of_views_; k++)
  {
    for(int j=0; j < sensor_dim_.get(1); j++)
    {
      for(int i=0; i < sensor_dim_.get(0); i++)
      {
        vcl_fprintf(fp, "%d ", int(proj_data_(i,j,k)));
      }
      vcl_fprintf(fp, "\n");
    }
    vcl_fprintf(fp, "\n");
  }
  vcl_fprintf(fp, "\n");
  vcl_fclose(fp);
}

void biocbs_scan::write_data_2d_views(vcl_string fnamebase)
{
  const char *fbase = fnamebase.c_str();
  char fname[1000];
  for(int k=0; k < number_of_views_ ; k++)
  {
    vil_image_view<unsigned short> img(sensor_dim_.get(0), sensor_dim_.get(1), 1);
    for(int j=0; j<sensor_dim_.get(1); j++)
      for(int i=0; i<sensor_dim_.get(0); i++)
        img(i,j) = (unsigned short)(proj_data_(i,j,k));
    vcl_sprintf(fname, "%s%0.4d.tif", fbase, k);
    vil_save(img, fname);
  }
}

void biocbs_scan::write_scan(vcl_string scanfile)
{
  // file extension check
  vcl_size_t dot_pos = scanfile.find_first_of(".");
  if(vcl_strcmp(scanfile.substr(dot_pos+1, 3).data(), "scn") != 0 && vcl_strcmp(scanfile.substr(dot_pos+1, 3).data(), "SCN") != 0)
  {
    vcl_cout << "************************************************" << vcl_endl;
    vcl_cout << "The specified scan file extension is not correct" << vcl_endl;
    vcl_cout << "************************************************" << vcl_endl;
    exit(-1);
  }
  vcl_ofstream scan_file(scanfile.c_str());
  scan_file << scan_;
  scan_file.close();
}

void biocbs_scan::write_box(vcl_string boxfile)
{
  // file extension check
  vcl_size_t dot_pos = boxfile.find_first_of(".");
  if(vcl_strcmp(boxfile.substr(dot_pos+1, 3).data(), "bx3") != 0 && vcl_strcmp(boxfile.substr(dot_pos+1, 3).data(), "BX3") != 0)
  {
    vcl_cout << "************************************************" << vcl_endl;
    vcl_cout << "The specified scan file extension is not correct" << vcl_endl;
    vcl_cout << "************************************************" << vcl_endl;
    exit(-1);
  }
  vcl_ofstream box_file(boxfile.c_str());
  box_file << box_;
  box_file.close();
}

void biocbs_scan::write_view(vbl_array_2d<double> view)
{
  FILE *fp = vcl_fopen("D:\\MyDocs\\Temp\\view.pgm", "w");
  vcl_fprintf(fp, "P2\n%d %d\n65535\n", sensor_dim_.get(0), sensor_dim_.get(1));
  for(int v = 0; v < sensor_dim_.get(1); v++)
  {
    for(int u = 0; u < sensor_dim_.get(0); u++)
    {
      vcl_fprintf(fp, "%d ", int(view(u,v)));
    }
    vcl_fprintf(fp, "\n");
  }
  vcl_fclose(fp);
}
