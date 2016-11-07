#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <deque>

using namespace std;

#include "utility.h"
#include "surface_tracer.h"

/******************************************************************************/
/* Function Prototypes for just this file */

inline double Score(const unsigned int triangle_idxs[NUM_DIMENSIONS],
                    const vector<double> & eno_coords,
                    const double prev_normal[NUM_DIMENSIONS],
                    double check_normal[NUM_DIMENSIONS],
                    const bool backward_flag);

inline bool CheckCrossings(const Face * face_ptr,
                           const unsigned int face_dim,
                           const unsigned int in_eno0,
                           const unsigned int in_eno1,
                           const vector<double> & eno_coords);

inline bool SegmentsDoNotCross(const double x0, const double y0,
                               const double x1, const double y1,
                               const double xa, const double ya,
                               const double xb, const double yb);

/******************************************************************************/

void PolygonizeVoxel(const WaveElement & in_element,
                     Voxel *** voxel_array,
                     const unsigned int file_dimensions[NUM_DIMENSIONS],
                     const vector<double> & eno_coords,
                     vector<EnoPoint> & eno_array, // set used flags
                     deque<WaveElement> & wave_front, // for outputs
                     vector<unsigned int> & poly_list,
                     vector<double> & normal_list)
{
  /*
   * Set up Variables for Propagation
   */
  
  Voxel * voxel_ptr = &voxel_array[in_element.voxel_idxs[DIM_X]]
                                  [in_element.voxel_idxs[DIM_Y]]
                                  [in_element.voxel_idxs[DIM_Z]];
  
  const unsigned int * voxel_idxs = in_element.voxel_idxs;
  //unsigned int poly_length;
  unsigned int poly_idxs[MAX_POINTS_PER_AMB_POLY+1];
  unsigned int poly_dims[MAX_POINTS_PER_AMB_POLY+1];
  unsigned int poly_sides[MAX_POINTS_PER_AMB_POLY];
  unsigned int poly_face_enos[2*MAX_POINTS_PER_AMB_POLY];
  poly_idxs[0] = voxel_ptr->faces[in_element.face_dim]
                                 [in_element.face_side]->
                      eno_indexes[in_element.eno0];
  poly_idxs[1] = voxel_ptr->faces[in_element.face_dim]
                                 [in_element.face_side]->
                      eno_indexes[in_element.eno1];
  poly_dims[0] = in_element.face_dim;
  poly_sides[0] = in_element.face_side;
  poly_face_enos[0] = in_element.eno0;
  poly_face_enos[1] = in_element.eno1;
  
  // Assume that the first two entries of poly_idxs are valid
  unsigned int forward_count = 1;
  unsigned int backward_count = MAX_POINTS_PER_AMB_POLY;
  
  Face * face_ptr = voxel_ptr->faces[poly_dims[0]][poly_sides[0]];
  face_ptr->eno_used[poly_face_enos[0]] = TRUE;
  face_ptr->eno_used[poly_face_enos[1]] = TRUE;
  
  //eno_array[poly_idxs[0]].used = TRUE;
  //eno_array[poly_idxs[1]].used = TRUE;
  
  // connect backward tracing to the first vertex
  poly_idxs[backward_count] = poly_idxs[0];
  poly_dims[backward_count] = poly_dims[0];
  bool trace_backward;
  
  double prev_normal[NUM_DIMENSIONS];
  memcpy(prev_normal, in_element.normal, NUM_DIMENSIONS*sizeof(double));
  
  unsigned int forward_dim,  backward_dim;
  unsigned int forward_side, backward_side;
  
  double best_score, check_score;
  unsigned int start_idx, idx_offset, check_idx;
  double check_normal[NUM_DIMENSIONS];
  double next_normal[NUM_DIMENSIONS];
  unsigned int triangle_idxs[NUM_DIMENSIONS];
  
#ifdef DEBUG_AMBIGUITY
  cout.precision(4);
  cout << "Polygonizing Voxel [" << in_element.voxel_idxs[DIM_X] << "]["
       << in_element.voxel_idxs[DIM_Y] << "][" << in_element.voxel_idxs[DIM_Z]
       << "]" << endl;
  cout << "Starting polygon with ENOs " << poly_idxs[0] << " " << poly_idxs[1]
       << " and normal " << in_element.normal[0] << " " << in_element.normal[1]
       << " " << in_element.normal[2] << endl;
#endif

  /*
   * Tracing Loop
   *
   * At each iteration, try all possible points, forward and backward.  Choose
   * the point which makes a triangle with normal "closest to" the prev_normal.
   * In other words, choose the path of least curvature.
   */
  
  while(poly_idxs[forward_count] != poly_idxs[backward_count])
  {
    /*
     * Ensure that the first valid choice is caught;
     * Normal dot products should range between -1.0 and 1.0.
     * Also set up start of triangle for normal-checking
     */
    best_score = INIT_SCORE;
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
      
#ifdef DEBUG_AMBIGUITY
      cout << "Forward Traversing Unambiguous Face with ENOs ";
      for(int dbi = 0; dbi < face_ptr->num_enos; dbi++)
        cout << face_ptr->eno_indexes[dbi] << " ";
      cout << endl;
#endif
      
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
             << "Voxel [" << voxel_idxs[DIM_X] << "]["
             << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." << endl
             << "Voxel has " << int(voxel_ptr->num_enos) << " points." << endl;
        exit(1);
      }
      
      if(poly_idxs[forward_count+1] == poly_idxs[backward_count])
      {
        if((forward_count != 1)||(backward_count != MAX_POINTS_PER_AMB_POLY))
          best_score = HIGHEST_SCORE; // Unbeatable -- Stops Search
        else
        {
          best_score = LOWEST_SCORE; // lowest priority, but a valid choice
          cout << "WARNING: A wave element on a single edge was propagated!"
               << endl << "This should have been discarded." << endl;
        }
      }
      else
      {
        // No need to check since this is the first option and is always caught
        triangle_idxs[2] = poly_idxs[forward_count+1];
        best_score = Score(triangle_idxs, eno_coords, 
                           prev_normal, next_normal, FALSE);
      }
      trace_backward = FALSE;
    }
    else // (face_ptr->num_enos != 2)
    {
#ifdef DEBUG_AMBIGUITY
      cout << "Forward Traversing Ambiguous Face with ENOs ";
      for(int dbi = 0; dbi < face_ptr->num_enos; dbi++)
        cout << face_ptr->eno_indexes[dbi] <<(face_ptr->eno_used[dbi]?"* ":" ");
      cout << endl;
#endif
#ifdef DEBUG_RESOLVE
      if(face_ptr->ambiguities)
      {
        cout << "Ambiguities Already Resolved: " << endl;
        for(int dbi = 0; dbi < face_ptr->num_enos; dbi++)
          if(face_ptr->ambiguities[dbi] != -1)
            cout << face_ptr->eno_indexes[dbi] << " "
                 << face_ptr->eno_indexes[face_ptr->ambiguities[dbi]] << endl;
      }
#endif
      
      // Find the face index of poly_idxs[forward_count]
      start_idx = 0;
      while((face_ptr->eno_indexes[start_idx] != poly_idxs[forward_count]) &&
            (start_idx < face_ptr->num_enos))
        start_idx++;
      
      if(start_idx >= face_ptr->num_enos)
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "The " << DIMENSION_CHARS[poly_dims[forward_count]]
             << poly_sides[forward_count] << " face in this Voxel ["
             << voxel_idxs[DIM_X] << "][" << voxel_idxs[DIM_Y] << "]["
             << voxel_idxs[DIM_Z] << "] did not contain the ENO index "
             << poly_idxs[forward_count] << endl;
        
        exit(1);
      }
      
      poly_face_enos[2*forward_count+0] = start_idx;
      
      /*
       * Check to see if Ambiguitiy is already resolved.
       */
      
      if(face_ptr->ambiguities[start_idx] != -1)
      {
        check_idx = face_ptr->ambiguities[start_idx];
#ifdef DEBUG_RESOLVE
        cout << "Using Ambiguity Already Resolved: "
             << face_ptr->eno_indexes[start_idx] << " "
             << face_ptr->eno_indexes[check_idx] << endl;
#endif
        
        if(face_ptr->eno_indexes[check_idx] == poly_idxs[backward_count])
        {
          best_score = HIGHEST_SCORE; // Unbeatable -- Stops Search
        }
        else
        {
          // No need to check since this is the first option; is always caught
          triangle_idxs[2] = face_ptr->eno_indexes[check_idx];
          best_score = Score(triangle_idxs, eno_coords, 
                             prev_normal, next_normal, FALSE);
        }
        trace_backward = FALSE;
        poly_idxs[forward_count+1] = face_ptr->eno_indexes[check_idx];
        poly_face_enos[2*forward_count+1] = check_idx;
      }
      else
      {
        // Try out all possible next points, choosing point with closest normal
        for(idx_offset = 1;
            (idx_offset < face_ptr->num_enos)&&(best_score < HIGHEST_SCORE);
            idx_offset+=2)
        {
          check_idx = (start_idx+idx_offset)%face_ptr->num_enos;
          
          // If this one's ambiguity is resolved (to something else),
          // you can't use it.
          if(face_ptr->ambiguities[check_idx] != -1)
            continue;
           
#ifdef DEBUG_AMBIGUITY
          cout << "Checking ENO " << face_ptr->eno_indexes[check_idx] << " -- "
               << start_idx << " " << idx_offset << " " << check_idx << endl;
#endif
          
          /*********************************************************************
          *
          * NOTE: When it is possible to close a polygon of at least 3 sides,
          *       that choice is made and the search ends.
          *
          *       If it is possible to close a "polygon" of 2 points, that
          *       choice is given lowest priority.  In this situation, you are
          *       propagating a WaveElement with points on the same voxel edge
          *       into the neighboring voxel, without generating any triangles. 
          *
          *********************************************************************/
          if(face_ptr->eno_indexes[check_idx] == poly_idxs[backward_count])
          {
            if((forward_count!=1)||(backward_count != MAX_POINTS_PER_AMB_POLY))
            {
              best_score = HIGHEST_SCORE; // Unbeatable -- Stops Search
              trace_backward = FALSE;
              poly_idxs[forward_count+1] = face_ptr->eno_indexes[check_idx];
              poly_face_enos[2*forward_count+1] = check_idx;
            }
            else if(best_score < LOWEST_SCORE)
            {
              cout << "WARNING: A wave element on a single edge was propagated!"
                   << endl << "This should have been discarded." << endl;
              best_score = LOWEST_SCORE; // lowest priority, but a valid choice
              trace_backward = FALSE;
              poly_idxs[forward_count+1] = face_ptr->eno_indexes[check_idx];
              poly_face_enos[2*forward_count+1] = check_idx;
            }
          }
          else if(!face_ptr->eno_used[check_idx])
          {
            triangle_idxs[2] = face_ptr->eno_indexes[check_idx];
            check_score = Score(triangle_idxs, eno_coords, 
                                prev_normal, check_normal, FALSE);
            if((check_score > best_score) &&
               ((face_ptr->num_enos < 6) ||
                CheckCrossings(face_ptr, forward_dim,
                               start_idx, check_idx, eno_coords)))
            {
              best_score = check_score;
              trace_backward = FALSE;
              poly_idxs[forward_count+1] = face_ptr->eno_indexes[check_idx];
              poly_face_enos[2*forward_count+1] = check_idx;
              memcpy(next_normal, check_normal, NUM_DIMENSIONS*sizeof(double));
            }
          }
        } // End for loop over every other eno in face
      } // End if Ambiguity Unresolved
    } // End if (face_ptr->num_enos != 2)
    
    if((best_score == HIGHEST_SCORE) || (best_score == LOWEST_SCORE))
    {
      if(face_ptr->eno_used[poly_face_enos[2*forward_count+1]])
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Forward Face propagation is being forced upon a "
             << "used ENO point in Voxel ["<<voxel_idxs[DIM_X]<<"]["
             << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." <<endl
             << "Voxel has " << int(voxel_ptr->num_enos) << " points." <<endl;
        exit(1);
      }
      
      poly_dims[forward_count] = forward_dim;
      poly_sides[forward_count] = forward_side;
      face_ptr->eno_used[poly_face_enos[2*forward_count+0]] = TRUE;
      face_ptr->eno_used[poly_face_enos[2*forward_count+1]] = TRUE;
      eno_array[poly_idxs[forward_count+1]].used = TRUE;
      
      if(face_ptr->ambiguities)
      {
        face_ptr->ambiguities[poly_face_enos[2*forward_count+0]] =
          poly_face_enos[2*forward_count+1];
        face_ptr->ambiguities[poly_face_enos[2*forward_count+1]] =
          poly_face_enos[2*forward_count+0];
      }
      
      WaveElement new_element(poly_face_enos[2*forward_count+1],
                              poly_face_enos[2*forward_count+0],
                              prev_normal,
                              voxel_idxs,
                              poly_dims[forward_count],
                              poly_sides[forward_count]);
      if(new_element.IsValid(voxel_array, file_dimensions))
        wave_front.push_back(new_element);
      
      forward_count++;
#ifdef DEBUG_AMBIGUITY
      cout << "Added " << poly_idxs[forward_count] << " to front of poly"<<endl;
#endif
      break; // Signal to end search; leave loop.
    }
    else if(best_score == INIT_SCORE)
    {
      // If we didn't find any valid choice, it must be an error.
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "Forward trace failed in Voxel ["
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
      
#ifdef DEBUG_AMBIGUITY
      cout << "Backward Traversing Unambiguous Face with ENOs ";
      for(int dbi = 0; dbi < face_ptr->num_enos; dbi++)
        cout << face_ptr->eno_indexes[dbi] << " ";
      cout << endl;
#endif
      
      // NO AMBIGUITY -- Next point is the other one in this face
      check_idx = (face_ptr->eno_indexes[0] == poly_idxs[backward_count]);
      start_idx = !check_idx;
      poly_face_enos[2*(backward_count-1)+1] = start_idx;
      
      if(face_ptr->eno_used[check_idx])
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Face propagation is being forced upon a used ENO point in "
             << "Voxel [" << voxel_idxs[DIM_X] << "]["
             << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." << endl
             << "Voxel has " << int(voxel_ptr->num_enos) << " points." << endl;
        exit(1);
      }
      
      triangle_idxs[2] = face_ptr->eno_indexes[check_idx];
      check_score = Score(triangle_idxs, eno_coords, 
                          prev_normal, check_normal, TRUE);
      if(check_score > best_score)
      {
        best_score = check_score;
        trace_backward = TRUE;
        poly_idxs[backward_count-1] = face_ptr->eno_indexes[check_idx];
        poly_face_enos[2*(backward_count-1)+0] = check_idx;
        memcpy(next_normal, check_normal, NUM_DIMENSIONS*sizeof(double));
      }
    }
    else // (face_ptr->num_enos != 2)
    {
#ifdef DEBUG_AMBIGUITY
      cout << "Backward Traversing Face with ENOs ";
      for(int dbi = 0; dbi < face_ptr->num_enos; dbi++)
        cout << face_ptr->eno_indexes[dbi] <<(face_ptr->eno_used[dbi]?"* ":" ");
      cout << endl;
#endif
#ifdef DEBUG_RESOLVE
      if(face_ptr->ambiguities)
      {
        cout << "Ambiguities Already Resolved: " << endl;
        for(int dbi = 0; dbi < face_ptr->num_enos; dbi++)
          if(face_ptr->ambiguities[dbi] != -1)
            cout << face_ptr->eno_indexes[dbi] << " "
                 << face_ptr->eno_indexes[face_ptr->ambiguities[dbi]] << endl;
      }
#endif
      
      // Find the face index of poly_idxs[backward_count]
      start_idx = 0;
      while((face_ptr->eno_indexes[start_idx] != poly_idxs[backward_count]) &&
            (start_idx < face_ptr->num_enos))
        start_idx++;
      
      if(start_idx >= face_ptr->num_enos)
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "The " << DIMENSION_CHARS[poly_dims[backward_count]]
             << poly_sides[backward_count] << " face in this Voxel ["
             << voxel_idxs[DIM_X] << "][" << voxel_idxs[DIM_Y] << "]["
             << voxel_idxs[DIM_Z] << "] did not contain the ENO index "
             << poly_idxs[backward_count] << endl;
        
        exit(1);
      }
    
      poly_face_enos[2*(backward_count-1)+1] = start_idx;
      
      /*
       * Check to see if Ambiguitiy is already resolved.
       */
      
      if(face_ptr->ambiguities[start_idx] != -1)
      {
        check_idx = face_ptr->ambiguities[start_idx];
#ifdef DEBUG_RESOLVE
        cout << "Using Ambiguity Already Resolved: "
             << face_ptr->eno_indexes[start_idx] << " "
             << face_ptr->eno_indexes[check_idx] << endl;
#endif
        
        triangle_idxs[2] = face_ptr->eno_indexes[check_idx];
        check_score = Score(triangle_idxs, eno_coords, 
                            prev_normal, check_normal, TRUE);
        if(check_score > best_score)
        {
          best_score = check_score;
          trace_backward = TRUE;
          poly_idxs[backward_count-1] = face_ptr->eno_indexes[check_idx];
          poly_face_enos[2*(backward_count-1)+0] = check_idx;
          memcpy(next_normal, check_normal, NUM_DIMENSIONS*sizeof(double));
        }
      }
      else // (face_ptr->ambiguities[start_idx] == -1) -- Ambiguity unresolved
      {
        for(idx_offset = 1;
            (idx_offset < face_ptr->num_enos)&&(best_score < HIGHEST_SCORE);
            idx_offset+=2)
        {
          check_idx = (start_idx+idx_offset)%face_ptr->num_enos;
          
          // If this one's ambiguity is resolved (to something else),
          // you can't use it.
          if(face_ptr->ambiguities[check_idx] != -1)
            continue;
           
#ifdef DEBUG_AMBIGUITY
          cout << "Checking ENO " << face_ptr->eno_indexes[check_idx] << " -- "
               << start_idx << " " << idx_offset << " " << check_idx << endl;
#endif
          
          if(!face_ptr->eno_used[check_idx] &&
             (face_ptr->eno_indexes[check_idx] != poly_idxs[forward_count]))
          {
            triangle_idxs[2] = face_ptr->eno_indexes[check_idx];
            check_score = Score(triangle_idxs, eno_coords, 
                                prev_normal, check_normal, TRUE);
            if((check_score > best_score) &&
               ((face_ptr->num_enos < 6) ||
                CheckCrossings(face_ptr, forward_dim,
                               start_idx, check_idx, eno_coords)))
            {
              best_score = check_score;
              trace_backward = TRUE;
              poly_idxs[backward_count-1] = face_ptr->eno_indexes[check_idx];
              poly_face_enos[2*(backward_count-1)+0] = check_idx;
              memcpy(next_normal, check_normal, NUM_DIMENSIONS*sizeof(double));
            }
          }
        } // End for loop over every other eno in face
      } // End if Ambiguity Unresolved
    } // End if (face_ptr->num_enos != 2)
    
    if(best_score == INIT_SCORE)
    {
      // If we didn't find any valid choice, it must be an error.
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "Backward trace failed in Voxel ["
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
             << "used ENO point in Voxel ["<<voxel_idxs[DIM_X]<<"]["
             << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." <<endl
             << "Voxel has " << int(voxel_ptr->num_enos) << " points." <<endl;
        exit(1);
      }
      
      poly_dims[backward_count-1] = backward_dim;
      poly_sides[backward_count-1] = backward_side;
      face_ptr->eno_used[poly_face_enos[2*(backward_count-1)+0]] = TRUE;
      face_ptr->eno_used[poly_face_enos[2*(backward_count-1)+1]] = TRUE;
      eno_array[poly_idxs[backward_count-1]].used = TRUE;
      
      if(face_ptr->ambiguities)
      {
        face_ptr->ambiguities[poly_face_enos[2*(backward_count-1)+0]] =
          poly_face_enos[2*(backward_count-1)+1];
        face_ptr->ambiguities[poly_face_enos[2*(backward_count-1)+1]] =
          poly_face_enos[2*(backward_count-1)+0];
      }
      
      /*
       * Make Triangle
       */
      
      if((best_score < -1.1)) // || (best_score > 1.1))
      {
        cout.precision(4);
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Trying to make a triangle when there is no new triangle."<<endl
             << "During Backward Trace" << endl
             << "best_score = " << best_score << endl
             << "normal = " << next_normal[0] << " " << next_normal[1] << " "
             << next_normal[2] << endl
             << poly_idxs[backward_count] << " ("
             << eno_coords[3*poly_idxs[backward_count]+0] << ", "
             << eno_coords[3*poly_idxs[backward_count]+1] << ", "
             << eno_coords[3*poly_idxs[backward_count]+2] << ")" << endl
             << poly_idxs[forward_count] << " ("
             << eno_coords[3*poly_idxs[forward_count]+0] << ", "
             << eno_coords[3*poly_idxs[forward_count]+1] << ", "
             << eno_coords[3*poly_idxs[forward_count]+2] << ")" << endl
             << poly_idxs[backward_count-1] << " ("
             << eno_coords[3*poly_idxs[backward_count-1]+0] << ", "
             << eno_coords[3*poly_idxs[backward_count-1]+1] << ", "
             << eno_coords[3*poly_idxs[backward_count-1]+2] << ")" << endl;
        exit(1);
      }
      
      poly_list.push_back(poly_idxs[backward_count]);
      poly_list.push_back(poly_idxs[forward_count]);
      poly_list.push_back(poly_idxs[backward_count-1]);
      
      if((poly_idxs[backward_count] == poly_idxs[ forward_count  ]) ||
         (poly_idxs[backward_count] == poly_idxs[backward_count-1]) ||
         (poly_idxs[ forward_count] == poly_idxs[backward_count-1]))
      {
        cout.precision(4);
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Trying to make a triangle of two sides."<<endl
             << "During Backward Trace" << endl
             << "forward_count  = " << forward_count << endl
             << "backward_count = " << backward_count << endl
             << "best_score  = " << best_score << endl
             << "normal = " << next_normal[0] << " " << next_normal[1] << " "
             << next_normal[2] << endl
             << "Triangle Vertices:" << endl
             << poly_idxs[backward_count] << " ("
             << eno_coords[3*poly_idxs[backward_count]+0] << ", "
             << eno_coords[3*poly_idxs[backward_count]+1] << ", "
             << eno_coords[3*poly_idxs[backward_count]+2] << ")" << endl
             << poly_idxs[forward_count] << " ("
             << eno_coords[3*poly_idxs[forward_count]+0] << ", "
             << eno_coords[3*poly_idxs[forward_count]+1] << ", "
             << eno_coords[3*poly_idxs[forward_count]+2] << ")" << endl
             << poly_idxs[backward_count-1] << " ("
             << eno_coords[3*poly_idxs[backward_count-1]+0] << ", "
             << eno_coords[3*poly_idxs[backward_count-1]+1] << ", "
             << eno_coords[3*poly_idxs[backward_count-1]+2] << ")" << endl;
        cout << "Face Vertices:" << endl;
        for(unsigned int dbi = 0; dbi < face_ptr->num_enos; dbi++)
        {
          cout << face_ptr->eno_indexes[dbi] << " ("
               << eno_coords[3*face_ptr->eno_indexes[dbi]+0] << ", "
               << eno_coords[3*face_ptr->eno_indexes[dbi]+1] << ", "
               << eno_coords[3*face_ptr->eno_indexes[dbi]+2] << ") "
               << (face_ptr->eno_used[dbi] ? "USED" : "NOT USED") << endl;
        }
        //exit(1);
      }
      
      //GetTriangleNormal(eno_coords.begin(), poly_idxs, new_normal);
      normal_list.push_back(next_normal[0]);
      normal_list.push_back(next_normal[1]);
      normal_list.push_back(next_normal[2]);
      
      WaveElement new_element(poly_face_enos[2*(backward_count-1)+1],
                              poly_face_enos[2*(backward_count-1)+0],
                              next_normal,
                              voxel_idxs,
                              poly_dims[(backward_count-1)],
                              poly_sides[(backward_count-1)]);
      if(new_element.IsValid(voxel_array, file_dimensions))
        wave_front.push_back(new_element);
      
      backward_count--;
#ifdef DEBUG_AMBIGUITY
      cout << "Added " << poly_idxs[backward_count] << " to back of poly"<<endl;
#endif
    }
    else // tracing forward
    {
      face_ptr = voxel_ptr->faces[forward_dim][forward_side];
      if(face_ptr->eno_used[poly_face_enos[2*forward_count+1]])
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Forward Face propagation is being forced upon a "
             << "used ENO point in Voxel ["<<voxel_idxs[DIM_X]<<"]["
             << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]." <<endl
             << "Voxel has " << int(voxel_ptr->num_enos) << " points." <<endl;
        exit(1);
      }
      
      poly_dims[forward_count] = forward_dim;
      poly_sides[forward_count] = forward_side;
      face_ptr->eno_used[poly_face_enos[2*forward_count+0]] = TRUE;
      face_ptr->eno_used[poly_face_enos[2*forward_count+1]] = TRUE;
      eno_array[poly_idxs[forward_count+1]].used = TRUE;
      
      if(face_ptr->ambiguities)
      {
        face_ptr->ambiguities[poly_face_enos[2*forward_count+0]] =
          poly_face_enos[2*forward_count+1];
        face_ptr->ambiguities[poly_face_enos[2*forward_count+1]] =
          poly_face_enos[2*forward_count+0];
      }
      
      /*
       * Make Triangle
       */
      
      if((best_score < -1.1)) // || (best_score > 1.1))
      {
        cout.precision(4);
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Trying to make a triangle when there is no new triangle."<<endl
             << "During Forward Trace" << endl
             << "best_score = " << best_score << endl
             << "normal = " << next_normal[0] << " " << next_normal[1] << " "
             << next_normal[2] << endl
             << poly_idxs[backward_count] << " ("
             << eno_coords[3*poly_idxs[backward_count]+0] << ", "
             << eno_coords[3*poly_idxs[backward_count]+1] << ", "
             << eno_coords[3*poly_idxs[backward_count]+2] << ")" << endl
             << poly_idxs[forward_count] << " ("
             << eno_coords[3*poly_idxs[forward_count]+0] << ", "
             << eno_coords[3*poly_idxs[forward_count]+1] << ", "
             << eno_coords[3*poly_idxs[forward_count]+2] << ")" << endl
             << poly_idxs[forward_count+1] << " ("
             << eno_coords[3*poly_idxs[forward_count+1]+0] << ", "
             << eno_coords[3*poly_idxs[forward_count+1]+1] << ", "
             << eno_coords[3*poly_idxs[forward_count+1]+2] << ")" << endl;
        //exit(1);
        exit(1);
      }
      
      poly_list.push_back(poly_idxs[backward_count]);
      poly_list.push_back(poly_idxs[forward_count]);
      poly_list.push_back(poly_idxs[forward_count+1]);
      
      if((poly_idxs[backward_count] == poly_idxs[forward_count  ]) ||
         (poly_idxs[backward_count] == poly_idxs[forward_count+1]) ||
         (poly_idxs[ forward_count] == poly_idxs[forward_count+1]))
      {
        cout.precision(4);
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Trying to make a triangle of two sides."<<endl
             << "During Forward Trace" << endl
             << "forward_count  = " << forward_count << endl
             << "backward_count = " << backward_count << endl
             << "best_score  = " << best_score << endl
             << "normal = " << next_normal[0] << " " << next_normal[1] << " "
             << next_normal[2] << endl
             << poly_idxs[backward_count] << " ("
             << eno_coords[3*poly_idxs[backward_count]+0] << ", "
             << eno_coords[3*poly_idxs[backward_count]+1] << ", "
             << eno_coords[3*poly_idxs[backward_count]+2] << ")" << endl
             << poly_idxs[forward_count] << " ("
             << eno_coords[3*poly_idxs[forward_count]+0] << ", "
             << eno_coords[3*poly_idxs[forward_count]+1] << ", "
             << eno_coords[3*poly_idxs[forward_count]+2] << ")" << endl
             << poly_idxs[forward_count+1] << " ("
             << eno_coords[3*poly_idxs[forward_count+1]+0] << ", "
             << eno_coords[3*poly_idxs[forward_count+1]+1] << ", "
             << eno_coords[3*poly_idxs[forward_count+1]+2] << ")" << endl;
        cout << "Face Vertices:" << endl;
        for(unsigned int dbi = 0; dbi < face_ptr->num_enos; dbi++)
        {
          cout << face_ptr->eno_indexes[dbi] << " ("
               << eno_coords[3*face_ptr->eno_indexes[dbi]+0] << ", "
               << eno_coords[3*face_ptr->eno_indexes[dbi]+1] << ", "
               << eno_coords[3*face_ptr->eno_indexes[dbi]+2] << ") "
               << (face_ptr->eno_used[dbi] ? "USED" : "NOT USED") << endl;
        }
        //exit(1);
      }
      
      //GetTriangleNormal(eno_coords.begin(), poly_idxs, new_normal);
      normal_list.push_back(next_normal[0]);
      normal_list.push_back(next_normal[1]);
      normal_list.push_back(next_normal[2]);
      
      WaveElement new_element(poly_face_enos[2*forward_count+1],
                              poly_face_enos[2*forward_count+0],
                              next_normal,
                              voxel_idxs,
                              poly_dims[forward_count],
                              poly_sides[forward_count]);
      if(new_element.IsValid(voxel_array, file_dimensions))
        wave_front.push_back(new_element);
      
      forward_count++;
      
#ifdef DEBUG_AMBIGUITY
      cout << "Added " << poly_idxs[forward_count] << " to front of poly"<<endl;
#endif
    }
  } // end while(poly_idxs[forward_count] != poly_idxs[backward_count])
    // (While the polygon isn't closed)
  
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
  
  if(forward_count > MAX_POINTS_PER_AMB_POLY/2)
  {
    cout << "INFO in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << forward_count << "-sided polygon generated." << endl;
  }
  
#ifdef DEBUG_AMBIGUITY
  cout << "Completed Polygon ";
  for(int dbi = 0; dbi < forward_count; dbi++)
    cout << poly_idxs[dbi] << " ";
  cout << endl;
#endif
}




/******************************************************************************/

inline double Score(const unsigned int triangle_idxs[NUM_DIMENSIONS],
                    const vector<double> & eno_coords,
                    const double prev_normal[NUM_DIMENSIONS],
                    double check_normal[NUM_DIMENSIONS],
                    const bool backward_flag)
{
  // No need to check since this is the first option and is always caught
  GetTriangleNormal(&*eno_coords.begin(), triangle_idxs, check_normal);
#ifdef CURVATURE_USING_DISTANCE
  double distance;
  if(backward_flag)
    distance = Distance(&eno_coords[NUM_DIMENSIONS*triangle_idxs[0]],
                        &eno_coords[NUM_DIMENSIONS*triangle_idxs[2]]);
  else
    distance = Distance(&eno_coords[NUM_DIMENSIONS*triangle_idxs[1]],
                        &eno_coords[NUM_DIMENSIONS*triangle_idxs[2]]);
  
  // Clamp distance from below to prevent bad situations
  //if(distance < 0.1)
  //  distance = 0.1;
  
  // Assume that the normals are already unit length.
  double dot_prod = DotProduct(prev_normal, check_normal);
  double angle = acos(dot_prod);

  // INVERSE CURVATURE SCORE -- distance used
  double score;
  if(dot_prod + FLOAT_ERROR_MARGIN >= 1.0)
    score = DBL_MAX/10;
  else
    score = distance / (2*sin(angle/2.0));
    // score = distance / angle;
  
#ifdef DEBUG_AMBIGUITY
  //if((score != DBL_MAX/10) && (score > 1e+200))
    cout << "Triangle " << triangle_idxs[0] << " " << triangle_idxs[1]
         << " " << triangle_idxs[2] << " has normal: " << check_normal[0]
         << " " << check_normal[1] << " " << check_normal[2] << endl
         << "Previous normal: " << prev_normal[0]
         << " " << prev_normal[1] << " " << prev_normal[2] << endl
         << "Dot Product: " << dot_prod << endl
         << "Angle: " << angle << endl
         << "Distance: " << distance << endl
         << "Score: " << score << endl;
#endif
  
  return score;

#else

#ifdef DEBUG_AMBIGUITY
   cout << "Triangle " << triangle_idxs[0] << " " << triangle_idxs[1]
        << " " << triangle_idxs[2] << " has normal " << check_normal[0]
        << " " << check_normal[1] << " " << check_normal[2] << endl;
#endif
  
  // NORMAL DOT PRODUCT SCORE -- no distance used
  return DotProduct(prev_normal, check_normal);
#endif   
}




/******************************************************************************/

inline bool CheckCrossings(const Face * face_ptr,
                           const unsigned int face_dim,
                           const unsigned int in_eno0,
                           const unsigned int in_eno1,
                           const vector<double> & eno_coords)
{
  bool no_crossings = TRUE;
  unsigned int dim_a = (face_dim+1)%NUM_DIMENSIONS;
  unsigned int dim_b = (face_dim+2)%NUM_DIMENSIONS;
  
  unsigned int check_eno0, check_eno1;
  unsigned int Pa, Pb, P0, P1;
  Pa = NUM_DIMENSIONS*face_ptr->eno_indexes[in_eno0];
  Pb = NUM_DIMENSIONS*face_ptr->eno_indexes[in_eno1];
  
  unsigned int search_start, search_end;
  if(in_eno0 < in_eno1)
  {
    search_start = in_eno0+1;
    search_end = in_eno1;
  }
  else
  {
    search_start = in_eno1+1;
    search_end = in_eno0;
  }
  
  for(check_eno0 = search_start; check_eno0 < search_end; check_eno0++)
  {
    if(face_ptr->ambiguities[check_eno0] != -1)
    {
      check_eno1 = face_ptr->ambiguities[check_eno0];
      P0 = NUM_DIMENSIONS*face_ptr->eno_indexes[check_eno0];
      P1 = NUM_DIMENSIONS*face_ptr->eno_indexes[check_eno1];
      no_crossings &=
        SegmentsDoNotCross(eno_coords[P0+dim_a], eno_coords[P0+dim_b],
                           eno_coords[P1+dim_a], eno_coords[P1+dim_b],
                           eno_coords[Pa+dim_a], eno_coords[Pa+dim_b],
                           eno_coords[Pb+dim_a], eno_coords[Pb+dim_b]);
    }
  }
  
  return no_crossings;
}




/******************************************************************************/

inline bool SegmentsDoNotCross(const double x0, const double y0,
                               const double x1, const double y1,
                               const double xa, const double ya,
                               const double xb, const double yb)
{
  /*
   * This calculation works on the basis of 2D cross-product orientation.
   * The segment Pa(xa, ya) -- Pb(xb, yb) intersects the line
   * P0(x0, y0) -- P1(x1, y1) when (xa, ya) and (xb, yb) are on opposite
   * sides of the line.  The sign of the 2D cross product (P-P0) X (P1-P0) 
   * identifies which side of the line P0--P1 the point P is on.
   */
  bool no_cross = ((((ya-y0)*(x1-x0)-(xa-x0)*(y1-y0)) *
                    ((yb-y0)*(x1-x0)-(xb-x0)*(y1-y0))) > 0);
#ifdef DEBUG19
  if(!no_cross)
    cout << "INFO in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << "Found a Segment Crossing." << endl;
#endif
  return no_cross;
}
                              
