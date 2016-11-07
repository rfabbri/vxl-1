/******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <fstream>

using namespace std;

/******************************************************************************/

void WriteVTKPolyFile(const char * file_path,
                      const double * coords,
                      const unsigned int num_coords,
                      const unsigned int * triangles,
                      const unsigned int num_triangles,
                      const char * header = "")
{
  ofstream out_file(file_path);
  
  if(!out_file.is_open())
  {
    cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << "Failed to open distance transform file " << file_path << endl;
    
    exit(1);
  }
  
  out_file << "# vtk DataFile Version 2.0" << endl;
  out_file << header << endl;
  out_file << "ASCII" << endl;
  out_file << "DATASET POLYDATA" << endl;

  out_file << "POINTS " << num_coords << " float" << endl;
  out_file.precision(6);
  
  for(int i = 0; i < num_coords; i++)
    out_file << coords[3*i+0] << " " << coords[3*i+1] << " " << coords[3*i+2]
             << endl;

  out_file << endl;
  out_file << "POLYGONS " << num_triangles << " " << num_triangles*4 << endl;
  
  for(int i = 0; i < num_triangles; i++)
    out_file << "3 " << triangles[3*i+0] << " " << triangles[3*i+1] << " "
             << triangles[3*i+2] << endl;
  
  out_file.close();
}




/******************************************************************************/
