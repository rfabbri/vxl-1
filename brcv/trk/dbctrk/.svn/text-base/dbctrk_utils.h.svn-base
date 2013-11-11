#ifndef __TBS_UTILS_DP_DEF__
#define __TBS_UTILS_DP_DEF__
//:
// \file
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>

class utils
{

//: return true if the difference between a and b is smaller than 1e-4
public:
//: return the maximum of three numbers
static bool  almostEqual(double a, double b);
//: return true if the difference between a and b is smaller than eps
static bool almostEqual(double a, double b, double eps);
//: return a modulo 2*pi, in the interval -pi to pi
static double fixAngleMPiPi(double a);
//: return a modulo 2*pi, in the interval 0 to 2*pi
static double fixAngleZTPi(double a);
//: return a1-a2 modulo 2*pi, in the interval -pi to pi
static inline double angleDiff(double a1, double a2) { return fixAngleMPiPi(a1-a2); }
//: return a1+a2 modulo 2*pi, in the interval -pi to pi
static inline double angleAdd(double a1, double  a2) { return fixAngleMPiPi(a1+a2); }
//: ??
static double angleFixForAdd(double ref, double  a);
//: return a modulo 2*pi, in the interval 0 to pi
static double fixAngleMPiPibytwo(double a);
//: function to generate colors
static void set_changing_colors(int id,float &r,float &g, float &b);

//: function to generate colors
static void set_rank_colors(int num,float &r,float &g, float &b);

static vcl_vector<vcl_string > parseLineForNumbers(vcl_string s);

static int rgb2lab(double r,double g,double b, double &L , double &a, double &lb);
static int IHS2lab(double I,double H,double S, double &L , double &a, double &lb);
static double mean(vcl_vector<double> numbers);
static double std(vcl_vector<double> numbers);
static void rgb_to_ihs(double r,double g, double b,double& i, double& h, double& s);
//: to create a 3d polar histogram for IHS cone
static bool norm3dpolar(vcl_vector<double> & dim1,vcl_vector<double> & dim2,vcl_vector<double> & dim3, vbl_array_3d<double> &polarhist,int thetabins,int rbins,int valuebins,double r1,double v1);    
//: bhattacharrya distance between two 3d histograms
static double dist3pdf_bhat(vbl_array_3d<double> hist1,vbl_array_3d<double> hist2, int xbins,int ybins,int zbins);
//: create a 2d symmetric gaussian kernel
  static void gauss2d(float sigma,vbl_array_2d<float>  & kernel);
 private:
  utils();
};
#endif
