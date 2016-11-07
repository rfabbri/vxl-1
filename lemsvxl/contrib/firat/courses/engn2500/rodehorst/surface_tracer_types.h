/*******************************************************************************
* 
*******************************************************************************/

#ifndef SURFACE_TRACER_TYPES_H
#define SURFACE_TRACER_TYPES_H

/******************************************************************************/
/* Libraries */

#include <cstdlib>
#include <cstring>

#include <iostream>
using namespace std;

#include "surface_tracer_consts.h"

/******************************************************************************/
/* SpaceUnit Struct -- basic unit of 3D space array */

struct SpaceUnit
{
/*
 ******
 ****** Is border boolean necessary??? ******
 ******
 */
  // true IFF you cannot propagate through this voxel
  bool border;
  
/*
 ******
 ****** Faster with int instead of char??? ******
 ******
 */
  // number of ENO points along each dimension (0, 1, or 2)
  char num_enos[NUM_DIMENSIONS];
  
  // Index of ENO points into the ENO point vector
  int eno_indexes[NUM_DIMENSIONS][MAX_ENOS_PER_EDGE]; // 
};




/******************************************************************************/
/* Face Struct -- represents a face of a cube of space */

struct Face
{
  // Stores ENO point idxs on this face in CCW order, starting with the "bottom"
  unsigned int * eno_indexes; //[MAX_ENOS_PER_FACE];
  
  signed char * ambiguities;
  
  // For each stored ENO point, which have been used in the surface?
  bool eno_used[MAX_ENOS_PER_FACE];

/*
 ******
 ****** Faster with int instead of char??? ******
 ******
 */
  // number of ENO points stored in eno_indexes
  char num_enos;
  
  // Add ENO points to face in CCW order, starting at edge 1 of this voxel
  // Inlined for efficiency
  int SetENOPoints(const SpaceUnit * this_unit,
                   const SpaceUnit * next_unit1,
                   const SpaceUnit * next_unit2,
                   const unsigned int edge1,
                   const unsigned int edge2)
  {
    unsigned int temp_idxs[MAX_ENOS_PER_FACE];
    eno_indexes = 0;
    num_enos = 0;
    
    if(this_unit->num_enos[edge1])
    {
      temp_idxs[num_enos++] = this_unit->eno_indexes[edge1][0];
      if(this_unit->num_enos[edge1] == 2)
        temp_idxs[num_enos++] = this_unit->eno_indexes[edge1][1];
    }
    if(next_unit1->num_enos[edge2])
    {
      temp_idxs[num_enos++] = next_unit1->eno_indexes[edge2][0];
      if(next_unit1->num_enos[edge2] == 2)
        temp_idxs[num_enos++] = next_unit1->eno_indexes[edge2][1];
    }
    if(next_unit2->num_enos[edge1])
    {
      if(next_unit2->num_enos[edge1] == 2)
        temp_idxs[num_enos++] = next_unit2->eno_indexes[edge1][1];
      temp_idxs[num_enos++] = next_unit2->eno_indexes[edge1][0];
    }
    if(this_unit->num_enos[edge2])
    {
      if(this_unit->num_enos[edge2] == 2)
        temp_idxs[num_enos++] = this_unit->eno_indexes[edge2][1];
      temp_idxs[num_enos++] = this_unit->eno_indexes[edge2][0];
    }
    
    if(num_enos > 0)
    {
      eno_indexes = new unsigned int[num_enos];
      for(int i = 0; i < num_enos; i++)
      {
        eno_used[i] = FALSE;
        eno_indexes[i] = temp_idxs[i];
      }
    }
    
    if(num_enos > 2)
    {
      ambiguities = new signed char[num_enos];
      
      if(!ambiguities)
        cout << "FAILED TO ALLOCATE AMBIGUITIES, size " << num_enos << endl;
      
      for(int i = 0; i < num_enos; i++)
      {
        ambiguities[i] = -1;
      }
    }
    else
      ambiguities = 0;
    
/*
    if(num_enos > MAX_ENOS_PER_FACE)
    {
      cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "Found more than the maximum allowed " << MAX_ENOS_PER_FACE
           << " ENOs in this face: " << num_enos << endl;
      exit(1);
    }
*/    
    return num_enos;
  }
  
  /*
   * Destructor deliberately does nothing; the dynamic eno_indexes pointer is
   * not the only pointer to its memory chunk.
   */
  ~Face() {}
  
};




/******************************************************************************/
/* Voxel Struct -- contains 6 faces and some other information */

struct Voxel
{
  // 6 faces of the cube, containing ENO point information
  Face * faces[NUM_DIMENSIONS][FACES_PER_DIMENSION];

/*
 ******
 ****** Is ENO count for Voxels necessary??? ******
 ******
 */
/*
 ******
 ****** Faster with int instead of char??? ******
 ******
 */
  // How many ENO points are in this whole Voxel
  char num_enos;
  
  // Whether this Voxel can be polygonized in different ways
  bool ambiguous;

  ~Voxel()
  {
    /*
     * NOTE: The dynamic eno_indexes pointer is not the only pointer to its
     * memory chunk.  Two coinciding faces share the same chunk.  So, only delete
     * the eno_indexes on the 0-sides to avoid deleting twice.
     */
    if(faces[DIM_X][0])
    {
      delete(faces[DIM_X][0]->eno_indexes);
      delete(faces[DIM_X][0]->ambiguities);
      delete(faces[DIM_X][0]);
    }
    if(faces[DIM_Y][0])
    {
      delete(faces[DIM_Y][0]->eno_indexes);
      delete(faces[DIM_Y][0]->ambiguities);
      delete(faces[DIM_Y][0]);
    }
    if(faces[DIM_Z][0])
    {
      delete(faces[DIM_Z][0]->eno_indexes);
      delete(faces[DIM_Z][0]->ambiguities);
      delete(faces[DIM_Z][0]);
    }
    if(faces[DIM_X][1])
      delete(faces[DIM_X][1]);
    if(faces[DIM_Y][1])
      delete(faces[DIM_Y][1]);
    if(faces[DIM_Z][1])
      delete(faces[DIM_Z][1]);
  }
};




/******************************************************************************/
/* EnoPoint struct */

struct EnoPoint
{
  // x, y, z indexes of the voxel of the point
  unsigned int indexes[NUM_DIMENSIONS];
  
/*
 ******
 ****** Faster with int instead of char??? ******
 ******
 */
  // which axis the point's grid line is parallel to
  char dim_index;
  
  // Has this ENO point been used in the surface yet?
  bool used;
};




/******************************************************************************/
/* WaveElement struct */

struct WaveElement
{
/*
 ******
 ****** Faster with int instead of char??? ******
 ******
 */
  // Indexes of ENO points in this wave edge into the eno arrays of the Face
  // which the wave is traversing
  char eno0;
  char eno1;
  
  // Pointer to normal vector of the polygon which this wave element came from;
  // Used to resolve ambiguities by choosing the next polygon as the option with
  // the closest normal to this (by angle).
  double normal[NUM_DIMENSIONS];
  
  // Indexes of the Voxel where the propogation will continue
  unsigned int voxel_idxs[NUM_DIMENSIONS];
  
/*
 ******
 ****** Faster with int instead of char??? ******
 ******
 */
  // Identifiers of the Face where propogation in this Voxel begins
  char face_dim;
  char face_side;
  
  // Default Constructor
  WaveElement() {}
  
  // Complete constructor
  WaveElement(const unsigned int in_eno0, // idxs into eno arrays of face
              const unsigned int in_eno1,
              double from_normal[NUM_DIMENSIONS], // normal of "from" polygon
              const unsigned int from_voxel_idxs[NUM_DIMENSIONS],
              const unsigned int from_dim,  // same dim for next Voxel
              const unsigned int from_side) // opposite side for next Voxel
    : eno0(in_eno0), eno1(in_eno1), face_dim(from_dim), face_side(!from_side)
  {
    memcpy(normal, from_normal, NUM_DIMENSIONS*sizeof(double));
    memcpy(voxel_idxs, from_voxel_idxs, NUM_DIMENSIONS*sizeof(unsigned int));
    
    // in_voxel_idxs are the "from" Voxel's indexes; change to new Voxel's idxs
    // if(side == 0), subtract 1 from this dim's index; if(side == 1), add 1
    voxel_idxs[from_dim] += 2*from_side - 1;
  }


  bool IsValid(Voxel *** voxel_array,
               const unsigned int file_dimensions[NUM_DIMENSIONS])
  {
#ifdef DEBUG10
    if((voxel_idxs[DIM_X] == 63) &&
       (voxel_idxs[DIM_Y] == 61) &&
       (voxel_idxs[DIM_Z] == 154))
    {
      cout.precision(4);
      cout << "face_dim: " << int(face_dim) << "\tface_side: "
           << int(face_side) << "\tnormal: " << normal[0] << " " << normal[1]
           << " " << normal[2] << endl;
    }
#endif
      
    if(voxel_idxs[face_dim] < file_dimensions[face_dim]-1)
    {
      Face * face_ptr = voxel_array[voxel_idxs[DIM_X]]
                                   [voxel_idxs[DIM_Y]]
                                   [voxel_idxs[DIM_Z]].
                                   faces[face_dim][face_side];
      
      if(face_ptr->eno_used[eno0] !=
         face_ptr->eno_used[eno1])
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Found a partially used edge!  The surface has apparently"<<endl
             << "developed a discontinuity!" << endl;
        //return TRUE;
        cin.get();
        //exit(1);
      }
      
      //int eno_idx_diff =face_ptr->eno_indexes[eno0]-face_ptr->eno_indexes[eno1];
      if(!face_ptr->eno_used[eno0])
      {
        //if((eno_idx_diff != 1) && (eno_idx_diff != -1))
          return TRUE;
        //else
        //  cout << "INFO in " << __FILE__ << ":" << __LINE__ << ":" << endl
        //       << "Invalidating wave element with points on the same edge."
        //       << endl;
      }
    }
    
    return FALSE;
  }
  
/*
  void SetUsed(Voxel *** voxel_array)
  {
    Voxel * voxel_ptr = &voxel_array[voxel_idxs[DIM_X]]
                                    [voxel_idxs[DIM_Y]]
                                    [voxel_idxs[DIM_Z]];
    voxel_ptr->faces[face_dim][face_side]->eno_used[eno0] = TRUE;
    voxel_ptr->faces[face_dim][face_side]->eno_used[eno1] = TRUE;
  }
*/
};




#endif
