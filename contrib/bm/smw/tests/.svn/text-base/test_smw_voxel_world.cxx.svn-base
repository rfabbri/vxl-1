#include "bvxm/bvxm_voxel_world.h" //voxel world
                                   //includes grid/bvxm_voxel_grid.h
                                   //bvxm_world_params.h
                                   //bvxm_voxel_traits.h
                                   //bvxm_voxel_traits.h
                                   //bvxm_world_params.h
//bvxm_voxel_grid includes: bvxm_voxel_storage.h, storage_disk.h, 
//                          storage_disk_cache.h, slab.h...
//#include "bvxm/bvxm_world_params.h"
//#include "bvxm/bvxm_mog_grey_processor.h"

#include <testlib/testlib_test.h> //test library

#include <vil/vil_load.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vpgl/vpgl_proj_camera.h>

#include <vul/vul_file.h>

static void test_smw_voxel_world()
{
    START("test_smw_voxel_world");

    //create a directory with the name "test_world_dir"
    vcl_string model_dir("test_world_dir");
    if (vul_file::is_directory(model_dir))
        vul_file::delete_file_glob(model_dir+"/*");
    else
    {
        if(vul_file::exists(model_dir))
            vul_file::delete_file_glob(model_dir);
        vul_file::make_directory(model_dir);
    }

    //creating a 2d voxel world

    vgl_point_3d<float> grid_corner(0.0f, 0.0f, 0.0f);
    vgl_vector_3d<unsigned> grid_size(10,10,1);
    float vox_len = 0.5f;

    bvxm_world_params_sptr params = new bvxm_world_params();
    bgeo_lvcs_sptr lvcs = new bgeo_lvcs(); //uses default params
    
    params->set_params(model_dir,grid_corner,grid_size,vox_len,lvcs);

    //create the world
    bvxm_voxel_world world;
    world.set_params(params);

    //initialize the data
    bvxm_mog_grey_processor::apm_datatype init_apm;

    float init_ocp = 1.0f/100;
    

}

TESTMAIN( test_smw_voxel_world );
