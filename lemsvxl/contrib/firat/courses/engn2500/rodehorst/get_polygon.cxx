#include <stdlib.h>

#include <iostream>
#include <vector>

using namespace std;

#include "utility.h"
#include "surface_tracer.h"

/******************************************************************************/
/* Function Prototypes for just this file */

inline bool SegmentsDoNotCross(const double x0, const double y0,
                               const double x1, const double y1,
                               const double xa, const double ya,
                               const double xb, const double yb);

/******************************************************************************/

unsigned int
GetUnambiguousPolygon(const Voxel * voxel_ptr,    // DON'T set eno_used flags
                      const vector<EnoPoint> & eno_array,
                      const unsigned int voxel_idxs[NUM_DIMENSIONS], // input
                      unsigned int poly_idxs[MAX_POINTS_PER_UNAMB_POLY],//output
                      unsigned int poly_dims[MAX_POINTS_PER_UNAMB_POLY],
                      unsigned int poly_sides[MAX_POINTS_PER_UNAMB_POLY],
                      unsigned int poly_face_enos[2*MAX_POINTS_PER_UNAMB_POLY])
{
  // Assume that the first two entries of poly_idxs are valid
  
  unsigned int next_idx = poly_idxs[1];
  Face * face_ptr = voxel_ptr->faces[poly_dims[0]][poly_sides[0]];
  //face_ptr->eno_used[poly_face_enos[0]] = TRUE;
  //face_ptr->eno_used[poly_face_enos[1]] = TRUE;
  
  //eno_array[poly_idxs[0]].used = TRUE;

  unsigned int count = 1;

  while(next_idx != poly_idxs[0])
  {
    if(count >= MAX_POINTS_PER_UNAMB_POLY)
    {
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "The polygon in this unambiguous Voxel [" << voxel_idxs[DIM_X]
           << "][" << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] 
           << "] doesn't complete a point loop." << endl;
      cout << "Voxel has " << int(voxel_ptr->num_enos) << " points." << endl;
      cout << "Number of ENOs in Faces:" << endl
           << "\tX0\t" << int(voxel_ptr->faces[DIM_X][0]->num_enos) << endl
           << "\tX1\t" << int(voxel_ptr->faces[DIM_X][1]->num_enos) << endl
           << "\tY0\t" << int(voxel_ptr->faces[DIM_Y][0]->num_enos) << endl
           << "\tY1\t" << int(voxel_ptr->faces[DIM_Y][1]->num_enos) << endl
           << "\tZ0\t" << int(voxel_ptr->faces[DIM_Z][0]->num_enos) << endl
           << "\tZ1\t" << int(voxel_ptr->faces[DIM_Z][1]->num_enos) << endl;
      cout << "GetUnambiguousPolygon() inputs:" << endl;
      cout << "voxel_ptr: " << voxel_ptr << endl;
      cout << "poly_idxs\tpoly_dims\tpoly_sides\tpoly_face_enos" << endl;
      for(int dbi = 0; dbi < MAX_POINTS_PER_UNAMB_POLY; dbi++)
      {
        cout << poly_idxs[dbi] << "\t\t" << DIMENSION_CHARS[poly_dims[dbi]]
             << "\t\t" << poly_sides[dbi] << "\t\t" << poly_face_enos[2*dbi]
             << "\t\t" << poly_face_enos[2*dbi+1] << endl;
      }
      
      exit(1);
    }
    
    //eno_array[next_idx].used = TRUE;
    poly_idxs[count] = next_idx;
    
    // Next face's dim is different from the face and edge dimensions
    // Subtraction works because NUM_DIMENSIONS == 3
    poly_dims[count] = NUM_DIMENSIONS - poly_dims[count-1] -
                       eno_array[poly_idxs[count]].dim_index;
    
    // Next face's side is 1 IFF the ENO point was in a diff SpaceUnit
    poly_sides[count] = int(voxel_idxs[poly_dims[count]] !=
                         eno_array[poly_idxs[count]].indexes[poly_dims[count]]);
    
    face_ptr = voxel_ptr->faces[poly_dims[count]][poly_sides[count]];

    // Next point is the other one in this face (there should be only 2)
    if(face_ptr->eno_indexes[0] == poly_idxs[count])
    {
      next_idx = face_ptr->eno_indexes[1];
      poly_face_enos[2*count+0] = 0;
      poly_face_enos[2*count+1] = 1;
    }
    else
    {
      next_idx = face_ptr->eno_indexes[0];
      poly_face_enos[2*count+0] = 1;
      poly_face_enos[2*count+1] = 0;
    }
    
    if(face_ptr->eno_used[poly_face_enos[2*count+1]])
    {
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "Face propagation is being forced upon a used ENO point in "
           << "Unambiguous Voxel [" << voxel_idxs[DIM_X] << "]["
           << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." << endl
           << "Voxel has " << int(voxel_ptr->num_enos) << " points." << endl;
      exit(1);
    }
      
    // Keep track that these points on the face have been used;
    // NOTE: This tells the wave to quench if appropriate
    //face_ptr->eno_used[0] = TRUE;
    //face_ptr->eno_used[1] = TRUE;
    
    count++;
  }
  
#ifdef DEBUG16
  cout << "Completed Unambiguous polygon ";
  for(int dbi = 0; dbi < count; dbi++)
    cout << poly_idxs[dbi] << " ";
  cout << endl;
#endif
  
  return count;
}




/******************************************************************************/

unsigned int
GetAmbiguousPolygon(const double in_normal[NUM_DIMENSIONS],
                    const vector<double> & eno_coords,
                    Voxel * voxel_ptr,            // set eno_used flags
                    vector<EnoPoint> & eno_array, // set used flags
                    const unsigned int voxel_idxs[NUM_DIMENSIONS], // input only
                    unsigned int poly_idxs[MAX_POINTS_PER_AMB_POLY+1],// outputs
                    unsigned int poly_dims[MAX_POINTS_PER_AMB_POLY+1],
                    unsigned int poly_sides[MAX_POINTS_PER_AMB_POLY],
                    unsigned int poly_face_enos[2*MAX_POINTS_PER_AMB_POLY])
{
  /*
   * Initialization
   */
  
  // Assume that the first two entries of poly_idxs are valid
  unsigned int forward_count = 1;
  unsigned int backward_count = MAX_POINTS_PER_AMB_POLY;
  
  Face * face_ptr = voxel_ptr->faces[poly_dims[0]][poly_sides[0]];
  face_ptr->eno_used[poly_face_enos[0]] = TRUE;
  face_ptr->eno_used[poly_face_enos[1]] = TRUE;
  
  eno_array[poly_idxs[0]].used = TRUE;
  eno_array[poly_idxs[1]].used = TRUE;
  
  // connect backward tracing to the first vertex
  poly_idxs[backward_count] = poly_idxs[0];
  poly_dims[backward_count] = poly_dims[0];
  bool trace_backward;
  
  double prev_normal[NUM_DIMENSIONS];
  memcpy(prev_normal, in_normal, NUM_DIMENSIONS*sizeof(double));
  
  unsigned int forward_dim,  backward_dim;
  unsigned int forward_side, backward_side;
  
  double best_dot_prod, check_dot_prod;
  unsigned int start_idx, idx_offset, check_idx;
  double check_normal[NUM_DIMENSIONS];
  double next_normal[NUM_DIMENSIONS];
  unsigned int triangle_idxs[NUM_DIMENSIONS];
  
  cout.precision(4);
  cout << "Starting polygon with ENOs " << poly_idxs[0] << " " << poly_idxs[1]
       << " and normal " << in_normal[0] << " " << in_normal[1] << " "
       << in_normal[2] << endl;

  /*
   * Tracing Loop
   *
   * At each iteration, try all possible points, forward and backward.  Choose
   * the point which makes a triangle with normal "closest to" the prev_normal.
   * In other words, choose the path of least curvature.
   */
  
  while(poly_idxs[forward_count] != poly_idxs[backward_count])
  {
/*************************************
    if(forward_count == backward_count)
    {
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "The polygon in this Ambiguous Voxel [" << voxel_idxs[DIM_X]
           << "][" << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] 
           << "] doesn't complete a point loop." << endl;
      cout << "Voxel has " << int(voxel_ptr->num_enos) << " points." << endl;
      cout << "Number of ENOs in Faces:" << endl
           << "\tX0\t" << int(voxel_ptr->faces[DIM_X][0]->num_enos) << endl
           << "\tX1\t" << int(voxel_ptr->faces[DIM_X][1]->num_enos) << endl
           << "\tY0\t" << int(voxel_ptr->faces[DIM_Y][0]->num_enos) << endl
           << "\tY1\t" << int(voxel_ptr->faces[DIM_Y][1]->num_enos) << endl
           << "\tZ0\t" << int(voxel_ptr->faces[DIM_Z][0]->num_enos) << endl
           << "\tZ1\t" << int(voxel_ptr->faces[DIM_Z][1]->num_enos) << endl;
      cout << "GetUnambiguousPolygon() inputs:" << endl;
      cout << "voxel_ptr: " << voxel_ptr << endl;
      cout << "poly_idxs\tpoly_dims\tpoly_sides\tpoly_face_enos" << endl;
      for(int dbi = 0; dbi < MAX_POINTS_PER_AMB_POLY; dbi++)
      {
        cout << poly_idxs[dbi] << "\t\t" << DIMENSION_CHARS[poly_dims[dbi]]
             << "\t\t" << poly_sides[dbi] << "\t\t" << poly_face_enos[2*dbi]
             << "\t\t" << poly_face_enos[2*dbi+1] << endl;
      }
      
      exit(1);
    }
*************************************/
    
    /*
     * Ensure that the first valid choice is caught;
     * Normal dot products should range between -1.0 and 1.0.
     * Also set up start of triangle for normal-checking
     */
    best_dot_prod = -2.0;
    triangle_idxs[0] = poly_idxs[backward_count];
    triangle_idxs[1] = poly_idxs[forward_count];
    
    /*
     * Forward Trace
     * Check indexes which can connect to the forward point: poly_idxs[count]
     */
    
    // Next face's dim is different from the face and edge dimensions
    // Subtraction works because NUM_DIMENSIONS == 3
    forward_dim = NUM_DIMENSIONS - poly_dims[forward_count-1] -
                  eno_array[poly_idxs[forward_count]].dim_index;
    
    // Next face's side is 1 IFF the ENO point was in a diff SpaceUnit
    forward_side = int(voxel_idxs[forward_dim] !=
                   eno_array[poly_idxs[forward_count]].indexes[forward_dim]);
    
    face_ptr = voxel_ptr->faces[forward_dim][forward_side];
    
    if(face_ptr->num_enos == 2)
    {
      // Unambiguous Forward Trace
      
      cout << "Forward Traversing Unambiguous Face with ENOs ";
      for(int dbi = 0; dbi < face_ptr->num_enos; dbi++)
        cout << face_ptr->eno_indexes[dbi] << " ";
      cout << endl;
      
      // NO AMBIGUITY -- Next point is the other one in this face
      if(face_ptr->eno_indexes[0] == poly_idxs[forward_count])
      {
        poly_idxs[forward_count+1] = face_ptr->eno_indexes[1];
        poly_face_enos[2*forward_count+0] = 0;
        poly_face_enos[2*forward_count+1] = 1;
      }
      else
      {
        poly_idxs[forward_count+1] = face_ptr->eno_indexes[0];
        poly_face_enos[2*forward_count+0] = 1;
        poly_face_enos[2*forward_count+1] = 0;
      }
      
      if(face_ptr->eno_used[poly_face_enos[2*forward_count+1]])
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Face propagation is being forced upon a used ENO point in "
             << "Ambiguous Voxel [" << voxel_idxs[DIM_X] << "]["
             << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." << endl
             << "Voxel has " << int(voxel_ptr->num_enos) << " points." << endl;
        exit(1);
      }
      
      if(poly_idxs[forward_count+1] == poly_idxs[backward_count])
      {
        best_dot_prod = 2.0; // Unbeatable dot product -- Stops Search
      }
      else
      {
        // No need to check since this is the first option and is always caught
        triangle_idxs[2] = poly_idxs[forward_count+1];
        GetTriangleNormal(&*eno_coords.begin(), triangle_idxs, next_normal);
        cout << "Triangle " << triangle_idxs[0] << " " << triangle_idxs[1]
             << " " << triangle_idxs[2] << " has normal " << next_normal[0]
             << " " << next_normal[1] << " " << next_normal[2] << endl;
        best_dot_prod = DotProduct(prev_normal, next_normal);
        trace_backward = FALSE;
      }
      
/***********
      // Keep track that these points on the face have been used;
      // NOTE: This tells the wave to quench if appropriate
      face_ptr->eno_used[0] = TRUE;
      face_ptr->eno_used[1] = TRUE;
      eno_array[poly_idxs[forward_count+1]].used = TRUE;
      forward_count++;
***********/
    }
    else
    {
/********/
      cout << "Forward Traversing Ambiguous Face with ENOs ";
      for(int dbi = 0; dbi < face_ptr->num_enos; dbi++)
        cout << face_ptr->eno_indexes[dbi] << " ";
      cout << endl;
/********/
    
      // Find the face index of poly_idxs[forward_count]
      start_idx = 0;
      while((face_ptr->eno_indexes[start_idx] != poly_idxs[forward_count]) &&
            (start_idx < face_ptr->num_enos))
        start_idx++;
      
      if(start_idx >= face_ptr->num_enos)
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "The " << DIMENSION_CHARS[poly_dims[forward_count]]
             << poly_sides[forward_count] << " face in this Ambiguous Voxel ["
             << voxel_idxs[DIM_X] << "][" << voxel_idxs[DIM_Y] << "]["
             << voxel_idxs[DIM_Z] << "] did not contain the ENO index "
             << poly_idxs[forward_count] << endl;
        
        exit(1);
      }
      
      poly_face_enos[2*forward_count+0] = start_idx;
      
      // Try out all possible next points, choosing point with "closest" normal
      for(idx_offset = 1;
          (idx_offset < face_ptr->num_enos) && (best_dot_prod < 2.0);
          idx_offset+=2)
      {
        check_idx = (start_idx+idx_offset)%face_ptr->num_enos;
         
        cout << "Checking ENO " << face_ptr->eno_indexes[check_idx] << " -- "
             << start_idx << " " << idx_offset << " " << check_idx << endl;
        
        /*************************************************************************
        *
        * NOTE: When it is possible to close the polygon, that choice is made
        *       and the search ends; it's debatable whether this is a good idea.
        *
        *       If it is possible to close a "polygon" of 2 points, that choice
        *       is given lowest priority.  In this situation, you are
        *       propagating a WaveElement with points on the same voxel edge into
        *       the neighboring voxel, without generating any triangles. 
        *
        *************************************************************************/
        if(face_ptr->eno_indexes[check_idx] == poly_idxs[backward_count])
        {
          if((forward_count != 1)||(backward_count != MAX_POINTS_PER_AMB_POLY))
          {
            best_dot_prod = 2.0; // Unbeatable dot product -- Stops Search
            trace_backward = FALSE;
            poly_idxs[forward_count+1] = face_ptr->eno_indexes[check_idx];
            poly_face_enos[2*forward_count+1] = check_idx;
          }
          else if(best_dot_prod <= -2.0)
          {
            best_dot_prod = -1.5; // lowest priority, but indicates valid choice
            trace_backward = FALSE;
            poly_idxs[forward_count+1] = face_ptr->eno_indexes[check_idx];
            poly_face_enos[2*forward_count+1] = check_idx;
          }
        }
        else if(!face_ptr->eno_used[check_idx])
        {
          triangle_idxs[2] = face_ptr->eno_indexes[check_idx];
          GetTriangleNormal(&*eno_coords.begin(), triangle_idxs, check_normal);
          cout << "Triangle " << triangle_idxs[0] << " " << triangle_idxs[1]
               << " " << triangle_idxs[2] << " has normal " << check_normal[0]
               << " " << check_normal[1] << " " << check_normal[2] << endl;
          check_dot_prod = DotProduct(prev_normal, check_normal);
          if(check_dot_prod > best_dot_prod)
          {
            best_dot_prod = check_dot_prod;
            trace_backward = FALSE;
            poly_idxs[forward_count+1] = face_ptr->eno_indexes[check_idx];
            poly_face_enos[2*forward_count+1] = check_idx;
            memcpy(next_normal, check_normal, NUM_DIMENSIONS*sizeof(double));
          }
        }
      }
    }
    
    if(best_dot_prod ==  2.0)
    {
      if(face_ptr->eno_used[poly_face_enos[2*forward_count+1]])
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Forward Face propagation is being forced upon a "
             << "used ENO point in Ambiguous Voxel ["<<voxel_idxs[DIM_X]<<"]["
             << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." <<endl
             << "Voxel has " << int(voxel_ptr->num_enos) << " points." <<endl;
        exit(1);
      }
      
      poly_dims[forward_count] = forward_dim;
      poly_sides[forward_count] = forward_side;
      face_ptr->eno_used[poly_face_enos[2*forward_count+0]] = TRUE;
      face_ptr->eno_used[poly_face_enos[2*forward_count+1]] = TRUE;
      eno_array[poly_idxs[forward_count+1]].used = TRUE;
      forward_count++;
      
      cout << "Added " << poly_idxs[forward_count] << " to front of poly"<<endl;
      break; // Signal to end search; leave loop.
    }
    if(best_dot_prod == -2.0)
    {
      // If we didn't find any valid choice, it must be an error.
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "Forward trace failed in Ambiguous Voxel ["
           << voxel_idxs[DIM_X] << "][" << voxel_idxs[DIM_Y] << "]["
           << voxel_idxs[DIM_Z] << "]." << endl;
    
      exit(1);
    }
    
    /*
     * Backward Trace:
     * Check indexes which can connect to the backward point:
     * poly_idxs[backward_count]
     */
      
    // Next face's dim is different from the face and edge dimensions
    // Subtraction works because NUM_DIMENSIONS == 3
    backward_dim = NUM_DIMENSIONS - poly_dims[backward_count] -
                   eno_array[poly_idxs[backward_count]].dim_index;
    
    // Next face's side is 1 IFF the ENO point was in a diff SpaceUnit
    backward_side = int(voxel_idxs[backward_dim] !=
                    eno_array[poly_idxs[backward_count]].indexes[backward_dim]);
    
    face_ptr = voxel_ptr->faces[backward_dim][backward_side];
    
    if(face_ptr->num_enos == 2)
    {
      // Unambiguous Backward Trace
      
      cout << "Backward Traversing Unambiguous Face with ENOs ";
      for(int dbi = 0; dbi < face_ptr->num_enos; dbi++)
        cout << face_ptr->eno_indexes[dbi] << " ";
      cout << endl;
      
      // NO AMBIGUITY -- Next point is the other one in this face
      check_idx = (face_ptr->eno_indexes[0] == poly_idxs[backward_count]);
      start_idx = !check_idx;
      poly_face_enos[2*(backward_count-1)+1] = start_idx;
      
      if(face_ptr->eno_used[check_idx])
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Face propagation is being forced upon a used ENO point in "
             << "Ambiguous Voxel [" << voxel_idxs[DIM_X] << "]["
             << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." << endl
             << "Voxel has " << int(voxel_ptr->num_enos) << " points." << endl;
        exit(1);
      }
      
      
      triangle_idxs[2] = face_ptr->eno_indexes[check_idx];
      GetTriangleNormal(&*eno_coords.begin(), triangle_idxs, check_normal);
      cout << "Triangle " << triangle_idxs[0] << " " << triangle_idxs[1]
           << " " << triangle_idxs[2] << " has normal " << check_normal[0]
           << " " << check_normal[1] << " " << check_normal[2] << endl;
      check_dot_prod = DotProduct(prev_normal, check_normal);
      if(check_dot_prod > best_dot_prod)
      {
        best_dot_prod = check_dot_prod;
        trace_backward = TRUE;
        poly_idxs[backward_count-1] = face_ptr->eno_indexes[check_idx];
        poly_face_enos[2*(backward_count-1)+0] = check_idx;
        memcpy(next_normal, check_normal, NUM_DIMENSIONS*sizeof(double));
      }
      
/***********
      // Keep track that these points on the face have been used;
      // NOTE: This tells the wave to quench if appropriate
      face_ptr->eno_used[0] = TRUE;
      face_ptr->eno_used[1] = TRUE;
      eno_array[poly_idxs[backward_count-1]].used = TRUE;
      forward_count++;
***********/
    }
    else
    {
/********/
      cout << "Backward Traversing Face with ENOs ";
      for(int dbi = 0; dbi < face_ptr->num_enos; dbi++)
        cout << face_ptr->eno_indexes[dbi] << " ";
      cout << endl;
/********/
      
      // Find the face index of poly_idxs[backward_count]
      start_idx = 0;
      while((face_ptr->eno_indexes[start_idx] != poly_idxs[backward_count]) &&
            (start_idx < face_ptr->num_enos))
        start_idx++;
      
      if(start_idx >= face_ptr->num_enos)
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "The " << DIMENSION_CHARS[poly_dims[backward_count]]
             << poly_sides[backward_count] << " face in this Ambiguous Voxel ["
             << voxel_idxs[DIM_X] << "][" << voxel_idxs[DIM_Y] << "]["
             << voxel_idxs[DIM_Z] << "] did not contain the ENO index "
             << poly_idxs[backward_count] << endl;
        
        exit(1);
      }
    
      poly_face_enos[2*(backward_count-1)+1] = start_idx;
      
      for(idx_offset = 1;
          (idx_offset < face_ptr->num_enos) && (best_dot_prod < 2.0);
          idx_offset+=2)
      {
        check_idx = (start_idx+idx_offset)%face_ptr->num_enos;
        
        cout << "Checking ENO " << face_ptr->eno_indexes[check_idx] << " -- "
             << start_idx << " " << idx_offset << " " << check_idx << endl;
        
        if(!face_ptr->eno_used[check_idx] &&
           (face_ptr->eno_indexes[check_idx] != poly_idxs[forward_count]))
        {
          triangle_idxs[2] = face_ptr->eno_indexes[check_idx];
          GetTriangleNormal(&*eno_coords.begin(), triangle_idxs, check_normal);
          cout << "Triangle " << triangle_idxs[0] << " " << triangle_idxs[1]
               << " " << triangle_idxs[2] << " has normal " << check_normal[0]
               << " " << check_normal[1] << " " << check_normal[2] << endl;
          check_dot_prod = DotProduct(prev_normal, check_normal);
          if(check_dot_prod > best_dot_prod)
          {
            best_dot_prod = check_dot_prod;
            trace_backward = TRUE;
            poly_idxs[backward_count-1] = face_ptr->eno_indexes[check_idx];
            poly_face_enos[2*(backward_count-1)+0] = check_idx;
            memcpy(next_normal, check_normal, NUM_DIMENSIONS*sizeof(double));
          }
        }
      }
    }
    
    // If we didn't find any valid choice, it must be an error.
    if(best_dot_prod == -2.0)
    {
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "Backward trace failed in Ambiguous Voxel ["
           << voxel_idxs[DIM_X] << "][" << voxel_idxs[DIM_Y] << "]["
           << voxel_idxs[DIM_Z] << "]." << endl;
      
      exit(1);
    }
      
    /*
     * Update counters and Used flags
     */
    
    memcpy(prev_normal, next_normal, NUM_DIMENSIONS*sizeof(double));
    if(trace_backward)
    {
      face_ptr = voxel_ptr->faces[backward_dim][backward_side];
      if(face_ptr->eno_used[poly_face_enos[2*(backward_count-1)+1]])
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Backward Face propagation is being forced upon a "
             << "used ENO point in Ambiguous Voxel ["<<voxel_idxs[DIM_X]<<"]["
             << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." <<endl
             << "Voxel has " << int(voxel_ptr->num_enos) << " points." <<endl;
        exit(1);
      }
      
      poly_dims[backward_count-1] = backward_dim;
      poly_sides[backward_count-1] = backward_side;
      face_ptr->eno_used[poly_face_enos[2*(backward_count-1)+0]] = TRUE;
      face_ptr->eno_used[poly_face_enos[2*(backward_count-1)+1]] = TRUE;
      eno_array[poly_idxs[backward_count-1]].used = TRUE;
      backward_count--;
      
      cout << "Added " << poly_idxs[backward_count] << " to back of poly"<<endl;
    }
    else
    {
      face_ptr = voxel_ptr->faces[forward_dim][forward_side];
      if(face_ptr->eno_used[poly_face_enos[2*forward_count+1]])
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Forward Face propagation is being forced upon a "
             << "used ENO point in Ambiguous Voxel ["<<voxel_idxs[DIM_X]<<"]["
             << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." <<endl
             << "Voxel has " << int(voxel_ptr->num_enos) << " points." <<endl;
        exit(1);
      }
      
      poly_dims[forward_count] = forward_dim;
      poly_sides[forward_count] = forward_side;
      face_ptr->eno_used[poly_face_enos[2*forward_count+0]] = TRUE;
      face_ptr->eno_used[poly_face_enos[2*forward_count+1]] = TRUE;
      eno_array[poly_idxs[forward_count+1]].used = TRUE;
      forward_count++;
      
      cout << "Added " << poly_idxs[forward_count] << " to front of poly"<<endl;
    }
    
/******************************
    }
    else
    {
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "The " << DIMENSION_CHARS[poly_dims[count]] << poly_sides[count]
           << " face in this Ambiguous Voxel [" << voxel_idxs[DIM_X]
           << "][" << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] 
           << "] has an invalid number of ENOs: " << face_ptr->num_enos << endl;
          
      exit(1);
    }
******************************/
  } // end while(poly_idxs[forward_count] != poly_idxs[backward_count])
  
  /*
   * Move the backward-traced points in line with the forward-traced points
   */
  
  while(backward_count < MAX_POINTS_PER_AMB_POLY)
  {
    poly_idxs[forward_count] = poly_idxs[backward_count];
    poly_dims[forward_count] = poly_dims[backward_count];
    poly_sides[forward_count] = poly_sides[backward_count];
    poly_face_enos[2*forward_count+0] = poly_face_enos[2*backward_count+0];
    poly_face_enos[2*forward_count+1] = poly_face_enos[2*backward_count+1];
    
    forward_count++;
    backward_count++;
  }
  
  cout << "Completed Ambiguous polygon ";
  for(int dbi = 0; dbi < forward_count; dbi++)
    cout << poly_idxs[dbi] << " ";
  cout << endl;
  
  // Returns total number of points in this polygon
  return forward_count;
}




/******************************************************************************/

inline bool SegmentsDoNotCross(const double x0, const double y0,
                               const double x1, const double y1,
                               const double xa, const double ya,
                               const double xb, const double yb)
{
  bool no_cross = ((((ya-y0)*(x1-x0)-(xa-x0)*(y1-y0)) *
                    ((yb-y0)*(x1-x0)-(xb-x0)*(y1-y0))) > 0);
#ifdef DEBUG19
  if(!no_cross)
    cout << "INFO in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << "Found a Segment Crossing." << endl;
#endif
  return no_cross;
}
                              
