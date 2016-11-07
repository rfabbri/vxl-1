#include <stdlib.h>

#include <iostream>

using namespace std;

#include "surface_tracer.h"

/******************************************************************************/

Voxel *** MakeVoxels(SpaceUnit *** space_array,
                     const unsigned int file_dimensions[NUM_DIMENSIONS])
{
  /*
   * Allocate and Initialize Voxel Array
   * single block of data with pointer blocks to allow [x][y][z] 3D referencing.
   */
  
  cout << "Allocating Voxel Array: ";
  //cout << "Allocating " << file_dimensions[DIM_X]*sizeof(Voxel**)
  //     << " bytes..." << endl;
  Voxel *** voxel_array = new Voxel**[file_dimensions[DIM_X]];
  //if(voxel_array == 0)
  //  cout << "Failed voxel_array allocation" << endl;
  //cout << "Allocating " << file_dimensions[DIM_X]*file_dimensions[DIM_Y]*
  //                         sizeof(Voxel*) << " bytes..." << endl;
  voxel_array[0] = new Voxel*[file_dimensions[DIM_X]*file_dimensions[DIM_Y]];
  //if(voxel_array[0] == 0)
  //  cout << "Failed voxel_array[0] allocation" << endl;
  cout << file_dimensions[DIM_X]*file_dimensions[DIM_Y]*file_dimensions[DIM_Z]*
          sizeof(Voxel) << " bytes..." << endl;
  voxel_array[0][0] = new Voxel[file_dimensions[DIM_X]*
                                file_dimensions[DIM_Y]*
                                file_dimensions[DIM_Z]];
  if(voxel_array[0][0] == 0)
    cout << "Failed voxel_array[0][0] allocation" << endl;
  //cout << "Allocation successful" << endl;
  
  int i, j, k;
  Voxel * this_voxel;
  int face_alloc_count = 0;
  
  for(i = 0; i < signed(file_dimensions[DIM_X]); i++)
  {
    voxel_array[i] = voxel_array[0] + i*file_dimensions[DIM_Y];
    
    for(j = 0; j < signed(file_dimensions[DIM_Y]); j++)
    {
      voxel_array[i][j] = voxel_array[0][0]
                          + (i*file_dimensions[DIM_Y]+j)*file_dimensions[DIM_Z];
      
      for(k = 0; k < signed(file_dimensions[DIM_Z]); k++)
      {
        //voxel_array[i][j][k].faces = 0;

        voxel_array[i][j][k].faces[DIM_X][0] = 0;
        voxel_array[i][j][k].faces[DIM_Y][0] = 0;
        voxel_array[i][j][k].faces[DIM_Z][0] = 0;
        voxel_array[i][j][k].faces[DIM_X][1] = 0;
        voxel_array[i][j][k].faces[DIM_Y][1] = 0;
        voxel_array[i][j][k].faces[DIM_Z][1] = 0;
        
        voxel_array[i][j][k].ambiguous = 0;
      }
    }
  }
  
  /*
   * Add ENO points to face in CCW order, starting at the "bottom."
   * Set up X-faces, Y-faces, and Z-faces.
   * 
   * Note 1: X-faces are perpendicular to the X-axis; similarly for Y and Z.
   *
   * Note 2: This convention is used for visualizing the X, Y, and Z axes:
   *               Z   
   *               |   
   *               |__Y
   *               /   
   *             X/    
   *
   *         The "bottom" edge of X-faces is the Y-edge on the Y-axis.
   *         Similarly, by rotating these axes, the "bottom" edge of Y-faces is
   *         the Z-edge, and the "bottom" edge of Z-faces is the X-edge.
   *
   * Note 3: Invalid ENO points have negative indexes (set in ReadENOFiles())
   *
   * Note 4: Each face is used in two Voxels, and each voxel has its own copy.
   *         In order to set this up efficiently, set up the borders first and
   *         fill the face space from the end to the beginning.
   */

  cout << "Allocating Faces..." << endl;

  // Use temps to prevent redundant evaluations
  const int x_dim_minus_1 = file_dimensions[DIM_X]-1;
  const int y_dim_minus_1 = file_dimensions[DIM_Y]-1;
  const int z_dim_minus_1 = file_dimensions[DIM_Z]-1;
  
  unsigned int eno_count;
  Face * new_face;
  
  // Faces on X-border of space
  for(j = 0; j < y_dim_minus_1; j++)
  {
    for(k = 0; k < z_dim_minus_1; k++)
    {
      this_voxel = &voxel_array[x_dim_minus_1][j][k];
      new_face = new Face;
      eno_count = new_face->SetENOPoints(&space_array[x_dim_minus_1][j][k],
                                         &space_array[x_dim_minus_1][j+1][k],
                                         &space_array[x_dim_minus_1][j][k+1],
                                         DIM_Y,
                                         DIM_Z);
      
      if(eno_count > 0)
      {
        this_voxel->faces[DIM_X][0] = new_face;
        face_alloc_count++;
        if(eno_count > 2)
          this_voxel->ambiguous = 1;
      }
      else
        delete(new_face);
      
      if(eno_count % 2)
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "X-Face [END]["<<j<<"]["<<k<<"] has odd number of ENO points: "
             << eno_count << endl << "First ENO index: "
             << this_voxel->faces[DIM_X][0]->eno_indexes[0]
             << endl;
        exit(1);
      }
      
      this_voxel->ambiguous = (eno_count > 2);
      this_voxel->num_enos = eno_count;
    }
  }

  // Faces on Y-border of space
  for(k = 0; k < z_dim_minus_1; k++)
  {
    for(i = 0; i < x_dim_minus_1; i++)
    {
      this_voxel = &voxel_array[i][y_dim_minus_1][k];
      new_face = new Face;
      eno_count = new_face->SetENOPoints(&space_array[i][y_dim_minus_1][k],
                                         &space_array[i][y_dim_minus_1][k+1],
                                         &space_array[i+1][y_dim_minus_1][k],
                                         DIM_Z,
                                         DIM_X);
      
      if(eno_count > 0)
      {
        this_voxel->faces[DIM_Y][0] = new_face;
        face_alloc_count++;
        if(eno_count > 2)
          this_voxel->ambiguous = 1;
      }
      else
        delete(new_face);
      
      if(eno_count % 2)
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Y-Face ["<<i<<"][END]["<<k<<"] has odd number of ENO points: "
             << eno_count << endl << "First ENO index: "
             << this_voxel->faces[DIM_Y][0]->eno_indexes[0]
             << endl;
        exit(1);
      }
      
      this_voxel->ambiguous = (eno_count > 2);
      this_voxel->num_enos = eno_count;
    }
  }

  // Faces on Z-border of space
  for(i = 0; i < x_dim_minus_1; i++)
  {
    for(j = 0; j < y_dim_minus_1; j++)
    {
      this_voxel = &voxel_array[i][j][z_dim_minus_1];
      new_face = new Face;
      eno_count = new_face->SetENOPoints(&space_array[i][j][z_dim_minus_1],
                                         &space_array[i+1][j][z_dim_minus_1],
                                         &space_array[i][j+1][z_dim_minus_1],
                                         DIM_X,
                                         DIM_Y);
      
      if(eno_count > 0)
      {
        this_voxel->faces[DIM_Z][0] = new_face;
        face_alloc_count++;
        if(eno_count > 2)
          this_voxel->ambiguous = 1;
      }
      else
        delete(new_face);
      
      if(eno_count % 2)
      {
        cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
             << "Z-Face ["<<i<<"]["<<j<<"][END] has odd number of ENO points: "
             << eno_count << endl << "First ENO index: "
             << this_voxel->faces[DIM_Z][0]->eno_indexes[0]
             << endl;
        exit(1);
      }
      
      this_voxel->ambiguous = (eno_count > 2);
      this_voxel->num_enos = eno_count;
    }
  }
  
  // All other Faces -- set up and make compies
  for(i = file_dimensions[DIM_X]-2; i >= 0; i--)
  {
    for(j = file_dimensions[DIM_Y]-2; j >= 0; j--)
    {
      for(k = file_dimensions[DIM_Z]-2; k >= 0; k--)
      {
        this_voxel = &voxel_array[i][j][k];
        // X-Face
        new_face = new Face;
        eno_count = new_face->SetENOPoints(&space_array[i][j][k],
                                           &space_array[i][j+1][k],
                                           &space_array[i][j][k+1],
                                           DIM_Y,
                                           DIM_Z);
        
        if(eno_count > 0)
        {
          this_voxel->faces[DIM_X][0] = new_face;
          face_alloc_count++;
          if(eno_count > 2)
            this_voxel->ambiguous = 1;
        }
        else
          delete(new_face);
        
        if(eno_count % 2)
        {
          cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
               << "X-Face ["<<i<<"]["<<j<<"]["<<k<<"] has an odd number of ENO "
               << "points: " << eno_count << endl << "First ENO index: "
               << this_voxel->faces[DIM_X][0]->eno_indexes[0] << endl;
          exit(1);
        }
        
        // Y-Face
        new_face = new Face;
        eno_count = new_face->SetENOPoints(&space_array[i][j][k],
                                           &space_array[i][j][k+1],
                                           &space_array[i+1][j][k],
                                           DIM_Z,
                                           DIM_X);
        
        if(eno_count > 0)
        {
          this_voxel->faces[DIM_Y][0] = new_face;
          face_alloc_count++;
          if(eno_count > 2)
            this_voxel->ambiguous = 1;
        }
        else
          delete(new_face);
        
        if(eno_count % 2)
        {
          cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
               << "Y-Face ["<<i<<"]["<<j<<"]["<<k<<"] has an odd number of ENO "
               << "points: " << this_voxel->faces[DIM_Y][0]->num_enos 
               << endl << "First ENO index: "
               << this_voxel->faces[DIM_Y][0]->eno_indexes[0] << endl;
          exit(1);
        }
        
        // Z-Face
        new_face = new Face;
        eno_count = new_face->SetENOPoints(&space_array[i][j][k],
                                           &space_array[i+1][j][k],
                                           &space_array[i][j+1][k],
                                           DIM_X,
                                           DIM_Y);
        
        if(eno_count > 0)
        {
          this_voxel->faces[DIM_Z][0] = new_face;
          face_alloc_count++;
          if(eno_count > 2)
            this_voxel->ambiguous = 1;
        }
        else
          delete(new_face);
        
        if(eno_count % 2)
        {
          cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
               << "Z-Face ["<<i<<"]["<<j<<"]["<<k<<"] has an odd number of ENO "
               << "points: " << this_voxel->faces[DIM_Z][0]->num_enos
               << endl << "First ENO index: "
               << this_voxel->faces[DIM_Z][0]->eno_indexes[0] << endl;
          exit(1);
        }
        
        /*
         * Copy Faces from adjacent voxels.
         * "Far" [1] faces coincide with the "near" [0] faces on the adjacent
         * voxel with higher index.  These voxels were calculated on the last
         * loop iteration (or when the border was calculated)
         */
        
        // NOTE: Copies the eno_indexes pointer in each face, not the actual mem
        if(voxel_array[i+1][j][k].faces[DIM_X][0])
        {
          //this_voxel->faces[DIM_X][1] = voxel_array[i+1][j][k].faces[DIM_X][0];
          this_voxel->faces[DIM_X][1] = new Face;
          memcpy(this_voxel->faces[DIM_X][1],
                 voxel_array[i+1][j][k].faces[DIM_X][0],
                 sizeof(Face));
          if(this_voxel->faces[DIM_X][1]->num_enos > 2)
            this_voxel->ambiguous = 1;
          face_alloc_count++;
        }
        
        if(voxel_array[i][j+1][k].faces[DIM_Y][0])
        {
          //this_voxel->faces[DIM_Y][1] = voxel_array[i][j+1][k].faces[DIM_Y][0];
          this_voxel->faces[DIM_Y][1] = new Face;
          memcpy(this_voxel->faces[DIM_Y][1],
                 voxel_array[i][j+1][k].faces[DIM_Y][0],
                 sizeof(Face));
          if(this_voxel->faces[DIM_Y][1]->num_enos > 2)
            this_voxel->ambiguous = 1;
          face_alloc_count++;
        }

        if(voxel_array[i][j][k+1].faces[DIM_Z][0])
        {
          //this_voxel->faces[DIM_Z][1] = voxel_array[i][j][k+1].faces[DIM_Z][0];
          this_voxel->faces[DIM_Z][1] = new Face;
          memcpy(this_voxel->faces[DIM_Z][1],
                 voxel_array[i][j][k+1].faces[DIM_Z][0],
                 sizeof(Face));
          if(this_voxel->faces[DIM_Z][1]->num_enos > 2)
            this_voxel->ambiguous = 1;
          face_alloc_count++;
        }
        
        /*
         ******
         ****** Is ENO count for Voxels necessary??? ******
         ******
         */
        
        // Count up number of ENO points in this Voxel
        this_voxel->num_enos = space_array[i][j][k].num_enos[0] +
                               space_array[i][j][k].num_enos[1] +
                               space_array[i][j][k].num_enos[2] +
                               space_array[i+1][j][k].num_enos[1] +
                               space_array[i+1][j][k].num_enos[2] +
                               space_array[i][j+1][k].num_enos[0] +
                               space_array[i][j+1][k].num_enos[2] +
                               space_array[i][j][k+1].num_enos[0] +
                               space_array[i][j][k+1].num_enos[1] +
                               space_array[i+1][j+1][k].num_enos[2] +
                               space_array[i][j+1][k+1].num_enos[0] +
                               space_array[i+1][j][k+1].num_enos[1];
        
/*
        // Determine Ambiguity
        this_voxel->ambiguous = ((this_voxel->faces[DIM_X][0]->num_enos > 2) ||
                                 (this_voxel->faces[DIM_X][1]->num_enos > 2) ||
                                 (this_voxel->faces[DIM_Y][0]->num_enos > 2) ||
                                 (this_voxel->faces[DIM_Y][1]->num_enos > 2) ||
                                 (this_voxel->faces[DIM_Z][0]->num_enos > 2) ||
                                 (this_voxel->faces[DIM_Z][1]->num_enos > 2));
*/
//#ifdef SKIP_AMBIGUITY
        if(!this_voxel->ambiguous)
        {
          if((space_array[i][j][k].num_enos[0] > 1) ||
             (space_array[i][j][k].num_enos[1] > 1) ||
             (space_array[i][j][k].num_enos[2] > 1) ||
             (space_array[i+1][j][k].num_enos[1] > 1) ||
             (space_array[i+1][j][k].num_enos[2] > 1) ||
             (space_array[i][j+1][k].num_enos[0] > 1) ||
             (space_array[i][j+1][k].num_enos[2] > 1) ||
             (space_array[i][j][k+1].num_enos[0] > 1) ||
             (space_array[i][j][k+1].num_enos[1] > 1) ||
             (space_array[i+1][j+1][k].num_enos[2] > 1) ||
             (space_array[i][j+1][k+1].num_enos[0] > 1) ||
             (space_array[i+1][j][k+1].num_enos[1] > 1))
            this_voxel->ambiguous = 1;
        }
//#endif
      }
    }
  }

  cout << face_alloc_count << " Faces allocated; "
       << face_alloc_count*sizeof(Face) << " bytes." << endl;
  
  return voxel_array;
}




/******************************************************************************/

/*
void FreeVoxels(Voxel *** voxel_array,
                const unsigned int file_dimensions[NUM_DIMENSIONS])
{
  unsigned int i, j, k;
  for(i = 0; i < file_dimensions[DIM_X]; i++)
  {
    for(j = 0; j < file_dimensions[DIM_Y]; j++)
    {
      for(k = 0; k < file_dimensions[DIM_Z]; k++)
      {
        delete(&voxel_array[i][j][k]); // BAD WAY TO DELETE
      }
    }
  }
  delete[](voxel_array[0][0]);
  delete[](voxel_array[0]);  
  delete[](voxel_array);
}
*/  




/******************************************************************************/
