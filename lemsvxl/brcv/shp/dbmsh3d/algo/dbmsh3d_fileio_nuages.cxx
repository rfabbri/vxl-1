//: dbmsh3d/algo/dbmsh3d_fileio_nuages.cxx
//  Ming-Ching Chang Feb 02, 2007.

#include <cstdio>
#include <cassert>
#include <iostream>
#include <vector>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>

#define MAX_LEN 1024

void sli_cons_to_nuages_cnt (std::string sli_file, std::string cnt_file)
{

  //:1) open the sli file
  FILE*    fp = std::fopen (sli_file.c_str(), "r");
  if (fp == NULL) {
    vul_printf (std::cout, "ERROR: Can't open file %s.\n", sli_file.c_str());
    return; 
  }
  std::cout<< "\nsli_cons_to_nuages_cnt() Loading data from "<< sli_file.c_str();
  
  //:2) open the cnt_file to write
  FILE*    fp2 = std::fopen (cnt_file.c_str(), "w");
  if (fp2 == NULL) {
    vul_printf (std::cout, "ERROR: Can't open file %s.\n", cnt_file.c_str());
    return; 
  }
  std::cout<< "\nsli_cons_to_nuages_cnt() Writing data to "<< cnt_file.c_str();


  //: file header
  int num_slices;
  std::fscanf (fp, "S %d\n", &num_slices);
  std::fprintf (fp2, "S %d\n", num_slices);

  //: each slice
  for (int i=0; i<num_slices; i++) {
    int num_pts;
    double slice_z;
    std::fscanf (fp, "v %d z %lf\n", &num_pts, &slice_z);
    std::fprintf (fp2, "v %d z %.16f\n", num_pts, slice_z);

    std::fscanf (fp, "{\n");
    std::fprintf (fp2, "{\n");

    //:3) read in each *.con file and write into the cnt_file
    char con_file[MAX_LEN];
    std::fscanf (fp, "%s\n", con_file);
    std::vector<double> pt_x, pt_y;
    pt_x.clear();
    pt_y.clear();

    //:3-1)the i-th con file
    FILE* fp3 = std::fopen (con_file, "r");
    if (fp3 == NULL) {
      vul_printf (std::cout, "ERROR: Can't open file %s.\n", con_file);
      return; 
    }

    char buffer[MAX_LEN];
    std::fscanf (fp3, "%s\n", buffer); //CONTOUR
    std::fscanf (fp3, "%s\n", buffer); //OPEN/CLOSE
    for (int i=0; i<num_pts; i++) {
      double x, y;
      std::fscanf (fp3, "%lf %lf\n", &x, &y); 
      pt_x.push_back (x);
      pt_y.push_back (y);
    }
    std::fclose (fp3);
    assert (pt_x.size() ==num_pts);
    assert (pt_x.size() == pt_y.size());

    //:3-2)write to the cnt file
    for (int i=0; i<num_pts; i++) {
      std::fprintf (fp2, "%.16f %.16f\n", pt_x[i], pt_y[i]); 
    }

    std::fscanf (fp, "}\n");
    std::fprintf (fp2, "}\n");

  }

  //:4) close the cnt_file and finish
  std::cout<< "\nJob done.\n";

  std::fclose (fp);
  std::fclose (fp2);
}


#if 0

void LoadCON_Point (std::vector<double>& x, std::vector<double>& y, const char* filename);

#define MAX_LEN 1024

//Read .CON contour file as a point set.
//This function can read both open and close .CON file.
void LoadCON_Point (std::vector<double>& xVector, std::vector<double>& yVector, const char* filename)
{  
   double x, y;
  char buffer[MAX_LEN];
   int nPoints;

  //1)If file open fails, return.
  std::ifstream fp (filename, std::ios::in);
  if (!fp){
    std::cout<<" : Unable to Open "<<filename<<std::endl;
    return;
  }

   xVector.clear();
   yVector.clear();

  //2)Read in file header.
   fp.getline (buffer,MAX_LEN); //CONTOUR
   fp.getline (buffer,MAX_LEN); //OPEN/CLOSE
   fp >> nPoints;
   std::cout << "Number of Points from Contour:" << nPoints << std::endl;
  
  //3)Read in 'nPoints' of lines of data.
   for (int i=0; i<nPoints; i++) {
    fp >> x >> y;
      xVector.push_back (x);
      yVector.push_back (y);
   }

  fp.close();
}



//: Ming temp CON file I/O.
void draw_CON (SoSeparator* root, std::vector<double> xVector, std::vector<double> yVector, double z)
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

std::string file[N_FILE] = {
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
   std::vector<double> xVector;
   std::vector<double> yVector;

   for (int i=0; i<N_FILE; i++) {
      xVector.clear();
      yVector.clear();
      double z = i*Z_RATIO;
      LoadCON_Point (xVector, yVector, file[i].c_str());
      draw_CON (root, xVector, yVector, z);
   }

}

#include "cstdio"

//  data from T:\images\medical\knee-images\07180653-Sept-2003\set_1
bool KneeData_Output_CNT_For_Nuages (const char* CNT_file)
{
   std::vector<double> xVector;
   std::vector<double> yVector;

   //: Output the slices into the Nuages .CNT file.
  FILE* fp1;
  if ((fp1 = fopen (CNT_file, "w")) == NULL) {
    vul_printf (std::cout, "ERROR: Can't write .CNT file %s\n", CNT_file);
    return false; 
  }
   //: CNT: Number of slices
  std::fprintf (fp1, "S %d\n", N_FILE);

   for (int i=0; i<N_FILE; i++) {
      xVector.clear();
      yVector.clear();
      double z = i*Z_RATIO;
      LoadCON_Point (xVector, yVector, file[i].c_str());

      //: Output each slice.
     std::fprintf (fp1, "v %d z %.16f\n", xVector.size(), z);
      std::fprintf (fp1, "{\n");

      //: Output each sample in this slice.
      for (unsigned int j=0; j<xVector.size(); j++) {
         std::fprintf (fp1, "%.16f %.16f\n", xVector[j], yVector[j]);
      }

      std::fprintf (fp1, "}\n");
   }

  fclose (fp1);
  return true;
}
#endif

