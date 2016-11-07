#include "Lie_spoke_utilities.h"
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vul/vul_file_iterator.h>



  
double spoke_curve_fixAngleMPiPi (double a)
{
  if (a <= -vnl_math::pi) return a+2*vnl_math::pi;
  if (a >   vnl_math::pi) return a-2*vnl_math::pi;
  return a;
}


  vsol_point_2d_sptr compute_centroid(vcl_vector<vsol_point_2d_sptr> contour)
    {
    double cx = 0,cy = 0;
    unsigned int num_points = contour.size();

    for (unsigned int i=0;i<num_points;i++)
         {
         cx += contour[i]->x();
         cy += contour[i]->y();
         }

     cx = cx/num_points;
     cy = cy/num_points;
     vsol_point_2d_sptr cp = new vsol_point_2d(cx,cy);
     return cp;
    }

void compute_spoke_scales_angles(vcl_vector<vsol_point_2d_sptr> contour,vcl_vector<vsol_point_2d_sptr> ref_contour,
                                                       vcl_vector<double> &scales,vcl_vector<double> &angles)
     {
     double cx = 0,cy = 0,ref_cx = 0,ref_cy = 0,dist,ref_dist,angle,ref_angle;
     //contour and ref_contour must have the same number of points
     assert(contour.size() == ref_contour.size());

     unsigned int num_points = contour.size();
     vsol_point_2d_sptr cp = compute_centroid(contour);
     vsol_point_2d_sptr ref_cp = compute_centroid(ref_contour);


     //not handling the pathological cases here
     for (unsigned int i = 0;i<num_points;i++)
         {
         dist = contour[i]->distance(cp);
         ref_dist = ref_contour[i]->distance(ref_cp);

         angle = vcl_atan2(contour[i]->y() - cp->y(),contour[i]->x() - cp->x());
         ref_angle = vcl_atan2(ref_contour[i]->y() - ref_cp->y(),ref_contour[i]->x() - ref_cp->x());

         if (vcl_fabs(ref_dist) > 1e-10)
         scales.push_back(vcl_log(dist/ref_dist));
         else
             scales.push_back(vcl_log(dist));
         
         angles.push_back(spoke_curve_fixAngleMPiPi(angle - ref_angle));
         }

     }

  vcl_vector<vsol_point_2d_sptr> transform_spoke_shape(vcl_vector<vsol_point_2d_sptr> contour,vcl_vector<double> scales,
                                                      vcl_vector<double> angles)
    {
    double x,y,cx=0,cy=0,stretched_x,stretched_y,rotated_x,rotated_y;

    vcl_vector<vsol_point_2d_sptr> mean_shape;
    unsigned int num_points = contour.size();

    for (unsigned int i=0;i<num_points;i++)
        {
        cx += contour[i]->x();
        cy += contour[i]->y();
        }

     cx = cx/num_points;
     cy = cy/num_points;

     for (unsigned int i = 0;i<contour.size();i++)
         {
         x = contour[i]->x() - cx;
         y = contour[i]->y() - cy;

         stretched_x = scales[i]*x;
         stretched_y = scales[i]*y;

         rotated_x = stretched_x*vcl_cos(angles[i]) - stretched_y*vcl_sin(angles[i]);
         rotated_y = stretched_x*vcl_sin(angles[i]) + stretched_y*vcl_cos(angles[i]);

         rotated_x += cx;
         rotated_y += cy;

         vsol_point_2d_sptr new_pt = new vsol_point_2d (rotated_x,rotated_y);
         mean_shape.push_back(new_pt);
         }

    return mean_shape;
    }

double compute_lie_spoke_cost(vcl_vector<vsol_point_2d_sptr> curve1,vcl_vector<vsol_point_2d_sptr> curve2)
    {

    assert(curve1.size() == curve2.size());

    double cost = 0;
     vcl_vector<double> scales,angles;
     compute_spoke_scales_angles(curve1,curve2,scales,angles);

     for (unsigned int i=0;i<scales.size();i++)
     cost = cost +  scales[i]*scales[i] + angles[i]*angles[i];

     cost = vcl_sqrt(cost);
     return cost;
    }

void loadCON(vcl_string fileName, vcl_vector<vsol_point_2d_sptr> &points)
    {
    vcl_ifstream infp(fileName.c_str());
    char magicNum[200];

    infp.getline(magicNum,200);
    if (strncmp(magicNum,"CONTOUR",7))
        {
        vcl_cerr << "Invalid File " << fileName.c_str() << vcl_endl;
        vcl_cerr << "Should be CONTOUR " << magicNum << vcl_endl;
        exit(1);
        }

    char openFlag[200];
    infp.getline(openFlag,200);
    if (!strncmp(openFlag,"OPEN",4))
        vcl_cout << "Open Curve\n" << vcl_endl;
    else if (!strncmp(openFlag,"CLOSE",5))
        vcl_cout << "Closed Curve\n" << vcl_endl;
    else
        {
        vcl_cerr << "Invalid File " << fileName.c_str() << vcl_endl;
        vcl_cerr << "Should be OPEN/CLOSE " << openFlag << vcl_endl;
        exit(1);
        }

    int i,numOfPoints;
    infp >> numOfPoints;

    double x,y;
    for (i=0;i<numOfPoints;i++)
        {
        infp >> x >> y;
        vcl_cout << "x: " << x << "y: " << y << vcl_endl;
        points.push_back(new vsol_point_2d(x, y));
        }
    infp.close();
    }

void writeCON(vcl_string fileName,vcl_vector<vsol_point_2d_sptr> points)
{
unsigned int numpoints = points.size();

  vcl_ofstream outfp(fileName.c_str());
  assert(outfp != NULL);
  outfp << "CONTOUR" << vcl_endl;
  outfp << "OPEN" << vcl_endl;
  outfp << numpoints << vcl_endl;

  for(int i=0; i<numpoints; i++)
  {
    outfp << points[i]->x() << " " << points[i]->y() << " " << vcl_endl;
  }
  outfp.close();
}

vcl_vector<vcl_string> get_all_files(vcl_string file_path)
    {
    vcl_vector<vcl_string> files;

    for (vul_file_iterator fn=file_path+"\\*.con"; fn; ++fn)
        {
        files.push_back(fn());
        }
    return files;
    }

