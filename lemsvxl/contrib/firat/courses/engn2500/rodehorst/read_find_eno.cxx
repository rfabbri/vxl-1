#include <stdlib.h>
#include <limits.h>

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#include "surface_tracer.h"

#include "eno_tracer/1d_eno_types.h" // <-- TEMPORARY SET UP.

/******************************************************************************/
/* Function Prototypes for just this file */

SpaceUnit *** NewSpaceArray(const unsigned int file_dimensions[NUM_DIMENSIONS]);

void GetENODataFromZerox(ENO_ZeroCrossing_t * zerox,
                         const int i,
                         const int j,
                         const int k,
                         const int dim_index,
                         SpaceUnit * this_unit,
                         double coords[NUM_DIMENSIONS],
                         vector<double> & eno_coords,
                         vector<EnoPoint> & eno_array);

void FindENOPointsInGridline(const double data[],
                             const unsigned int length,
                             ENO_Interval_t interval_array[]);

/******************************************************************************/

SpaceUnit *** ReadENOFiles(const char file_dir[],
                           const char file_prefix[],
                           const unsigned int file_dimensions[NUM_DIMENSIONS],
                           vector<EnoPoint> & eno_array,
                           vector<double> & eno_coords)
{
  // Loop variables
  unsigned int i, dim, slice;
  
  char file_path[MAX_STRING_LENGTH];
  char header_line[MAX_STRING_LENGTH];
  char direction_char;
  unsigned int num_contours;
  //int * contour_lengths = 0;
  unsigned int num_points;
  double in_point[NUM_DIMENSIONS];
  void * void_ptr = (void *) in_point;
  unsigned int eno_indexes[NUM_DIMENSIONS];
  bool x_int, y_int, z_int; // true IFF the corresponding coordinate is an int
  unsigned int num_non_ints;
  unsigned int dim_index;
  SpaceUnit * this_unit;
  EnoPoint new_eno;
  
  /*
   * Try openning the first ENO file; if it's not there, assume we have to make
   * our own ENO data.
   */
  
  sprintf(file_path, "%s%s.x.0.eno", file_dir, file_prefix);
  ifstream in_file(file_path, ios::binary | ios::in);
  if(!in_file.is_open())
  {
    cout << "ENO files not found -- Calculating ENO points now." << endl;
    return 0;
  }
  
  SpaceUnit *** space_array = NewSpaceArray(file_dimensions);
  
  /*
   * Read each ENO Slice File.
   * For x files, insert points on grid lines || to y axis.
   * For y files, insert points on grid lines || to z axis.
   * For z files, insert points on grid lines || to x axis.
   */
   
  for(dim = 0; dim < NUM_DIMENSIONS; dim++)
  {
    for(slice = 0; slice < file_dimensions[dim]; slice++)
    {
      sprintf(file_path, "%s%s.%c.%d.eno", file_dir, file_prefix,
              DIMENSION_CHARS[dim], slice);
      
      ifstream in_file(file_path, ios::binary | ios::in);
      
      if(!in_file.is_open())
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Failed to Open ENO file " << file_path << endl;
        exit(1);
      }
      
      in_file.getline(header_line, MAX_STRING_LENGTH);
      
#ifdef DEBUG1
      cout << endl << file_path << endl << header_line << endl << endl;
#endif
      
      // Note: subsequent calls to strtok with a NULL pointer will read where
      // the function left off in the previous call; Returns 0 when it runs out.
      direction_char = *strtok(header_line, " \t\n\r");
      
      if(direction_char != DIMENSION_CHARS[dim])
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Incorrect direction " << direction_char << "in header of "
             << file_path << endl;
        exit(1);
      }
      
      if(slice != unsigned(atoi(strtok(0, " \t\n\r"))))
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Incorrect slice number in header of " << file_path << endl;
        exit(1);
      }
      
      num_contours = atoi(strtok(0, " \t\n\r"));
      //contour_lengths = new int[num_contours];
      
      num_points = 0;
      for(i = 0; i < num_contours; i++)
      {
        //contour_lengths[i] = atoi(strtok(0, " \t\n\r"));
        num_points += atoi(strtok(0, " \t\n\r"));
      }
      
      //delete[](contour_lengths);
      //contour_lengths = 0;
      
      for(i = 0; i < num_points; i++)
      {
        // NOTE: void_ptr points to same buffer as in_point
        in_file.read((char *) void_ptr, NUM_DIMENSIONS*sizeof(double));
        
        if((in_point[DIM_X] < 0)||(in_point[DIM_X] > file_dimensions[DIM_X]) ||
           (in_point[DIM_Y] < 0)||(in_point[DIM_Y] > file_dimensions[DIM_Y]) ||
           (in_point[DIM_Z] < 0)||(in_point[DIM_Z] > file_dimensions[DIM_Z]))
        {
          cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
               << "ENO point #" << i << " in " << file_path
               << " is out of range: (" << in_point[DIM_X] << ","
               << in_point[DIM_Y] << "," << in_point[DIM_Z] << ")" << endl;
          exit(1);
        }

        eno_indexes[DIM_X] = int(in_point[DIM_X]);
        eno_indexes[DIM_Y] = int(in_point[DIM_Y]);
        eno_indexes[DIM_Z] = int(in_point[DIM_Z]);
        
        x_int = (in_point[DIM_X] - eno_indexes[DIM_X]) < FLOAT_ERROR_MARGIN;
        y_int = (in_point[DIM_Y] - eno_indexes[DIM_Y]) < FLOAT_ERROR_MARGIN;
        z_int = (in_point[DIM_Z] - eno_indexes[DIM_Z]) < FLOAT_ERROR_MARGIN;
        
        num_non_ints = NUM_DIMENSIONS - int(x_int) - int(y_int) - int(z_int);
        
        if(num_non_ints == 0)
        {
          cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
               << "ENO point #" << i << " in " << file_path
               << " is exactly on a grid point." << endl;
          exit(1);
        }
        else if(num_non_ints != 1)
        {
          cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
               << "ENO point #" << i << " in " << file_path
               << " is not on a grid line." << endl;
          exit(1);
        }
        
        if(!x_int)      dim_index = DIM_X;
        else if(!y_int) dim_index = DIM_Y;
        else            dim_index = DIM_Z;
        
        /*
         * Note: to avoid redundancy of ENO points when reading intersecting
         * slices (i.e. and xy and yz plane), only the ENO points along one type
         * of grid line in each slice type are added.  If the data is
         * complete, this ought to catch all the points exactly once.
         */
         
#ifdef DEBUG2
        cout << in_point[DIM_X] << "\t" << in_point[DIM_Y] << "\t"
             << in_point[DIM_Z] << endl;
#endif
        
        if((dim + 1) % NUM_DIMENSIONS == dim_index)
        {
#ifdef DEBUG3
          cout << in_point[DIM_X] << "\t" << in_point[DIM_Y] << "\t"
               << in_point[DIM_Z] << endl;
#endif    
          // Add ENO point into 3D SpaceUnit space array - index to ENO array stored
          this_unit = &space_array[eno_indexes[DIM_X]]
                                  [eno_indexes[DIM_Y]]
                                  [eno_indexes[DIM_Z]];
          if(this_unit->num_enos[dim_index] >= MAX_ENOS_PER_EDGE)
          {
            cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
                 << "More than " << MAX_ENOS_PER_EDGE
                 << " ENO points are on an edge in " << file_path << endl
                 << DIMENSION_CHARS[dim_index] << " values: "
                 << eno_coords[NUM_DIMENSIONS*this_unit->eno_indexes[dim_index][0]+dim_index] << ", "
                 << eno_coords[NUM_DIMENSIONS*this_unit->eno_indexes[dim_index][1]+dim_index] << ", "
                 << in_point[dim_index] << endl;
            exit(1);
          }
          
          // Ensure that ENO points are stored in order; lesser coords first
          if((this_unit->num_enos[dim_index] > 0) &&
             (in_point[dim_index] < eno_coords
             [NUM_DIMENSIONS*this_unit->eno_indexes[dim_index][0] + dim_index]))
          {
            this_unit->eno_indexes[dim_index][1] = 
              this_unit->eno_indexes[dim_index][0];
            this_unit->eno_indexes[dim_index][0] = eno_array.size();
          }
          else
          {
            this_unit->eno_indexes[dim_index][this_unit->num_enos[dim_index]]
              = eno_array.size();
          }
          this_unit->num_enos[dim_index]++;

          // Insert new ENO point into ENO point array
          memcpy(new_eno.indexes, eno_indexes, NUM_DIMENSIONS*sizeof(int));
          new_eno.dim_index = dim_index;
          new_eno.used = FALSE;
          eno_array.push_back(new_eno);

          // Insert new ENO point into ENO coordinate array
          eno_coords.push_back(in_point[DIM_X]);
          eno_coords.push_back(in_point[DIM_Y]);
          eno_coords.push_back(in_point[DIM_Z]);
          
          //memcpy(&eno_coords[NUM_DIMENSIONS*total_num_enos],
          //       in_point, NUM_DIMENSIONS*sizeof(double));
          //total_num_enos++;
        }
      }
    }
  }
  
  return space_array;
}




/******************************************************************************/

SpaceUnit *** FindENOPoints_old(float *** distance_transform,
                            const unsigned int file_dimensions[NUM_DIMENSIONS],
                            vector<EnoPoint> & eno_array,
                            vector<double> & eno_coords)
{
  SpaceUnit *** space_array = NewSpaceArray(file_dimensions);
  
  int i, j, k;
  ENO_Interval_Image_t * eno_image = 0;
  ENO_Interval_t *x_intervals = 0, *y_intervals = 0, *z_intervals = 0;
  double temp_coords[NUM_DIMENSIONS];
  
  cout << "File Dimensions: "
       << file_dimensions[DIM_X] << "\t"
       << file_dimensions[DIM_Y] << "\t"
       << file_dimensions[DIM_Z] << endl;
  int prev_eno_count = 0;
  
  unsigned int pad_x_dim = file_dimensions[DIM_X]+2;
  unsigned int pad_y_dim = file_dimensions[DIM_Y]+2;
  unsigned int pad_z_dim = file_dimensions[DIM_Z]+2;
  
  //Get ENO points of YZ slices
  double * slice = new double[pad_y_dim*pad_z_dim];
  double * slice_ptr = 0;
  for(i = 0; i < file_dimensions[DIM_X]; i++)
  {
    prev_eno_count = eno_array.size();
    
    /*
     * Set Up Slice Buffer
     * Each Slice is padded on all sides with a 2-width border, which holds
     * values replicated from the adjacent edges of the distance transform
     */
    
    // Border Corners
    slice[0] = distance_transform[i][0][0];
    slice[pad_z_dim-1] = distance_transform[i][0][file_dimensions[DIM_Z]-1];
    slice[pad_z_dim*(pad_y_dim-1)] =
      distance_transform[i][file_dimensions[DIM_Y]-1][0];
    slice[pad_z_dim*pad_y_dim-1] =
      distance_transform[i][file_dimensions[DIM_Y]-1][file_dimensions[DIM_Z]-1];
    
    for(k = 0; k < file_dimensions[DIM_Z]; k++)
      slice[k+1] = distance_transform[i][0][k];
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
    {
      slice[pad_z_dim*(j+1)] = distance_transform[i][j][0];
      slice_ptr = &slice[pad_z_dim*(j+1)+1];
      for(k = 0; k < file_dimensions[DIM_Z]; k++)
      {
        *slice_ptr++ = double(distance_transform[i][j][k]);
#ifdef DEBUG7
        if(distance_transform[i][j][k] != 0.0f)
          cout << "["<<i<<"]["<<j<<"]["<<k<<"]: "
               << distance_transform[i][j][k] << endl;
#endif
      }
      slice[pad_z_dim*(j+2)-1] = distance_transform[i][j][file_dimensions[DIM_Z]-1];
    }
    for(k = 0; k < file_dimensions[DIM_Z]; k++)
      slice[pad_z_dim*(pad_y_dim-1)+k+1] = distance_transform[i][file_dimensions[DIM_Y]-1][k];
    
/***********************
    for(int blah = 0; blah < pad_y_dim*pad_z_dim; blah++)
    {
      if(blah % pad_z_dim == 0)
        cout << endl;
      cout << slice[blah] << "\t";
    }
    cout << endl;
***********************/    
    
    eno_image = enoimage_from_data(slice,
                                   pad_y_dim,
                                   pad_z_dim);
    
    // Skip border
    y_intervals = &eno_image->vert[pad_z_dim];
    z_intervals = &eno_image->horiz[pad_z_dim];
    
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
    {
      // Skip border
      y_intervals++;
      z_intervals++;
      for(k = 0; k < file_dimensions[DIM_Z]; k++)
      {
        //cout << y_intervals->zerox << endl;
        //cout << z_intervals->zerox << endl;
        
        temp_coords[DIM_X] = double(i);
        
        temp_coords[DIM_Z] = double(k);
        GetENODataFromZerox(y_intervals->zerox,
                            i, j, k, DIM_Y,
                            &space_array[i][j][k],
                            temp_coords,
                            eno_coords,
                            eno_array);
        //cout << y_intervals->zerox->cnt << " ENOs" << endl;
        
        temp_coords[DIM_Y] = double(j);
        GetENODataFromZerox(z_intervals->zerox,
                            i, j, k, DIM_Z,
                            &space_array[i][j][k],
                            temp_coords,
                            eno_coords,
                            eno_array);
        //cout << z_intervals->zerox->cnt << " ENOs" << endl;
        
        // Skip border
        y_intervals++;
        z_intervals++;
      }
      // Skip border
      y_intervals++;
      z_intervals++;
    }
    
#ifdef DEBUG6
    cout << eno_array.size() - prev_eno_count << " Y and Z ENOs found in YZ slice " << i << endl;
#endif
#ifdef DEBUG8
    for(k = 0; k < file_dimensions[DIM_Z]; k++)
    {
      for(j = 0; j < file_dimensions[DIM_Y]; j++)
      {
        cout << distance_transform[i][j][k] << "\t";
      }
      cout << endl;
    }
    
    for(int p = prev_eno_count; p < eno_array.size(); p++)
      cout << eno_coords[3*p+0] << "\t"
           << eno_coords[3*p+1] << "\t"
           << eno_coords[3*p+2] << endl;
#endif

    enoimage_free(eno_image);
  }
  
  // Get X-gridline ENO points of XY slices
  
  if((pad_x_dim != pad_y_dim) ||
     (pad_y_dim != pad_z_dim))
  {
    delete[](slice);
    slice = new double[pad_x_dim*pad_y_dim];
  }
  
  //prev_eno_count = eno_array.size();
  for(k = 0; k < file_dimensions[DIM_Z]; k++)
  {
    prev_eno_count = eno_array.size();
    
    /*
     * Set Up Slice Buffer
     * Each Slice is padded on all sides with a 2-width border, which holds
     * values replicated from the adjacent edges of the distance transform
     */
    
    // Border Corners
    slice[0] = distance_transform[0][0][k];
    slice[pad_y_dim-1] = distance_transform[0][file_dimensions[DIM_Y]-1][k];
    slice[pad_y_dim*(pad_x_dim-1)] =
      distance_transform[file_dimensions[DIM_X]-1][0][k];
    slice[pad_y_dim*pad_x_dim-1] =
      distance_transform[file_dimensions[DIM_X]-1][file_dimensions[DIM_Y]-1][k];
    
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
      slice[j+1] = distance_transform[0][j][k];
    for(i = 0; i < file_dimensions[DIM_X]; i++)
    {
      slice[pad_y_dim*(i+1)] = distance_transform[i][0][k];
      slice_ptr = &slice[pad_y_dim*(i+1)+1];
      for(j = 0; j < file_dimensions[DIM_Y]; j++)
      {
        *slice_ptr++ = double(distance_transform[i][j][k]);
#ifdef DEBUG7
        if(distance_transform[i][j][k] != 0.0f)
          cout << "["<<i<<"]["<<j<<"]["<<k<<"]: "
               << distance_transform[i][j][k] << endl;
#endif
      }
      slice[pad_y_dim*(i+2)-1] = distance_transform[i][file_dimensions[DIM_Y]-1][k];
    }
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
      slice[pad_y_dim*(pad_x_dim-1)+j+1] = distance_transform[file_dimensions[DIM_X]-1][j][k];
    
    eno_image = enoimage_from_data(slice,
                                   pad_x_dim,
                                   pad_y_dim);
    
    // Skip Border
    x_intervals = &eno_image->vert[pad_y_dim];
    //y_intervals = eno_image->horiz[pad_y_dim];
    
    for(i = 0; i < file_dimensions[DIM_X]; i++)
    {
      // Skip Border
      x_intervals++;
      for(j = 0; j < file_dimensions[DIM_Y]; j++)
      {
        //cout << x_intervals->zerox << endl;
        temp_coords[DIM_Z] = double(k);
        
        temp_coords[DIM_Y] = double(j);
        GetENODataFromZerox(x_intervals->zerox,
                            i, j, k, DIM_X,
                            &space_array[i][j][k],
                            temp_coords,
                            eno_coords,
                            eno_array);
        //cout << x_intervals->zerox->cnt << " ENOs" << endl;
        
        // Skip Border
        x_intervals++;
      }
      // Skip Border
      x_intervals++;
    }
    
#ifdef DEBUG6
    cout << eno_array.size() - prev_eno_count << " X ENOs found in XY slice " << k << endl;
#endif
#ifdef DEBUG8
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
    {
      for(i = 0; i < file_dimensions[DIM_X]; i++)
      {
        cout << distance_transform[i][j][k] << "\t";
      }
      cout << endl;
    }
    
    for(int p = prev_eno_count; p < eno_array.size(); p++)
      cout << eno_coords[3*p+0] << "\t"
           << eno_coords[3*p+1] << "\t"
           << eno_coords[3*p+2] << endl;
#endif
    
    enoimage_free(eno_image);
  }
  
  cout << eno_array.size() << " ENO points found." << endl;
  
  delete[](slice);

/*    
    cout << eno_array[51].indexes[0] << "\t"
         << eno_array[51].indexes[1] << "\t"
         << eno_array[51].indexes[2] << "\t"
         << DIMENSION_CHARS[eno_array[51].dim_index] << endl;
    cout << eno_coords[3*51+0] << "\t"
         << eno_coords[3*51+1] << "\t"
         << eno_coords[3*51+2] << endl;
    //exit(0);
*/
#ifdef DEBUG10
  i = 63;
  j = 61;
  k = 154;
  //SpaceUnit * this_unit = &space_array[63][61][154];
  cout << "num_enos for space_array[63][61][154]" << endl;
  cout << "000X\t" << int(space_array[i][j][k].num_enos[0]) << "\t" << space_array[i][j][k].eno_indexes[0][0] << "\t" << space_array[i][j][k].eno_indexes[0][1] << endl
       << "000Y\t" << int(space_array[i][j][k].num_enos[1]) << "\t" << space_array[i][j][k].eno_indexes[1][0] << "\t" << space_array[i][j][k].eno_indexes[1][1] << endl
       << "000Z\t" << int(space_array[i][j][k].num_enos[2]) << "\t" << space_array[i][j][k].eno_indexes[2][0] << "\t" << space_array[i][j][k].eno_indexes[2][1] << endl
       << "100Y\t" << int(space_array[i+1][j][k].num_enos[1]) << "\t" << space_array[i+1][j][k].eno_indexes[1][0] << "\t" << space_array[i+1][j][k].eno_indexes[1][1] << endl
       << "100Z\t" << int(space_array[i+1][j][k].num_enos[2]) << "\t" << space_array[i+1][j][k].eno_indexes[2][0] << "\t" << space_array[i+1][j][k].eno_indexes[2][1] << endl
       << "010X\t" << int(space_array[i][j+1][k].num_enos[0]) << "\t" << space_array[i][j+1][k].eno_indexes[0][0] << "\t" << space_array[i][j+1][k].eno_indexes[0][1] << endl
       << "010Z\t" << int(space_array[i][j+1][k].num_enos[2]) << "\t" << space_array[i][j+1][k].eno_indexes[2][0] << "\t" << space_array[i][j+1][k].eno_indexes[2][1] << endl
       << "001X\t" << int(space_array[i][j][k+1].num_enos[0]) << "\t" << space_array[i][j][k+1].eno_indexes[0][0] << "\t" << space_array[i][j][k+1].eno_indexes[0][1] << endl
       << "001Y\t" << int(space_array[i][j][k+1].num_enos[1]) << "\t" << space_array[i][j][k+1].eno_indexes[1][0] << "\t" << space_array[i][j][k+1].eno_indexes[1][1] << endl
       << "110Z\t" << int(space_array[i+1][j+1][k].num_enos[2]) << "\t" << space_array[i+1][j+1][k].eno_indexes[2][0] << "\t" << space_array[i+1][j+1][k].eno_indexes[2][1] << endl
       << "011X\t" << int(space_array[i][j+1][k+1].num_enos[0]) << "\t" << space_array[i][j+1][k+1].eno_indexes[0][0] << "\t" << space_array[i][j+1][k+1].eno_indexes[0][1] << endl
       << "101Y\t" << int(space_array[i+1][j][k+1].num_enos[1]) << "\t" << space_array[i+1][j][k+1].eno_indexes[1][0] << "\t" << space_array[i+1][j][k+1].eno_indexes[1][1] << endl;
#endif


  return space_array;
}




/******************************************************************************/

SpaceUnit *** NewSpaceArray(const unsigned int file_dimensions[NUM_DIMENSIONS])
{
  /*
   * Allocate and Initialize 3D Space Array of SpaceUnits
   * single block of data with pointer blocks to allow [x][y][z] 3D referencing.
   */
   
  SpaceUnit *** space_array = new SpaceUnit**[file_dimensions[DIM_X]];
  space_array[0] = new SpaceUnit*[file_dimensions[DIM_X]*file_dimensions[DIM_Y]];
  space_array[0][0] = new SpaceUnit[file_dimensions[DIM_X]*
                                    file_dimensions[DIM_Y]*
                                    file_dimensions[DIM_Z]];
  
  SpaceUnit * this_unit = 0;
  int i, j, k;
  
  for(i = 0; i < file_dimensions[DIM_X]; i++)
  {
    space_array[i] = space_array[0] + i*file_dimensions[DIM_Y];
    
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
    {
      space_array[i][j] = space_array[0][0]
                          + (i*file_dimensions[DIM_Y]+j)*file_dimensions[DIM_Z];
      
      for(k = 0; k < file_dimensions[DIM_Z]; k++)
      {
        this_unit = &space_array[i][j][k];
        this_unit->border = (i == file_dimensions[DIM_X]-1) ||
                            (j == file_dimensions[DIM_Y]-1) ||
                            (k == file_dimensions[DIM_Z]-1);
        
        // Loops expanded for better efficiency (N^3 coefficient)
        this_unit->num_enos[DIM_X] = 0;
        this_unit->num_enos[DIM_Y] = 0;
        this_unit->num_enos[DIM_Z] = 0;
        
        /*
         ******
         ******  Below Inits Unnecessary???
         ******
         */
        
        // Negative index means invalid point
        this_unit->eno_indexes[DIM_X][0] = -1;
        this_unit->eno_indexes[DIM_X][1] = -1;
        this_unit->eno_indexes[DIM_Y][0] = -1;
        this_unit->eno_indexes[DIM_Y][1] = -1;
        this_unit->eno_indexes[DIM_Z][0] = -1;
        this_unit->eno_indexes[DIM_Z][1] = -1;
      }
    }
  }
  
  return space_array;
}




/******************************************************************************/

inline void GetENODataFromZerox(ENO_ZeroCrossing_t * zerox,
                                const int i,
                                const int j,
                                const int k,
                                const int dim_index,
                                SpaceUnit * this_unit,
                                double coords[NUM_DIMENSIONS],
                                vector<double> & eno_coords,
                                vector<EnoPoint> & eno_array)
{
  if(!zerox)
  {
    this_unit->num_enos[dim_index] = 0;
    return;
  }
  
  EnoPoint new_eno;
  new_eno.indexes[DIM_X] = i;
  new_eno.indexes[DIM_Y] = j;
  new_eno.indexes[DIM_Z] = k;
  new_eno.dim_index = dim_index;
  new_eno.used = FALSE;
  
  this_unit->num_enos[dim_index] = zerox->cnt;
  if(zerox->cnt > 0)
  {
    this_unit->eno_indexes[dim_index][0] = eno_array.size();
    
    /*************************************************************
    * NOTE: WHEN USING OLD FindENOPoints,
    * Subtract 1 from coordinate to compensate for border offset
    *************************************************************/
    coords[dim_index] = zerox->loc[0];
    eno_coords.push_back(coords[DIM_X]);
    eno_coords.push_back(coords[DIM_Y]);
    eno_coords.push_back(coords[DIM_Z]);
    
    // Insert new ENO point into ENO point array
    eno_array.push_back(new_eno);
    
#ifdef DEBUG9
    cout << "Adding ENO0 at indexes ["<<i<<"]["<<j<<"]["<<k<<"] with coords"
         << endl << coords[DIM_X] << "\t" << coords[DIM_Y] << "\t"
         << coords[DIM_Z] << endl;
#endif
  }
  if(zerox->cnt == 2)
  {
    this_unit->eno_indexes[dim_index][1] = eno_array.size();
    
    /*************************************************************
    * NOTE: WHEN USING OLD FindENOPoints,
    * Subtract 1 from coordinate to compensate for border offset
    *************************************************************/
    coords[dim_index] = zerox->loc[1];
    eno_coords.push_back(coords[DIM_X]);
    eno_coords.push_back(coords[DIM_Y]);
    eno_coords.push_back(coords[DIM_Z]);
    
    // Insert new ENO point into ENO point array
    eno_array.push_back(new_eno);
    
#ifdef DEBUG9
    cout << "Adding ENO1 at indexes ["<<i<<"]["<<j<<"]["<<k<<"] with coords"
         << endl << coords[DIM_X] << "\t" << coords[DIM_Y] << "\t"
         << coords[DIM_Z] << endl;
#endif
  }
  if(zerox->cnt > MAX_ENOS_PER_EDGE)
  {
    cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << "More than " << MAX_ENOS_PER_EDGE << " ENO points found on "
         << "gridline ["<<i<<"]["<<j<<"]["<<k<<"] Y." << endl;
    exit(1);
  }
}




/******************************************************************************/

SpaceUnit *** FindENOPoints(float *** distance_transform,
                            const unsigned int file_dimensions[NUM_DIMENSIONS],
                            vector<EnoPoint> & eno_array,
                            vector<double> & eno_coords)
{
  // Value used to force ENO interpolantion to use data from other direction
  // (borrowed from the ENO tracer in eno_interval.c)
  const double kBorderValue = DBL_MAX/1.0e10;
  
  SpaceUnit *** space_array = NewSpaceArray(file_dimensions);
  
  int i, j, k;
  //ENO_Interval_t *x_intervals = 0, *y_intervals = 0, *z_intervals = 0;
  
  cout << "File Dimensions: "
       << file_dimensions[DIM_X] << "\t"
       << file_dimensions[DIM_Y] << "\t"
       << file_dimensions[DIM_Z] << endl;
  //int prev_eno_count = 0;
  
  unsigned int pad_x_dim = file_dimensions[DIM_X]+2;
  unsigned int pad_y_dim = file_dimensions[DIM_Y]+2;
  unsigned int pad_z_dim = file_dimensions[DIM_Z]+2;
  
#ifdef DEBUG6
  unsigned int prev_eno_count = eno_array.size();
#endif
  
  /*
   * Find X ENOs
   */
  double * gridline_data = new double[pad_x_dim+2];
  gridline_data[0] = kBorderValue;
  gridline_data[pad_x_dim+1] = kBorderValue;

  ENO_Interval_t * interval_array = new ENO_Interval_t[pad_x_dim];
  double temp_coords[NUM_DIMENSIONS];

  for(j = 0; j < file_dimensions[DIM_Y]; j++)
  {
    for(k = 0; k < file_dimensions[DIM_Z]; k++)
    {
      // Set up DT data block
      gridline_data[1] = distance_transform[0][j][k];
      for(i = 0; i < file_dimensions[DIM_X]; i++)
        gridline_data[i+2] = distance_transform[i][j][k];
      gridline_data[pad_x_dim] = 
        distance_transform[file_dimensions[DIM_X]-1][j][k];
      
      FindENOPointsInGridline(gridline_data,
                              file_dimensions[DIM_X],
                              interval_array);
      
      temp_coords[DIM_Y] = double(j);
      temp_coords[DIM_Z] = double(k);
      for(i = 0; i < file_dimensions[DIM_X]; i++)
      {
        GetENODataFromZerox(interval_array[i+1].zerox,
                            i, j, k, DIM_X,
                            &space_array[i][j][k],
                            temp_coords,
                            eno_coords,
                            eno_array);
        
#ifdef DEBUG11
        cout << "["<<i<<"]["<<j<<"]["<<k<<"]: ";
        if(interval_array[i+1].zerox)
          cout << interval_array[i+1].zerox->cnt << " X ENOs" << endl;
        else
          cout << "No (" << interval_array[i+1].zerox << ") X ENOs" << endl;
#endif
      }
      
      for(i = 0; i < pad_x_dim; i++)
      {
        if(interval_array[i].shock)
          enoDEL(interval_array[i].shock);
        if(interval_array[i].zerox)
          enoDEL(interval_array[i].zerox);
      }
    }
  }
  delete[](gridline_data);
  delete[](interval_array);
#ifdef DEBUG6
  cout << eno_array.size() - prev_eno_count << " X ENOs found" << endl;
  prev_eno_count = eno_array.size();
#endif

  /*
   * Find Y ENOs
   */
  gridline_data = new double[pad_y_dim+2];
  gridline_data[0] = kBorderValue;
  gridline_data[pad_y_dim+1] = kBorderValue;

  interval_array = new ENO_Interval_t[pad_y_dim];

  for(i = 0; i < file_dimensions[DIM_X]; i++)
  {
    for(k = 0; k < file_dimensions[DIM_Z]; k++)
    {
      // Set up DT data block
      gridline_data[1] = distance_transform[i][0][k];
      for(j = 0; j < file_dimensions[DIM_Y]; j++)
        gridline_data[j+2] = distance_transform[i][j][k];
      gridline_data[pad_y_dim] = 
        distance_transform[i][file_dimensions[DIM_Y]-1][k];
      
      FindENOPointsInGridline(gridline_data,
                              file_dimensions[DIM_Y],
                              interval_array);
      
      temp_coords[DIM_X] = double(i);
      temp_coords[DIM_Z] = double(k);
      for(j = 0; j < file_dimensions[DIM_Y]; j++)
      {
        GetENODataFromZerox(interval_array[j+1].zerox,
                            i, j, k, DIM_Y,
                            &space_array[i][j][k],
                            temp_coords,
                            eno_coords,
                            eno_array);
        
#ifdef DEBUG11
        cout << "["<<i<<"]["<<j<<"]["<<k<<"]: ";
        if(interval_array[j+1].zerox)
          cout << interval_array[j+1].zerox->cnt << " Y ENOs" << endl;
        else
          cout << "No (" << interval_array[j+1].zerox << ") Y ENOs" << endl;
#endif
      }
      
      for(j = 0; j < pad_y_dim; j++)
      {
        if(interval_array[j].shock)
          enoDEL(interval_array[j].shock);
        if(interval_array[j].zerox)
          enoDEL(interval_array[j].zerox);
      }
    }
  }
  delete[](gridline_data);
  delete[](interval_array);
#ifdef DEBUG6
  cout << eno_array.size() - prev_eno_count << " Y ENOs found" << endl;
  prev_eno_count = eno_array.size();
#endif
  
  /*
   * Find Z ENOs
   */
  gridline_data = new double[pad_z_dim+2];
  gridline_data[0] = kBorderValue;
  gridline_data[pad_z_dim+1] = kBorderValue;

  interval_array = new ENO_Interval_t[pad_z_dim];

  for(i = 0; i < file_dimensions[DIM_X]; i++)
  {
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
    {
      // Set up DT data block
      gridline_data[1] = distance_transform[i][j][0];
      for(k = 0; k < file_dimensions[DIM_Z]; k++)
        gridline_data[k+2] = distance_transform[i][j][k];
      gridline_data[pad_z_dim] = 
        distance_transform[i][j][file_dimensions[DIM_Z]-1];
      
      FindENOPointsInGridline(gridline_data,
                              file_dimensions[DIM_Z],
                              interval_array);
      
      temp_coords[DIM_X] = double(i);
      temp_coords[DIM_Y] = double(j);
      for(k = 0; k < file_dimensions[DIM_Z]; k++)
      {
        GetENODataFromZerox(interval_array[k+1].zerox,
                            i, j, k, DIM_Z,
                            &space_array[i][j][k],
                            temp_coords,
                            eno_coords,
                            eno_array);
        
#ifdef DEBUG11
        cout << "["<<i<<"]["<<j<<"]["<<k<<"]: ";
        if(interval_array[k+1].zerox)
          cout << interval_array[k+1].zerox->cnt << " Z ENOs" << endl;
        else
          cout << "No (" << interval_array[k+1].zerox << ") Z ENOs" << endl;
#endif
      }
      
      for(k = 0; k < pad_z_dim; k++)
      {
        if(interval_array[k].shock)
          enoDEL(interval_array[k].shock);
        if(interval_array[k].zerox)
          enoDEL(interval_array[k].zerox);
      }
    }
  }
  delete[](gridline_data);
  delete[](interval_array);
#ifdef DEBUG6
  cout << eno_array.size() - prev_eno_count << " Z ENOs found" << endl;
#endif
  
  //exit(0);
  
  return space_array;
}




/******************************************************************************/

inline void FindENOPointsInGridline(const double data[],
                                    const unsigned int length,
                                    ENO_Interval_t interval_array[])
{
  // Clear interval struct array on each iteration
  memset((char *)interval_array, 0, (length+2)*sizeof(ENO_Interval_t));
  
  unsigned int idx;
  for(idx = 0; idx < length+2; idx++)
//    if(data[idx+1]*data[idx+2]<4)
      enointerp_make_interpolant(&interval_array[idx].interp,
                                 idx-1, idx, &data[idx]);
  for(idx = 0; idx < length; idx++)
//    if(data[idx+2]*data[idx+3]>0 && data[idx+2]*data[idx+3]<1)
      enoshock_check_for_shock(&interval_array[idx],
                               &interval_array[idx+1],
                               &interval_array[idx+2]);
  for(idx = 0; idx < length; idx++)
//    if(data[idx+2]*data[idx+3]<4)
      enozerox_check_for_zerox_in_interval(&interval_array[idx],
                                           &interval_array[idx+1],
                                           &interval_array[idx+2]);
  
}
