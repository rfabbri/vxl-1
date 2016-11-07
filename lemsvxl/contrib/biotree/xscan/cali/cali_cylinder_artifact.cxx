#include <cali/cali_cylinder_artifact.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <dbgl/algo/dbgl_intersect.h>
#include <vcl_string.h>


cali_cylinder_artifact::~cali_cylinder_artifact(void) { 
  if(bounding_box_) delete bounding_box_;
} 

cali_cylinder_artifact::cali_cylinder_artifact(vgl_point_3d<double> center)
  : center_(center),bounding_box_(0)
{
  create_balls(); 
  create_bounding_box();
}

cali_cylinder_artifact::cali_cylinder_artifact(vgl_point_3d<double> center,cali_param par)
  : par_(par),center_(center),bounding_box_(0)

{
  create_balls(); 
  create_bounding_box();
}

cali_cylinder_artifact::cali_cylinder_artifact(vgl_point_3d<double> center,
                                               cali_param par,
                                               vcl_vector<double> x_coord_tol, 
                                               vcl_vector<double> y_coord_tol,
                                               vcl_vector<double> z_coord_tol,
                                               vcl_vector<double> rad_tol) :par_(par),
                                                                            center_(center),
                                                                            bounding_box_(0)

{
  create_balls_with_tolerance(x_coord_tol,y_coord_tol,z_coord_tol,rad_tol); 
  create_bounding_box_with_tolerance();
}


cali_cylinder_artifact::cali_cylinder_artifact(cali_cylinder_artifact const& artf) :
center_(artf.center_), 
bounding_box_(artf.bounding_box_)

{
        balls.resize(par_.BALL_NUMBER);
        for(int i=0; i<par_.BALL_NUMBER; i++) {
                balls[i] = vgl_sphere_3d<double>(artf.balls[i].centre(), artf.balls[i].radius()); 
        }
        create_bounding_box();
}

// reading the ball centres and radii from a txt file called CMMinfo.txt
// the values in the txt file are stored in the following format:
// BALL_NO:        1
// Radius:         0.7978
// X:              -0.0183
// Y:              13.0493 
// Z:              -0.7856 
// BALL_NO:        2
// Radius:         0.4131
// X:              9.0630
// Y:              9.0523 
// Z:             -2.0726

void cali_cylinder_artifact::create_balls() {

        vcl_string txt_file = par_.CMMINFO;
        vcl_ifstream fstream(txt_file.c_str(),vcl_ios::in);

        int data_size = 5*par_.BALL_NUMBER;
        vcl_vector<double>values(data_size);
        char val_string[256];
        int j = 0;
        while (j < data_size && !fstream.eof()) {
                fstream.getline(val_string, 256,':');
                fstream >> values[j];
                j++;
        }         
        fstream.close();

        if(j < data_size){
                vcl_cerr << "ERROR Read " << j << " values from " << par_.CMMINFO << "\n";
                vcl_cerr << "Expected " << data_size << "\n";
                vcl_exit(1);
        }

        int k = 0;
        double x,y,z,radius;
        balls.resize(par_.BALL_NUMBER);
        for (int i = 0;i<par_.BALL_NUMBER;++i)
        {   
                radius = values[k+1] ;
                x = values[k+2] ;
                y = values[k+3] ;
                z = values[k+4] ;
                balls[i] = vgl_sphere_3d<double> (vgl_homg_point_3d<double>(x,y,z), radius);
                k = k+5;
        }
}

void cali_cylinder_artifact::create_balls_with_tolerance(const vcl_vector<double>& x_coord_tol,
                                                         const vcl_vector<double>& y_coord_tol,
                                                         const vcl_vector<double>& z_coord_tol,
                                                         const vcl_vector<double>& rad_tol) 
    
{
        vcl_string txt_file = par_.CMMINFO;
        vcl_ifstream fstream(txt_file.c_str(),vcl_ios::in);

        char val_string[256];
        int data_size = 5*par_.BALL_NUMBER;
        vcl_vector<double>values(data_size);
        int j = 0;
        while (j < data_size && !fstream.eof()) {
                fstream.getline(val_string, 256,':');
                fstream >> values[j];
                j++;
        }         
        fstream.close();

        if(j < data_size){
                vcl_cerr << "ERROR Read " << j << " values from " << par_.CMMINFO << "\n";
                vcl_cerr << "Expected " << data_size << "\n";
                vcl_exit(1);
        }

        int k = 0;
        double x,y,z,radius;
        balls.resize(par_.BALL_NUMBER);
        for (unsigned i = 0;i<par_.BALL_NUMBER && i < x_coord_tol.size() ;++i)
        {   
                radius = values[k+1] + rad_tol[i];
                x = values[k+2]      + x_coord_tol[i];
                y = values[k+3]      + y_coord_tol[i];
                z = values[k+4]      + z_coord_tol[i];
                balls[i] = vgl_sphere_3d<double> (vgl_homg_point_3d<double>(x,y,z), radius);
                k = k+5;
        }
}

void
cali_cylinder_artifact::create_bounding_box_with_tolerance() {

  // a small addition to the sphere radius for the ball extensions
    

 
        double r = par_.RADIUS + 2*par_.BALL_RADIUS_BIG ;

  // min position of the box

  vgl_point_3d<double> min_point(-1*r, -1*r, -par_.HEIGHT);

  // max position of the box
  vgl_point_3d<double> max_point(r, r, par_.HEIGHT);

  if(bounding_box_) delete bounding_box_;
  bounding_box_ = new vgl_box_3d<double> (min_point, max_point);
}


// return the bounding box of the cylinder   
void
cali_cylinder_artifact::create_bounding_box() {

  // a small addition to the sphere radius for the ball extensions
    

 
     double r = par_.RADIUS + par_.BALL_RADIUS_BIG;

  // min position of the box

  vgl_point_3d<double> min_point(-1*r, -1*r, -par_.HEIGHT);

  // max position of the box
  vgl_point_3d<double> max_point(r, r, par_.HEIGHT);

  if(bounding_box_) delete bounding_box_;
  bounding_box_ = new vgl_box_3d<double> (min_point, max_point);
}

// translates a given point from p1 to p2
vgl_point_3d<double>
cali_cylinder_artifact::project_point(vgl_point_3d<double> point, 
         vnl_quaternion<double> const& rot, 
         vgl_point_3d<double> trans) {
  vnl_vector<double> p1(3);
  p1[0] = point.x();
  p1[1] = point.y();
  p1[2] = point.z();
  vnl_vector<double> result = rot.rotate(p1);
  vgl_point_3d<double> p2(result[0]+trans.x(), result[1]+trans.y(), result[2]+trans.z());
  return p2;
}

void
cali_cylinder_artifact::change_position(vnl_quaternion<double> const& rot, 
                                   vgl_point_3d<double> const& trans) {

  // change the center of the cylinder
  vgl_point_3d<double> p = project_point(center_, rot, trans);
  center_.set(p.x(), p.y(), p.z());

  //create a min and max
  double min[3], max[3];
  min[0]=min[1]=min[2]=max[0]=max[1]=max[2]=0;

  // change the balls position
  for (int i=0; i<par_.BALL_NUMBER; i++) {
    p = project_point(balls[i].centre(), rot, trans);
    balls[i].set_centre(p);

    // recompute the bounding box min, max points
    if (p.x() < min[0]) 
      min[0]=p.x();
    if (p.y() < min[1]) 
      min[1]=p.y();
    if (p.z() < min[2]) 
      min[2]=p.z();

    if (p.x() > max[0]) 
      max[0]=p.x();
    if (p.y() > max[1]) 
      max[1]=p.y();
    if (p.z() > max[2]) 
      max[2]=p.z();
  }
  
  // change the bounding box position, namely create a new one based on the new 
  // sphere positions
  if(bounding_box_) delete bounding_box_;
  bounding_box_ = new vgl_box_3d<double> (min, max);
 

   bounding_box_->set_depth(bounding_box_->depth() + 2*par_.BALL_RADIUS_BIG);
  bounding_box_->set_width(bounding_box_->width() + 2*par_.BALL_RADIUS_BIG);
  bounding_box_->set_height(bounding_box_->height() + 2*par_.BALL_RADIUS_BIG);
}

vcl_vector<vgl_point_3d<double> > 
cali_cylinder_artifact::ball_centers(void)
{
  vcl_vector<vgl_point_3d<double> > list;
  for (int i=0; i<par_.BALL_NUMBER; i++) { 
    vgl_point_3d<double> center = balls[i].centre();
    list.push_back(center);
  }
  return list;
}

vcl_vector<double> cali_cylinder_artifact::ball_radii(void)
{
  vcl_vector<double>radii;
  for (int i=0; i<par_.BALL_NUMBER; i++) { 
//    double radius = balls[i]->radius();
    double radius = balls[i].radius();
    radii.push_back(radius);
  }
  return radii;
}


double
cali_cylinder_artifact::ray_artifact_intersect(vgl_homg_line_3d_2_points<double> const& line){
  // return the first intersecting sphere's length of the intersection
  for (int i=0; i<par_.BALL_NUMBER; i++) { 
    double length_of_intersect = dbgl_intersect::sphere_homg_line_intersect(line, balls[i]);
    if (length_of_intersect> 0) {
      return length_of_intersect;
    }
  }
  return 0;
}


double
cali_cylinder_artifact::ray_artifact_intersect(vgl_homg_line_3d_2_points<double> const& line) const{
  // return the first intersecting sphere's length of the intersection
  for (int i=0; i<par_.BALL_NUMBER; i++) { 
    double length_of_intersect = dbgl_intersect::sphere_homg_line_intersect(line, 
                    *const_cast<vgl_sphere_3d<double>* >(&(balls[i])));
    if (length_of_intersect> 0) {
      return length_of_intersect;
    }
  }
  return 0;
}

void
cali_cylinder_artifact::print(vcl_ostream &s) {
  for (int i=0; i < par_.BALL_NUMBER; i++) {
    s << "Cylinder Center=" <<  center_ << vcl_endl;
    //s << i << " Centre=" << balls[i]->centre() << " radius=" << balls[i]->radius() << "\n"; 
    s << i << " Centre=" << balls[i].centre() << " radius=" << balls[i].radius() << "\n"; 
  }
}
