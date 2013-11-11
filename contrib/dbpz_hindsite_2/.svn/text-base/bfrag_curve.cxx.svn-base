#include "bfrag_curve.h"
#include "extern_params.h"
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbgl/algo/dbgl_curve_smoothing.h>
#include <dbsol/algo/dbsol_corner_finder.h>

bfrag_curve::bfrag_curve()
{
  transform_.set_identity();
  is_open_ = true;

  level1_.clear();
  level2_.clear();
  level3_.clear();
  level4_.clear();

  arclength_.clear();
  cum_arclength_.clear();
  angle_.clear();
  length_ = 0.0;
}

bfrag_curve::~bfrag_curve()
{
}

bfrag_curve::bfrag_curve(vcl_vector<vgl_point_2d<double> > &points, bool is_open)
{
  transform_.set_identity();
  is_open_ = is_open;
  for (unsigned i=0; i < points.size(); i++)
  {
    level3_.push_back(points[i]);
    level4_.push_back(points[i]);
  }
  compute_properties();
}

void bfrag_curve::write_out(vcl_ofstream &out)
{
  unsigned size;

  out << "START_DUMPING_BFRAG_CURVE" << vcl_endl;
  out << "is_open_: " << (is_open_ != 0) << vcl_endl;
  out << "box_: "; box_.write(out);
  out << "frag_id_: " << frag_id_ << vcl_endl;
  out << "transform_: " << vcl_endl;
  transform_.print(out);

  size = level1_.size();
  out << "level1_ " << size << vcl_endl;
  for(unsigned i=0; i<size; i++)
    out << level1_[i] << vcl_endl;
  
  size = level2_.size();
  out << "level2_ " << size << vcl_endl;
  for(unsigned i=0; i<size; i++)
    out << level2_[i].x() << " " << level2_[i].y() << vcl_endl;
  
  size = coarse_fine_corr_.size();
  out << "coarse_fine_corr_ " << size << vcl_endl;
  for(unsigned i=0; i<size; i++)
    out << coarse_fine_corr_[i] << vcl_endl;
  
  size = level3_.size();
  out << "level3_ " << size << vcl_endl;
  for(unsigned i=0; i<size; i++)
    out << level3_[i].x() << " " << level3_[i].y() << vcl_endl;
  
  size = level4_.size();
  out << "level4_ " << size << vcl_endl;
  for(unsigned i=0; i<size; i++)
    out << level4_[i].x() << " " << level4_[i].y() << vcl_endl;

  size = arclength_.size();
  out << "arclength_ " << size << vcl_endl;
  for(unsigned i=0; i<size; i++)
    out << arclength_[i] << vcl_endl;

  size = cum_arclength_.size();
  out << "cum_arclength_ " << size << vcl_endl;
  for(unsigned i=0; i<size; i++)
    out << cum_arclength_[i] << vcl_endl;

  size = angle_.size();
  out << "angle_ " << size << vcl_endl;
  for(unsigned i=0; i<size; i++)
    out << angle_[i] << vcl_endl;

  out << "length_: " << length_ << vcl_endl;

  out << "isTop: " << (isTop != 0) << vcl_endl;
  out << "END_DUMPING_BFRAG_CURVE" << vcl_endl;
}

void bfrag_curve::read_in(vcl_ifstream &in)
{
  unsigned size;
  vcl_string dummy;

  in >> dummy;
  assert(dummy == "START_DUMPING_BFRAG_CURVE");

  in >> dummy;
  assert(dummy == "is_open_:");
  in >> is_open_;
  
  in >> dummy;
  assert(dummy == "box_:");
  box_.read(in);

  in >> dummy;
  assert(dummy == "frag_id_:");
  in >> frag_id_;

  in >> dummy;
  assert(dummy == "transform_:");
  in >> transform_[0][0]; in >> transform_[0][1]; in >> transform_[0][2];
  in >> transform_[1][0]; in >> transform_[1][1]; in >> transform_[1][2];
  in >> transform_[2][0]; in >> transform_[2][1]; in >> transform_[2][2];

  in >> dummy;
  assert(dummy == "level1_");
  in >> size;
  level1_.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> level1_[i];

  in >> dummy;
  assert(dummy == "level2_");
  in >> size;
  level2_.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> level2_[i];

  in >> dummy;
  assert(dummy == "coarse_fine_corr_");
  in >> size;
  coarse_fine_corr_.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> coarse_fine_corr_[i];

  in >> dummy;
  assert(dummy == "level3_");
  in >> size;
  level3_.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> level3_[i];

  in >> dummy;
  assert(dummy == "level4_");
  in >> size;
  level4_.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> level4_[i];

  in >> dummy;
  assert(dummy == "arclength_");
  in >> size;
  arclength_.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> arclength_[i];

  in >> dummy;
  assert(dummy == "cum_arclength_");
  in >> size;
  cum_arclength_.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> cum_arclength_[i];

  in >> dummy;
  assert(dummy == "angle_");
  in >> size;
  angle_.resize(size);
  for(unsigned i=0; i<size; i++)
    in >> angle_[i];

  in >> dummy;
  assert(dummy == "length_:");
  in >> length_;

  in >> dummy;
  assert(dummy == "isTop:");
  in >> isTop;

  in >> dummy;
  assert(dummy == "END_DUMPING_BFRAG_CURVE");
}

#if JONAH_FORMULAS
double bfrag_curve::total_length(int ip, int i)
{
  double l = 0.0;
  int s;
  if(ip<=i)
  {
    for(s=ip+1;s<=i;s++)
      l += arclength_[s];
  }
  else
  {
    for(s=ip+1;s < int(arclength_.size());s++)
      l += arclength_[s];
    for(s=0;s<=i;s++)
      l += arclength_[s];
  }
  return l;
}

double bfrag_curve::merge_length(int ip, int i)
{
  double lx=0;
  double ly=0;
  double l;
  
  if(i-ip==1)
    l = arclength_[i];
  else
  {
    lx = x(i) - x(ip);
    ly = y(i) - y(ip);
    l = sqrt(lx*lx+ly*ly);
  }
  return l;
}

double bfrag_curve::merge_angle(int ip, int i)
{
  double a=0;
  double lx=0;
  double ly=0;
  
  if(i-ip==1) 
    a = angle_[i];
  else 
  {
    lx = x(i) - x(ip);
    ly = y(i) - y(ip); 
    a = atan2(ly,lx);
  }  
  return a;
}
#endif

#if CAN_FORMULAS
double bfrag_curve::total_length(int ip, int i)
{
  if(is_open_ == false)
  {
    if(ip <= i)
      return cum_arclength_[i] - cum_arclength_[ip];
    else
      return length_ - cum_arclength_[ip] + cum_arclength_[i];
  }
  else // handling open curve
  {
    assert(i >= ip);
    return cum_arclength_[i] - cum_arclength_[ip];
  }
}

double bfrag_curve::merge_length(int ip, int i)
{
  return length_from_to(ip, i);
}

double bfrag_curve::merge_angle(int ip, int i)
{
  double lx = x(i) - x(ip);
  double ly = y(i) - y(ip);
  return atan2(ly,lx);
  if(i == ip)
    return 0;
  else
  {
    if(is_corner(ip) == true)
    {
      double lx = x(i) - x(ip);
      double ly = y(i) - y(ip);
      return atan2(ly,lx);
    }
//    else if(is_corner(i) == true)
//      return angle_[ip];
    else
      return angle_[i] - angle_[ip]; // We may need correction here, compare with Jonah's function
  }
}
#endif

bool bfrag_curve::operator ==(bfrag_curve &other)
{
  if(this->num_fine_points() != other.num_fine_points())
    return false;
  if(this->is_open_ != other.is_open_)
    return false;
  else
  {
    double avg_dist = (*this) - other;
    if(avg_dist <= SAME_CURVE_DISTANCE_THRESHOLD)
      return true;
    else
      return false;
  }
}

double bfrag_curve::operator -(bfrag_curve &other)
{
  if(this->num_fine_points() != other.num_fine_points())
    return DBL_MAX;
  if(this->is_open_ != other.is_open_)
    return DBL_MAX;
  else
  {
    double total_dist = 0;
    for(unsigned i=0; i < this->num_fine_points(); i++)
    {
      double x_diff = this->x(i) - other.x(i);
      double y_diff = this->y(i) - other.y(i);
      total_dist += vcl_sqrt(vcl_pow(x_diff, 2.0) + vcl_pow(y_diff, 2.0));
    }
    double avg_dist = total_dist / this->num_fine_points();
    return avg_dist;
  }
}

#define isnan(x) ((x) != (x))

void bfrag_curve::compute_level3_interpolated_curve(dbsol_interp_curve_2d &level3_curve)
{
  // Deletion of old level3_curve_ is already handled by the interpolated curve class
  if(is_open_ == false)
  {
    vnl_vector<double> samples;
    int flag = 0;

    if(level3_[0].x() != level3_[num_fine_points()-1].x() || 
       level3_[0].y() != level3_[num_fine_points()-1].y())
      flag = 1;

    if(flag)
      level3_.push_back(level3_[0]);

    dbsol_curve_algs::interpolate_eno(&level3_curve, level3_, samples);

    if(flag)
      level3_.pop_back();
  }
  else // handling open curve
  {
    vnl_vector<double> samples;
    dbsol_curve_algs::interpolate_eno(&level3_curve, level3_, samples);
  }
}

void bfrag_curve::compute_properties()
{
  if(num_fine_points() != 0)
  {
    if(level4_.size() == 0)
    {
      for(unsigned i=0; i < num_fine_points(); i++)
        level4_.push_back(level3_[i]);
    }
    dbsol_interp_curve_2d level3_curve;
    if(CAN_FORMULAS)
      compute_level3_interpolated_curve(level3_curve);
    compute_arclength(level3_curve);
    compute_angles(level3_curve);
    compute_box();
  }
}

#if JONAH_FORMULAS
void bfrag_curve::compute_arclength(dbsol_interp_curve_2d &level3_curve)
{
  double px,py,cx,cy,dL;
  unsigned i;
  arclength_.clear();
  length_=0;
  arclength_.push_back(0.0);
  
  px = x(0);
  py = y(0);
  for (i=1; i < num_fine_points();i++)
  {
    cx = x(i);
    cy = y(i);
    dL = sqrt(pow(cx-px,2)+pow(cy-py,2));
    length_ += dL;
    arclength_.push_back(dL);
    px = cx;
    py = cy;
  }
  
  //Deal with the last point for a closed curve separately.
  if (!is_open_)
  {
    px = x(num_fine_points()-1);
    py = y(num_fine_points()-1);
    cx = x(0);
    cy = y(0);
    dL = sqrt(pow(cx-px,2)+pow(cy-py,2));
    length_ += dL;
    arclength_[0] = dL;
  }
}

void bfrag_curve::compute_angles(dbsol_interp_curve_2d &level3_curve)
{
  double px,py,cx,cy,theta;
  unsigned i;
  
  angle_.clear();
  angle_.push_back(0.0); // for closed curve, this will be dealt with later
  
  px = x(0);
  py = y(0);
  for (i=1;i < num_fine_points();i++) 
  {
    cx = x(i);
    cy = y(i);
    theta = atan2(cy-py,cx-px);
    angle_.push_back(theta);
    px = cx;
    py = cy;
  }

  //Deal with the last point for a closed curve separately.
  if (!is_open_)
  {
    px = x(num_fine_points()-1);
    py = y(num_fine_points()-1);
    cx = x(0);
    cy = y(0);
    theta = atan2(cy-py,cx-px);
    angle_[0] =theta;
  }
}
#endif

#if CAN_FORMULAS
void bfrag_curve::compute_arclength(dbsol_interp_curve_2d &level3_curve)
{
  arclength_.clear();
  arclength_.push_back(0.0);
  length_ = 0;
  for(unsigned i=0; i < level3_curve.size(); i++)
  {
    double len = level3_curve.length_at(i+1) - level3_curve.length_at(i);
    arclength_.push_back(len);
    length_ += len;
  }
  if(is_open_ == false) //this is not necessary if the curve is open
  {
    arclength_[0] = arclength_[arclength_.size()-1];
    arclength_.pop_back();
  }
  assert(length_ == level3_curve.length());

  cum_arclength_.clear();
  cum_arclength_.push_back(0.0);
  for(unsigned i=1; i < level3_curve.size()+1; i++)
    cum_arclength_.push_back(level3_curve.length_at(i));
}

void bfrag_curve::compute_angles(dbsol_interp_curve_2d &level3_curve)
{
  angle_.clear();
  for(unsigned i=0; i < level3_curve.size()+1; i++)
  {
    double ang = level3_curve.tangent_angle_at(level3_curve.length_at(i));
    // Value returned by interpolated curve for the angle is always in [0, 2*PI).
    // We would like to have the angle in [-PI, PI] to be consistent with Jonah's design
    if(ang > vnl_math::pi)
      ang -= (2*vnl_math::pi);
    angle_.push_back(ang);
  }
  if(is_open_ == false)
    angle_.pop_back();
}
#endif

void bfrag_curve::update_angles(double rot)
{
  unsigned size = angle_.size();
  for(unsigned i=0; i<size; i++)
  {
    angle_[i] += rot;
    if(angle_[i] > vnl_math::pi)
      angle_[i] -= 2*vnl_math::pi;
    else if(angle_[i] < -vnl_math::pi)
      angle_[i] += 2*vnl_math::pi;
  }
}

void bfrag_curve::compute_box()
{
  double min_x, min_y, max_x, max_y;
  box_.empty();
  
  min_x = x(0);
  min_y = y(0);
  max_x = x(0);
  max_y = y(0);  
  
  for(unsigned i = 1; i < num_fine_points(); i++) 
  {
    if(x(i) < min_x) min_x = x(i);
    if(y(i) < min_y) min_y = y(i);
    if(x(i) > max_x) max_x = x(i);
    if(y(i) > max_y) max_y = y(i);
  }

  box_.set_min_x(min_x);
  box_.set_min_y(min_y);
  box_.set_max_x(max_x);
  box_.set_max_y(max_y);
}

void bfrag_curve::resample(double ds)
{
  dbsol_interp_curve_2d level3_curve;
  compute_level3_interpolated_curve(level3_curve);
  vcl_cout << level3_curve.length() << vcl_endl;
  level3_.clear();
  // calculate the number of points after resampling
  int new_num_points = int(vcl_ceil(level3_curve.length() / ds)) + 1;
  assert(new_num_points > 2);
  double exact_dS = level3_curve.length() / new_num_points;
  for(int i=0; i<=new_num_points; i++)
  {
    vsol_point_2d_sptr p = level3_curve.point_at(exact_dS * i);
    level3_.push_back(vgl_point_2d<double>(p->x(), p->y()));
  }

  if(is_open_ == false)
    level3_.pop_back();

  compute_properties();
}

void bfrag_curve::smooth_by_discrete_curvature_algo(float psi, unsigned num_times)
{
  dbgl_csm(level3_, psi, num_times);
  compute_properties();
}

// IMPORTANT: Currently, this function only handles coarse resampling on closed curves!!!
void bfrag_curve::resample_coarsely(double ds)
{
  assert(this->is_open_ == false);
  level2_.clear();
  coarse_fine_corr_.clear();

  // Find the corners according to the level-3 point indices
  find_corners();
  // Shift points such that the first point is the first corner
  vcl_vector<vgl_point_2d<double> > temp_pts;
  for(unsigned i=level1_[0]; i < num_fine_points(); i++)
    temp_pts.push_back(level3_[i]);
  for(int i=0; i < level1_[0]; i++)
    temp_pts.push_back(level3_[i]);

  level3_.clear();

  for(unsigned i=0; i < temp_pts.size(); i++)
    level3_.push_back(temp_pts[i]);
  // add the first point to the end of the list for the circularity
  level3_.push_back(level3_[0]);

  //update corner indices
  int offset = level1_[0];
  for(unsigned i=0; i < level1_.size(); i++)
    level1_[i] -= offset;
  // Add the last point of the level-3 list as a corner
  level1_.push_back(num_fine_points()-1);

  compute_properties();

  // Smoothing might be necessary here
 
  // Resampling starts here
  for(unsigned i=1; i < level1_.size(); i++)
  {
    int corner_curr = level1_[i];
    int corner_prev = level1_[i-1];
    level2_.push_back(level3_[corner_prev]);
    coarse_fine_corr_.push_back(corner_prev);
    // We change the corner indices according to the level-2 point indices during the process
    level1_[i-1] = num_coarse_points()-1;

    double len = total_length(corner_prev, corner_curr);
    if(len > 0)
    {
      int num_segments = int(vcl_ceil(len / ds));
      if(num_segments > 1)
      {
        double exact_ds = len / num_segments;
        double sub_len = 0;
        for(int j = corner_prev+1; j < corner_curr; j++)
        {
          sub_len += arclength_[j];
          if(sub_len >= exact_ds)
          {
            level2_.push_back(level3_[j]);
            coarse_fine_corr_.push_back(j);
            sub_len -= exact_ds;
          }
        }
      }
    }
  }
  level3_.pop_back();
  level1_.pop_back();
  compute_properties();
}

void bfrag_curve::become_coarse()
{
  level3_.clear();
  for(unsigned i=0; i < num_coarse_points(); i++)
    level3_.push_back(level2_[i]);

  level2_.clear();
  compute_properties();
}

void bfrag_curve::find_corners()
{
  level1_.clear();

  dbsol_corner_finder cf;
  dbsol_interp_curve_2d level3_curve;
  this->compute_level3_interpolated_curve(level3_curve);
  cf.find_corners(&level3_curve, false, VICINITY, DIST_STEP, MIN_TAN_TURN);
  vcl_vector<int> *indices;
  indices = cf.get_corner_indices();

  for(unsigned i=0; i < indices->size(); i++)
    level1_.push_back(indices->at(i));
}

void bfrag_curve::rotate(double angle)
{
  double cos_angle = vcl_cos(angle);
  double sin_angle = vcl_sin(angle);
  if(angle != 0)
  {
    for(unsigned i=0; i < num_coarse_points(); i++)
    {
      double x = level2_[i].x();
      double y = level2_[i].y();
      double rot_x =  x * cos_angle + y * sin_angle;
      double rot_y = -x * sin_angle + y * cos_angle;
      level2_[i].set(rot_x, rot_y);
    }
    for(unsigned i=0; i < num_fine_points(); i++)
    {
      double x = level3_[i].x();
      double y = level3_[i].y();
      double rot_x =  x * cos_angle + y * sin_angle;
      double rot_y = -x * sin_angle + y * cos_angle;
      level3_[i].set(rot_x, rot_y);
    }
    for(unsigned i=0; i < num_orig_points(); i++)
    {
      double x = level4_[i].x();
      double y = level4_[i].y();
      double rot_x =  x * cos_angle + y * sin_angle;
      double rot_y = -x * sin_angle + y * cos_angle;
      level4_[i].set(rot_x, rot_y);      
    }
  }
}

void bfrag_curve::translate(double tx, double ty)
{
  if(tx != 0 && ty != 0)
  {
    for(unsigned i=0; i < num_coarse_points(); i++)
      level2_[i].set(level2_[i].x() + tx, level2_[i].y() + ty);
    for(unsigned i=0; i < num_fine_points(); i++)
      level3_[i].set(level3_[i].x() + tx, level3_[i].y() + ty);
    for(unsigned i=0; i < num_orig_points(); i++)
      level4_[i].set(level4_[i].x() + tx, level4_[i].y() + ty);
  }
}

void bfrag_curve::rotate_translate(double angle, double tx, double ty)
{
  this->rotate(angle);
  this->translate(tx, ty);
  // compute current transformation matrix and update cumulative transformation matrix of the class
  double coeffs[9] = {vcl_cos(angle), vcl_sin(angle), tx, -vcl_sin(angle), vcl_cos(angle), ty, 0.0, 0.0, 1.0};
  vnl_matrix_fixed<double,3,3> curr(coeffs);
  transform_ = curr * transform_;

  // AMIR-CAN
  //compute_properties();
  compute_box();
  // AMIR-CAN
}

// IMPORTANT: Currently, this function only handles coarse resampling on closed curves!!!
void bfrag_curve::invert()
{
  assert(is_open_ == false);
  // Algorithm:
  // 1. Set front to the first element index (0) of the array
  // 2. Set rear to the last element index of the array
  // 3. Swap the elements indexed by front and back
  // 4. Increment the front index, decremen the back index
  // 5. Repeat steps 3-4 as long as the fron index is smaller than the back index
  // This is more efficient than Jonah's algorithm in terms of time and space

  // invert level-2
  int front = 0;
  int rear = num_coarse_points()-1;
  while(front < rear)
  {
    vgl_point_2d<double> temp = level2_[front];
    level2_[front] = level2_[rear];
    level2_[rear] = temp;
    front++;
    rear--;
  }
  // invert level-3
  front = 0;
  rear = num_fine_points()-1;
  while(front < rear)
  {
    vgl_point_2d<double> temp = level3_[front];
    level3_[front] = level3_[rear];
    level3_[rear] = temp;
    front++;
    rear--;
  }
  // invert level-4
  front = 0;
  rear = num_orig_points()-1;
  while(front < rear)
  {
    vgl_point_2d<double> temp = level4_[front];
    level4_[front] = level4_[rear];
    level4_[rear] = temp;
    front++;
    rear--;
  }
  // update corner indices
  for(unsigned i=0; i < num_corners(); i++)
    level1_[i] = num_coarse_points() - level1_[i] - 1;
  // update fine-coarse correspondences
  front = 0;
  rear = coarse_fine_corr_.size()-1;
  while(front < rear)
  {
    int temp = coarse_fine_corr_[front];
    coarse_fine_corr_[front] = coarse_fine_corr_[rear];
    coarse_fine_corr_[rear] = temp;
    front++;
    rear--;
  }
  for(unsigned i=0; i < coarse_fine_corr_.size(); i++)
  {
    coarse_fine_corr_[i] = num_fine_points() - coarse_fine_corr_[i] - 1;
  }

  compute_properties();
}
void bfrag_curve::append(vgl_point_2d<double> &p)
{
  level3_.push_back(p);
}

void bfrag_curve::empty()
{
  level2_.clear();
  level3_.clear();
//  level4_.clear();
}

void bfrag_curve::write_level1(vcl_string fname)
{
  vcl_ofstream fp(fname.c_str());
  fp << num_corners() << vcl_endl;
  for(unsigned i=0; i < num_corners(); i++)
    fp << level1_[i] << vcl_endl;
}

void bfrag_curve::write_level2(vcl_string fname)
{
  vcl_ofstream fp(fname.c_str());
  for(unsigned i=0; i < num_coarse_points(); i++)
    fp << level2_[i].x() << " " << level2_[i].y() << vcl_endl;
}

void bfrag_curve::write_level3(vcl_string fname)
{
  vcl_ofstream fp(fname.c_str());
  for(unsigned i=0; i < num_fine_points(); i++)
    fp << level3_[i].x() << " " << level3_[i].y() << vcl_endl;
}

void bfrag_curve::write_level4(vcl_string fname)
{
  vcl_ofstream fp(fname.c_str());
  for(unsigned i=0; i < num_orig_points(); i++)
    fp << level4_[i].x() << " " << level4_[i].y() << vcl_endl;
}

void bfrag_curve::write_coarse_fine_correspondence(vcl_string fname)
{
  vcl_ofstream fp(fname.c_str());
  for(unsigned i=0; i < num_coarse_points(); i++)
    fp << coarse_fine_corr_[i] << vcl_endl;
}
void bfrag_curve::write_arclengths(vcl_string fname)
{
  vcl_ofstream fp(fname.c_str());
  for(unsigned i=0; i < arclength_.size(); i++)
    fp << arclength_[i] << vcl_endl;
}
void bfrag_curve::write_angles(vcl_string fname)
{
  vcl_ofstream fp(fname.c_str());
  for(unsigned i=0; i < angle_.size(); i++)
    fp << angle_[i] << vcl_endl;
}

