//---------------------------------------------------------------------
// This is dbdet/examples/contourtracing_command.cxx
//:
// \file
// \brief command line program that takes image name and contour traces the image
//        output is written to the file <image_name>-<contour_number>.con 
//        TODO: also eps file <image_name>.eps is created in the same directory.
//
// \author
//  Ricardo Fabbri - October 2005
//
//  based on old code by
//
//  O.C. Ozcanli - July 28, 2004
//
// \verbatim
//  Modifications
// Totally rewritten to use new class & debugged  - Ricardo Fabbri
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_threshold.h>
#include <vul/vul_psfile.h>
#include <bdgl/bdgl_curve_algs.h>
#include <vcl_cstring.h>

int main( int argc, char* argv[] )
{
  char strtemp1[100];
  char strtemp2[100];

  if (argc != 4) {
    vcl_cout << "USAGE: contourtracing_command.exe <image file name> <smoothing sigma> <number of times to be smoothed>\n";
    return 1;
  }

  vcl_string image_filename(argv[1]);
  float s = atof(argv[2]);
  unsigned cnt = atoi(argv[3]);

  vil_image_view<vxl_byte> loaded_image  = vil_load(image_filename.c_str());

  vil_image_view<bool> binary_img;
  vil_threshold_above<vxl_byte>(loaded_image,binary_img,127);

  dbdet_contour_tracer ctracer;

  ctracer.set_sigma(s);
  ctracer.trace(binary_img);
  ctracer.output_con_file(image_filename.c_str());

  const vcl_vector<vsol_point_2d_sptr>& points = ctracer.largest_contour();

  vcl_cout << "number of points in largest contour: " << points.size() << vcl_endl;
  
  vcl_strcpy(strtemp1, image_filename.c_str());    
  {
  unsigned i;
  for ( i=0; strtemp1[i] != '.'; i++ ){}
  strtemp1[i]= 0;
  }
  sprintf(strtemp2, "%s.eps", strtemp1);

  //1)If file open fails, return.
  vul_psfile psfile1(strtemp2, false);

  if (!psfile1){
    vcl_cout << " Error opening file  " << strtemp2 << vcl_endl;
    return false;
  }

  vcl_cout << " Opened file " << strtemp2 << " for writing!\n";
  vcl_cout << " Image dimensions are, width: " << loaded_image.ni();
  vcl_cout << " height: " << loaded_image.nj() << "\n";

  unsigned char * data= new unsigned char[loaded_image.ni()*loaded_image.nj()];
  for( unsigned i=0;i<loaded_image.ni()*loaded_image.nj();i++)
    data[i]=255;

  psfile1.set_scale_x(50);
  psfile1.set_scale_y(50);
  psfile1.print_greyscale_image(data,loaded_image.ni(),loaded_image.nj());
  psfile1.set_line_width(1.0);
  psfile1.set_fg_color(0,0,1);

  // parse through all the vsol classes and save curve objects only
  for (unsigned i=1; i<points.size();i++)
  {
    vsol_point_2d_sptr p1 = points[i-1];
    vsol_point_2d_sptr p2 = points[i];
    psfile1.line(p1->x(), p1->y(), p2->x(), p2->y());
  }
  
  //close file
  psfile1.close();

  if (cnt > 0) {
    unsigned i;
    strcpy(strtemp1, image_filename.c_str());    
    for ( i=0; strtemp1[i] != '.'; i++ ){}
    strtemp1[i]= 0;
    sprintf(strtemp2, "%s.con", strtemp1);

    vcl_vector<vgl_point_2d<double> > curve;
    curve.clear();

    for (i=0; i<points.size();i++){
      vsol_point_2d_sptr pt = points[i];
      curve.push_back(vgl_point_2d<double>(pt->x(), pt->y()));
    }

    for (i = 0; i<cnt; i++) {
      bdgl_curve_algs::smooth_curve(curve, s);
      curve.erase(curve.begin());
      curve.erase(curve.begin());
      curve.erase(curve.begin());
      curve.erase(curve.end()-1);
      curve.erase(curve.end()-1);
      curve.erase(curve.end()-1);
    }

    FILE *point_file = fopen(strtemp2, "w");
    
    fprintf(point_file, "CONTOUR\nCLOSE\n%d\n", (int)curve.size());
    for (i = 0; i<curve.size(); i++)
      fprintf(point_file, "%f %f ", curve[i].x(), curve[i].y());
    fprintf(point_file, "\n");
    fclose(point_file);

  }

  return 0;


}

