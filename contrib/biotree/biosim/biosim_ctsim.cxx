#include "biosim_ctsim.h"

biosim_ctsim::biosim_ctsim()
{
  pix_size_ = 10;
  src_org_d_ = 40.0;
  src_sns_d_ = 160.0;
  sns_dim_[0] = 256;
  sns_dim_[1] = 256;
  prn_pnt_[0] = sns_dim_[0]/2;
  prn_pnt_[1] = sns_dim_[1]/2;
  init_src_pos_.set(40.0, 0.0, 0.0);
  rot_axis_[0] = 0.0;
  rot_axis_[1] = 0.0;
  rot_axis_[2] = 1.0;
  rot_step_size_ = 1;
  noise_ = "no_noise";
  num_views_ = int(360.0 / rot_step_size_);
  data_.resize(sns_dim_[0], sns_dim_[1], num_views_);
  data_.fill(0.0);

//  construct_initial_camera();
  set_xscan();
}

biosim_ctsim::biosim_ctsim(double pixel_size, 
                           double source_origin_dist, 
                           double source_sensor_distance, 
                           vnl_int_2 sensor_dimensions, 
                           vnl_double_2 principal_point, 
                           vgl_point_3d<double> initial_source_position, 
                           vnl_double_3 rotation_axis,
                           double rotation_step_size,
                           vcl_string noise_type) :
pix_size_(pixel_size), src_org_d_(source_origin_dist), src_sns_d_(source_sensor_distance),
sns_dim_(sensor_dimensions), prn_pnt_(principal_point), init_src_pos_(initial_source_position),
rot_axis_(rotation_axis), rot_step_size_(rotation_step_size), noise_(noise_type)
{
  double x = init_src_pos_.x();
  double y = init_src_pos_.y();
  double z = init_src_pos_.z();
  double length = vcl_sqrt(x*x+y*y+z*z);
  assert(vcl_fabs(length - src_org_d_) < 1e-06);

  num_views_ = vcl_abs(int(360.0 / rot_step_size_));
  data_.resize(sns_dim_[0], sns_dim_[1], num_views_);
  data_.fill(0.0);

//  construct_initial_camera();
  set_xscan();
}

biosim_ctsim::biosim_ctsim(xscan_dummy_scan dummy_scan, vcl_string noise_type)
{
  pix_size_      = dummy_scan.pixel_size();
  src_org_d_     = dummy_scan.source_origin_dist();
  src_sns_d_     = dummy_scan.source_sensor_dist();
  sns_dim_       = dummy_scan.sensor_dimensions();
  prn_pnt_       = dummy_scan.principal_point();
  init_src_pos_  = dummy_scan.initial_source_position();
  rot_axis_      = dummy_scan.rotation_axis();
  rot_step_size_ = dummy_scan.rotation_step_angle() * (180/vnl_math::pi);
  noise_ = noise_type;

  num_views_ = vcl_abs(int(360.0 / rot_step_size_));
  data_.resize(sns_dim_[0], sns_dim_[1], num_views_);
  data_.fill(0.0);

//  construct_initial_camera();
  set_xscan();
}

void biosim_ctsim::write_scan(vcl_string scanfile)
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

void biosim_ctsim::set_xscan()
{
  // number of views
  scan_.set_scan_size(num_views_);
  // image file path
  scan_.set_image_file_path("dummy");
  // source
  scan_.set_source(source_);
  // calibration matrix
  vnl_double_3x3 m(0.0);
  m[0][0] = src_sns_d_*1000 / pix_size_;
  m[0][1] = 0;
  m[0][2] = prn_pnt_[0];
  m[1][1] = src_sns_d_*1000 / pix_size_;
  m[1][2] = prn_pnt_[1];
  m[2][2] = 1;
  vpgl_calibration_matrix<double> K(m);
  scan_.set_calibration_matrix(K);
  // orbit
  vnl_quaternion<double> rot(rot_axis_, -rot_step_size_ * vnl_math::pi/180);
  // translation step is zero for now (circular)
  vnl_double_3 t(0,0,0);
  // the initial camera assumed to sit on x-axis and look towards
  // the negative x-axis for now
  vnl_double_3x3 R0(0.0);
  R0[0][0] = 1;
  R0[1][2] = -1;
  R0[2][1] = 1;
  vnl_quaternion<double> r0(R0);
  vnl_double_3 ttemp(init_src_pos_.x(), init_src_pos_.y(), init_src_pos_.z());
  vnl_double_3 t0(-R0 * ttemp);
  xscan_orbit_base_sptr orbit = new xscan_uniform_orbit(rot, t,  r0, t0);
  scan_.set_orbit(orbit);
}

void biosim_ctsim::construct_initial_camera()
{
  double x = init_src_pos_.x();
  double y = init_src_pos_.y();
  double z = init_src_pos_.z();
  double length = vcl_sqrt(x*x+y*y+z*z);
  assert(vcl_fabs(length - src_org_d_) < 1e-06);

  xmvg_source source;
  vnl_double_3x3 m(0.0);
  m[0][0] = src_sns_d_*1000 / pix_size_;
  m[0][1] = 0;
  m[0][2] = prn_pnt_[0];
  m[1][1] = src_sns_d_*1000 / pix_size_;
  m[1][2] = prn_pnt_[1];
  m[2][2] = 1;

  // For now, the rotation is only around the z-axis, so the rotation
  // axis is fixed, this part will be changed later 
  vpgl_calibration_matrix<double> K(m);
  vnl_double_4x4 rot(0.0);
  rot[0][1]=1;
  rot[1][2]=-1;
  rot[2][0]=-1;
  rot[3][3]=1;
  vgl_h_matrix_3d<double> R(rot);
  vgl_homg_point_3d<double> center(init_src_pos_);
  xmvg_perspective_camera<double> cam(K, center, R, source);
  init_cam_ = cam;
}

xmvg_perspective_camera<double> biosim_ctsim::construct_camera(int index)
{
  assert(index >= 0 && index < num_views_);
  //rotate the camera
  double theta = 2 * (vnl_math::pi / num_views_) * index;
  //z axis is the rotation axis
  rot_axis_.normalize();
  vgl_h_matrix_3d<double> tr;
  tr.set_identity();
  //use minus theta since the postmultiply operation is equivalent to a rotation of 
  //the coordinate frame not the camera
  tr.set_rotation_about_axis(rot_axis_, -theta);
  xmvg_perspective_camera<double> cam_rot = xmvg_perspective_camera<double>::postmultiply(init_cam_, tr);
  return cam_rot;
}

void biosim_ctsim::write_data_3d(char* fname)
{
  FILE *fp;
  fp = vcl_fopen(fname, "wb");
  vcl_fprintf(fp, "%d %d %d\n", sns_dim_[0], sns_dim_[1], num_views_);
  for(int i=0; i < sns_dim_[0]; i++)
  {
    vcl_cout << i << vcl_endl;
    for(int j=0; j < sns_dim_[1]; j++)
    {
      for(int k=0; k < num_views_; k++)
      {
        vcl_fprintf(fp, "%f ", data_[i][j][k]);
      }
      vcl_fprintf(fp, "\n");
    }
    vcl_fprintf(fp, "\n");
  }
  vcl_fprintf(fp, "\n");
  vcl_fclose(fp);
}

void biosim_ctsim::write_data_2d(char* folder, char *file, bool normalize)
{
  assert(vcl_strlen(folder) <= 248);
  // write the info file
  FILE *fp;
  char infofile[256];
  vcl_strcpy(infofile, folder);
  vcl_strcat(infofile, "\\info.txt");
  fp = vcl_fopen(infofile, "w");
  vcl_fprintf(fp, "camera pixel size: %f um\n", pix_size_);
  vcl_fprintf(fp, "source to origin distance: %f mm\n", src_org_d_);
  vcl_fprintf(fp, "source to sensor distance: %f mm\n", src_sns_d_);
  vcl_fprintf(fp, "image rows: %d\n", sns_dim_[1]);
  vcl_fprintf(fp, "image columns: %d\n", sns_dim_[0]);
  vcl_fprintf(fp, "principal point: (%f,%f)\n", prn_pnt_[0], prn_pnt_[1]);
  vcl_fprintf(fp, "initial source position: (%f,%f,%f)\n", init_src_pos_.x(), 
                                                           init_src_pos_.y(),
                                                           init_src_pos_.z());
  vcl_fprintf(fp, "rotation axis: (%f,%f,%f)\n", rot_axis_[0], rot_axis_[1], rot_axis_[2]);
  vcl_fprintf(fp, "rotation step size angle: %f\n", rot_step_size_);
  vcl_fprintf(fp, "number of views: %d\n", num_views_);

  vcl_fclose(fp);

  char* num = new char[4];
  num[0] = num[1] = num[2] = num[3] = '0';
  char imgfileroot[512];
  vcl_strcpy(imgfileroot, folder);
  vcl_strcat(imgfileroot, "\\");
  vcl_strcat(imgfileroot, file);
/* have to adjust noise addition according to the new normalization rules
  if(noise_.compare("from_file") == 0)
  {
    // adjust to 255 since the noise file has 255 maximum
    adjust_data_range(255);
    // add noise from file
    srand(time(NULL));
    vil_image_view<vxl_byte> noise_image;
    noise_image = vil_load("bnoise.pgm");
    assert(noise_image.ni() > 0 && noise_image.nj() > 0);
    int size_x = noise_image.ni();
    int size_y = noise_image.nj();
    for(int view=0; view < num_views_; view++)
    {
      for(int j=0; j < sns_dim_[1]; j++)
      {
        for(int i=0; i < sns_dim_[0]; i++)
        {
          int x = rand() % (size_x-1);
          int y = rand() % (size_y-1);
          data_[i][j][view] += noise_image(x,y);
        }
      }
    }
  }*/

  // adjust to 65535 now
  if(normalize)
    adjust_data_range(65535);
  else
    scale_data(1000);

  vil_image_view <unsigned short> img;
  img.set_size(sns_dim_[0], sns_dim_[1]);
  img.fill(0);

  // write data to image files slice-by-slice
  for(int view=0; view < num_views_; view++)
  {
    char imgfile[512];
    increment_number_string(num, 4);
    vcl_strcpy(imgfile, imgfileroot);
    vcl_strncat(imgfile, num, 4);
    vcl_strcat(imgfile, ".pgm");
//    fp = vcl_fopen(imgfile, "w");
    vcl_fprintf(fp, "P2\n%d %d\n%d\n", sns_dim_[0], sns_dim_[1], 65535);
    for(int j=0; j < sns_dim_[1]; j++)
    {
      for(int i=0; i < sns_dim_[0]; i++)
      {
        int imval = (int)(data_[i][j][view]);
        assert(imval >= 0);
        if(imval > 65535)
          imval = 65535;
        img(i,j) = imval;
//        vcl_fprintf(fp, "%d ", int(data_[i][j][view]));
      }
//      vcl_fprintf(fp, "\n");
    }
    vil_save(img, imgfile);
  }
}

void biosim_ctsim::scan(char *fname)
{
  FILE *fp;
  fp = vcl_fopen(fname, "r");
  assert(fp != NULL);
  char dummy[80];
  vcl_fscanf(fp, "%s\n", dummy);
  assert(!vcl_strcmp(dummy, "biosim_ctsim_input"));

  double posx, posy, posz;
  double orx, ory, orz;
  double radius, length;
  double xmin, ymin, zmin, xmax, ymax, zmax;
  char start_of_line;
  char check;
  int counter = 0;
  while(1)
  {
    vcl_fscanf(fp, "%c", &start_of_line);
    if(start_of_line == 'e')
      break;
    else if(start_of_line == 'c') //this line is a cylinder input
    {
      vcl_fscanf(fp, " %c", &check);
      assert(check == '(');
      vcl_fscanf(fp, "%lf%c", &posx, &check);
      assert(check == ',');
      vcl_fscanf(fp, "%lf%c", &posy, &check);
      assert(check == ',');
      vcl_fscanf(fp, "%lf%c", &posz, &check);
      assert(check == ')');
      vcl_fscanf(fp, "%lf%lf", &radius, &length);
      vcl_fscanf(fp, " %c", &check);
      assert(check == '(');
      vcl_fscanf(fp, "%lf%c", &orx, &check);
      assert(check == ',');
      vcl_fscanf(fp, "%lf%c", &ory, &check);
      assert(check == ',');
      vcl_fscanf(fp, "%lf%c\n", &orz, &check);
      assert(check == ')');

      vgl_point_3d<double> centre(posx, posy, posz);
      vgl_vector_3d<double> orientation(orx, ory, orz);

      vcl_cout << "splatting cylinder " << ++counter << vcl_endl;

      xmvg_no_noise_filter_descriptor nnfd(radius, length, centre, orientation);
      for(int steps=0; steps < num_views_; steps++)
      {
        vcl_cout << "-";
        // construct the camera
        xmvg_perspective_camera<double> cam(scan_(steps));
        // splat
        splat_cylinder(nnfd, cam, steps);
      }
      vcl_cout << vcl_endl;
    }
    else if(start_of_line == 'r') //this line is a cuboid
    {
      fscanf(fp, "%lf %lf %lf %lf %lf %lf", &xmin, &ymin, &zmin, &xmax, &ymax, &zmax);
      vgl_box_3d<double> cuboid(xmin, ymin, zmin, xmax, ymax, zmax);
      for(int steps=0; steps < num_views_; steps++)
      {
        vcl_cout << "-";
        // construct the camera
        xmvg_perspective_camera<double> cam(scan_(steps));
        // splat
        splat_cuboid(cuboid, cam, steps);
      }
    }
  }
  fclose(fp);
}

void biosim_ctsim::adjust_data_range(int pgm_max)
{
  // find maximum value of data
  double max_val = 0;
  for(int i=0; i<this->sns_dim_[0]; i++)
  {
    for(int j=0; j<sns_dim_[1]; j++)
    {
      for(int k=0; k<num_views_; k++)
      {
        if(data_[i][j][k] > max_val)
          max_val = data_[i][j][k];
      }
    }
  }
  // adjust the data with max value to be the given value
  for(int i=0; i<sns_dim_[0]; i++)
  {
    for(int j=0; j<sns_dim_[1]; j++)
    {
      for(int k=0; k<num_views_; k++)
      {
        data_[i][j][k] *= (pgm_max/max_val);
      }
    }
  }
}

void biosim_ctsim::scale_data(double scale_factor)
{
  // scale the data with the given scale factor
  for(int i=0; i<sns_dim_[0]; i++)
  {
    for(int j=0; j<sns_dim_[1]; j++)
    {
      for(int k=0; k<num_views_; k++)
      {
        data_[i][j][k] *= scale_factor;
      }
    }
  }
}

void biosim_ctsim::splat_cylinder(xmvg_no_noise_filter_descriptor descriptor, 
                                  xmvg_perspective_camera<double> cam, int step)
{
  // translate the system to the origin, rotate it to collide the cylinder orientation with the z-axis
  vgl_h_matrix_3d<double> transformer;
  transformer.set_identity();
  transformer.set_translation(descriptor.centre().x(), descriptor.centre().y(), descriptor.centre().z());
  vnl_double_3 axis(descriptor.rotation_axis()[0], 
                    descriptor.rotation_axis()[1], 
                    descriptor.rotation_axis()[2]);
  axis.normalize();
  // first minus : the angle is from [0,0,1] to the orientation of the cylinder, but here we are
  //               rotating from the orientation to [0,0,1], so we take the minus
  // second minus: the postmultiply operation is the rotation of the coordinate frame, not
  //               the camera, so we take the minus.
  transformer.set_rotation_about_axis(axis, -(-descriptor.rotation_angle()));
  xmvg_perspective_camera<double> cam_trans = xmvg_perspective_camera<double>::postmultiply(cam, transformer);

  vgl_box_2d<double> box_2d = vpgl_project::project_bounding_box(cam_trans, descriptor.box());

  int min_x = int(vcl_floor(box_2d.min_x()));   int max_x = int(vcl_ceil(box_2d.max_x()));
  int min_y = int(vcl_floor(box_2d.min_y()));   int max_y = int(vcl_ceil(box_2d.max_y()));
  // if the splat is outside the bounding box, return immediately
  if(min_x >= sns_dim_[0] || min_y >= sns_dim_[1] || max_x < 0 || max_y < 0)
    return;
  if(min_x < 0)
    min_x = 0;
  if(min_y < 0)
    min_y = 0;
  if(max_x >= sns_dim_[0])
    max_x = sns_dim_[0] - 1;
  if(max_y >= sns_dim_[1])
    max_y = sns_dim_[1] - 1;

  unsigned u_size = max_x - min_x + 1;
  unsigned v_size = max_y - min_y + 1;
  // the variables to create an atomic_filter_2d
//  vnl_int_2 left_upper(min_x, min_y);
//  vnl_int_2 size(u_size, v_size);
  vbl_array_2d<double> image(u_size, v_size);
  for(int j=min_y; j<=max_y; j++)
  {
    for(int i=min_x; i<=max_x; i++)
    {
          vgl_homg_line_3d_2_points<double> line = cam_trans.backproject(vgl_homg_point_2d<double>(double(i), double(j), 1.0));
          vgl_point_3d<double> ray_start(line.point_finite());
          vgl_vector_3d<double> ray_direction(line.point_infinite().x(), line.point_infinite().y(), line.point_infinite().z());
          double val = line_integral_cylinder(descriptor, ray_start, ray_direction, descriptor.inner_radius());
//          image[i-min_x][j - min_y] = val;
          data_[i][j][step] += val;
    }
  }
}

void biosim_ctsim::splat_cuboid(vgl_box_3d<double> cuboid, xmvg_perspective_camera<double> cam, int step)
{
  vgl_box_2d<double> box_2d = vpgl_project::project_bounding_box(cam, cuboid);

  int min_x = int(vcl_floor(box_2d.min_x()));   int max_x = int(vcl_ceil(box_2d.max_x()));
  int min_y = int(vcl_floor(box_2d.min_y()));   int max_y = int(vcl_ceil(box_2d.max_y()));
  // if the splat is outside the bounding box, return immediately
  if(min_x >= sns_dim_[0] || min_y >= sns_dim_[1] || max_x < 0 || max_y < 0)
    return;
  if(min_x < 0)
    min_x = 0;
  if(min_y < 0)
    min_y = 0;
  if(max_x >= sns_dim_[0])
    max_x = sns_dim_[0] - 1;
  if(max_y >= sns_dim_[1])
    max_y = sns_dim_[1] - 1;

  unsigned u_size = max_x - min_x + 1;
  unsigned v_size = max_y - min_y + 1;
  vbl_array_2d<double> image(u_size, v_size);
  for(int j=min_y; j<=max_y; j++)
  {
    for(int i=min_x; i<=max_x; i++)
    {
          vgl_homg_line_3d_2_points<double> line = cam.backproject(vgl_homg_point_2d<double>(double(i), double(j), 1.0));
          vgl_point_3d<double> ray_start(line.point_finite());
          vgl_vector_3d<double> ray_direction(line.point_infinite().x(), line.point_infinite().y(), line.point_infinite().z());
          double val = line_integral_cuboid(cuboid, ray_start, ray_direction);
          data_[i][j][step] += val;
    }
  }
}

double biosim_ctsim::line_integral_cylinder(xmvg_no_noise_filter_descriptor descriptor, 
                                            vgl_point_3d<double> ray_start, vgl_vector_3d<double> ray_direction, 
                                            double radius)
{
  ray_direction = normalize(ray_direction);
  double r = radius;
  double H = descriptor.length();
  double x0 = ray_start.x();
  double y0 = ray_start.y();
  double z0 = ray_start.z();
  double x1 = ray_direction.x();
  double y1 = ray_direction.y();
  double z1 = ray_direction.z();

  double h = H/2;
  // First, we check the special case when the ray is parallel to the cylinder orientation
  if(x1 == 0 && y1 == 0)
  {
    // The ray may be passing through the cylinder
    // If the ray start is over the top tap and the ray direction is downwards OR
    // is below the bottom tap and the ray direction is upwards, then it intersects the cylinder
    if((x0*x0 + y0*y0 <= r*r) && (z0*z1 < 0))
      return H;
    else
      return 0;
  }
  // Coefficients of the equation Au^2 + Bu + C=0
  double A = x1*x1 + y1*y1;
  double B = 2*(x0*x1 + y0*y1);
  double C = x0*x0 + y0*y0 - r*r;
  double delta = B*B - 4*A*C;
  if(delta <= 0)
    return 0;
  else
  {
    double u_1 = (-B + sqrt(delta)) / (2*A);
    double u_2 = (-B - sqrt(delta)) / (2*A);
    // Sort u values
    if(u_1 > u_2)
    {
      double temp = u_1;
      u_1 = u_2;
      u_2 = temp;
    }
    // Handle another special case here where the ray direction is parallel to the cylinder taps
    if(z1 == 0)
    {
      if(vcl_abs(z0) > h)
        return 0;
      else
      {
        // Intersection points
        vgl_point_3d<double> p1 = ray_start + u_1 * ray_direction;
        vgl_point_3d<double> p2 = ray_start + u_2 * ray_direction;
        vgl_vector_3d<double> p = p1 - p2;
        return p.length();
      }
    }
    // Find the u values of taps and sort them, note that z1=0 case already handled above
    double u_tap_1 = (h - z0) / z1;
    double u_tap_2 = (-h - z0) / z1;
    if (u_tap_1 > u_tap_2)
    {
      double temp = u_tap_1;
      u_tap_1 = u_tap_2;
      u_tap_2 = temp;
    }
    // Check for different cases of intersection
    if(u_1 < u_tap_1)
    {
      if(u_2 < u_tap_1) // No intersection
        return 0;
      else if(u_2 <= u_tap_2) // Update small u
        u_1 = u_tap_1;
      else //Update both
      {
        u_1 = u_tap_1;
        u_2 = u_tap_2;
      }
    }
    else if(u_1 <= u_tap_2)
    {
      // u_2 < u_tap_1 is not a case
      if(u_2 <= u_tap_2) // Change nothing
        assert (1);
      else //Update big u
        u_2 = u_tap_2;
    }
    else // No intersection, i.e. u_1 > u_tap_2
    {
      // u_2 < u_tap_1 is not a case
      // u_2 <= u_tap_2 is not a case
      return 0;
    }
    // Intersection points
    vgl_point_3d<double> p1 = ray_start + u_1 * ray_direction;
    vgl_point_3d<double> p2 = ray_start + u_2 * ray_direction;
    vgl_vector_3d<double> p = p1 - p2;
    return p.length();
  }
}

double biosim_ctsim::line_integral_cuboid(vgl_box_3d<double> cuboid, vgl_point_3d<double> ray_start, 
                                          vgl_vector_3d<double> ray_direction)
{
  vcl_vector< vgl_point_3d <double> > intersection_points;
  vgl_point_3d<double> intersection;
  double tol = 1e-12;
  double x0 = ray_start.x(); double y0 = ray_start.y(); double z0 = ray_start.z();
  double x1 = ray_direction.x(); double y1 = ray_direction.y(); double z1 = ray_direction.z();
  double xmin = cuboid.min_x(); double xmax = cuboid.max_x();
  double ymin = cuboid.min_y(); double ymax = cuboid.max_y();
  double zmin = cuboid.min_z(); double zmax = cuboid.max_z();
  // fake cuboid is needed to check if a point is inside the cuboid with some tolerance, so fake cuboid
  // is "tol" longer on every side
  vgl_box_3d<double> fake_cuboid(xmin-tol, ymin-tol, zmin-tol, xmax+tol, ymax+tol, zmax+tol);
  //find intersection with xmin-plane
  if(vcl_fabs(x1) > tol) //otherwise line an plane are parallel
  {
    double k = (xmin - x0) / x1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_cuboid.contains(intersection))
      intersection_points.push_back(intersection);
  }
  //find intersection with ymin-plane
  if(vcl_fabs(y1) > tol) //otherwise line an plane are parallel
  {
    double k = (ymin - y0) / y1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_cuboid.contains(intersection))
      intersection_points.push_back(intersection);
  }
  //find intersection with zmin-plane
  if(vcl_fabs(z1) > tol) //otherwise line an plane are parallel
  {
    double k = (zmin - z0) / z1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_cuboid.contains(intersection))
      intersection_points.push_back(intersection);
  }
  //find intersection with xmax-plane
  if(vcl_fabs(x1) > tol) //otherwise line an plane are parallel
  {
    double k = (xmax - x0) / x1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_cuboid.contains(intersection))
      intersection_points.push_back(intersection);
  }
  //find intersection with ymax-plane
  if(vcl_fabs(y1) > tol) //otherwise line an plane are parallel
  {
    double k = (ymax - y0) / y1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_cuboid.contains(intersection))
      intersection_points.push_back(intersection);
  }
  //find intersection with zmax-plane
  if(vcl_fabs(z1) > tol) //otherwise line an plane are parallel
  {
    double k = (zmax - z0) / z1;
    intersection.set(x0+k*x1, y0+k*y1, z0+k*z1);
    if(fake_cuboid.contains(intersection))
      intersection_points.push_back(intersection);
  }
  if(intersection_points.size() == 2)
  {
    vgl_vector_3d<double> diff = intersection_points[0] - intersection_points[1];
//    vcl_cout << diff.length() << vcl_endl;
    return diff.length();
  }
  else
    return 0;
}

void increment_number_string(char *num, int length)
{
  int len = length;
  for(int i = len-1; i >= 0; i--)
  {
    num[i] += 1;
    if(num[i] == ':')
      num[i] = '0';
    else
      break;
  }
}
