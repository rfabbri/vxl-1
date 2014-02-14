#include <vcl_iostream.h>
#include <dbskr/dbskr_scurve.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_line_2d.h>
#include <dbgl/algo/dbgl_eulerspiral.h>

#include <vcl_cstdio.h>
#include <vcl_algorithm.h>
#include <dbsk2d/dbsk2d_geometry_utils.h>
#include <dbskr/dbskr_dpmatch_combined.h>
#include <vsol/vsol_polygon_2d.h>

#define ZERO_TOLERANCE 1E-1

double fixAngleMPiPi_new(double a) {
  if (a < -vnl_math::pi)
    return a+2*vnl_math::pi;
  else if (a > vnl_math::pi)
    return a-2*vnl_math::pi;
  else
    return a;
}

//: Does a1-a2
double angleDiff_new(double a1, double  a2) {
  a1=fixAngleMPiPi_new(a1);
  a2=fixAngleMPiPi_new(a2);
  if (a1 > a2)
    if (a1-a2 > vnl_math::pi)
      return a1-a2-2*vnl_math::pi;
    else
      return a1-a2;
  else if (a2 > a1)
    if (a1-a2 < -vnl_math::pi)
      return a1-a2+2*vnl_math::pi;
    else
      return a1-a2;
  return 0.0;
}


//
//: constructors
//
dbskr_scurve::dbskr_scurve(int num_points,
                           vcl_vector<vgl_point_2d<double> > &sh_pt, 
                           vcl_vector<double> &time,
                           vcl_vector<double> &theta,
                           vcl_vector<double> &phi, 
                           bool binterpolate, double interpolate_ds,
                           bool bsub_sample, double subsample_ds): 
    interpolate_ds_(interpolate_ds), subsample_ds_(subsample_ds),
    virtual_length_(0.0) 
{
  if (binterpolate){
    vcl_vector<int> lmap; //dummy map 
    interpolate(num_points, sh_pt, time, theta, phi, lmap);
  } 
  else {
    sh_pt_ = sh_pt;
    time_ = time;
    theta_ = theta;
    phi_ = phi;
    num_points_ = sh_pt_.size();
  }
  //reconstruct the boundary points and tangents before subsampling
  reconstruct_boundary();
  // if there are more than two points on the curve, its likely that
  // the curve was too densely sampled, so subsample it
  if (num_points_ > 2 && bsub_sample)
  {
    subsample();

    //reconstruct the boundary points and tangents after subsampling
    reconstruct_boundary();
  }

  //now compute the arclengths
  compute_arclengths();

  // now compute the areas
  compute_areas();

}

//Interpolate/copy constructor
dbskr_scurve::dbskr_scurve(dbskr_scurve& old,
                           vcl_vector<int> &lmap,
                           double interpolate_ds) 
{
  if (this == &old)
    return;

  interpolate_ds_ = interpolate_ds;
  subsample_ds_ = interpolate_ds;

  //check to see if this shock curve needs to be resampled
  if (interpolate_ds_ == old.subsample_ds_)
  {
    //just needs to be copied as is
    num_points_ = old.num_points_;

    sh_pt_ = old.sh_pt_;
    arclength_ = old.arclength_;
    time_ = old.time_;
    theta_ = old.theta_;
    phi_ = old.phi_;

    bdry_plus_ = old.bdry_plus_;
    bdry_minus_ = old.bdry_minus_;
    bdry_plus_length_ = old.bdry_plus_length_;
    bdry_minus_length_ = old.bdry_minus_length_;
    bdry_plus_arclength_ = old.bdry_plus_arclength_;
    bdry_minus_arclength_ = old.bdry_minus_arclength_;
    bdry_plus_angle_ = old.bdry_plus_angle_;
    bdry_minus_angle_ = old.bdry_minus_angle_;

    //just a one to one map since nothing was changed
    lmap.clear();
    for (int i=0; i<num_points_; i++)
      lmap.push_back(i);

  }
  else {
    //needs to be resampled with the given parameters
    interpolate(old.num_points_, old.sh_pt_, old.time_, old.theta_, old.phi_, lmap);

    //reconstruct the boundary points and tangents
    reconstruct_boundary();

    //now compute the arclengths
    compute_arclengths();
  }
}

dbskr_scurve::~dbskr_scurve() 
{
  sh_pt_.clear(); ///< point along the shock edge
  arclength_.clear();            ///< arclength along shock branch
  time_.clear();                 ///< radius of the maximal circle
  theta_.clear();                ///< tangent vector to the shock curve
  phi_.clear();                  ///< Angle between the tangent to the shock and the 
                                            //   vector from shock to the bndry point.
  bdry_plus_.clear(); ///< corresponding bndry pt on the + side 
  bdry_plus_arclength_.clear();       ///< arclength along the + bndry curve
  bdry_plus_angle_.clear();           ///< tangent to the + bndry curve
  bdry_minus_.clear(); ///< corresponding bndry pt on the - side 
  bdry_minus_arclength_.clear();       ///< arclength along the - bndry curve
  bdry_minus_angle_.clear();           ///< tangent to the - bndry curve
}

//: assignment operator
dbskr_scurve & 
dbskr_scurve::operator=(const dbskr_scurve &rhs)
{
  if (this != &rhs){
    interpolate_ds_ = rhs.interpolate_ds_;
    subsample_ds_ = rhs.subsample_ds_;

    num_points_ = rhs.num_points_;

    sh_pt_ = rhs.sh_pt_;
    arclength_ = rhs.arclength_;
    time_ = rhs.time_;
    theta_ = rhs.theta_;
    phi_ = rhs.phi_;
  
    bdry_plus_ = rhs.bdry_plus_;
    bdry_minus_ = rhs.bdry_minus_;
    bdry_plus_length_ = rhs.bdry_plus_length_;
    bdry_minus_length_ = rhs.bdry_minus_length_;
    bdry_plus_arclength_ = rhs.bdry_plus_arclength_;
    bdry_minus_arclength_ = rhs.bdry_minus_arclength_;
    bdry_plus_angle_ = rhs.bdry_plus_angle_;
    bdry_minus_angle_ = rhs.bdry_minus_angle_;
  }
  return *this;
}

//: This function takes a sampled shock curve and interpolates it.
void dbskr_scurve::interpolate( int num_points,
                                vcl_vector<vgl_point_2d<double> > &sh_pt,
                                vcl_vector<double> &time,
                                vcl_vector<double> &theta,
                                vcl_vector<double> &phi,
                                vcl_vector<int> &lmap)
{
  // we need to interpolate along the length of the shock curve
  // to fill the gaps between the shock samples
  
  lmap.clear(); //keep track of sample correspondences between the old and the interpolated pts
  int new_ind = 0;  //index into the newly created samples

  // allocate memory // added by Nhon
  this->sh_pt_.clear();
  this->sh_pt_.reserve(2*num_points);

  this->time_.clear();
  this->time_.reserve(2*num_points);

  this->theta_.clear();
  this->theta_.reserve(2*num_points);

  this->phi_.clear();
  this->phi_.reserve(2*num_points);

  lmap.clear();
  lmap.reserve(2*num_points);


  //add the very first sample
  sh_pt_.push_back(sh_pt[0]);
  time_.push_back(time[0]);
  theta_.push_back(theta[0]);
  phi_.push_back(phi[0]);

  lmap.push_back(new_ind++); //insert first sample into the map

  for (int i=1;i<num_points;i++)
  {
    double dphi = angleDiff_new(phi[i],phi[i-1]);
    double dtheta = angleDiff_new(theta[i], theta[i-1]);
    double dx = sh_pt[i].x()-sh_pt[i-1].x();
    double dy = sh_pt[i].y()-sh_pt[i-1].y();
    double ds = sqrt(dx*dx + dy*dy);
    double dt = time[i]-time[i-1];

#if 0 //this is wrong, but added to match Sebastian's matching as in Matching-Tek version of the code
    double apprxds = (vcl_fabs(dtheta)+vcl_fabs(dphi))*(time[i-1]+time[i])/2;
#else
    double apprxds = ds+(vcl_abs(dtheta)+vcl_abs(dphi))*(time[i-1]+time[i])/2;
#endif

#if 0 //again same purpose: Matching Sebastian's Matching-Tek 
    if (apprxds>interpolate_ds_ && vcl_fabs(dt)<0.01){
#else //modified Sebastian's to allow for interppolation at pruned points because these samples are missing
      //(this is Amir's working interpolation as well.)
    if (apprxds>interpolate_ds_){
#endif

      int num = int(apprxds/interpolate_ds_);

      for(int j=1;j<num;j++)
      {
        float ratio = (float)j/(float)num;

        vgl_point_2d<double> p_int(sh_pt[i-1].x()+ratio*dx,sh_pt[i-1].y()+ratio*dy);
        double time_int = time[i-1] + ratio*dt;
        double phi_int = phi[i-1] + ratio*dphi;
        double theta_int = theta[i-1] + ratio*dtheta;

        sh_pt_.push_back(p_int);
        time_.push_back(time_int);
        theta_.push_back(theta_int);
        phi_.push_back(phi_int);

        new_ind++; //update new sample id
      }
    }

    //add the current original sample
    sh_pt_.push_back(sh_pt[i]);
    time_.push_back(time[i]);
    theta_.push_back(theta[i]);
    phi_.push_back(phi[i]);

    lmap.push_back(new_ind++); //insert the current sample into the map
  }

  num_points_ = sh_pt_.size();
  assert(new_ind == num_points_);//just making sure
}

void dbskr_scurve::subsample()
{
  vcl_vector<vgl_point_2d<double> > sub_sh_pt, sub_bdry_plus, sub_bdry_minus;
  vcl_vector<double> sub_time, sub_theta, sub_phi, sub_bdry_plus_angle, sub_bdry_minus_angle;

  //keep the first and last points and subsample the rest
  
  //first sample
  sub_sh_pt.push_back(sh_pt_[0]);
  sub_time.push_back(time_[0]);  
  sub_theta.push_back(theta_[0]);
  sub_phi.push_back(phi_[0]);
  sub_bdry_plus.push_back(bdry_plus_[0]);
  sub_bdry_minus.push_back(bdry_minus_[0]);
  sub_bdry_plus_angle.push_back(bdry_plus_angle_[0]);
  sub_bdry_minus_angle.push_back(bdry_minus_angle_[0]);

  vgl_point_2d<double> So,Sn,Se;     //shock point at start, middle and end
  vgl_point_2d<double>  PBo,PBn,PBe; //bnd_plus pt at start, middle and end
  vgl_point_2d<double> MBo,MBn,MBe;  //bnd_minus pt at start, middle and end

  So = sh_pt_[0];
  Se = sh_pt_[num_points_-1];
  PBo = bdry_plus_[0];
  PBe = bdry_plus_[num_points_-1];
  MBo = bdry_minus_[0];
  MBe = bdry_minus_[num_points_-1];
  
  for (int i=1;i<num_points_-1;i++)
  {
    Sn=sh_pt_[i];
    PBn=bdry_plus_[i];
    MBn=bdry_minus_[i];

    //core subsampling criteria
    if ( ((vgl_distance(So, Sn)   > subsample_ds_) && (vgl_distance(Sn, Se)   > subsample_ds_)) ||
         ((vgl_distance(PBo, PBn) > subsample_ds_) && (vgl_distance(PBn, PBe) > subsample_ds_)) ||
         ((vgl_distance(MBo, MBn) > subsample_ds_) && (vgl_distance(MBn, MBe) > subsample_ds_)) 
       )
    {
      sub_sh_pt.push_back(sh_pt_[i]);
      sub_time.push_back(time_[i]);
      sub_theta.push_back(theta_[i]);
      sub_phi.push_back(phi_[i]);      
      sub_bdry_plus.push_back(bdry_plus_[i]);
      sub_bdry_minus.push_back(bdry_minus_[i]);
      sub_bdry_plus_angle.push_back(bdry_plus_angle_[i]);
      sub_bdry_minus_angle.push_back(bdry_minus_angle_[i]);

      So=Sn;
      PBo=PBn;
      MBo=MBn;
    }
  }

  //last sample
  sub_sh_pt.push_back(sh_pt_[num_points_-1]);
  sub_time.push_back(time_[num_points_-1]);
  sub_theta.push_back(theta_[num_points_-1]);
  sub_phi.push_back(phi_[num_points_-1]);      
  sub_bdry_plus.push_back(bdry_plus_[num_points_-1]);
  sub_bdry_minus.push_back(bdry_minus_[num_points_-1]);
  sub_bdry_plus_angle.push_back(bdry_plus_angle_[num_points_-1]);
  sub_bdry_minus_angle.push_back(bdry_minus_angle_[num_points_-1]);

  //clear the existing samples and replace it with these ones
  sh_pt_ = sub_sh_pt;
  time_ = sub_time;
  theta_ = sub_theta;
  phi_ = sub_phi;
  bdry_plus_ = sub_bdry_plus;
  bdry_minus_ = sub_bdry_minus;
  bdry_plus_angle_ = sub_bdry_plus_angle;
  bdry_minus_angle_ = sub_bdry_minus_angle;

  num_points_ = sh_pt_.size();
}

// ---------------------------------------------------
// functions to compute the properties along the curve
//----------------------------------------------------

//: reconstruct the plus and minus boundary points from the shock parameters
void dbskr_scurve::reconstruct_boundary()
{
  // clear old data
  this->bdry_plus_.clear();
  this->bdry_minus_.clear();
  this->bdry_plus_angle_.clear();
  this->bdry_minus_angle_.clear();

  // allocate memory
  this->bdry_plus_.reserve(num_points_);
  this->bdry_minus_.reserve(num_points_);
  this->bdry_plus_angle_.reserve(num_points_);
  this->bdry_minus_angle_.reserve(num_points_);
  

  //reconstruct the bnd points from the intrinsic parameters
  for (int i=0; i<num_points_; i++)
  {
    vgl_point_2d<double> pt_p = _translatePoint(sh_pt_[i], 
                                  theta_[i]+phi_[i], 
                                  time_[i]);
    vgl_point_2d<double> pt_m = _translatePoint(sh_pt_[i], 
                                  theta_[i]-phi_[i], 
                                  time_[i]);

    bdry_plus_.push_back(pt_p); 
    bdry_minus_.push_back(pt_m);

    bdry_plus_angle_.push_back(fixAngleMPiPi_new(theta_[i]+phi_[i]-vnl_math::pi/2));
    bdry_minus_angle_.push_back(fixAngleMPiPi_new(theta_[i]-phi_[i]+vnl_math::pi/2));
  }

#if 0 //Recreating the stupid mistakes made by Thomas in computing the boundary tangents

  vgl_point_2d<double> curr, prev;
  double dtheta;

  bdry_minus_angle_.clear();
  bdry_minus_angle_.push_back(99999.0); //init first point (this will be corrected later

  bdry_plus_angle_.clear();
  bdry_plus_angle_.push_back(99999.0); //init first point (this will be corrected later

  int initProblemP=0,initProblemM=0;

  for (int i=1;i<num_points_;i++)
  {
    prev = bdry_plus_[i-1];
    curr = bdry_plus_[i];

    //If points are very close to each other angle is not reliable
    if (vcl_pow(prev.x()-curr.x(),2)+vcl_pow(prev.y()-curr.y(),2) > 1E-1)
    {
      dtheta = atan2(prev.y()-curr.y(), prev.x()-curr.x());
      bdry_plus_angle_.push_back(dtheta);
    }
    else{
      if (i==1)
        initProblemP=1;

      dtheta = bdry_plus_angle_[bdry_plus_angle_.size()-1];
      bdry_plus_angle_.push_back(dtheta);
    }

    prev = bdry_minus_[i-1];
    curr = bdry_minus_[i];

    //If points are very close to each other angle is not reliable
    if (vcl_pow(prev.x()-curr.x(),2)+vcl_pow(prev.y()-curr.y(),2) > 1E-1)
    {
      dtheta = atan2(prev.y()-curr.y(), prev.x()-curr.x());
      bdry_minus_angle_.push_back(dtheta);
    }
    else{
      if (i==1)
        initProblemM=1;

      dtheta = bdry_minus_angle_[bdry_minus_angle_.size()-1];
      bdry_minus_angle_.push_back(dtheta);
    }
  }

  //Set the orientation for the first point
  //If the first intervals are very small, we have stored incorrect 
  //angle information.
  int j;
  if (num_points_>=2)
  {
    if (bdry_plus_angle_[num_points_-1] == 99999.0){
      for (j=0;j<num_points_;j++)
        bdry_plus_angle_[j]=0.0;
    }
    else if (!initProblemP)
      bdry_plus_angle_[0]=bdry_plus_angle_[1];
    else 
    {
      int nz=0;
      while (nz<num_points_ && bdry_plus_angle_[nz] == 99999.0)
        nz++;
      for (j=0;j<nz;j++)
        bdry_plus_angle_[j]=bdry_plus_angle_[nz];
    }


    if (bdry_minus_angle_[num_points_-1] == 99999.0){
      for (j=0;j<num_points_;j++)
        bdry_minus_angle_[j]=0.0;
    }
    else if (!initProblemM)
      bdry_minus_angle_[0]=bdry_minus_angle_[1];
    else 
    {
      int nz=0;
      while (nz<num_points_ && bdry_minus_angle_[nz] == 99999.0)
        nz++;
      for (j=0;j<nz;j++)
        bdry_minus_angle_[j]=bdry_minus_angle_[nz];
    }
  }

#endif
}

//: compute boundary arclengths and shock arclengths
void dbskr_scurve::compute_arclengths() 
{
  vgl_point_2d<double> curr,prev;
  double dL1,dL2;

  bdry_minus_arclength_.clear();
  bdry_minus_arclength_.reserve(num_points_);

  bdry_minus_arclength_.push_back(0.0);
  bdry_minus_length_ = 0.0;

  bdry_plus_arclength_.clear();
  bdry_plus_arclength_.reserve(num_points_);
  bdry_plus_arclength_.push_back(0.0);
  bdry_plus_length_ = 0.0;

  for (int i = 1; i < num_points_; i++) {
    prev = bdry_plus_[i-1];
    curr = bdry_plus_[i];
    dL1 = vnl_math_hypot(prev.x()-curr.x(), prev.y()-curr.y());
    bdry_plus_length_ += dL1;
    bdry_plus_arclength_.push_back(bdry_plus_length_);

    prev = bdry_minus_[i-1];
    curr = bdry_minus_[i];
    dL2 = vnl_math_hypot(prev.x()-curr.x(), prev.y()-curr.y());
    bdry_minus_length_ += dL2;
    bdry_minus_arclength_.push_back(bdry_minus_length_);
  }

  // compute arc length along the shock
  arclength_.clear();
  arclength_.reserve(num_points_);
  arclength_.push_back(0.0);
  
  double dL, px, py, cx, cy, length;
  length = 0;
  px=sh_pt_[0].x();
  py=sh_pt_[0].y();
  for (int i = 1; i < num_points_; i++) {
    cx=sh_pt_[i].x();
    cy=sh_pt_[i].y();
    dL=vcl_sqrt(vcl_pow(cx-px,2.0)+vcl_pow(cy-py,2.0));
    length += dL;
    arclength_.push_back(length);
    px=cx;
    py=cy;
  }
}

//: compute areas along shock curve
void dbskr_scurve::compute_areas() 
{
  vgl_point_2d<double> curr,prev;
  double dA;
  double shock_area=0.0;

  area_.clear();
  area_.reserve(num_points_);
  area_.push_back(0.0);
  total_area_=0;

  for (unsigned int i = 1; i < num_points_; i++) 
  {
      vgl_polygon<double> poly(1);
      poly.push_back(sh_pt_[i-1].x(),sh_pt_[i-1].y());
      poly.push_back(bdry_plus_[i-1].x(),bdry_plus_[i-1].y());
      poly.push_back(bdry_plus_[i].x(),bdry_plus_[i].y());

      poly.push_back(sh_pt_[i].x(),sh_pt_[i].y());
      poly.push_back(bdry_minus_[i].x(),bdry_minus_[i].y());
      poly.push_back(bdry_minus_[i-1].x(),bdry_minus_[i-1].y());

      dA=vgl_area(poly);
      shock_area+=dA;
      total_area_+=dA;

      area_.push_back(shock_area);
  }    
  
  
}

//: return an extrinsic point corresponding to the intrinsic 
//  fragment coordinates(s,t) { i.e., (s,t)->(x,y) }
//  radius>0 for plus side and <0 for minus side
vgl_point_2d<double> 
dbskr_scurve::fragment_pt(int index, double radius)
{
  assert(vcl_fabs(radius)<=time_[index]+1e-10);
  double vec = theta_[index]+ vnl_math_sgn0(radius)*phi_[index];
  return sh_pt_[index] + vcl_fabs(radius)*vgl_vector_2d<double>(vcl_cos(vec), vcl_sin(vec));
}

//: return the radius at an interpolated point
double dbskr_scurve::interp_radius(int i1, int i2, int N, int n)
{
  assert(n<=N);
  float ratio = (float)n/(float)N;
  return time_[i1]+ratio*(time_[i2]-time_[i1]);
}

//: return an extrinsic point corresponding to the intrinsic 
//  fragment coordinates(s,t) { i.e., (s,t)->(x,y) }
//  [radius>0 for plus side and <0 for minus side]
//  Note: the shock edge is linearly interpolated to n/N of the chosen interval
vgl_point_2d<double> 
dbskr_scurve::fragment_pt(int i1, int i2, int N, int n, double radius)
{
  assert(n<=N);
  
  float ratio = (float)n/(float)N;
  
  double dphi = phi_[i2] - phi_[i1];
  double dtheta = angleDiff_new(theta_[i2], theta_[i1]);
  double dx = sh_pt_[i2].x() - sh_pt_[i1].x();
  double dy = sh_pt_[i2].y() - sh_pt_[i1].y();
  double dt = time_[i2] - time_[i1];

  vgl_point_2d<double> pt(sh_pt_[i1].x()+ratio*dx, sh_pt_[i1].y()+ratio*dy);
  double time_int = time_[i1] + ratio*dt;
  double phi_int = phi_[i1] + ratio*dphi;
  double theta_int = theta_[i1] + ratio*dtheta;

  assert(vcl_fabs(radius)<=time_int+1e-7);

  double vec = theta_int+ vnl_math_sgn0(radius)*phi_int;
  return pt + vcl_fabs(radius)*vgl_vector_2d<double>(vcl_cos(vec), vcl_sin(vec));
}

//: return the radius at an interpolated point defined by the continuous index
double dbskr_scurve::interp_radius(double index)
{
  assert(index>=0 && index<=num_points_-1);

  //the integer index less than the continuous one
  int ind = (int)vcl_floor(index);

  //if the continuous index is between two integer indices
  // we need to interpolate the values
  if (ind<index){
    double ratio = index - vcl_floor(index);
    return time_[ind]+ratio*(time_[ind+1]-time_[ind]);
  }
  else //return the value at the integer index
    return time_[ind];
}

//: return an extrinsic point corresponding to the intrinsic 
//  fragment coordinates(s(i),t) { i.e., (s(i),t)->(x,y) }
//  radius>0 for plus side and <0 for minus side defined in terms
//  of a continuous index
vgl_point_2d<double> dbskr_scurve::fragment_pt(double index, double radius)
{
  assert(index>=0 && index<=num_points_-1);

  int i1 = (int)vcl_floor(index); //the integer index less than the continuous one
  int i2 = i1 + 1;

  //if the continuous index is between two integer indices
  // we need to interpolate the values
  if (i1<index)
  {
    double ratio = index - vcl_floor(index);

    double dphi = phi_[i2] - phi_[i1];
    double dtheta = angleDiff_new(theta_[i2], theta_[i1]);
    double dx = sh_pt_[i2].x() - sh_pt_[i1].x();
    double dy = sh_pt_[i2].y() - sh_pt_[i1].y();
    double dt = time_[i2] - time_[i1];

    vgl_point_2d<double> pt(sh_pt_[i1].x()+ratio*dx, sh_pt_[i1].y()+ratio*dy);
    double time_int = time_[i1] + ratio*dt;
    double phi_int = phi_[i1] + ratio*dphi;
    double theta_int = theta_[i1] + ratio*dtheta;

    assert(vcl_fabs(radius)<=time_int+1e-7);

    double vec = theta_int+ vnl_math_sgn0(radius)*phi_int;
    return pt + vcl_fabs(radius)*vgl_vector_2d<double>(vcl_cos(vec), vcl_sin(vec));
  }
  else {
    //return the value at the integer index

    vgl_point_2d<double> pt(sh_pt_[i1].x(),sh_pt_[i1].y());
    double time_int = time_[i1];
    double phi_int = phi_[i1];
    double theta_int = theta_[i1];

    assert(vcl_fabs(radius)<=time_int+1e-7);

    double vec = theta_int+ vnl_math_sgn0(radius)*phi_int;
    return pt + vcl_fabs(radius)*vgl_vector_2d<double>(vcl_cos(vec), vcl_sin(vec));
  }
}

// ---------------------------------------
// Functions to compute costs for dpmatch
//----------------------------------------

//: Stretch Cost consists of three elements: 
// The difference in lengths of the corresponding boundarys segments
// The difference in the radius (time of formation) of the shock
void dbskr_scurve::stretch_cost(int i, int ip, vcl_vector<double> &a) 
{
  a[0]=vcl_fabs(bdry_plus_arclength_[i]-bdry_plus_arclength_[ip]);
  a[1]=vcl_fabs(bdry_minus_arclength_[i]-bdry_minus_arclength_[ip]);
  
  a[2]=2.0*(time_[i] - time_[ip]);
}

void dbskr_scurve::bend_cost(int i, int ip, vcl_vector<double> &a) 
{
  if (vcl_fabs(bdry_plus_arclength_[i]-bdry_plus_arclength_[ip])>EPS)
    a[0]=angleDiff_new(bdry_plus_angle_[i],bdry_plus_angle_[ip]);
  else
    a[0]=0.0;
  if (vcl_fabs( bdry_minus_arclength_[i]-bdry_minus_arclength_[ip])>EPS)
    a[1]=angleDiff_new(bdry_minus_angle_[i],bdry_minus_angle_[ip]);
  else
    a[1]=0.0;
#if 0 //turn on to replicate Sebastian's mistakes !!! 
      //Taking absolute of angleDiff is actually a mistake.
  a[2]=2.0*vcl_fabs(angleDiff_new(phi_[i], phi_[ip]));
#else //this is corrected version!!!
  a[2]=2.0*angleDiff_new(phi_[i], phi_[ip]);
#endif
}

void dbskr_scurve::area_cost(int i, int ip, vcl_vector<double> &a) 
{
  a[0]=vcl_fabs(area_[i]-area_[ip]);
}

//: In the original implementation of Sebastian et al. PAMI 2006, the interval cost is given by:
//  (|d_r1 - d_r2| + |d_phi1 - d_phi2|) where d_r1 = |r1_i - r1_ip|, 
//                                            d_r2 = |r1_j - r1_jp|, 
//                                            d_phi1 = anglediff(phi1_i, phi1_ip), 
//                                            d_phi2 = anglediff(phi2_j, phi2_jp)
//  and the initial costs:
//  (|r1_0 - r2_0| + |r1_end - r2_end|)/2 + (anglediff(phi1_0, phi2_0) + anglediff(phi1_end, phi2_end))/2 are added to get the shock branch deformation cost.
//
//  In this new version proposed by Kimia, Ozcanli, Tamrakar in 2006, the width and orientation differences are combined
//  in the interval cost which is given by
//  2*(|d_r1 - d_r2|) where
//  d_r1 = |r1_i*sin(phi1_i)- r1_ip*sin(phi1_ip)|, d_r2 = |r2_j*sin(phi2_j)- r2_jp*sin(phi2_jp)| 
//  and the initial costs:
//  2*|r1_0*sin(phi1_0) - r2_0*sin(phi2_0)|
void dbskr_scurve::stretch_cost_combined(int i, int ip, vcl_vector<double> &a) 
{
  a[0]=vcl_fabs(bdry_plus_arclength_[i]-bdry_plus_arclength_[ip]);
  a[1]=vcl_fabs(bdry_minus_arclength_[i]-bdry_minus_arclength_[ip]);
  
  //Amir: changing the radius cost to r*sin(phi) cost
  //a[2] = 0.0;
  a[2]=2.0*(time_[i]*vcl_sin(phi_[i]) - time_[ip]*vcl_sin(phi_[ip]));
  //a[2]=2.0*(time_[i] - time_[ip]);
}

void dbskr_scurve::bend_cost_combined(int i, int ip, vcl_vector<double> &a) 
{
  if (vcl_fabs(bdry_plus_arclength_[i]-bdry_plus_arclength_[ip])>EPS)
    a[0]=angleDiff_new(bdry_plus_angle_[i],bdry_plus_angle_[ip]);
  else
    a[0]=0.0;
  if (vcl_fabs( bdry_minus_arclength_[i]-bdry_minus_arclength_[ip])>EPS)
    a[1]=angleDiff_new(bdry_minus_angle_[i],bdry_minus_angle_[ip]);
  else
    a[1]=0.0;
  //Amir: removing the phi cost
  a[2] = 0.0;
  //a[2]=2.0*vcl_fabs(phi_[i] - phi_[ip]));
}

double dbskr_scurve::contract_cost() 
{
  double Ro=time_[0];
  double Rf=time_[num_points_-1];
  double dr=2*vcl_abs(Rf-Ro);
#if 0 //Fatih added this switch to match Sebastian's matching. Duplicated from the code at Matching-Tek folder
    double avg_r=0.15*(Rf+Ro);
  double addLen;
  if (bdry_plus_length_ + bdry_minus_length_ > 25.0)
    addLen=1.5*(bdry_plus_length_ + bdry_minus_length_ - 25.0);
  else 
    addLen=0.0;
    return 1.2*((bdry_plus_length_+bdry_minus_length_+addLen)+dr+avg_r);
#else 
  double avg_r=0.25*(Rf+Ro);  
  return 1.5*((bdry_plus_length_+bdry_minus_length_)+dr+avg_r);
#endif
}

dbskr_scurve_sptr dbskr_scurve::get_original_scurve(bool construct_circular_ends)
{
  // construct a version of the current shock curve with the opposite end closed

  vcl_vector< vgl_point_2d<double> > intp_sh_pt;
  vcl_vector<double> intp_time, intp_phi, intp_theta;

  // first copy the current scurve
  for(int j=0; j<num_points_; j++)
  {
    intp_sh_pt.push_back(sh_pt_[j]);
    intp_time.push_back(time_[j]);
    intp_theta.push_back(theta_[j]);
    intp_phi.push_back(phi_[j]);
  }

  if (construct_circular_ends) {
    //interpolate the endpoint with a circular arc to close it off

    int n = num_points_-1; //index of the opposite point to be closed
    double phiStart = phi_[n];
    double phiEnd = 0;

    double dphi = phiEnd-phiStart;
    double apprxds = vcl_abs(dphi)*time_[n];

    //num of extra samples
    int num_pts = int(apprxds/subsample_ds_); //at the end point
    
    //add the interpolated samples
    for(int j=1; j<=num_pts; j++)
    {
      float ratio = (float)j/(float)num_pts;

      intp_sh_pt.push_back(sh_pt_[n]);     //at an A-inf point point remains stationary
      intp_time.push_back(time_[n]);
      intp_phi.push_back(phiStart+ratio*dphi); 
      intp_theta.push_back(theta_[n]);
    }
  }

  dbskr_scurve_sptr orig_scurve = new dbskr_scurve( intp_sh_pt.size(),
                                                    intp_sh_pt, 
                                                    intp_time, intp_theta, intp_phi,
                                                    false, interpolate_ds_,
                                                    false, subsample_ds_);

  return orig_scurve;
}

dbskr_scurve_sptr dbskr_scurve::get_replacement_scurve(int num_pts)
{
  // construct the replacement contour: this is a shock curve 
  // consisting of point samples at the splice point.

  // To reduce errors due to sampling mismatched between the 
  // original and the pruned curve, the pruned curve should be sampled
  // with the same number of points as the original curve

  vcl_vector< vgl_point_2d<double> > intp_sh_pts;
  vcl_vector<double> intp_time, intp_phi, intp_theta;

  int n = 0; //index of the splice point
  double phiStart = phi_[n];
  double phiEnd = 0;

  double dphi = phiEnd-phiStart;
  double apprxds = vcl_abs(dphi)*time_[n];

  //num of extra samples
  int num_samples = vcl_max(num_pts, int(apprxds/subsample_ds_));
  
  //add the interpolated samples
  for(int j=0; j<=num_samples; j++) //not ot forget the one at the starting point
  {
    float ratio = (float)j/(float)num_samples;

    intp_sh_pts.push_back(sh_pt_[n]);     //at an A-inf point point remains stationary
    intp_time.push_back(time_[n]);
    intp_phi.push_back(phiStart+ratio*dphi); //phi needs to be interpolated
    intp_theta.push_back(theta_[n]);
  }

  dbskr_scurve_sptr pruned_scurve = new dbskr_scurve( intp_sh_pts.size(),
                                                      intp_sh_pts, 
                                                      intp_time, intp_theta, intp_phi,
                                                      false, interpolate_ds_,
                                                      false, subsample_ds_);
  return pruned_scurve;
}


void dbskr_scurve::set_euler_spiral_completion_length()
{

    virtual_length_=0.0;

    // Compute Euler Spiral

    // 1) Determine tangent pairs first
    vgl_point_2d<double> minus_last_point=bdry_minus_.back();
    vgl_point_2d<double> minus_next_to_last_point=
        bdry_minus_[bdry_minus_.size()-2];

    vgl_point_2d<double> plus_last_point=bdry_plus_.back();
    vgl_point_2d<double> plus_next_to_last_point=
        bdry_plus_[bdry_plus_.size()-2];
    
    vgl_line_2d<double> minus_line(minus_next_to_last_point,
                                   minus_last_point);

    vgl_line_2d<double> plus_line(plus_last_point,
                                  plus_next_to_last_point);
    
    // 2) Compute Euler Spiral
    dbgl_eulerspiral es(
        plus_last_point,
        plus_line.slope_radians(),
        minus_last_point,
        minus_line.slope_radians());

    // For debugging purposes
    // vcl_vector<vgl_point_2d<double> > samples;
    // es.compute_spiral(samples, 0.1);
    
    // vcl_cout<<"curve=[";
    // for ( unsigned int s=0; s < samples.size() ; ++s)
    // {
    //     if ( s == samples.size()-1)
    //     {
    //         vcl_cout<<samples[s].x()<<" "<<samples[s].y()<<"];"<<vcl_endl;
    //         break;
    //     }
    //     vcl_cout<<samples[s].x()<<" "<<samples[s].y()<<"; ..."<<vcl_endl;
    // }

    // vcl_cout<<"Es length: "<<es.length()<<vcl_endl;

    virtual_length_ = es.length();

}

//: new splice cost definition that uses joint curve matching to compute
//  the splice cost instead of the simplified version of the cost function
//  Amir: passed the R constant to the splice cost function for elastic splice cost
double dbskr_scurve::splice_cost_new(double R_const, bool construct_circular_ends, bool dpmatch_combined)
{
  //1) First construct a version of the current shock curve with the opposite end closed
  dbskr_scurve_sptr orig_scurve = get_original_scurve(construct_circular_ends);

  //2) Next construct the replacement contour: this is a shock curve 
  //   consisting of point samples at the splice point.
  dbskr_scurve_sptr pruned_scurve = get_replacement_scurve(orig_scurve->num_points());

  if (dpmatch_combined) {
      //3) compute splice cost by computing the elastic match cost
    dbskr_dpmatch_combined d(orig_scurve, pruned_scurve);
    d.set_R(R_const);
    
    d.Match();
    //amir's recommendation:: 
    //double init_dr = d.init_dr();
    //double init_alp = d.init_phi();
    //double match_cost = d.finalCost() + init_dr + init_alp;

    double match_cost = d.finalCost();
    return match_cost;
  } 
  else {
    //3) compute splice cost by computing the elastic match cost
    dbskr_dpmatch d(orig_scurve, pruned_scurve);
    d.set_R(R_const);
    
    d.Match();
    //amir's recommendation:: 
    //double init_dr = d.init_dr();
    //double init_alp = d.init_phi();
    //double match_cost = d.finalCost() + init_dr + init_alp;

    double match_cost = d.finalCost();
    return match_cost;
  }
}

//:
//  Amir: passed the R constant to the splice cost function for elastic splice cost
double dbskr_scurve::splice_cost(double R_const, bool new_def, bool construct_circular_ends, bool dpmatch_combined, bool leaf_dart) 
{
  if (new_def)
    return splice_cost_new(R_const, construct_circular_ends, dpmatch_combined);

  int n;
  double xp,yp,xm,ym,xs,ys;
  double R, R1, R2;
  double tp=0,tm=0,dt=0,dt1=0,dt2=0;
  double delLen=0,replLen=0; //unused newCost=0;
  //unused double newDelLen=0;
  
  //all splices are done to remove the subtree starting at the given dart
  //so all scurves are sampled so that it is deleted up to the starting point
  //
  //if (time_[num_points_-1] < 1e-1)  //current dart ends at an A3 point
  //Amir:: leaf darts do not end at a point anymore so we need a flag to 
  //       indicate that this dart is a leaf dart
  if (leaf_dart)
  {
    //compute the length of the new contour(circular arc) to replace it
    n = 0;
    xp = boundary_plus_x(n);
    yp = boundary_plus_y(n);
    xm = boundary_minus_x(n);
    ym = boundary_minus_y(n);
    
    xs = sh_pt_x(n);
    ys = sh_pt_y(n);
   
    tp = vcl_atan2(yp-ys,xp-xs);
    tm = vcl_atan2(ym-ys,xm-xs);
    dt = angleDiff_new(tp,tm);
    R = time_[n];

    //the length of the contour to be deleted
    delLen = bdry_plus_length_+bdry_minus_length_+virtual_length_;
    //0.9 was replaced with 1.0 by Fatih, since Sebastian had 1.0 in his original version (which original version?). 05/01/08
    //replLen = 1.0*R*vcl_fabs(dt);
    replLen = 0.9*R*vcl_fabs(dt); //Amir: As in Matching-Tek
   

  } 
  else { //regular shock edge (internal or one with an A-inf node)

    double d1, d2;

    // compute the length of the replacement circular arc at the end of the shock
    n = 0;
    xp = boundary_plus_x(n);
    yp = boundary_plus_y(n);
    xm = boundary_minus_x(n);
    ym = boundary_minus_y(n);

    xs = sh_pt_x(n);
    ys = sh_pt_y(n);

    tp = atan2(yp-ys,xp-xs);
    tm = atan2(ym-ys,xm-xs);
    dt1 = angleDiff_new(tp,tm);
    R1 = time_[n];
    d1 = R1*vcl_fabs(dt1);
    
    
    // need to fit an arc on the end node (either for A-inf or for the 
    // other branches at that node that have been spliced)

    //compute the length of the arc at the starting point
    n = num_points_-1;
    xp = boundary_plus_x(n);
    yp = boundary_plus_y(n);
    xm = boundary_minus_x(n);
    ym = boundary_minus_y(n);

    xs = sh_pt_x(n);
    ys = sh_pt_y(n);
    
    tp = atan2(yp-ys,xp-xs);
    tm = atan2(ym-ys,xm-xs);
    dt2 = angleDiff_new(tp,tm);
    R2 = time_[n];
    d2 = R2*vcl_fabs(dt2);

#if 0     // this switch is actually a MISTAKE in the original code of Thomas Sebastian in
    // /vision\projects\kimia\shock-matching\code\CODE-IRIX6.5\Matching-ESF
    // optionally repeated here to be able to get exact same numbers with the experiment in
    // \vision\projects\kimia\shock-matching\huge-database-new-shocks\contour-files\1030match\shgs
    // 1 should be switched to 0 to remove this code, see the reasoning why its wrong:
    // \vision\docs\kimia\intranet\Shock matching meetings\Shock Matching Debug.ppt
    if(time_[0] < time_[n]){
      d1=vcl_fabs(dt1)*time_[0];
      d2=vcl_fabs(dt2)*time_[n];
    }
    else {
      d1=vcl_fabs(dt2)*time_[n];
      d2=vcl_fabs(dt1)*time_[0];
    }
#endif

    //compute the total length of the contour to be deleted
    //delLen = bdry_plus_length_ + bdry_minus_length_ + 1.0*d1;
    delLen = bdry_plus_length_ + bdry_minus_length_ + 0.9*d1; //Amir: As in Matching-Tek

    //0.9 was replaced with 1.0 by Fatih, since Sebastian had 1.0 in his original version. 05/01/08
    //replLen = 1.0*d2;
    replLen = 0.9*d2; //Amir: As in Matching-Tek
  }
  
#if 0 // the original version in /vision\projects\kimia\shock-matching\code\CODE-IRIX6.5\Matching-ESF
  //float fac=2.2f;//to account for sampling error
  float fac = 3.0; //in Matching-Tek version
  float ratio=(float)vcl_fabs(delLen/replLen);
  if (ratio > 2.5){
    float newDelLen=(float)(2.0*replLen+0.25*delLen*(ratio-2.0)/ratio);
    return fac*vcl_fabs(newDelLen-replLen);
  }
  else{
    return fac*vcl_fabs(delLen-replLen);
  }
#else // the corrected version by Amir Tamrakar and Fatih Calakli 
  float fac=2.2f;//to account for sampling error
  float ratio=vcl_fabs(delLen/replLen);
  if (ratio > 2.5){
    float newDelLen=2.0*replLen+0.25*delLen*(ratio-2.0)/ratio;
    return fac*vcl_fabs(newDelLen-replLen) + 2*vcl_fabs(time_[0]-time_[num_points_-1]);//2*|r0_hat - rf|
  }
  else{
    return fac*vcl_fabs(delLen-replLen) + 2*vcl_fabs(time_[0]-time_[num_points_-1]);//2*|r0_hat - rf|
  }
#endif
}

void dbskr_scurve::writeData(vcl_string fname)
{
  vcl_ofstream outfp(fname.c_str(), vcl_ios::out);

  //write out all the information for each sample point along the shock branch
  for (int i=0; i<num_points_; i++){
    outfp.precision(15);
    outfp << arclength_[i] << " ";
    outfp.precision(15);
    outfp << sh_pt_[i].x() << " " << sh_pt_[i].y() << " "; 
    outfp.precision(15);
    outfp << time_[i] << " ";
    outfp.precision(15);
    outfp << theta_[i] << " ";
    outfp.precision(15);
    outfp << phi_[i] << " ";
    outfp.precision(15);    
    outfp << bdry_plus_[i].x() << " " << bdry_plus_[i].y() << " ";
    outfp.precision(15);
    outfp << bdry_plus_arclength_[i] << " ";
    outfp.precision(15);
    outfp << bdry_plus_angle_[i] << " ";
    outfp.precision(15);
    outfp << bdry_minus_[i].x() << " " << bdry_minus_[i].y() << " ";
    outfp.precision(15);
    outfp << bdry_minus_arclength_[i] << " ";
    outfp.precision(15);
    outfp << bdry_minus_angle_[i] << " ";
    outfp.precision(15);
    outfp<< area_[i];
    outfp << vcl_endl;
  }

  outfp.close();
}


//: for visualization purposes
void dbskr_scurve::get_polys(vcl_vector<vsol_polygon_2d_sptr>& polys)
{
  vcl_vector<vsol_point_2d_sptr> pts_s;
  vcl_vector<vsol_point_2d_sptr> pts_bp;
  vcl_vector<vsol_point_2d_sptr> pts_bm;
  
  for (int i=0; i<num_points_; i++) {
    pts_s.push_back(new vsol_point_2d(sh_pt_[i].x(), sh_pt_[i].y()));
    pts_bp.push_back(new vsol_point_2d(bdry_plus_[i].x(), bdry_plus_[i].y()));
    pts_bm.push_back(new vsol_point_2d(bdry_minus_[i].x(), bdry_minus_[i].y()));
  }

  vsol_polygon_2d_sptr p_s = new vsol_polygon_2d(pts_s);
  vsol_polygon_2d_sptr p_bp = new vsol_polygon_2d(pts_bp);
  vsol_polygon_2d_sptr p_bm = new vsol_polygon_2d(pts_bm);
  polys.push_back(p_s);
  polys.push_back(p_bp);
  polys.push_back(p_bm);

}
