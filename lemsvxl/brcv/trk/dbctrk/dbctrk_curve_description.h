#ifndef dbctrk_curve_description_h_
#define dbctrk_curve_description_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
// \brief interaction of vdgl_digital_curve and 2d images
//
// \verbatim
// Initial version January 24, 2003
// Modifications
// 21 August 2003 - Vishal Jain : added some more properties such as angles,points
// bounding_box.
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_iostream.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsl/vsl_binary_io.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vil/vil_image_view.h>
#include<vbl/vbl_array_2d.h>
#include <dbctrk/dbctrk_curve.h>

class dbctrk_curve_description
{
 public:
  vgl_point_2d<double> center_;

  vsol_digital_curve_2d_sptr curvepos;
  vsol_digital_curve_2d_sptr curveneg;

  int maskradius;

  vcl_vector<vcl_vector<vsol_point_2d_sptr> > maskpos;
  vcl_vector<vcl_vector<vsol_point_2d_sptr> > maskneg;

  //: stores the valid mask entries starts from top left 
  vcl_vector<vcl_vector<bool> > isvalidmaskpos;
  vcl_vector<vcl_vector<bool> > isvalidmaskneg;
  

  Curve * curve_;
  Curve * coarser_curve_;

  vcl_vector<double> angles_;
  vcl_vector<double> grad_;
  vcl_vector<vgl_point_2d<double> > cms_; // center of masses
  vsol_box_2d_sptr   cbox_;
  double         splice_cost_;

  vcl_vector<double> Prcolor,Nrcolor;
  vcl_vector<double> Pgcolor,Ngcolor;
  vcl_vector<double> Pbcolor,Nbcolor;

  vcl_vector<vsol_point_2d_sptr> pospoints;
  vcl_vector<vsol_point_2d_sptr> negpoints;

  vcl_vector<vil_image_view<float> > tpmask;
  vcl_vector<vil_image_view<float> > tnmask;
  int spacing_on_curve;
  bool isIHS;
  vcl_vector<double> meanp;
  vcl_vector<double> meann;

  vcl_vector<double> stdp;
  vcl_vector<double> stdn;

  //:3d polar or euclidean histogram 
  vbl_array_3d<double> chistp,chistn;
  //:Constructors/Destructor-------------------------
  ~dbctrk_curve_description();



  
  dbctrk_curve_description();
  //:Creates a description from a digital curve (edgel chain)
  dbctrk_curve_description(vdgl_edgel_chain_sptr  ec)
  {  
    curve_=new Curve();
    coarser_curve_=new Curve();

    init(ec);
    compute_bounding_box(ec);
  }
  void compute_bounding_box(vdgl_edgel_chain_sptr const& ec);
  void compute_cms();
  void init(vdgl_edgel_chain_sptr const& ec);
  void compute_mirror_curves(double r,vil_image_view<float> dtimg, vil_image_view<int> dtmap,int id);

  void assign_rgb_values(vil_image_view<float> p0,vil_image_view<float> p1,vil_image_view<float> p2, int maskradius,vil_image_view<float> dtimg, vil_image_view<int> dtmap,int id);
  // display information
  void info();

  //: to create 3d polar histograms based on IHS info 
  void compute_IHS_histograms(int thetabins,double r1, double v1);

  // perform gaussian smoothing over a region
  float maskedsmooth(vil_image_view<float> in,
                     vil_image_view<float> mask ,
                     vbl_array_2d<float> g2d);
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  vcl_string is_a() const;

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(vcl_string const&) const;

};


//: Binary save dbctrk_curve_description
void vsl_b_write(vsl_b_ostream & os, const dbctrk_curve_description * p);
//: Binary load dbctrk_curve_description
void vsl_b_read(vsl_b_istream &is,dbctrk_curve_description  * &p);
#endif
