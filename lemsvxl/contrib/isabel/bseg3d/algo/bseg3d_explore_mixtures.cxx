 
#include "bseg3d_explore_mixtures.h"
#include "bseg3d_gmm_l2norm.h"
#include "bseg3d_merge_mixtures.h"
#include <vcl_limits.h>

#include <bvxm/grid/bvxm_voxel_grid.h>

bool bseg3d_explore_mixtures::get_region_above_threshold(bvxm_voxel_grid_base_sptr grid_in_base,
                                                         bvxm_voxel_grid_base_sptr grid_out_base,
                                                         bvxm_voxel_grid_base_sptr mask_base,
                                                         float min_thresh)
{
  // cast grids
  bvxm_voxel_grid<datatype_>* grid_in = static_cast<bvxm_voxel_grid<float>* >(grid_in_base.ptr());
  if(!grid_in){
    vcl_cerr<<"Error in thresholding grid: Input grid is null\n";
    return false;
  }

  bvxm_voxel_grid<datatype_>* grid_out = static_cast<bvxm_voxel_grid<float>* >(grid_out_base.ptr());
  if(!grid_out){
    vcl_cerr<<"Error in thresholding grid: Input grid is null\n";
    return false;
  }

  bvxm_voxel_grid<datatype_>* mask_grid = static_cast<bvxm_voxel_grid<float>* >(mask_base.ptr());
   if(!mask_grid){
    vcl_cerr<<"Error in thresholding grid: Ouput grid is null\n";
    return false;
  }
  
   //intitialize grids
  grid_out->initialize_data(float(0));
  mask_grid->initialize_data(float(0));
  
  
  // ierate though the grids
  bvxm_voxel_grid<datatype_>::iterator in_slab_it = grid_in->begin();
    bvxm_voxel_grid<datatype_>::iterator out_slab_it = grid_out->begin();
  bvxm_voxel_grid<datatype_>::iterator mask_slab_it = mask_grid->begin();

  vcl_cout << "Thresholding Grid: " << vcl_endl;
  for (unsigned z=0; z<(unsigned)(grid_in->grid_size().z()); ++z, ++in_slab_it, ++mask_slab_it, ++out_slab_it)
  {
    vcl_cout << '.';

    //iterate through slab and threshold. At this point the grids get updated on disk
    bvxm_voxel_slab<datatype_>::iterator in_it = (*in_slab_it).begin();
    bvxm_voxel_slab<datatype_>::iterator out_it = (*out_slab_it).begin();
    bvxm_voxel_slab<datatype_>::iterator mask_it = (*mask_slab_it).begin();

    for (; in_it!= (*in_slab_it).end(); ++in_it, ++out_it, ++mask_it)
    {
      if (*in_it > min_thresh){
        //if point is above threshold set to 1, otherwise set to 0
        (*mask_it) = 1.0f;
        //if point is above threshold leave unchanged, otherwise set to 0
        (*out_it) = (*in_it);
      }
       
    }
    
      }
  return true;
}

bool bseg3d_explore_mixtures::calculate_l2distances(bvxm_voxel_grid_base_sptr apm_base,
                                                    bvxm_voxel_grid_base_sptr mask_base,
                                                    bvxm_voxel_grid_base_sptr dist_base,
                                                    bool reference_given,
                                                    mix_gauss_type reference)
{

 // cast grids
  bvxm_voxel_grid< mix_gauss_type>* apm_grid = static_cast<bvxm_voxel_grid< mix_gauss_type>* >(apm_base.ptr());
  bvxm_voxel_grid<float>* mask_grid = static_cast<bvxm_voxel_grid< float>* >(mask_base.ptr());
  bvxm_voxel_grid<float>* distance_grid = static_cast<bvxm_voxel_grid< float>* >(dist_base.ptr());
  distance_grid->initialize_data(float(0));
   if(!apm_grid ||!mask_grid || !distance_grid)
    return false;

  vgl_vector_3d<unsigned> grid_size = apm_grid->grid_size();
  vcl_cout <<"Grid Size: " <<grid_size << vcl_endl;

  // iterate though the apm grid and measure distances
  bvxm_voxel_grid<mix_gauss_type>::iterator apm_slab_it = apm_grid->begin();
  bvxm_voxel_grid<float>::iterator dist_slab_it = distance_grid->begin();
  bvxm_voxel_grid<float>::iterator mask_slab_it = mask_grid->begin();

  if(reference_given){
    for (unsigned i=0; i<reference.num_components(); i++)
    {    
      vcl_cout<<"Mean " <<reference.distribution(i).mean() << vcl_endl;
      vcl_cout<<"Var " <<reference.distribution(i).var() << vcl_endl;
    }
  }

  vcl_cout << "Measuring distances: " << vcl_endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++apm_slab_it, ++mask_slab_it, ++dist_slab_it)
  {
    vcl_cout << '.';

    //3. Iterate through slab and threshold. At this point the grids get updated on disk
    bvxm_voxel_slab<mix_gauss_type>::iterator apm_it = (*apm_slab_it).begin();
    bvxm_voxel_slab<float>::iterator mask_it = (*mask_slab_it).begin();
    bvxm_voxel_slab<float>::iterator dist_it = (*dist_slab_it).begin();
    bseg3d_gmm_l2norm measure;
    
    //If the reference is not given then it is the one in the first voxel
    if(!reference_given){
      reference = *((*apm_slab_it).first_voxel());
      vcl_cout << "Reference Mixture: \n ";
      reference_given = true;
      for (unsigned i=0; i<reference.num_components(); i++)
      {
        vcl_cout<<"Mean " <<reference.distribution(i).mean() << vcl_endl;
        vcl_cout<<"Var " <<reference.distribution(i).var() << vcl_endl;
      }
    }
    
    for (; apm_it!= (*apm_slab_it).end(); ++apm_it, ++mask_it, ++dist_it)
    {
      double distance = measure.distance(*apm_it, reference);
      if(*mask_it > 0.9)   //give some space for floating point discrepancies
      ( *dist_it) = (float)distance;
    }
    
      }
  return true;
}

//: Merges the components of the gaussian mixtures at each voxel into a single gaussian
//  thus the resulting grid contains unimodal gaussians
bool bseg3d_explore_mixtures::merge_mixtures(bvxm_voxel_grid_base_sptr apm_base,
                                             bvxm_voxel_grid_base_sptr gauss_base)
{
  //cast grids
 bvxm_voxel_grid< mix_gauss_type >* apm_grid = static_cast<bvxm_voxel_grid< mix_gauss_type>* >(apm_base.ptr());
 bvxm_voxel_grid< gauss_type >* gauss_grid = static_cast<bvxm_voxel_grid< gauss_type>* >(gauss_base.ptr());

 //the merger
 bseg3d_merge_mixtures merge;

 //iterate through the grid merging mixtures
 bvxm_voxel_grid< mix_gauss_type >::iterator apm_it = apm_grid->begin();
 bvxm_voxel_grid< gauss_type >::iterator gauss_it = gauss_grid->begin();

 for (unsigned z=0; z<apm_grid->grid_size().z(); ++z, ++apm_it, ++gauss_it)
 {
   bvxm_voxel_slab< mix_gauss_type >::iterator apm_slab_it = (*apm_it).begin();
   bvxm_voxel_slab< gauss_type >::iterator gauss_slab_it = (*gauss_it).begin();

   for(; apm_slab_it!=(*apm_it).end(); apm_slab_it++, gauss_slab_it++)
   {
     merge.all_to_one(*apm_slab_it, *gauss_slab_it);
   }


 }
 return true;
}

bool bseg3d_explore_mixtures::save_byte_grid_raw(bvxm_voxel_grid_base_sptr grid_base,
                                                  vcl_string filename)
{
 
  vcl_fstream ofs(filename.c_str(),vcl_ios::binary | vcl_ios::out);
  if (!ofs.is_open()) {
    vcl_cerr << "error opening file " << filename << " for write!\n";
    return false;
  }
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // write header
  unsigned char data_type = 0; // 0 means unsigned byte

  bvxm_voxel_grid<float> *grid =
    dynamic_cast<bvxm_voxel_grid<float>*>(grid_base.ptr());

  vxl_uint_32 nx = grid->grid_size().x();
  vxl_uint_32 ny = grid->grid_size().y();
  vxl_uint_32 nz = grid->grid_size().z();

  ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  // write data
  // iterate through slabs and fill in memory array
  char *data_array = new char[nx*ny*nz];

  //get the range
  bvxm_voxel_grid<float>::iterator grid_it = grid->begin();
  float max = 0.0;
  float min = vcl_numeric_limits<float>::infinity();
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if ((*grid_it)(i,j)> max)
          max = (*grid_it)(i,j);
        if ((*grid_it)(i,j)< min)
          min = (*grid_it)(i,j);
      }
    }
  }
  vcl_cout << "max =  " << max << " min= " <<min << vcl_endl;
  grid_it = grid->begin();
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    vcl_cout << '.';
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        data_array[i*ny*nz + j*nz + k] = (unsigned char)((*grid_it)(i,j)* 255.0);
      }
    }
  }
  vcl_cout << vcl_endl;
  ofs.write(reinterpret_cast<char*>(data_array),sizeof(unsigned char)*nx*ny*nz);

  ofs.close();

  delete[] data_array;

  return true;
}

bool bseg3d_explore_mixtures::save_float_grid_raw(bvxm_voxel_grid_base_sptr grid_base,
                                                  vcl_string filename)
{
 
  vcl_fstream ofs(filename.c_str(),vcl_ios::binary | vcl_ios::out);
  if (!ofs.is_open()) {
    vcl_cerr << "error opening file " << filename << " for write!\n";
    return false;
  }
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // write header
  unsigned char data_type = 8; // 8 means float
  bvxm_voxel_grid<float> *grid =
    dynamic_cast<bvxm_voxel_grid<float>*>(grid_base.ptr());

  vxl_uint_32 nx = grid->grid_size().x();
  vxl_uint_32 ny = grid->grid_size().y();
  vxl_uint_32 nz = grid->grid_size().z();

  ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  // write data
  // iterate through slabs and fill in memory array
  float *data_array = new float[nx*ny*nz];

    //get the range
  bvxm_voxel_grid<float>::iterator grid_it = grid->begin();
  float max = -1.0 * vcl_numeric_limits<float>::infinity();
  float min = vcl_numeric_limits<float>::infinity();
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
        if ((*grid_it)(i,j)> max)
          max = (*grid_it)(i,j);
        if ((*grid_it)(i,j)< min)
          min = (*grid_it)(i,j);
      }
    }
  }
  vcl_cout << "max =  " << max << " min= " <<min << vcl_endl;

  grid_it = grid->begin();
  for (unsigned k=0; grid_it != grid->end(); ++grid_it, ++k) {
    vcl_cout << '.';
    for (unsigned i=0; i<(*grid_it).nx(); ++i) {
      for (unsigned j=0; j < (*grid_it).ny(); ++j) {
    /*    if( (*grid_it)(i,j) == 0)
          (*grid_it)(i,j) = -1000;*/
        //data_array[i*ny*nz + j*nz + k] = vcl_exp(1.0f - (-1.0f*(*grid_it)(i,j))/(-1.0f* min));
       // data_array[i*ny*nz + j*nz + k] =1.0f/(1.0f + vcl_abs((*grid_it)(i,j)));
      data_array[i*ny*nz + j*nz + k] =(*grid_it)(i,j);
      }
    }
  }
  vcl_cout << vcl_endl;
  ofs.write(reinterpret_cast<char*>(data_array),sizeof(float)*nx*ny*nz);

  ofs.close();

  delete[] data_array;

  return true;
}
