//: dbmsh3d/algo/dbmsh3d_fileio_nuages.cxx
//  Ming-Ching Chang Feb 02, 2007.

#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>

#define MAX_LEN 1024

void sli_cons_to_nuages_cnt (vcl_string sli_file, vcl_string cnt_file)
{

  //:1) open the sli file
  FILE*    fp = vcl_fopen (sli_file.c_str(), "r");
  if (fp == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open file %s.\n", sli_file.c_str());
    return; 
  }
  vcl_cout<< "\nsli_cons_to_nuages_cnt() Loading data from "<< sli_file.c_str();
  
  //:2) open the cnt_file to write
  FILE*    fp2 = vcl_fopen (cnt_file.c_str(), "w");
  if (fp2 == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open file %s.\n", cnt_file.c_str());
    return; 
  }
  vcl_cout<< "\nsli_cons_to_nuages_cnt() Writing data to "<< cnt_file.c_str();


  //: file header
  int num_slices;
  vcl_fscanf (fp, "S %d\n", &num_slices);
  vcl_fprintf (fp2, "S %d\n", num_slices);

  //: each slice
  for (int i=0; i<num_slices; i++) {
    int num_pts;
    double slice_z;
    vcl_fscanf (fp, "v %d z %lf\n", &num_pts, &slice_z);
    vcl_fprintf (fp2, "v %d z %.16f\n", num_pts, slice_z);

    vcl_fscanf (fp, "{\n");
    vcl_fprintf (fp2, "{\n");

    //:3) read in each *.con file and write into the cnt_file
    char con_file[MAX_LEN];
    vcl_fscanf (fp, "%s\n", con_file);
    vcl_vector<double> pt_x, pt_y;
    pt_x.clear();
    pt_y.clear();

    //:3-1)the i-th con file
    FILE* fp3 = vcl_fopen (con_file, "r");
    if (fp3 == NULL) {
      vul_printf (vcl_cout, "ERROR: Can't open file %s.\n", con_file);
      return; 
    }

    char buffer[MAX_LEN];
    vcl_fscanf (fp3, "%s\n", buffer); //CONTOUR
    vcl_fscanf (fp3, "%s\n", buffer); //OPEN/CLOSE
    for (int i=0; i<num_pts; i++) {
      double x, y;
      vcl_fscanf (fp3, "%lf %lf\n", &x, &y); 
      pt_x.push_back (x);
      pt_y.push_back (y);
    }
    vcl_fclose (fp3);
    assert (pt_x.size() ==num_pts);
    assert (pt_x.size() == pt_y.size());

    //:3-2)write to the cnt file
    for (int i=0; i<num_pts; i++) {
      vcl_fprintf (fp2, "%.16f %.16f\n", pt_x[i], pt_y[i]); 
    }

    vcl_fscanf (fp, "}\n");
    vcl_fprintf (fp2, "}\n");

  }

  //:4) close the cnt_file and finish
  vcl_cout<< "\nJob done.\n";

  vcl_fclose (fp);
  vcl_fclose (fp2);
}


#if 0

void LoadCON_Point (vcl_vector<double>& x, vcl_vector<double>& y, const char* filename);

#define MAX_LEN 1024

//Read .CON contour file as a point set.
//This function can read both open and close .CON file.
void LoadCON_Point (vcl_vector<double>& xVector, vcl_vector<double>& yVector, const char* filename)
{  
   double x, y;
  char buffer[MAX_LEN];
   int nPoints;

  //1)If file open fails, return.
  vcl_ifstream fp (filename, vcl_ios::in);
  if (!fp){
    vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
    return;
  }

   xVector.clear();
   yVector.clear();

  //2)Read in file header.
   fp.getline (buffer,MAX_LEN); //CONTOUR
   fp.getline (buffer,MAX_LEN); //OPEN/CLOSE
   fp >> nPoints;
   vcl_cout << "Number of Points from Contour:" << nPoints << vcl_endl;
  
  //3)Read in 'nPoints' of lines of data.
   for (int i=0; i<nPoints; i++) {
    fp >> x >> y;
      xVector.push_back (x);
      yVector.push_back (y);
   }

  fp.close();
}



//: Ming temp CON file I/O.
void draw_CON (SoSeparator* root, vcl_vector<double> xVector, vcl_vector<double> yVector, double z)
{
   SoCoordinate3 * coord3 = new SoCoordinate3;

   for (unsigned int i=0; i<xVector.size(); i++) {
      coord3->point.set1Value (i, (float) xVector[i], (float) yVector[i], (float) z);
   }

  SoBaseColor* col = new SoBaseColor;
  col->rgb = SbColor (1, 1, 1);
  root->addChild (col);
  
  root->addChild(coord3);
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = 1;
  root->addChild(drawstyle);
  SoPointSet * pointset = new SoPointSet;
  root->addChild (pointset);
}

#define N_FILE 24//18
#define Z_RATIO 4.38857

vcl_string file[N_FILE] = {
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670282.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670297.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670312.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670327.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670342.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670357.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670372.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670387.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670402.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670417.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670432.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670447.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670492.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670507.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670522.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670537.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670552.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670567.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670582.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670597.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670612.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670627.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670642.con",
   "T:/images/medical/knee-images/07180653-Sept-2003/set_1_con/23670657.con",
};

//  data from T:\images\medical\knee-images\07180653-Sept-2003\set_1
void KneeData_Visualization_Point (SoSeparator* root)
{
   vcl_vector<double> xVector;
   vcl_vector<double> yVector;

   for (int i=0; i<N_FILE; i++) {
      xVector.clear();
      yVector.clear();
      double z = i*Z_RATIO;
      LoadCON_Point (xVector, yVector, file[i].c_str());
      draw_CON (root, xVector, yVector, z);
   }

}

#include "vcl_cstdio.h"

//  data from T:\images\medical\knee-images\07180653-Sept-2003\set_1
bool KneeData_Output_CNT_For_Nuages (const char* CNT_file)
{
   vcl_vector<double> xVector;
   vcl_vector<double> yVector;

   //: Output the slices into the Nuages .CNT file.
  FILE* fp1;
  if ((fp1 = fopen (CNT_file, "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't write .CNT file %s\n", CNT_file);
    return false; 
  }
   //: CNT: Number of slices
  vcl_fprintf (fp1, "S %d\n", N_FILE);

   for (int i=0; i<N_FILE; i++) {
      xVector.clear();
      yVector.clear();
      double z = i*Z_RATIO;
      LoadCON_Point (xVector, yVector, file[i].c_str());

      //: Output each slice.
     vcl_fprintf (fp1, "v %d z %.16f\n", xVector.size(), z);
      vcl_fprintf (fp1, "{\n");

      //: Output each sample in this slice.
      for (unsigned int j=0; j<xVector.size(); j++) {
         vcl_fprintf (fp1, "%.16f %.16f\n", xVector[j], yVector[j]);
      }

      vcl_fprintf (fp1, "}\n");
   }

  fclose (fp1);
  return true;
}
#endif

