#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>

using namespace std;

#include "surface_tracer.h"

/******************************************************************************/

float *** ReadDTFile(const char file_dir[],
                     const char file_name[],
                     const bool file_has_header,
                     const bool file_has_shocks,
                     unsigned int file_dimensions[NUM_DIMENSIONS])
{
  cout << "Reading Distance Transform File..." << endl;
  
  // Loop variables
  unsigned int i, j, k;
  
  char file_path[MAX_STRING_LENGTH];
  sprintf(file_path, "%s%s", file_dir, file_name);
  
  ifstream in_file(file_path, ios::binary | ios::in);
  
  if(!in_file.is_open())
  {
    cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << "Failed to open distance transform file " << file_path << endl;
    
    exit(1);
  }
  
  if(file_has_header)
  {
    char header_lines[DT_FILE_HEADER_NUM_LINES][MAX_STRING_LENGTH];
    
    for(i = 0; i < DT_FILE_HEADER_NUM_LINES; i++)
      in_file.getline(header_lines[i], MAX_STRING_LENGTH);
    
    if(strncmp(header_lines[0], "float", 5) != 0)
    {
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "Distance transform file does not contain float data." << endl;
      
      exit(1);
    }
    
    if(sscanf(header_lines[1], " %u %u %u",
              &file_dimensions[DIM_Z],
              &file_dimensions[DIM_Y],
              &file_dimensions[DIM_X]) != signed(NUM_DIMENSIONS))
    {
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "Dimensions not found in Distance Transform file header." << endl;
      
      exit(1);
    }
    
    // Ignore remaining two header lines
  }
  
  /*
   * Allocate DT Array
   * single block of data with pointer blocks to allow [x][y][z] 3D referencing.
   */
  
  float *** dt_array = new float**[file_dimensions[DIM_X]];
  dt_array[0] = new float*[file_dimensions[DIM_X]*file_dimensions[DIM_Y]];
  dt_array[0][0] = new float[file_dimensions[DIM_X]*
                             file_dimensions[DIM_Y]*
                             file_dimensions[DIM_Z]];
  
  for(i = 0; i < file_dimensions[DIM_X]; i++)
  {
    dt_array[i] = dt_array[0] + i*file_dimensions[DIM_Y];
    
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
    {
      dt_array[i][j] = dt_array[0][0]
                     + (i*file_dimensions[DIM_Y]+j)*file_dimensions[DIM_Z];
    }
  }
  
  // Use intermediary void pointer to copy the binary input to the float array.
  // This gets around the type limitations of read().
  const unsigned int total_dim = file_dimensions[DIM_X] *
                                 file_dimensions[DIM_Y] *
                                 file_dimensions[DIM_Z];
  
  float * in_ptr = new float[total_dim];
  in_file.read((char *)(void *) in_ptr, total_dim*sizeof(float));
  
  if(!in_file)
  {
    cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << "Failed to read Distance Transform file data buffer" << endl;
    
    exit(1);
  }
  
  /*
   * RAW file dimensions are ordered ZYX,
   * so we're going to transpose the array: ZYX -> XYZ
   */
  for(i = 0; i < file_dimensions[DIM_X]; i++)
  {
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
    {
      for(k = 0; k < file_dimensions[DIM_Z]; k++)
      {
        dt_array[i][j][k] =
          in_ptr[(k*file_dimensions[DIM_Y]+j)*file_dimensions[DIM_X] + i];
      }
    }
  }
  
  delete[](in_ptr);
  in_ptr = 0;
  in_file.close();
  
/********************
  cout << "File Dimensions:"
       << file_dimensions[DIM_X] << "\t"
       << file_dimensions[DIM_Y] << "\t"
       << file_dimensions[DIM_Z] << endl;
  cout.precision(2);
  cin >> k;//k = file_dimensions[DIM_Z]/2;
  cout << "X-Slice " << k << endl;
  for(j = 0; j < file_dimensions[DIM_Y]; j++)
  {
    for(i = 0; i < file_dimensions[DIM_X]; i++)
    {
      cout << dt_array[i][j][k] << "\t";
    }
    cout << endl;
  }
  exit(0);
********************/

/********************
  
  //
  // Text Slice Viewer for Debug
  //

  cout << "File Dimensions:"
       << file_dimensions[DIM_X] << "\t"
       << file_dimensions[DIM_Y] << "\t"
       << file_dimensions[DIM_Z] << endl;
  cin >> i;//i = file_dimensions[DIM_X]/2;
  cout << "X-Slice " << i << endl;
  for(j = 0; j < file_dimensions[DIM_Y]; j++)
  {
    for(k = 0; k < file_dimensions[DIM_Z]; k++)
    {
      if(dt_array[i][j][k] >= 10)
        cout << '+';
      else if(dt_array[i][j][k] < 0)
        cout << '-';
      else
        cout << int(dt_array[i][j][k]);
    }
    cout << endl;
  }
  exit(0);
********************/
  
/********************
  cout.precision(4);
  for(int blah = 0; blah < 27; blah++)
    cout << dt_array[0][0][blah] << " ";
  cout << endl;
  //exit(0);
********************/  

/********************
    cout << "-- Tweaking DT for Exponential format" << endl;
    for(i = 0; i < file_dimensions[DIM_X]; i++)
    {
      for(j = 0; j < file_dimensions[DIM_Y]; j++)
      {
        for(k = 0; k < file_dimensions[DIM_Z]; k++)
        {
          if(dt_array[i][j][k] == 0.0)
          {
            dt_array[i][j][k] = 20;
          }
          else
          {
            dt_array[i][j][k] = log(dt_array[i][j][k]);
          }
        }
      }
    }
********************/  
  
/********************
  cout.precision(2);
  k = file_dimensions[DIM_Z]/2;
  for(j = 0; j < file_dimensions[DIM_Y]; j++)
  {
    for(i = 0; i < file_dimensions[DIM_X]; i++)
    {
      cout << dt_array[i][j][k] << "\t";
    }
    cout << endl;
  }
  cout << endl;
  exit(0);
********************/
  
  if(file_has_shocks)
  {
    cout << "-- Tweaking DT for special format including Shocks --" << endl;
    for(i = 0; i < file_dimensions[DIM_X]; i++)
    {
      for(j = 0; j < file_dimensions[DIM_Y]; j++)
      {
        for(k = 0; k < file_dimensions[DIM_Z]; k++)
        {
          if(dt_array[i][j][k] > 0.0)
          {
            dt_array[i][j][k] = -dt_array[i][j][k];
          }
          if(dt_array[i][j][k] == 0.0)
          {
            dt_array[i][j][k] = 1;
          }
        }
      }
    }
  }
  else
  {
    cout << "Setting zeros of DT file to " << -FLOAT_ERROR_MARGIN << endl;
    //cout << "ISOSURFACE WILL BE AT 1.0" << endl;
    int num_zeros = 0;
    for(i = 0; i < file_dimensions[DIM_X]; i++)
    {
      for(j = 0; j < file_dimensions[DIM_Y]; j++)
      {
        for(k = 0; k < file_dimensions[DIM_Z]; k++)
        {
          //dt_array[i][j][k] -= 1.0;
        
          if(dt_array[i][j][k] == 0.0)
          {
            dt_array[i][j][k] = REPLACE_ZERO_VALUE;
            num_zeros++;
          }
        }
      }
    }
    cout << num_zeros << " zeros found in Distance Transform out of a total "
         << file_dimensions[DIM_X]*file_dimensions[DIM_Y]*file_dimensions[DIM_Z]
         << " data points." << endl;
  }
  
/*******************
  int x = file_dimensions[DIM_X]-1, y = 1, z = 61;
  //x = (x-5 < 0) ? 0 : x-5;
  y = (y-5 < 0) ? 0 : y-5;
  z = (z-5 < 0) ? 0 : z-5;
  
  cout << "\t";
  for(k = 0; k < 11; k++)
  {
    cout << z+k << "\t";
  }
  cout << endl;
  
  for(i = 0; i < 1; i++)
  {
    for(j = 0; j < 11; j++)
    {
      cout << y+j << "\t";
      for(k = 0; k < 11; k++)
      {
        cout << dt_array[x+i][y+j][z+k] << "\t";
      }
      cout << endl;
    }
  }
*******************/
  
/******************
  cout.precision(4);
  for(i = 0; i < file_dimensions[DIM_X]; i++)
  {
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
    {
      //if(i == 26)
      //  cout << endl;
      for(k = 0; k < file_dimensions[DIM_Z]; k++)
      {
        //if(i == 26 && (k == 97 || k == 99))
        //  cout << "*";
        cout << dt_array[i][j][k] << "\t";
      }
      cout << endl;
    }
    cout << endl;
  }
  exit(0);
******************/

  return dt_array;
}




