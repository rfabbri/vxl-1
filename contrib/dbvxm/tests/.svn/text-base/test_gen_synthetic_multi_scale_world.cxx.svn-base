#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vul/vul_file.h>

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>

#include <bvxm/bvxm_voxel_grid.h>
#include <bvxm/bvxm_voxel_slab.h>
#include <dbvxm_multi_scale_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_lidar_camera.h>
#include <bvxm/bvxm_util.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

//hard-coded values ... should be removed and added as a parameter class
const int IMAGE_U = 200;
const int IMAGE_V = 200;
const double x_scale = 1;
const double y_scale = 1;
const double focal_length = 450;
const double z_base_height = 450;
const double camera_dist= 500.0;

//size of the world
const unsigned nx=200, ny=200, nz=50;
const float vox_length = 1.35f;
vcl_vector<vgl_box_3d<double> > boxes;

typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype apm_datatype;

// returns a face number if a point is on the surface of a box [0,1,2,3,4,5],
// -1 otherwise (not on the surface case)
int on_box_surface(vgl_box_3d<double> box, vgl_point_3d<double> v)
{
    // create a box a size smaller
    vgl_box_3d<double> in_box;
    const int thickness = 1;
    in_box.set_min_point(vgl_point_3d<double>(box.min_x()+thickness, box.min_y()+thickness, box.min_z()+thickness));
    in_box.set_max_point(vgl_point_3d<double>(box.max_x()-thickness, box.max_y()-thickness, box.max_z()-thickness));

    bool on = false;
    if (box.contains(v) && !in_box.contains(v))
    {
        on = true;

        // find the face index
        if ((box.min_z() <= v.z()) && (v.z() <= box.min_z()+thickness))
            return 0;

        if ((box.max_z()-thickness <= v.z()) && (v.z() <= box.max_z()))
            return 0;//5;

        if ((box.min_y() <= v.y()) && (v.y() <= box.min_y()+thickness))
            return 1;

        if ((box.max_y()-thickness <= v.y()) && (v.y() <= box.max_y()))
            return 1;//3;

        if ((box.min_x() <= v.x()) && (v.x() <= box.min_x()+thickness))
            return 2;//4;

        if ((box.max_x()-thickness <= v.x()) && (v.x() <= box.max_x()))
            return 2;
    }
    return -1;
}

int in_box(vgl_box_3d<double> box, vgl_point_3d<double> v)
{
    if (box.contains(v))
        return 1;
    else
        return -1;
}

void generate_persp_camera(double focal_length,
                           vgl_point_2d<double>& pp,  //principal point
                           double x_scale, double y_scale,
                           vgl_point_3d<double>& camera_center,
                           vpgl_perspective_camera<double>& cam)
{
    vpgl_calibration_matrix<double> K(focal_length,pp, x_scale, y_scale);
    cam.set_calibration(K);
    cam.set_camera_center(camera_center);
}

vpgl_rational_camera<double>
perspective_to_rational(vpgl_perspective_camera<double>& cam_pers)
{
    vnl_matrix_fixed<double,3,4> cam_pers_matrix = cam_pers.get_matrix();
    vcl_vector<double> neu_u,den_u,neu_v,den_v;
    double x_scale = 1.0,
        x_off = 0.0,
        y_scale = 1.0,
        y_off = 0.0,
        z_scale = 1.0,
        z_off = 0.0,
        u_scale = 1.0,
        u_off = 0.0,
        v_scale = 1.0,
        v_off = 0.0;

    for (int i=0; i<20; i++){
        neu_u.push_back(0.0);
        neu_v.push_back(0.0);
        den_u.push_back(0.0);
        den_v.push_back(0.0);
    }

    int vector_map[] = {9,15,18,19};

    for (int i=0; i<4; i++){
        neu_u[vector_map[i]] = cam_pers_matrix(0,i);
        neu_v[vector_map[i]] = cam_pers_matrix(1,i);
        den_u[vector_map[i]] = cam_pers_matrix(2,i);
        den_v[vector_map[i]] = cam_pers_matrix(2,i);
    }

    vpgl_rational_camera<double> cam_rat(neu_u,den_u,neu_v,den_v,
        x_scale,x_off,y_scale,y_off,z_scale,z_off,
        u_scale,u_off,v_scale,v_off);
    return cam_rat;
}


vcl_vector<vpgl_camera_double_sptr >
generate_cameras_yz(vgl_box_3d<double>& world)
{
    vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

    vgl_point_3d<double> centroid = world.centroid();
    double y,z;
    //double alpha = (vnl_math::pi/8.) * 3;
    double alpha = vnl_math::pi/4;
    double delta_alpha = -1*vnl_math::pi/200;
    vcl_vector<vgl_point_3d<double> > centers;
    for (unsigned i=0; i<20; i++) {
        y = camera_dist*vcl_sin(alpha);
        z = (camera_dist+z_base_height)*vcl_cos(alpha);
        //centers.push_back(vgl_point_3d<double> (x+centroid.x(), y+centroid.y(), 1e15+centroid.z()));
        centers.push_back(vgl_point_3d<double> (centroid.x(), y+centroid.y(), z+centroid.z()));
#ifdef DEBUG
        vcl_cout << centers[i] << vcl_endl;
#endif
        alpha += delta_alpha;
    }

    vgl_box_2d<double> bb;
    vcl_vector<vpgl_camera_double_sptr> persp_cameras;
    for (unsigned i=0; i<centers.size(); i++)
    {
        vgl_point_3d<double> camera_center  = centers[i];
        vpgl_perspective_camera<double>* persp_cam = new vpgl_perspective_camera<double>;
        generate_persp_camera(focal_length,principal_point, x_scale, y_scale, camera_center, *persp_cam);
        persp_cam->look_at(vgl_homg_point_3d<double>(centroid));

        //vpgl_rational_camera<double>* rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(persp_cam));
        persp_cameras.push_back(persp_cam);

        vcl_vector<vgl_point_3d<double> > corners = bvxm_util::corners_of_box_3d<double>(world);
        for (unsigned i=0; i<corners.size(); i++) {
            vgl_point_3d<double> c = corners[i];
            double u,v, u2, v2;
            persp_cam->project(c.x(), c.y() ,c.z(), u, v);
            bb.add(vgl_point_2d<double> (u,v));
#ifdef DEBUG
            vcl_cout << "Perspective [" << u << ',' << v << "]\n"
                << "Rational [" << u2 << ',' << v2 << "]\n" << vcl_endl;
#endif
        }
#ifdef DEBUG
        vcl_cout << bb << vcl_endl;
#endif
    }
    return persp_cameras;
}

bool gen_images(vgl_vector_3d<unsigned> grid_size,
                dbvxm_multi_scale_voxel_world_sptr world,
                // bvxm_world_params_sptr world_params,
                bvxm_voxel_grid<float>* intensity_grid,
                bvxm_voxel_grid<float>* ocp_grid,
                bvxm_voxel_grid<apm_datatype>* apm_grid,
                vcl_vector<vpgl_camera_double_sptr>& cameras,
                vcl_vector <vil_image_view_base_sptr>& image_set,
                unsigned int bin_num)
{
    apm_datatype sample;
    apm_grid->initialize_data(sample);

    bvxm_voxel_grid<apm_datatype>::iterator apm_slab_it;
    bvxm_voxel_grid<float>::iterator obs_it = intensity_grid->begin();
    bool update_status;
    //slab thickness is 1
    bvxm_voxel_slab<float>* weight = new bvxm_voxel_slab<float>(grid_size.x(),grid_size.y(),1);
    weight->fill(1);

    for (apm_slab_it = apm_grid->begin(); apm_slab_it != apm_grid->end(); ++apm_slab_it,++obs_it) {
        bvxm_mog_grey_processor processor;
        update_status = processor.update(*apm_slab_it, *obs_it, *weight);
    }

    vcl_string path = "./test_gen_synthetic_multi_scale_world/test_img";

    unsigned int scale_idx = 0;
    vcl_vector<vil_image_view_base_sptr> image_set_smooth;

    //for (unsigned i=0; i<cameras.size(); i++) {
    //    vil_image_view_base_sptr img_arg;
    //    vil_image_view<float>* mask = new vil_image_view<float>(IMAGE_U, IMAGE_V);
    //    vil_image_view_base_sptr expected = new vil_image_view<unsigned char>(IMAGE_U, IMAGE_V);
    //    bvxm_image_metadata camera(img_arg, cameras[i]);

    //    world->expected_image<APM_MOG_GREY>(camera, expected, *mask, bin_num,scale_idx);

    //    vil_image_view<unsigned char> * expected_copy = new vil_image_view<unsigned char> (IMAGE_U, IMAGE_V);
    //    expected_copy->deep_copy(expected);

    //    vcl_stringstream s;
    //    s << path << i << "_bin_" << bin_num <<"_scale_" << scale_idx << ".tif";
    //    vil_save(*expected, s.str().c_str());
    //    image_set.push_back(expected);
    //    image_set_smooth.push_back(expected_copy);
    //}


    for (unsigned i=0;i<cameras.size();i++)
    {
        for (scale_idx = 0;scale_idx < world->get_params()->max_scale();scale_idx++)
        {

            double factor=vcl_pow(2.0,scale_idx);

            vcl_stringstream s;
            s << path << i << "_bin_" << bin_num <<"_scale_" << scale_idx << ".tif";

            vpgl_camera_double_sptr camera = dbvxm_util::downsample_persp_camera(cameras[i],scale_idx);

            vpgl_perspective_camera<double>* persp_cam = dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer());
            vpgl_rational_camera<double>* rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(*persp_cam));

            vil_image_view_base_sptr img_arg;
            vil_image_view<float>* mask = new vil_image_view<float>(IMAGE_U/factor, IMAGE_V/factor);
            vil_image_view_base_sptr expected = new vil_image_view<unsigned char>(IMAGE_U/factor, IMAGE_V/factor);
            if(scale_idx==0)
                image_set.push_back(expected);
            bvxm_image_metadata met_camera(img_arg, rat_cam);

            world->expected_image<APM_MOG_GREY>(met_camera, expected, *mask, bin_num,0);
            vil_save(*expected,s.str().c_str());

            //vil_image_view<unsigned char>*img = dbvxm_util::downsample_image_by_two(image_set_smooth[i]);
            //image_set_smooth[i] = img;
            //vil_save(*image_set_smooth[i],s.str().c_str());

        }
    }


    return true;
}

bool reconstruct_world( dbvxm_multi_scale_voxel_world_sptr recon_world, vcl_vector<vpgl_camera_double_sptr>& cameras,
                       vcl_vector <vil_image_view_base_sptr>& image_set,unsigned int bin_num)
{
    vcl_string recon_path = "recon_world/test_img";
    vcl_string camera_path = "test_gen_cameras/camera";

    for (unsigned i = 0;i<cameras.size();i++)
    {
        vil_image_view_base_sptr curr_img=image_set[i];
        for (unsigned scale_idx = 0;scale_idx < recon_world->get_params()->max_scale();scale_idx++)
        {
            
            if (scale_idx != 0)
                curr_img=dbvxm_util::downsample_image_by_two(curr_img);
            

            vpgl_camera_double_sptr camera = dbvxm_util::downsample_persp_camera(cameras[i],scale_idx);

            vpgl_perspective_camera<double>* persp_cam = dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer());
            vpgl_rational_camera<double>* rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(*persp_cam));

            bvxm_image_metadata observation(curr_img,rat_cam);
            recon_world->update<APM_MOG_GREY>(observation,bin_num,scale_idx);


        }
    }

    for (unsigned i = 0;i<cameras.size();i++)
    {
        for (unsigned scale_idx = 0;scale_idx < recon_world->get_params()->max_scale();scale_idx++)
        {
            double factor = vcl_pow(2.0,scale_idx);
            vil_image_view_base_sptr img_arg;
            vil_image_view<float> mask(IMAGE_U/factor, IMAGE_V/factor);
            vil_image_view_base_sptr expected = new vil_image_view<unsigned char>(IMAGE_U/factor, IMAGE_V/factor);

            vpgl_camera_double_sptr camera_sptr = dbvxm_util::downsample_persp_camera(cameras[i],scale_idx);

            vpgl_perspective_camera<double>* persp_cam = dynamic_cast<vpgl_perspective_camera<double>*> (camera_sptr.as_pointer());
            vpgl_rational_camera<double>* rat_cam = new vpgl_rational_camera<double>(perspective_to_rational(*persp_cam));

            bvxm_image_metadata camera(img_arg,rat_cam);
            recon_world->expected_image<APM_MOG_GREY>(camera, expected, mask, bin_num,scale_idx);
            vcl_stringstream s;
            s << recon_path << i << "_bin_" << bin_num << "_scale_" << scale_idx << ".tif";
            vcl_stringstream c;
            c << camera_path << i << "_scale_" << scale_idx<< ".rpc";
            vil_save(*expected, s.str().c_str());
            // vpgl_rational_camera<double>* cam = static_cast<vpgl_rational_camera<double>*> (rat_cam.as_pointer());
            rat_cam->save(c.str().c_str());
        }
    }
    return true;
}


void gen_texture_map(vgl_box_3d<double> box,
                     vcl_vector<vcl_vector<float> >& intens_map_bt,
                     vcl_vector<vcl_vector<float> >& intens_map_side1,
                     vcl_vector<vcl_vector<float> >& intens_map_side2)
{
    // generate intensity maps
    intens_map_bt.resize(long(box.width()/8+1));
    intens_map_side1.resize(long(box.width()/8+1));
    intens_map_side2.resize(long(box.width()/8+1));

#ifdef DEBUG
    vcl_cout << box.width() << ' ' << box.depth() << ' ' << box.height() << vcl_endl;
#endif

    for (unsigned i=0; i<box.width()/8;i++) {
        intens_map_bt[i].resize(long(box.height()/8+1));
        for (unsigned j=0; j<box.height()/8;j++) {
            intens_map_bt[i][j] = float(rand() % 85)/255.0f;
            //intens_map_bt[i][j] = 1.0f;
        }
    }

    for (unsigned i=0; i<box.width()/8;i++) {
        intens_map_side1[i].resize(long(box.depth()/8+1));
        for (unsigned j=0; j<box.depth()/8;j++) {
            intens_map_side1[i][j] = float(rand() % 85)/255.0f + 0.4f;
            //intens_map_bt[i][j] = 1.0f;
        }
    }

    for (unsigned i=0; i<box.height()/8;i++) {
        intens_map_side2[i].resize(long(box.depth()/8+1));
        for (unsigned j=0; j<box.depth()/8;j++) {
            intens_map_side2[i][j] = float(rand() % 85)/255.0f + 0.7f;
            //intens_map_bt[i][j] = 1.0f;
            if (intens_map_side2[i][j] > 1.0f)
                intens_map_side2[i][j] = 0.99f;
        }
    }
}

void gen_voxel_world_2box(vgl_vector_3d<unsigned> grid_size,
                          vgl_box_3d<double> voxel_world,
                          bvxm_voxel_grid<float>* ocp_grid,
                          bvxm_voxel_grid<float>* intensity_grid)
{
    // fill with test data
    float init_val = 0.00;//0.01;
    ocp_grid->initialize_data(init_val);
    intensity_grid->initialize_data(init_val);

    bvxm_voxel_grid<float>::iterator ocp_slab_it;
    bvxm_voxel_grid<float>::iterator intensity_slab_it = intensity_grid->begin();

    //object (essentially two boxes) placed in the voxel world
    bvxm_util::generate_test_boxes<double>(40,40,10,140,140,23,nx,ny,nz,boxes);
    vcl_ofstream is("test_gen_synthetic_multi_scale_world/intensity_grid.txt");

    assert (boxes.size() == 2);

    vgl_box_3d<double> box=boxes[0], top_box = boxes[1];

    // generate intensity maps
    vcl_vector<vcl_vector<float> > intens_map_bt;
    vcl_vector<vcl_vector<float> > intens_map_side1;
    vcl_vector<vcl_vector<float> > intens_map_side2;
    gen_texture_map(box, intens_map_bt, intens_map_side1, intens_map_side2);

    vcl_vector<vcl_vector<float> > top_intens_map_bt;
    vcl_vector<vcl_vector<float> > top_intens_map_side1;
    vcl_vector<vcl_vector<float> > top_intens_map_side2;
    gen_texture_map(top_box, top_intens_map_bt, top_intens_map_side1, top_intens_map_side2);

    unsigned z=nz;
    for (ocp_slab_it = ocp_grid->begin();ocp_slab_it != ocp_grid->end();++ocp_slab_it,++intensity_slab_it)
    {
        --z;
        is << z << "--->" << vcl_endl;

        for (unsigned i=0; i<nx; i++)
        {
            is << vcl_endl;
            for (unsigned j=0; j<ny; j++)
            {
                int face1 = on_box_surface(box, vgl_point_3d<double>(i,j,z));
                int face2 = on_box_surface(top_box, vgl_point_3d<double>(i,j,z));
                // create a checkerboard intensity
                if ((face1 != -1) || (face2 != -1))
                {
                    if (face1 != -1) {
                        if (face1 == 0) {
                            int a = int(i-box.min_x())/16;
                            int b = int(j-box.min_y())/16;
                            (*intensity_slab_it)(i,j,0) = intens_map_bt[a][b];
                        }
                        else if (face1 == 1) {
                            int a = int(i-box.min_x())/16;
                            int b = int(z-box.min_z())/16;
                            (*intensity_slab_it)(i,j,0) = intens_map_side1[a][b];
                        }
                        else {
                            int a = int(j-box.min_y())/16;
                            int b = int(z-box.min_z())/16;
                            (*intensity_slab_it)(i,j,0) = intens_map_side2[a][b];
                        }
#ifdef DEBUG
                        vcl_cout << face1 << '=' << a << ' ' << b << vcl_endl;
#endif
                    }
                    else {
                        //(*intensity_slab_it)(i,j,0) = face_intens[5-face2];
                        if (face2 == 0) {
                            int a = int(i-top_box.min_x())/16;
                            int b = int(j-top_box.min_y())/16;
                            (*intensity_slab_it)(i,j,0) = top_intens_map_bt[a][b];
                        }
                        else if (face2 == 1) {
                            int a = int(i-top_box.min_x())/16;
                            int b = int(z-top_box.min_z())/16;
                            (*intensity_slab_it)(i,j,0) = top_intens_map_side1[a][b];
                        }
                        else {
                            int a = int(j-top_box.min_y())/16;
                            int b = int(z-top_box.min_z())/16;
                            (*intensity_slab_it)(i,j,0) = top_intens_map_side2[a][b];
                        }
                    }

                    (*ocp_slab_it)(i,j,0) = 1.0f;
                    is << " x" ;
                }
                else
                    is << " 0";
            }
        }
    }
    vcl_cout << "grid done." << vcl_endl;
}

bool test_reconstructed_ocp(dbvxm_multi_scale_voxel_world_sptr recon_world)
{
    //FIX IT: For now,the scale is 0
    bvxm_voxel_grid<float>* ocp_grid =
        dynamic_cast<bvxm_voxel_grid<float>*>(recon_world->get_grid<OCCUPANCY>(0,0).ptr());

    vxl_uint_32 nx = ocp_grid->grid_size().x();
    vxl_uint_32 ny = ocp_grid->grid_size().y();
    vxl_uint_32 nz = ocp_grid->grid_size().z();

    // iterate through slabs
    unsigned i = 60, j=60, k=nz;
    bvxm_voxel_grid<float>::iterator ocp_slab_it = ocp_grid->begin();
    for (ocp_slab_it = ocp_grid->begin(); ocp_slab_it != ocp_grid->end(); ++ocp_slab_it ) {
        k--;
        float ocp = (*ocp_slab_it)(i,j,0);
        vcl_cout << "z=" << k << "  " << ocp << vcl_endl;
        for (unsigned b=0; b<boxes.size(); b++) {
            if (on_box_surface(boxes[b], vgl_point_3d<double>(i,j,k)) != -1)
                vcl_cout << "ON " << b << vcl_endl;
        }
    }
    return true;
}


static void test_gen_synthetic_multi_scale_world()
{
    START("test_gen_synthetic_multi_scale_world test");

    // create the directory under build to put the intermediate files and the generated images
    vcl_string model_dir("./test_gen_synthetic_multi_scale_world");
    vul_file::make_directory(model_dir);

    vcl_string recon_model_dir("./recon_world");
    vul_file::make_directory(recon_model_dir);

    vul_file::make_directory("./test_gen_cameras");

    vgl_vector_3d<unsigned> grid_size(nx,ny,nz);
    vgl_box_3d<double> voxel_world(vgl_point_3d<double> (0,0,0),
        vgl_point_3d<double> (nx, ny, nz));

    bgeo_lvcs_sptr lvcs = new bgeo_lvcs();

    bvxm_world_params_sptr world_params = new bvxm_world_params();

    world_params->set_params("./test_gen_synthetic_multi_scale_world",
        vgl_point_3d<float> (0,0,0),
        vgl_vector_3d<unsigned int>(nx, ny, nz),
        vox_length,
        lvcs,
        0.001,
        0.999,
        3);

    dbvxm_multi_scale_voxel_world_sptr world = new dbvxm_multi_scale_voxel_world();
    world->set_params(world_params);
    world->clean_grids();

    unsigned int bin_num_1 = 0,bin_num_2 = 1;

    //test the get_grid method by calling with different scale indexes 
    bvxm_voxel_grid<float>* ocp_grid_scale_1 = static_cast<bvxm_voxel_grid<float>* >
        (world->get_grid<OCCUPANCY>(0,0).as_pointer());

    bvxm_voxel_grid<float>* ocp_grid_scale_2 = static_cast<bvxm_voxel_grid<float>* >
        (world->get_grid<OCCUPANCY>(0,2).as_pointer());

    bvxm_voxel_grid<apm_datatype>* apm_grid_1 = static_cast<bvxm_voxel_grid<apm_datatype>* >
        (world->get_grid<APM_MOG_GREY>(bin_num_1,0).as_pointer());

    bvxm_voxel_grid<apm_datatype>* apm_grid_2 = static_cast<bvxm_voxel_grid<apm_datatype>* >
        (world->get_grid<APM_MOG_GREY>(bin_num_2,2).as_pointer());

    bvxm_voxel_grid<float>* intensity_grid = new bvxm_voxel_grid<float>
        ("test_gen_synthetic_multi_scale_world/intensity.vox",grid_size);

    TEST("ocp_grid_scale_1", ocp_grid_scale_1 == NULL,false);
    TEST("ocp_grid_scale_2", ocp_grid_scale_2 == NULL,false);
    TEST("apm_grid_1", apm_grid_1 == NULL,false);
    TEST("apm_grid_2", apm_grid_2 == NULL,false);


    gen_voxel_world_2box(grid_size, voxel_world, ocp_grid_scale_1, intensity_grid);
    vcl_vector<vpgl_camera_double_sptr> cameras = generate_cameras_yz(voxel_world);

    vcl_vector <vil_image_view_base_sptr> image_set_1,image_set_2;

    // generate images from synthetic world
    gen_images(grid_size, world, intensity_grid, ocp_grid_scale_1, apm_grid_1,
        cameras, image_set_1, bin_num_1);

    //world->save_occupancy_raw("./test_gen_synthetic_multi_scale_world/ocp.raw");

    dbvxm_multi_scale_voxel_world_sptr recon_world = new dbvxm_multi_scale_voxel_world();

    bvxm_world_params_sptr recon_world_params = new bvxm_world_params();
    recon_world_params->set_params("./recon_world",vgl_point_3d<float> (0,0,0),
        vgl_vector_3d<unsigned int>(nx, ny, nz), vox_length,lvcs,0.001,0.999,3);

    recon_world->set_params(recon_world_params);
    recon_world->clean_grids();

    //reconstruct the world from synthetic images image_set_1 and the apm grid stored in bin_num_1
    reconstruct_world(recon_world,cameras, image_set_1,bin_num_1);
    /*recon_world->save_occupancy_raw("./recon_world/ocp1.raw",0);
    recon_world->save_occupancy_raw("./recon_world/ocp2.raw",1);
    recon_world->save_occupancy_raw("./recon_world/ocp3.raw",2);*/

    return;
}

TESTMAIN( test_gen_synthetic_multi_scale_world );





