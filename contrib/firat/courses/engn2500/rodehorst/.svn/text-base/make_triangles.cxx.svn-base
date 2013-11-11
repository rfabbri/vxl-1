#include <stdlib.h>

#include <iostream>
#include <vector>
#include <deque>

using namespace std;

#include "utility.h"
#include "surface_tracer.h"

/******************************************************************************/
/* Function Prototypes for just this file */

/******************************************************************************/

void MakeTriangles(const vector<double> & eno_coords, // inputs
                   vector<EnoPoint> & eno_array, // set used flags
                   const unsigned int * poly_idxs,
                   const unsigned int * poly_dims,
                   const unsigned int * poly_sides,
                   const unsigned int * poly_face_enos,
                   const unsigned int poly_length,
                   Voxel *** voxel_array, // set eno_used flags in faces
                   const unsigned int voxel_idxs[NUM_DIMENSIONS],
                   const unsigned int file_dimensions[NUM_DIMENSIONS],
                   deque<WaveElement> & wave_front, // (in-)outputs
                   vector<unsigned int> & poly_list,
                   vector<double> & normal_list)
{
  double new_normal[NUM_DIMENSIONS];
  unsigned int i, edge;
  unsigned int swap_temp;

#ifdef DEBUG17
  unsigned int start_poly_list_size = poly_list.size();
#endif
  
  if((poly_length < POINTS_PER_TRIANGLE) || (poly_length > MAX_ENOS_PER_VOXEL))
  {
    cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << "Polygon has invalid number of vertices: " << poly_length << endl;
    
    exit(1);
  }
/********
  else if(poly_length == POINTS_PER_TRIANGLE)
  {
    // Just make a triangle.
    
    poly_list.push_back(poly_idxs[0]);
    poly_list.push_back(poly_idxs[1]);
    poly_list.push_back(poly_idxs[2]);

    GetTriangleNormal(eno_coords.begin(), poly_idxs, new_normal);
    normal_list.push_back(new_normal[0]);
    normal_list.push_back(new_normal[1]);
    normal_list.push_back(new_normal[2]);

    for(edge = 0; edge < poly_length; edge++)
      wave_front.push_back(WaveElement(poly_face_enos[2*edge+1],
                                       poly_face_enos[2*edge],
                                       new_normal,
                                       voxel_idxs,
                                       poly_dims[edge],
                                       poly_sides[edge]));
  }
********/
  else
  {
    // Make the first triangle from points 0, 1, and 2.
    
    poly_list.push_back(poly_idxs[0]);
    poly_list.push_back(poly_idxs[1]);
    poly_list.push_back(poly_idxs[2]);

    GetTriangleNormal(&*eno_coords.begin(), poly_idxs, new_normal);
    normal_list.push_back(new_normal[0]);
    normal_list.push_back(new_normal[1]);
    normal_list.push_back(new_normal[2]);

    // Push the first two edges onto the wave front
    for(edge = 0; edge < 2; edge++)
    {
      WaveElement new_element(poly_face_enos[2*edge+1],
                              poly_face_enos[2*edge],
                              new_normal,
                              voxel_idxs,
                              poly_dims[edge],
                              poly_sides[edge]);
      if(new_element.IsValid(voxel_array, file_dimensions))
        wave_front.push_back(new_element);
    }
    
    // Initialize indexes for the following triangles
    unsigned int new_tri_idxs[POINTS_PER_TRIANGLE];
    memcpy(new_tri_idxs, poly_idxs, POINTS_PER_TRIANGLE*sizeof(unsigned int));
    
    for(i = POINTS_PER_TRIANGLE; i < poly_length; i++)
    {
      /*
       * Traverse polygon in alternating directions; better for 6+ sided polys.
       * This will generate the following sequence of length N = poly_length:
       * 
       * edge = (   0, 1,) N-1, 2, N-2, 3, N-3, 4, N-4, ... , (N+1)/2
       *
       * When setting up the next triangle, which is adjacent to the last one,
       * maintain CCW-positive orientation.  New vertex becomes idxs[1] (middle)
       * and the new edge for the wave front is idxs[2]-idxs[1].
       */
      
      if(i%2)
      {
        edge = poly_length - i/2;
        
        new_tri_idxs[1] = poly_idxs[edge];
        swap_temp = new_tri_idxs[0];
        new_tri_idxs[0] = new_tri_idxs[2];
        new_tri_idxs[2] = swap_temp;
      }
      else
      {
        edge = i/2;
        
        new_tri_idxs[2] = poly_idxs[edge + 1];
        swap_temp = new_tri_idxs[0];
        new_tri_idxs[0] = new_tri_idxs[1];
        new_tri_idxs[1] = swap_temp;
      }
      
      poly_list.push_back(new_tri_idxs[0]);
      poly_list.push_back(new_tri_idxs[1]);
      poly_list.push_back(new_tri_idxs[2]);
      
      GetTriangleNormal(&*eno_coords.begin(), new_tri_idxs, new_normal);
      normal_list.push_back(new_normal[0]);
      normal_list.push_back(new_normal[1]);
      normal_list.push_back(new_normal[2]);
      
      WaveElement new_element(poly_face_enos[2*edge+1],
                              poly_face_enos[2*edge],
                              new_normal,
                              voxel_idxs,
                              poly_dims[edge],
                              poly_sides[edge]);
      if(new_element.IsValid(voxel_array, file_dimensions))
        wave_front.push_back(new_element);
    }
    
    // Push the last closing edge onto the wave front
    edge = (poly_length+1)/2;
    WaveElement new_element(poly_face_enos[2*edge+1],
                            poly_face_enos[2*edge],
                            new_normal,
                            voxel_idxs,
                            poly_dims[edge],
                            poly_sides[edge]);
    if(new_element.IsValid(voxel_array, file_dimensions))
      wave_front.push_back(new_element);
  }

#ifdef DEBUG17
  if(((voxel_idxs[DIM_X] >= 48) && (voxel_idxs[DIM_X] <= 48)) &&
     ((voxel_idxs[DIM_Y] >= 35) && (voxel_idxs[DIM_Y] <= 36)) &&
     ((voxel_idxs[DIM_Z] >= 58) && (voxel_idxs[DIM_Y] <= 59)))
  {
    cout << "Triangles generated in Voxel [" << voxel_idxs[DIM_X] << "]["
         << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]";
    for(int dbi = start_poly_list_size; dbi < poly_list.size(); dbi++)
    {
      if(dbi%3 == 0)
        cout << endl;
      cout << poly_list[dbi] << " ";
    }
    cout << endl;
  }
#endif
  
  // Set Used Flags in Faces
  Voxel * voxel_ptr = &voxel_array[voxel_idxs[DIM_X]]
                                  [voxel_idxs[DIM_Y]]
                                  [voxel_idxs[DIM_Z]];
  for(edge = 0; edge < poly_length; edge++)
  {
    eno_array[poly_idxs[edge]].used = TRUE;
    voxel_ptr->faces[poly_dims[edge]][poly_sides[edge]]->
      eno_used[poly_face_enos[2*edge]] = TRUE;
    voxel_ptr->faces[poly_dims[edge]][poly_sides[edge]]->
      eno_used[poly_face_enos[2*edge+1]] = TRUE;
  }
}




