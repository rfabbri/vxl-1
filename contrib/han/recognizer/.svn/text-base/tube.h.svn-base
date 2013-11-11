// This is 
#ifndef tube_h_
#define tube_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  defines a tube in 3D by a center point, radius, length and orientation
// \author Dongjin Han (han@lems.brown.edu)
// \date   8/2/2006
//
// \verbatim
//  Modifications
//   Aug 2006 created
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_iosfwd.h>
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

class tube
{
  vgl_point_3d<double> location_,center_,center1_,center2_,center3_,center4_;
  double radius_,radius1_,radius2_,radius3_,radius4_;
  double length_,length1_,length2_,length3_,length4_;
  vgl_vector_3d<double> orient_,orient1_,orient2_,orient3_,orient4_;
  vgl_polygon <double> pol1_,pol2_,pol3_,pol4_;
  vgl_point_3d<double> e11_,e12_,e21_,e22_,e31_,e32_,e41_,e42_;

  vgl_vector_3d<double> t_,t1_,t2_,t3_,t4_;

  vgl_polygon<double> p1_,p2_,p3_,p4_;
  bool use1_,use2_,use3_,use4_;

 public:
  //: Default constructor
  tube(void): center_(0.0, 0.0, 0.0), radius_(0.0), length_(0.0) {}


  // one end point and vector definition
 tube(vgl_point_3d<double> location, vgl_point_3d<double> e12, vgl_point_3d<double> e22,
    vgl_point_3d<double> e32, vgl_point_3d<double> e42, double radius, bool use1,bool use2,bool use3,bool use4)
    :location_(location),e12_(e12), e22_(e22),e32_(e32),e42_(e42),radius_(radius),
    e11_(location), e21_(location),e31_(location),e41_(location),
    use1_(use1),use2_(use2),use3_(use3),use4_(use4)
 {
   t1_=e12_-e11_;
   t2_=e22_-e21_;
   t3_=e32_-e31_;
   t4_=e42_-e41_;

   
   length1_=t1_.length();
   length2_=t2_.length();
   length3_=t3_.length();
   length4_=t4_.length();

   radius1_=radius_; radius2_=radius_;radius3_=radius_;radius4_=radius_;
 }

// two end point definition
 tube(vgl_point_3d<double> location,vgl_vector_3d<double> t1,vgl_vector_3d<double> t2,vgl_vector_3d<double> t3,vgl_vector_3d<double> t4 ,
   double radius,double length, bool use1,bool use2,bool use3,bool use4)
   :location_(location),t1_(t1),t2_(t2),t3_(t3),t4_(t4),radius_(radius),length_(length),
   e11_(location), e21_(location),e31_(location),e41_(location),
   use1_(use1),use2_(use2),use3_(use3),use4_(use4)
 {
   
   e12_=e11_+t1_;
   e22_=e21_+t2_;
   e32_=e31_+t3_;
   e42_=e41_+t4_;
   //length1_=length_; length2_=length_; length3_=length_; length4_=length_;
   length1_=t1_.length();
   length2_=t2_.length();
   length3_=t3_.length();
   length4_=t4_.length();

   radius1_=radius_; radius2_=radius_;radius3_=radius_;radius4_=radius_;

 }


 // tube(const double v[8])
 //   :center_(v[0], v[1], v[2]), radius_(v[3]), length_(v[5]), orient_(vgl_vector_3d<double> (v[6], v[7], v[8])) {}

 // tube(double cx, double cy, double cz, double radius, double length)
 //   :center_(cx, cy, cz), radius_(radius), length_(length), orient_(vgl_vector_3d<double> (0,0,1)) {}

//  tube(vgl_point_3d<double> center, double radius, double length)
 //   :center_(center), radius_(radius), length_(length), orient_(vgl_vector_3d<double> (0,0,1)) {}

 // tube(vgl_point_3d<double> center, double radius, double length, vgl_vector_3d<double> orient)
 //   :center_(center), radius_(radius), length_(length), orient_(orient) {}

  ~tube(void){}

  //: getters
  vgl_point_3d<double> center() const { return center_; }
  double radius() const { return radius_; }
  double length() const { return length_; }
  vgl_vector_3d<double> orientation() const { return orient_; }

  //: setters
  void set_center(vgl_point_3d<double> center) { center_ = center; }
  void set_radius(double radius) {
    radius_ = radius;
    radius1_ = radius;
    radius2_ = radius;
    radius3_ = radius;
    radius4_ = radius;}
  void set_radius(double radius1,double radius2,double radius3,double radius4) {
    
    radius1_ = radius1;
    radius2_ = radius2;
    radius3_ = radius3;
    radius4_ = radius4;}
  void set_length(double length) { 
    t1_=length*t1_/t1_.length();
    t2_=length*t2_/t2_.length();
    t3_=length*t3_/t3_.length();
    t4_=length*t4_/t4_.length(); 

    length_ = length;
    length1_ = length;
    length2_ = length;
    length3_ = length;
    length4_ = length;}
  void set_length(double length1,double length2,double length3,double length4) 
  {
    length1_ = length1;
    length2_ = length2;
    length3_ = length3;
    length4_ = length4;


    t1_=length1*t1_/t1_.length();
    t2_=length2*t2_/t2_.length();
    t3_=length3*t3_/t3_.length();
    t4_=length4*t4_/t4_.length(); 


  }
  void set_orientation (vgl_vector_3d<double> orient) { orient_ = orient; }

  //: operations
 // bool operator==(tube<double> const& cyl) const;

  //: Writes "<tube center=(x0,y0,z0), radius=r, length=l, direction=(x1,y1,z1)>" to stream
//  vcl_ostream& print(vcl_ostream& s) const;

  bool contains(vgl_point_3d<double>);
  int  countIN(vcl_vector <vgl_point_3d<double> >);
  void init();
  vnl_vector <int> search(vcl_vector <vgl_point_3d<double> >, vgl_point_3d <double> );

  void ranger(vcl_vector<vgl_point_3d<double> > pts);

};



#endif // tube_h_

