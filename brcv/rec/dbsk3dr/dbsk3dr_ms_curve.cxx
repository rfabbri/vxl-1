#include <dbsk3dr/dbsk3dr_ms_curve.h>
//:
// \file
#include <vgl/vgl_point_3d.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_fstream.h>
#include <vnl/vnl_math.h>

#include <vcl_string.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor
//---------------------------------------------------------------------------
dbsk3dr_ms_curve::dbsk3dr_ms_curve()
{
  storage_=new vcl_vector<vgl_point_3d<double>*>();
  isOpen_=true;
}

//---------------------------------------------------------------------------
//: Constructor from a vcl_vector of points
//---------------------------------------------------------------------------

dbsk3dr_ms_curve::dbsk3dr_ms_curve(const vcl_vector<vgl_point_3d<double>*> &new_vertices)
{
  storage_=new vcl_vector<vgl_point_3d<double>*>(new_vertices);
  isOpen_=true;
}

//---------------------------------------------------------------------------
//: Constructor from a dbmsh3d_curve
//---------------------------------------------------------------------------

dbsk3dr_ms_curve::dbsk3dr_ms_curve (dbsk3d_ms_curve* MC, const bool flip)
{
  storage_=new vcl_vector<vgl_point_3d<double>*>();

  vcl_vector<dbmsh3d_vertex*> V_vec;
  MC->get_V_vec (V_vec);

  if (flip == false) {
    for (unsigned int i=0; i<V_vec.size(); i++) {
      dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) V_vec[i];
      double r = FV->compute_time_ve ();
      add_vertex (&(FV->get_pt()), r);

    }
  }
  else {
    for (int i=int(V_vec.size())-1; i>=0; i--) {
      dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) V_vec[i];
      double r = FV->compute_time_ve ();
      add_vertex (&(FV->get_pt()), r);
    }
  }

  isOpen_ = !MC->is_self_loop();
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
dbsk3dr_ms_curve::~dbsk3dr_ms_curve()
{
  delete storage_;
}

//***************************************************************************
// Comparison
//***************************************************************************

//***************************************************************************
// Internal status setting functions
//***************************************************************************
void dbsk3dr_ms_curve::clear(void)
{
  arcLength_.clear();
  s_.clear();
  phi_.clear();
  phis_.clear();
  phiss_.clear();
  theta_.clear();
  thetas_.clear();
  thetass_.clear();
  Tangent_.clear();
  Normal_.clear();
  Binormal_.clear();
  angle_.clear();
  curvature_.clear();
  torsion_.clear();
  totalCurvature_ = 0;
  totalAngleChange_ = 0;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the curvature of the vertex `i'
//---------------------------------------------------------------------------
double dbsk3dr_ms_curve::curvature(const int i) const
{
  assert(valid_index(i));
  return curvature_[i];
}

//---------------------------------------------------------------------------
//: Return the angle of the vertex `i'
//---------------------------------------------------------------------------
double dbsk3dr_ms_curve::angle(const int i) const
{
  assert(valid_index(i));
  return angle_[i];
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first point of the curve
// Require: in(new_p0)
//---------------------------------------------------------------------------
void dbsk3dr_ms_curve::set_p0(const vgl_point_3d<double>* new_p0)
{
  p0_ = (vgl_point_3d<double>*) new_p0;
  storage_->push_back(p0_);
}

//---------------------------------------------------------------------------
//: Set the last point of the curve
// Require: in(new_p1)
//---------------------------------------------------------------------------
void dbsk3dr_ms_curve::set_p1(const vgl_point_3d<double>* new_p1)
{
  p1_ = (vgl_point_3d<double>*) new_p1;
  storage_->push_back(p0_);
}

//---------------------------------------------------------------------------
//: Add another point to the curve
//---------------------------------------------------------------------------
void dbsk3dr_ms_curve::add_vertex(vgl_point_3d<double>* new_p, double r, bool bRecomputeProperties)
{
  storage_->push_back (new_p);
  radius_.push_back (r);
  if (bRecomputeProperties) 
    computeProperties();
}

//---------------------------------------------------------------------------
//: Remove one vertex from the intrinsic curve
//---------------------------------------------------------------------------
void dbsk3dr_ms_curve::remove_vertex(const int i, bool bRecomputeProperties)
{
  assert (valid_index(i));
  storage_->erase(storage_->begin() + i);
  if (bRecomputeProperties) computeProperties();
}

void dbsk3dr_ms_curve::modify_vertex(const int i, double x, double y, double z, bool bRecomputeProperties)
{
  assert (valid_index(i));
  (*storage_)[i]->set (x, y, z);
  if (bRecomputeProperties) computeProperties();
}

//: insert into i-1
void dbsk3dr_ms_curve::insert_vertex(const int i, double x, double y, double z, bool bRecomputeProperties)
{
  assert (valid_index(i));
  vgl_point_3d<double>* pt = new vgl_point_3d<double>(x, y, z);
  vcl_vector<vgl_point_3d<double>*>::iterator it = storage_->begin();
  it += i;
  storage_->insert(it, pt);
  if (bRecomputeProperties) computeProperties();
}

void dbsk3dr_ms_curve::read_con3_file (vcl_string fileName)
{
  double x, y, z;
  char buffer[2000];
  int nPoints;

  //clear the existing curve data
  if (size() !=0)
    clear();

  //1)If file open fails, return.
  vcl_ifstream fp(fileName.c_str(), vcl_ios::in);
  if (!fp) {
    vcl_cout<<" : Unable to Open "<<fileName<<vcl_endl;
    return;
  }

  //2)Read in file header.
  fp.getline(buffer,2000); //CONTOUR

  //fp.getline(buffer,2000); //OPEN/CLOSE
  //char openFlag[2000];
  vcl_string openFlag;
  //fp.getline(openFlag,2000);
  vcl_getline(fp, openFlag);
  //if (!vcl_Strncmp(openFlag,"OPEN",4))
  if (openFlag.find("OPEN",0) != vcl_string::npos)
    isOpen_ = true;
  //else if (!vcl_Strncmp(openFlag,"CLOSE",5))
  else if (openFlag.find("CLOSE",0) != vcl_string::npos)
    isOpen_ = false;
  else{
    vcl_cerr << "Invalid File " << fileName.c_str() << vcl_endl
             << "Should be OPEN/CLOSE " << openFlag << vcl_endl;
    return;
  }

  fp >> nPoints;
  vcl_cout << "Number of Points from Contour: " << nPoints
           << "\nContour flag is "<< isOpen_ << " (1 for open, 0 for close)\n";

  for (int i=0;i<nPoints;i++) {
    fp >> x >> y >> z;
    add_vertex (x, y, z);
  }

  fp.close();
  computeProperties();
}


//: Public function that calls the private functions to compute the various curve properties.
void dbsk3dr_ms_curve::computeProperties()
{
  // 0) initialize starting point and end point.
  p0_ = (*storage_)[1];
  p1_ = (*storage_)[storage_->size()-1];

  // 1) Reset datastructures
  arcLength_.clear();
  s_.clear();
  phi_.clear();
  phis_.clear();
  phiss_.clear();
  theta_.clear();
  thetas_.clear();
  thetass_.clear();
  Tangent_.clear();
  Normal_.clear();
  Binormal_.clear();
  angle_.clear();
  curvature_.clear();
  torsion_.clear();

  // 2) Setup the starting conditions
  //    Using finite difference, some pos has no value.
  //    These values will be copied from neighbor later.  
  if (size() ==0)
    return;
  else {
    arcLength_.push_back(-1);
    s_.push_back(-1);
    phi_.push_back(-1);
    phis_.push_back(-1); 
    phiss_.push_back(-1); 
    theta_.push_back(-1);
    thetas_.push_back(-1); 
    thetass_.push_back(-1); 
    Tangent_.push_back(NULL);
    Normal_.push_back(NULL); 
    Binormal_.push_back(NULL); 
    angle_.push_back(-1.0); 
    curvature_.push_back(-1.0); 
    torsion_.push_back(-1); 

    //The 2nd order terms.
    if (size()>1) {
      phis_.push_back(-1);
      phiss_.push_back(-1);
      thetas_.push_back(-1);
      thetass_.push_back(-1);
      Normal_.push_back(NULL);
      Binormal_.push_back(NULL);
      curvature_.push_back(-1);
      torsion_.push_back(-1);

      //The third order terms.
      if (size()>2) {
        phiss_.push_back(-1);
        thetass_.push_back(-1);
        torsion_.push_back(-1);
      }
    }
  }

  // 3) Compute the first derivative: arc length and angle.
  double prev_x = (*storage_)[0]->x();
  double prev_y = (*storage_)[0]->y();
  double prev_z = (*storage_)[0]->z();
  double length = 0;
  for (unsigned int i=1; i<size(); ++i)
  {
    double cur_x=(*storage_)[i]->x();
    double cur_y=(*storage_)[i]->y();
    double cur_z=(*storage_)[i]->z();
    double cur_dx = cur_x - prev_x;
    double cur_dy = cur_y - prev_y;
    double cur_dz = cur_z - prev_z;
    double dL = vcl_sqrt(cur_dx*cur_dx + cur_dy*cur_dy + cur_dz*cur_dz);
    s_.push_back(dL);
    length += dL;
    arcLength_.push_back(length);

    double phi = vcl_acos(cur_dz/dL);
    phi_.push_back(phi);
    double dLxy = dL*vcl_sin(phi);
    double theta = vcl_acos(cur_dx/dLxy);
    theta_.push_back(theta);

    vgl_vector_3d<double>* tangent = new vgl_vector_3d<double>(cur_dx, cur_dy, cur_dz);
    normalize(*tangent); //normalize the tangent vector.
    Tangent_.push_back(tangent);

    prev_x = cur_x;
    prev_y = cur_y;
    prev_z = cur_z;
  }
  assert (s_.size() == size());
  assert (arcLength_.size() == size());
  assert (phi_.size() == size());
  assert (theta_.size() == size());
  assert (Tangent_.size() == size());  

  // 4) Compute the second derivative: phi_s, theta_s, normal, binormal, curvature, angle
  totalCurvature_ = 0;
  totalAngleChange_ = 0;
  for (unsigned int i=2; i<size(); ++i)
  {
    double phis = (phi_[i] - phi_[i-1])/s_[i];
    phis_.push_back(phis);
    double thetas = (theta_[i] - theta_[i-1])/s_[i];
    thetas_.push_back(thetas);
    double curvature = vnl_math::hypot(phis, vcl_sin(phi_[i])*thetas);
    curvature_.push_back(curvature);

    //the Tangent[] is normalized.
    double angle = vcl_acos (vcl_fabs (dot_product (*(Tangent_[i]), *(Tangent_[i-1]))));
    angle_.push_back (angle); //at pos [i-1]

    totalCurvature_ += curvature;
    totalAngleChange_ += vcl_fabs(curvature);

    double cos_phi = vcl_cos(phi_[i]);
    double sin_phi = vcl_sin(phi_[i]);
    double cos_theta = vcl_cos(theta_[i]);
    double sin_theta = vcl_sin(theta_[i]);
    double normalx = cos_phi * cos_theta * phis - sin_phi * sin_theta * thetas;
    double normaly = cos_phi * sin_theta * phis - sin_phi * cos_theta * thetas;
    double normalz = - vcl_sin(phi_[i]) * phis;
    vgl_vector_3d<double>* normal = new vgl_vector_3d<double>(normalx, normaly, normalz);
    normalize(*normal); //normalize the normal vector.
    Normal_.push_back(normal);

    vgl_vector_3d<double>* binormal = new vgl_vector_3d<double>;
    *binormal = cross_product(*(Tangent_[i]), *normal);
    normalize(*binormal); //normalize the binormal vector.
    Binormal_.push_back(binormal);
  }
  assert (phis_.size() == size());
  assert (thetas_.size() == size());
  assert (curvature_.size() == size());
  angle_.push_back (-1);
  assert (angle_.size() == size());
  assert (Normal_.size() == size());
  assert (Binormal_.size() == size());

  // 5) Compute the third derivative: phi_ss, theta_ss, torsion
  for (unsigned int i=3; i<size(); ++i) {
    double phiss = (phis_[i] - phis_[i-1])/s_[i];
    phiss_.push_back(phiss);
    double thetass = (thetas_[i] - thetas_[i-1])/s_[i];
    thetass_.push_back(thetass);

    //compute torsion. 
    //torsion = -N dot B'. B' = (B[i]-B[i-1])/s[i]
    vgl_vector_3d<double> bp = (*Binormal_[i] - *Binormal_[i-1])/s_[i];
    normalize(bp);
    double torsion = - dot_product(*Normal_[i], bp);
    torsion_.push_back(torsion);
  }
  assert (phiss_.size() == size());
  assert (thetass_.size() == size());
  assert (torsion_.size() == size());

  assert (radius_.size() == size());

  // 6) Copy missing values from neighbors.
  assert (size() > 0);
  if (size() > 3) {
    phiss_[2] = phiss_[3]; 
    thetass_[2] = thetass_[3];
    torsion_[2] = torsion_[3];
  }

  if (size() > 2) {
    phis_[1] = phis_[2];
    phiss_[1] = phiss_[2]; 
    thetas_[1] = thetas_[2];
    thetass_[1] = thetass_[2];
    Normal_[1] = Normal_[2];
    Binormal_[1] = Binormal_[2];
    curvature_[1] = curvature_[2];
    torsion_[1] = torsion_[2];
    angle_[angle_.size()-1] = angle_[angle_.size()-2];
  }

  if (size() > 1) {
    arcLength_[0] = arcLength_[1];
    s_[0] = s_[1];
    phi_[0] = phi_[1];
    phis_[0] = phis_[1];
    phiss_[0] = phiss_[1];
    theta_[0] = theta_[1];
    thetas_[0] = thetas_[1];
    thetass_[0] = thetass_[1];
    Tangent_[0] = Tangent_[1];
    Normal_[0] = Normal_[1];
    Binormal_[0] = Binormal_[1];
    angle_[0] = angle_[1];
    curvature_[0] = curvature_[1];
    torsion_[0] = torsion_[1];  
  }
}

//##############################################################################
// OLD 2D curve code
#if 0

//: Compute arc length and normalized arc length
void dbsk3dr_ms_curve::computeArcLength()
{
  //Compute arc length
  arcLength_.clear();
  s_.clear();
  length_=0;
  arcLength_.push_back(0.0);
  s_.push_back(0.0);

  double px=(*storage_)[0]->x();
  double py=(*storage_)[0]->y();
  for (int i=1;i<size();i++)
  {
    double cx=(*storage_)[i]->x();
    double cy=(*storage_)[i]->y();
    double dL = vnl_math::hypot(cx-px,cy-py);
    length_ += dL;
    arcLength_.push_back(length_);
    s_.push_back(dL);
    px=cx;
    py=cy;
  }

  assert (s_.size()==arcLength_.size());


  //Compute normalized arc length
  normArcLength_.clear();
  for (int i=0;i<size();i++)
    normArcLength_.push_back(arcLength_[i]/length_);

#ifdef DEBUG
  vcl_cout << "Norm arc length values:\n";
  for (int i = 0; i<size(); i++)
    vcl_cout << "normArcLength_[" << i << "]: " << normArcLength_[i] << vcl_endl;

  vcl_cout << "arc length values:\n";
  for (int i = 0; i<size(); i++)
    vcl_cout << "arcLength_[" << i << "]: " << arcLength_[i] << vcl_endl;
#endif
}

//: Compute curvature. Assumes derivative computation has been done.
void dbsk3dr_ms_curve::computeCurvatures()
{
  //Compute curvature
  curvature_.clear();
  curvature_.push_back(0.0);
  totalCurvature_=0.0;

  for (int i=1;i<size();i++)
  {
    double pdx=dx_[i-1];
    double pdy=dy_[i-1];
    double cdx=dx_[i];
    double cdy=dy_[i];
    double dL=arcLength_[i]-arcLength_[i-1];
    double d2x=0, d2y=0;
    if (dL > ZERO_TOLERANCE) {
      d2x=(cdx-pdx)/dL;
      d2y=(cdy-pdy)/dL;
    }
    double K = 0;
    if (vcl_fabs(cdx) >= ZERO_TOLERANCE || vcl_fabs(cdy) >= ZERO_TOLERANCE)
      K=(d2y*cdx-d2x*cdy)/vcl_pow((vcl_pow(cdx,2)+vcl_pow(cdy,2)),3/2);
#ifdef DEBUG
    vcl_cout << d2x << ' ' << d2y << ' ' << dL << ' ' << cdx << ' ' << cdy << ' ' << K << vcl_endl;
#endif
    curvature_.push_back(K);
    totalCurvature_+=K;
  }

#if 1 // commented out
  // Deal with the last point for a closed curve separately.
  if (!isOpen_)
  {
    double pdx=dx_[size()-1];
    double pdy=dy_[size()-1];
    double cdx=dx_[0];
    double cdy=dy_[0];
    double dL=arcLength_[0]-arcLength_[size()-1];
    double d2x, d2y;
    if (dL > ZERO_TOLERANCE ) {
      d2x=(cdx-pdx)/dL;
      d2y=(cdy-pdy)/dL;
    }
    else
      d2x=d2y=0;
    double K;
    if (vcl_fabs(cdx) < ZERO_TOLERANCE && vcl_fabs(cdy) < ZERO_TOLERANCE)
      K=0;
    else
      K=(d2y*cdx-d2x*cdy)/vcl_pow((vcl_pow(cdx,2)+vcl_pow(cdy,2)),3/2);
    curvature_[0]=K;
  }
#endif // 0
}

//: Compute derivatives
void dbsk3dr_ms_curve::computeDerivatives()
{
  //Compute derivatives
  dx_.clear();
  dx_.push_back(0.0);
  dy_.clear();
  dy_.push_back(0.0);

  double px=(*storage_)[0]->x();
  double py=(*storage_)[0]->y();
  for (int i=1;i<size();i++)
  {
    double cx=(*storage_)[i]->x();
    double cy=(*storage_)[i]->y();
    double dL=vcl_sqrt(vcl_pow(cx-px,2)+vcl_pow(cy-py,2));
    if (dL > ZERO_TOLERANCE) {
      dx_.push_back((cx-px)/dL);
      dy_.push_back((cy-py)/dL);
    }
    else{
      dx_.push_back(0.0);
      dy_.push_back(0.0);
    }
    px=cx;
    py=cy;
  }

#if 1 // commented out
  //Deal with the last point for a closed curve separately.
  if (!isOpen_)
  {
    double px=(*storage_)[size()-1]->x();
    double py=(*storage_)[size()-1]->y();
    double cx=(*storage_)[0]->x();
    double cy=(*storage_)[0]->y();
    double dL=vcl_sqrt(vcl_pow(cx-px,2)+vcl_pow(cy-py,2));
    dx_[0]=(cx-px)/dL;
    dy_[0]=(cy-py)/dL;
  }
#endif // 0
}

//: Compute angles
void dbsk3dr_ms_curve::computeAngles()
{
  angle_.clear();
  angle_.push_back(0.0);
  totalAngleChange_=0.0;

  double px=(*storage_)[0]->x();
  double py=(*storage_)[0]->y();
  for (int i=1;i<size();i++)
  {
    double cx=(*storage_)[i]->x();
    double cy=(*storage_)[i]->y();
    double theta=vcl_atan2(cy-py,cx-px);
    angle_.push_back(theta);
    px=cx;
    py=cy;
  }

  if (size()>2) {
    angle_[0]=angle_[1];
    for (unsigned int i=1;i<angle_.size();i++) {
#ifdef DEBUG
      vcl_cout << angle_[i] << ' ' << angle_[i-1] << vcl_endl;
#endif
      totalAngleChange_ += vcl_fabs(angle_[i]-angle_[i-1]);
    }
  }

//: IMPORTANT NOTE: in open curve matching giving the inputs as
//  OPEN curves or CLOSE curves (i.e. in .con file)
//  changes the cost computation
//  due to the following operation
//  In closed curve matching, input curves should always be given
//  as CLOSE curves.
#if 1
  //Deal with the last point for a closed curve separately.
  if (!isOpen_)
  {
    double px=(*storage_)[size()-1]->x();
    double py=(*storage_)[size()-1]->y();
    double cx=(*storage_)[0]->x();
    double cy=(*storage_)[0]->y();
    double theta=vcl_atan2(cy-py,cx-px);
    angle_[0]=theta;

#if 0 // commented out
    // TBS source code tests the distance between first and last points!!
    if (vcl_sqrt((cx-px)*(cx-px)+(cy-py)*(cy-py))<2)
      c->angle[0]=vcl_atan2(cy-py,cx-px);
#endif // 0
  }
#endif
}

#endif
