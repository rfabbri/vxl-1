#include <stdlib.h>

#include <iostream>
#include <vector>
#include <deque>

using namespace std;

#include "surface_tracer.h"



/******************************************************************************/

bool PropagateWave(Voxel *** voxel_array,              // eno_used flags set
                   const unsigned int file_dimensions[NUM_DIMENSIONS], // input
                   vector<EnoPoint> & eno_array,       // used flags set
                   const vector<double> & eno_coords,  // input only
                   deque<WaveElement> & wave_front,    // new wave edges added
                   vector<unsigned int> & poly_list,   // new polys added
                   vector<double> & normal_list)       // new normals added
{
  /*
   * Get Next Wave Element
   */
  WaveElement next_element;
  bool done = FALSE;
  while(!done)
  {
    if(wave_front.empty()) // No more wave; the surface should be complete
      return FALSE;

    next_element = wave_front.front();
    wave_front.pop_front();
    
    Face * face_ptr = voxel_array[next_element.voxel_idxs[DIM_X]]
                                 [next_element.voxel_idxs[DIM_Y]]
                                 [next_element.voxel_idxs[DIM_Z]].
                           faces[next_element.face_dim][next_element.face_side];
    int eno_idx_diff = face_ptr->eno_indexes[next_element.eno0] -
                       face_ptr->eno_indexes[next_element.eno1];
    if((eno_idx_diff == 1) || (eno_idx_diff == -1))
    {
#ifdef DEBUG_BAD_WAVE_ELEMENTS
      cout << "INFO in " << __FILE__ << ":" << __LINE__ << ":" << endl
           << "Invalidating wave element with points on the same edge."
           << endl;
#endif
    }
    else if(next_element.IsValid(voxel_array, file_dimensions))
      done = TRUE;
  }
  
  /*
   * Polygonize this Voxel and Continue the Wave Front
   */
  
#ifdef SKIP_AMBIGUITY
  Voxel * this_voxel = &voxel_array[next_element.voxel_idxs[DIM_X]]
                                   [next_element.voxel_idxs[DIM_Y]]
                                   [next_element.voxel_idxs[DIM_Z]];
  
  if(this_voxel->ambiguous)
  {
    cout << "Ambiguous Voxel found in propagation:" << endl;
    cout     << next_element.voxel_idxs[DIM_X] << "\t"
         << next_element.voxel_idxs[DIM_Y] << "\t"
         << next_element.voxel_idxs[DIM_Z] << endl;
    
    return TRUE;
  }
#endif
  
  PolygonizeVoxel(next_element,
                  voxel_array,
                  file_dimensions,
                  eno_coords,
                  eno_array,
                  wave_front,
                  poly_list,
                  normal_list);
    
  // Return TRUE indicating the wave front isn't empty yet.
  return TRUE;
}




