//---------------------------------------------------------------------
// This is algo/contourtracing/contourtracing_command.cxx
//:
// \file
// \brief command line program that takes image name and contour traces the image
//        output is written to the file <image_name>.con 
//        also eps file <image_name>.eps is created in the same directory.
//
// \author
//  O.C. Ozcanli - July 28, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <contourtracing/ContourTracing.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vul/vul_psfile.h>
#include <bdgl/bdgl_curve_algs.h>


int main( int argc, char* argv[] )
{
  int i;
  char strtemp1[100];
  char strtemp2[100];

  if (argc != 4) {
    vcl_cout << "USAGE: contourtracing_command.exe <image file name> <smoothing sigma> <number of times to be smoothed>\n";
    exit(-1);
  }

  vcl_string image_filename(argv[1]);
  float s = atof(argv[2]);
  int cnt = atoi(argv[3]);

  vil_image_resource_sptr loaded_image = vil_load_image_resource( image_filename.c_str() );
  if( !loaded_image ) {
    vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
    return false;
  } else 
    vcl_cout << "loaded image "<<image_filename.c_str()<<vcl_endl;

  ContourTracing* contourTracing = new ContourTracing;
  contourTracing->setSigma (s);
  contourTracing->setImage (loaded_image);
  contourTracing->setImageFileName (image_filename.c_str());

  contourTracing->detectContour();

  vcl_vector<vsol_point_2d_sptr>& points = contourTracing->getResult ();

  vcl_cout << "number of points: " << points.size() << vcl_endl;
  
  strcpy(strtemp1, image_filename.c_str());    
  for ( i=0; strtemp1[i] != '.'; i++ ){}
  strtemp1[i]= 0;
  sprintf(strtemp2, "%s.eps", strtemp1);

  //1)If file open fails, return.
  vul_psfile psfile1(strtemp2, false);

  if (!psfile1){
    vcl_cout << " Error opening file  " << strtemp2 << vcl_endl;
    return false;
  }

  vcl_cout << " Opened file " << strtemp2 << " for writing!\n";
  vcl_cout << " Image dimensions are, width: " << loaded_image->ni();
  vcl_cout << " height: " << loaded_image->nj() << "\n";

  unsigned char * data= new unsigned char[loaded_image->ni()*loaded_image->nj()];
  for( i=0;i<loaded_image->ni()*loaded_image->nj();i++)
    data[i]=255;

  psfile1.set_scale_x(50);
  psfile1.set_scale_y(50);
  psfile1.print_greyscale_image(data,loaded_image->ni(),loaded_image->nj());
  psfile1.set_line_width(1.0);
  psfile1.set_fg_color(0,0,1);

  // parse through all the vsol classes and save curve objects only
  for (i=1; i<points.size();i++)
  {
    vsol_point_2d_sptr p1 = points[i-1];
    vsol_point_2d_sptr p2 = points[i];
    psfile1.line(p1->x(), p1->y(), p2->x(), p2->y());
  }
  
  //close file
  psfile1.close();

  if (cnt > 0) {
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
    
    fprintf(point_file, "CONTOUR\nCLOSE\n%d\n", curve.size());
    for (i = 0; i<curve.size(); i++)
      fprintf(point_file, "%f %f ", curve[i].x(), curve[i].y());
    fprintf(point_file, "\n");
    fclose(point_file);

  }

  return 0;


}

