#include <stdlib.h>

#include <vcl_iostream.h>
#include <vector>
#include <deque>

using namespace std;

#include "surface_tracer.h"

#define DEBUG15

/******************************************************************************/
/* Function Prototypes for just this file */

bool FindUnambiguousVoxel(Voxel *** voxel_array,
                          const unsigned int file_dimensions[NUM_DIMENSIONS],
                          const unsigned int dim_index,
                          unsigned int & i, // in-outputs: ID found voxel
                          unsigned int & j,
                          unsigned int & k,
                          unsigned int & face_dim,
                          unsigned int & face_side);

bool AreDiff(const unsigned int v1[NUM_DIMENSIONS],
             const unsigned int v2[NUM_DIMENSIONS]);

void SetOrientation(float *** distance_transform,
                    const vector<EnoPoint> & eno_array,
                    const unsigned int voxel_idxs[],
                    unsigned int poly_length,
                    unsigned int poly_idxs[],
                    unsigned int poly_dims[],
                    unsigned int poly_sides[],
                    unsigned int poly_face_enos[]);

/******************************************************************************/

bool InitWaveFront(float *** distance_transform,       // input only
                   Voxel *** voxel_array,              // eno_used flags set
                   const unsigned int file_dimensions[NUM_DIMENSIONS], // input
                   vector<EnoPoint> & eno_array,       // used flags set
                   const vector<double> & eno_coords,  // input only
                   deque<WaveElement> & wave_front,    // output only
                   vector<unsigned int> & poly_list,   // new polys added
                   vector<double> & normal_list)       // new normals added
{
  /*
   * Find an ENO point in a Voxel without ambiguity
   */
  
  bool found = FALSE;
  unsigned int num_unused_enos = 0;
  vector<EnoPoint>::iterator it;
  Voxel * this_voxel;
  unsigned int face_dim, face_side;
  unsigned int i;
  
  // The data for the best-choice initial polygon are placed here
  unsigned int voxel_idxs[NUM_DIMENSIONS];
  unsigned int poly_length = MAX_POINTS_PER_UNAMB_POLY+1;
  unsigned int poly_idxs[MAX_POINTS_PER_UNAMB_POLY];
  unsigned int poly_dims[MAX_POINTS_PER_UNAMB_POLY];
  unsigned int poly_sides[MAX_POINTS_PER_UNAMB_POLY];
  unsigned int poly_face_enos[2*MAX_POINTS_PER_UNAMB_POLY];
  
  // Temporary versions of the data are placed here when making a polygon
  // so the data for the last best-choice polygon aren't overwritten
  unsigned int voxel_idxs_check[NUM_DIMENSIONS];
  unsigned int poly_length_check;
  unsigned int poly_idxs_check[MAX_POINTS_PER_UNAMB_POLY];
  unsigned int poly_dims_check[MAX_POINTS_PER_UNAMB_POLY];
  unsigned int poly_sides_check[MAX_POINTS_PER_UNAMB_POLY];
  unsigned int poly_face_enos_check[2*MAX_POINTS_PER_UNAMB_POLY];
  
  for(it = eno_array.begin();
      it != eno_array.end() && (poly_length != POINTS_PER_TRIANGLE);
      it++)
  {
    if(!it->used)
    {
      num_unused_enos++;
      
      memcpy(voxel_idxs_check, it->indexes, NUM_DIMENSIONS*sizeof(int));
      
      // NOTE: Can't propagate from border because we don't know the normal.
      
      // Check the 4 Voxels containing this ENO point
      found = FindUnambiguousVoxel(voxel_array,       // inputs
                                   file_dimensions,
                                   it->dim_index,
                                   voxel_idxs_check[DIM_X], // in-outputs
                                   voxel_idxs_check[DIM_Y],
                                   voxel_idxs_check[DIM_Z],
                                   face_dim,
                                   face_side);
      
#ifdef DEBUG12
      if(!found)
      {
        cout.precision(4);
        cout << "ENO in ambiguous Voxel during init:\t"
             << eno_coords[NUM_DIMENSIONS*(it-eno_array.begin())+DIM_X] << "\t"
             << eno_coords[NUM_DIMENSIONS*(it-eno_array.begin())+DIM_Y] << "\t"
             << eno_coords[NUM_DIMENSIONS*(it-eno_array.begin())+DIM_Z] << endl;
      }
#endif
      
      // if Unambiguous Voxel found containing the ENO point
      if(found)
      {
        /*
         * Polygonize this Voxel
         */
      
        // Find a face with two points on different edges
        this_voxel = &voxel_array[voxel_idxs_check[DIM_X]]
                                 [voxel_idxs_check[DIM_Y]]
                                 [voxel_idxs_check[DIM_Z]];
        poly_idxs_check[0] =
          this_voxel->faces[face_dim][face_side]->eno_indexes[0];
        poly_idxs_check[1] =
          this_voxel->faces[face_dim][face_side]->eno_indexes[1];
        
        // if poly_idxs_check[0] and poly_idxs_check[1] are on different edges
        if((eno_array[poly_idxs_check[0]].dim_index !=
            eno_array[poly_idxs_check[1]].dim_index) ||
           AreDiff(eno_array[poly_idxs_check[0]].indexes,
                   eno_array[poly_idxs_check[1]].indexes))
        {
          poly_dims_check[0] = face_dim;
          poly_sides_check[0] = face_side;
          poly_face_enos_check[0] = 0;
          poly_face_enos_check[1] = 1;
          
          poly_length_check = GetUnambiguousPolygon(this_voxel,
                                                    eno_array,
                                                    voxel_idxs_check,
                                                    poly_idxs_check,
                                                    poly_dims_check,
                                                    poly_sides_check,
                                                    poly_face_enos_check);
          
          if(poly_length_check < POINTS_PER_TRIANGLE)
          {
            cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
                 << "Unambiguous polygon made in initialization has "
                 << poly_length << " points." << endl;
            
            exit(1);
          }
          
          if(poly_length_check < poly_length)
          {
            memcpy(voxel_idxs, voxel_idxs_check,
                   NUM_DIMENSIONS*sizeof(unsigned int));
            poly_length = poly_length_check;
            for(i = 0; i < poly_length; i++)
            {
              poly_idxs[i] = poly_idxs_check[i];
              poly_dims[i] = poly_dims_check[i];
              poly_sides[i] = poly_sides_check[i];
              poly_face_enos[2*i] = poly_face_enos_check[2*i];
              poly_face_enos[2*i+1] = poly_face_enos_check[2*i+1];
            }
            
#ifdef DEBUG15
            cout << "Valid Init Polygon found with " << poly_length
                 << " points in Voxel [" << voxel_idxs[DIM_X] << "]["
                 << voxel_idxs[DIM_Y] <<"]["<< voxel_idxs[DIM_Z] <<"]."<< endl;
#endif
          }
        }
/***************************************************************************
        else // poly_idxs_check[0] and poly_idxs_check[1] are on same edge
        {
          There are EXCEPTIONS -- These points may still be connectable if
          some other voxel containing them is unambiguous.
        
           *
           * We have just a pair of points on an edge with no possible
           * connections.  We know this because any connections would have a
           * face of at least 4 points, resulting in an ambiguous Voxel.
           * We can't use these points, so throw them out.
           *
           
           eno_array[poly_idxs_check[0]].used = TRUE;
           eno_array[poly_idxs_check[1]].used = TRUE;
           
           ********************
           *** Marking all the eno_used flags in all 8 faces for these points
           *** isn't worth the effort. -- Maybe do it later???
           this_voxel->faces[face_dim][face_side]->
             eno_used[poly_face_enos_check[0]] = TRUE;
           this_voxel->faces[face_dim][face_side]->
             eno_used[poly_face_enos_check[1]] = TRUE;
           ********************
        }
***************************************************************************/
      } // endif Unambiguous Voxel found containing the ENO point
    } // endif ENO point is not used
  } // endfor over ENO points
  
  if(num_unused_enos == 0)
    return FALSE; // There is no more surface to find.
  else if(poly_length == MAX_POINTS_PER_UNAMB_POLY+1)
  {
    /*************************
    *
    * Should we Prune away useless points here???
    * It would be a bit of a pain...
    *
    *************************/
#ifdef PRINT_DISCARDED_POINTS
    cout << "Discarding the following ENO points (ignoring those with Z == 1):" << endl;
#endif
    num_unused_enos = 0;
    for(it = eno_array.begin(); it != eno_array.end(); it++)
    {
      if(!it->used)
      {
#ifdef PRINT_DISCARDED_POINTS
        if(it->indexes[2] != 1)
        {
          cout << it - eno_array.begin() << '\t' << DIMENSION_CHARS[it->dim_index] << '\t' 
               << it->indexes[0] << '\t' << it->indexes[1] << '\t' << it->indexes[2] << endl;
          num_unused_enos++;
        }
#else
        num_unused_enos++;
#endif
      }
    }
    
    cout << "WARNING in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << num_unused_enos << " ENO points are still unused, but no "
         << "suitable Voxels could be found for initialization." << endl;
    
    /*
     * Later, There will have to be some sort of special handling for this
     */
    
    return FALSE;
  }
  
#ifdef DEBUG15
  cout << "Initializing Wave From in Voxel [" << voxel_idxs[DIM_X] << "]["
       << voxel_idxs[DIM_Y] << "][" << voxel_idxs[DIM_Z] << "]" << endl;
#endif
  
  /*
   * Get the initial orientation correct with respect to the DT.  The
   * orientation of all triangles in this surface piece follows from this.
   */
  
  SetOrientation(distance_transform,
                 eno_array,
                 voxel_idxs,
                 poly_length,
                 poly_idxs,
                 poly_dims,
                 poly_sides,
                 poly_face_enos);
  
  /*
   * Make Triangle(s) and begin the Wave Front; set Used Flags
   */
  
  MakeTriangles(eno_coords, // inputs
                eno_array,
                poly_idxs,
                poly_dims,
                poly_sides,
                poly_face_enos,
                poly_length,
                voxel_array,
                voxel_idxs,
                file_dimensions,
                wave_front, // (in-)outputs
                poly_list,
                normal_list);
  
  return TRUE;
}




/******************************************************************************/

inline
bool FindUnambiguousVoxel(Voxel *** voxel_array,
                          const unsigned int file_dimensions[NUM_DIMENSIONS],
                          const unsigned int dim_index,
                          unsigned int & i, // in-outputs: ID found voxel
                          unsigned int & j,
                          unsigned int & k,
                          unsigned int & face_dim,
                          unsigned int & face_side)
{
  bool start_ok = (!voxel_array[i][j][k].ambiguous) &&
                  (i != file_dimensions[DIM_X]-1) &&
                  (j != file_dimensions[DIM_Y]-1) &&
                  (k != file_dimensions[DIM_Z]-1);
  
  switch(dim_index)
  {
  case DIM_X:
    face_dim = DIM_Y;
    
    if(start_ok)
    {
      face_side = 0;
      return TRUE;
    }
    if(j>0 && k<file_dimensions[DIM_Z]-1 && !voxel_array[i][j-1][k].ambiguous)
    {
      j--;
      face_side = 1;
      return TRUE;
    }
    if(j<file_dimensions[DIM_Y]-1 && k>0 && !voxel_array[i][j][k-1].ambiguous)
    {
      k--;
      face_side = 0;
      return TRUE;
    }
    if(j>0 && k>0 && !voxel_array[i][j-1][k-1].ambiguous)
    {
      j--; k--;
      face_side = 1;
      return TRUE;
    }
    break;
    
  case DIM_Y:
    face_dim = DIM_X;
    
    if(start_ok)
    {
      face_side = 0;
      return TRUE;
    }
    if(k>0 && i<file_dimensions[DIM_X]-1 && !voxel_array[i][j][k-1].ambiguous)
    {
      k--;
      face_side = 0;
      return TRUE;
    }
    if(k<file_dimensions[DIM_Z]-1 && i>0 && !voxel_array[i-1][j][k].ambiguous)
    {
      i--;
      face_side = 1;
      return TRUE;
    }
    if(k>0 && i>0 && !voxel_array[i-1][j][k-1].ambiguous)
    {
      k--; i--;
      face_side = 1;
      return TRUE;
    }
    break;
    
  case DIM_Z:
    face_dim = DIM_X;
    
    if(start_ok)
    {
      face_side = 0;
      return TRUE;
    }
    if(i>0 && j<file_dimensions[DIM_Y]-1 && !voxel_array[i-1][j][k].ambiguous)
    {
      i--;
      face_side = 1;
      return TRUE;
    }
    if(i<file_dimensions[DIM_X]-1 && j>0 && !voxel_array[i][j-1][k].ambiguous)
    {
      j--;
      face_side = 0;
      return TRUE;
    }
    if(i>0 && j>0 && !voxel_array[i-1][j-1][k].ambiguous)
    {
      i--; j--;
      face_side = 1;
      return TRUE;
    }
    break;

  default:
    cout << "ERROR in " << __FILE__ << ":" << __LINE__ << ":" << endl
         << "Invalid dim_index in EnoPoint ["<<i<<"]["<<j<<"]["<<k<<"]: "
         << dim_index << endl;
    
    exit(1);
  }
  
  // None of these Voxels are unambiguous.
  return FALSE;
}




/******************************************************************************/

inline bool AreDiff(const unsigned int v1[NUM_DIMENSIONS], // just inputs
                    const unsigned int v2[NUM_DIMENSIONS])
{
  return((v1[0] != v2[0]) ||
         (v1[1] != v2[1]) ||
         (v1[2] != v2[2]));
}




/******************************************************************************/

inline void SetOrientation(float *** distance_transform,
                           const vector<EnoPoint> & eno_array,
                           const unsigned int voxel_idxs[],
                           unsigned int poly_length,
                           unsigned int poly_idxs[],
                           unsigned int poly_dims[],
                           unsigned int poly_sides[],
                           unsigned int poly_face_enos[])
{
  /*
   * Make sure first two points of polygon are in the proper order for the
   * normal to point outside the surface.  Looking from the outside, a polygon's
   * vertices should be ordered counter-clockwise.
   */
  
  unsigned int face_dim = poly_dims[0];
  unsigned int face_side = poly_sides[0];
  // Assuming the two points are on different edges, we only need to check one.
  
  // Obtain base indexes for this face
  unsigned int face_idxs[NUM_DIMENSIONS];
  memcpy(face_idxs, voxel_idxs, NUM_DIMENSIONS*sizeof(unsigned int));
  face_idxs[face_dim] += face_side;
  
  // Obtain indexes for the DT points on either side of the first ENO point
  unsigned int eno_idxs0[NUM_DIMENSIONS];
  memcpy(eno_idxs0, eno_array[poly_idxs[0]].indexes,
         NUM_DIMENSIONS*sizeof(unsigned int));
  unsigned int eno_idxs1[NUM_DIMENSIONS];
  memcpy(eno_idxs1, eno_array[poly_idxs[0]].indexes,
         NUM_DIMENSIONS*sizeof(unsigned int));
  unsigned int eno_dim = eno_array[poly_idxs[0]].dim_index;
  eno_idxs1[eno_dim]++;
  
  // Does the increasing-index direction of this edge run clockwise on the face?
  // Note use of exclusive-or: ^ or !=
  bool edge_cw = ((eno_dim == (face_dim+2)%NUM_DIMENSIONS) !=
                  AreDiff(eno_idxs0, face_idxs));
  
  // Does the gradient point in the direction of increasing index?
  bool pos_dir = (distance_transform[eno_idxs0[DIM_X]]
                                    [eno_idxs0[DIM_Y]]
                                    [eno_idxs0[DIM_Z]] <
                  distance_transform[eno_idxs1[DIM_X]]
                                    [eno_idxs1[DIM_Y]]
                                    [eno_idxs1[DIM_Z]]);
  
  // Does the gradient pos_dir run clockwise on the face vertices?
  bool gradient_cw = (pos_dir == edge_cw);
  
  // A swap is needed if the gradient runs clockwise on this edge for a face of
  // side 0.  The opposite is true for a face of side 1.
  if(gradient_cw != bool(face_side))
  {
    unsigned int swap_temp;
    
#ifdef DEBUG15
    cout << "Inverting Polygon Order" << endl;
#endif
    
    // Invert Polygon order
    for(unsigned int i = 0; i < poly_length/2; i++)
    {
      swap_temp = poly_idxs[i];
      poly_idxs[i] = poly_idxs[poly_length - 1 - i];
      poly_idxs[poly_length - 1 - i] = swap_temp;
    
      swap_temp = poly_dims[i];
      poly_dims[i] = poly_dims[poly_length - 1 - i];
      poly_dims[poly_length - 1 - i] = swap_temp;
    
      swap_temp = poly_sides[i];
      poly_sides[i] = poly_sides[poly_length - 1 - i];
      poly_sides[poly_length - 1 - i] = swap_temp;
    
      swap_temp = poly_face_enos[2*i];
      poly_face_enos[2*i] = poly_face_enos[2*(poly_length - i) - 1];
      poly_face_enos[2*(poly_length - i) - 1] = swap_temp;
    
      swap_temp = poly_face_enos[2*i+1];
      poly_face_enos[2*i+1] = poly_face_enos[2*(poly_length - i - 1)];
      poly_face_enos[2*(poly_length - i - 1)] = swap_temp;
    }
    
    if(poly_length % 2) // Swap face ENOs in the middle of the array
    {
#ifdef DEBUG15
      cout << "Inverting Middle Face ENOs" << endl;
#endif
      swap_temp = poly_face_enos[poly_length - 1];
      poly_face_enos[poly_length - 1] = poly_face_enos[poly_length];
      poly_face_enos[poly_length] = swap_temp;
    }
  }
}
