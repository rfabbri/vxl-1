//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_subgrid_iterator.h>
#include <bvpl/bvpl_voxel_subgrid.h>
#include <bvpl/bvpl_edge2d_kernel.h>
#include <bvpl/bvpl_edge2d_functor.h>
#include <bvpl/bvpl_neighb_operator.h>

#include <vul/vul_file.h>
#include <vnl/vnl_math.h>

bool save_occupancy_raw(vcl_string filename, bvxm_voxel_grid<float>* grid)
{
  vcl_fstream ofs(filename.c_str(),vcl_ios::binary | vcl_ios::out);
  if (!ofs.is_open()) {
    vcl_cerr << "error opening file " << filename << " for write!\n";
    return false;
  }

  // write header
  unsigned char data_type = 0; // 0 means unsigned byte

  vxl_uint_32 nx = grid->grid_size().x();
  vxl_uint_32 ny = grid->grid_size().y();
  vxl_uint_32 nz = grid->grid_size().z();

  ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  // write data
  // iterate through slabs and fill in memory array
  char *ocp_array = new char[nx*ny*nz];

  bvxm_voxel_grid<float>::iterator ocp_it = grid->begin();
  for (unsigned k=0; ocp_it != grid->end(); ++ocp_it, ++k) {
    vcl_cout << '.';
    for (unsigned i=0; i<(*ocp_it).nx(); ++i) {
      for (unsigned j=0; j < (*ocp_it).ny(); ++j) {
        ocp_array[i*ny*nz + j*nz + k] = (unsigned char)((*ocp_it)(i,j) * 255.0);
      }
    }
  }
  vcl_cout << vcl_endl;
  ofs.write(reinterpret_cast<char*>(ocp_array),sizeof(unsigned char)*nx*ny*nz);

  ofs.close();

  delete[] ocp_array;

  return true;
}

//: Test changes
static void test_neighb_oper()
{
  // create the grid
  // we need temporary disk storage for this test.
  vcl_string storage_fname("bvxm_voxel_grid_test_temp.vox");
  vcl_string storage_fname2("bvxm_voxel_grid_test_temp2.vox");
  vcl_string storage_cached_fname("bvxm_voxel_grid_cached_test_temp.vox");
  // remove file if exists from previous test.
  if (vul_file::exists(storage_fname.c_str())) {
    vul_file::delete_file_glob(storage_fname.c_str());
  }
  if (vul_file::exists(storage_fname2.c_str())) {
    vul_file::delete_file_glob(storage_fname2.c_str());
  }
  if (vul_file::exists(storage_cached_fname.c_str())) {
    vul_file::delete_file_glob(storage_cached_fname.c_str());
  }

  int grid_x=50, grid_y=50, grid_z=20;
  vgl_vector_3d<unsigned> grid_size(grid_x, grid_y, grid_z);
  unsigned max_cache_size = grid_size.x()*grid_size.y()*18;

  // try test with all types of underlying storage.
  bvxm_voxel_grid<float>* grid = new bvxm_voxel_grid<float>(storage_fname,grid_size); // disk storage;
  bvxm_voxel_grid<float>* grid_out = new bvxm_voxel_grid<float>(storage_fname2,grid_size);
  vcl_string test_name;

  // check num_observations
  unsigned nobs = grid->num_observations();

  // fill with test data
  float init_val = 0.0f;
  grid->initialize_data(init_val);
  grid_out->initialize_data(init_val);
  bool init_check = true;
  bool write_read_check = true;

  //grid->increment_observations();

  // read in each slice, check that init_val was set, and fill with new value
  unsigned count = 0.1f;
  vcl_cout << "read/write: ";
  bvxm_voxel_slab_iterator<float> slab_it;
  for (slab_it = grid->begin(); slab_it != grid->end(); ++slab_it) {
    vcl_cout << '.';
    bvxm_voxel_slab<float> vit=*slab_it;
    for (unsigned i=0; i<grid_x/2; i++) {
      for (unsigned j=0; j<grid_y; j++) {
        float &v = vit(i,j);
        v = 0.9f;
      }
    }
  }
  vcl_cout << "done." << vcl_endl;
  save_occupancy_raw("first.raw", grid);
  
  bvpl_edge2d_kernel* kernel = new bvpl_edge2d_kernel(); 
  kernel->create(5, 5, vnl_vector_fixed<double,3>(vnl_math::pi, 0.0, 0.0));
  bvpl_kernel_base_sptr kernel_sptr = kernel;
  bvpl_edge2d_functor<float> func;
  bvpl_neighb_operator<float, bvpl_edge2d_functor<float> > oper(func);
  oper.operate(grid, kernel_sptr, grid_out); 
  save_occupancy_raw("out.raw", grid_out);
}

TESTMAIN( test_neighb_oper );
