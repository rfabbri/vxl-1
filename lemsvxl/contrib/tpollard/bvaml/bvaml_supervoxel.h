#ifndef _bvaml_supervoxel_h_
#define _bvaml_supervoxel_h_

//:
// \file
// \brief A class for holding a large block of voxels.
// \author Thomas Pollard
// \date 012/21/06
// 
//   This is....

#include <vcl_vector.h>
#include "bvaml_params.h"
#include "bvaml_voxel.h"


class bvaml_supervoxel {

public:

  bvaml_supervoxel(
    bvaml_params* params,
    vcl_string file_namebase );

  ~bvaml_supervoxel();

  // Load a voxel for the given index with whatever fields are specified.
  void get_voxel(
    const vgl_point_3d<int>& voxel_index,
    bvaml_voxel& voxel,
    bool load_occupancy,
    bool load_misc,
    bool load_appearance,
    const vnl_vector<float>* local_light );

  // Load voxel fields into memory, will be called in get_voxel if not
  // called here.
  void load_occupancies();
  void load_misc_storage();
  void load_local_appearance(
    const vnl_vector<float>& local_light );

  void save();
  void clear();


protected:

  float* occupancy_cache_;
  float* misc_cache_;
  float** appearance_cache_;

  bool load_cache( // return true if initialized.
    vcl_string file_name,
    int data_size,
    float*& cache,
    bool allocate_if_needed = true );
  void save_cache(
    vcl_string file_name,
    int data_size,
    float*& cache );

  vcl_string file_namebase_;
  vcl_string occupancy_filename();
  vcl_string misc_filename();
  vcl_string appearance_filename( int i );

  vnl_vector<int> appearance_sizes_;
  vnl_vector<int> appearance_ids_;
  int num_appearance_components_;
  vnl_vector<float> local_light_;

  // Make a new appearance model using the current local_light_.
  baml_appearance* new_local_appearance();

  bvaml_params* params_;

};


#endif // _bvaml_supervoxel_h_
