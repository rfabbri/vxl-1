
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>

#include <psm/psm_scene_base.h>
#include <psm/psm_scene.h>
#include <psm/psm_sample.h>
#include <psm/psm_apm_traits.h>
#include <psm/algo/psm_update_parallel.h>
#include <psm/algo/psm_refine_scene.h>
#include <psm/algo/psm_render_expected_aa.h>

#include <psm_opt/psm_opt_generate_opt_samples.h>
#include <psm_opt/psm_opt_sample.h>
#include <psm_opt/psm_opt_bayesian_optimizer.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_vector.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_view_as.h>


//#define USE_COLOR
#ifdef USE_COLOR
#define APM_T  PSM_APM_SIMPLE_RGB
#define AUX_T  PSM_AUX_OPT_RGB
#else
#define APM_T PSM_APM_SIMPLE_GREY
#define AUX_T PSM_AUX_OPT_GREY
#endif


int main(int argc, char* argv[])
{
  // create and initialize scene
  vgl_point_3d<double> origin(-180.0, -160.0, -75.0);
  double block_len = 358.4;
#ifdef USE_COLOR
  vcl_string storage_dir = "c:/research/psm/models/capitol_high_bundle_rgb";
#else
  vcl_string storage_dir = "c:/research/psm/models/capitol_high_bundle";
#endif
  psm_scene<APM_T> scene(origin, block_len, storage_dir, 10);

#if 1
  for (int x_idx = -1; x_idx <= 1; ++x_idx) {
    for (int y_idx = -1; y_idx <=1; ++y_idx) {
      scene.init_block(vgl_point_3d<int>(x_idx,y_idx,0),6);
    }
  }
  // initialize with incremental update
  //initialize_with_incremental(scene, false);
#endif

  vcl_string image_set[] = { "_halfres", "_halfres", "_halfres", "", "", "", "", "", "", ""};

  for (unsigned int refine_i = 0; refine_i < 10; refine_i++) {

    vpgl_perspective_camera<double> vcam;
    //vcl_ifstream vcam_ifs("c:/research/data/CapitolSiteHigh/cameras_KRT/camera_00200.txt");
    vcl_ifstream vcam_ifs("c:/research/psm/output/capitol_flythrough/cameras_KRT/camera_00116.txt");
    if (!vcam_ifs.good()) {
      vcl_cerr << "error opening virtual camera file." << vcl_endl;
    }
    vcam_ifs >> vcam;

    if (refine_i > 0) {
      psm_refine_scene(scene, 0.20f, false);
    }

    unsigned int n_its[] = {5, 4, 3, 3, 3, 3, 3, 3, 3, 3};

    for (unsigned int it=0; it < n_its[refine_i]; ++it) {

      vcl_cout << "refine_i = " << refine_i << "  iteration " << it << vcl_endl;

      //psm_refine_scene(scene, 0.4f, false);
           
      vcl_vector<unsigned int> image_indices;
      vcl_string camera_dir = "c:/research/data/CapitolSiteHigh/cameras_KRT" + image_set[refine_i];
#ifdef USE_COLOR
      vcl_string image_dir = "c:/research/data/CapitolSiteHigh/video" + image_set[refine_i];
#else
      vcl_string image_dir = "c:/research/data/CapitolSiteHigh/video_grey" + image_set[refine_i];
#endif

      vcl_vector<vcl_string> edge_set_fnames;

      for (unsigned int c=0; c<255; c+=5) {
        image_indices.push_back(c);
      }

      vcl_vector<vcl_string> img_ids;

      vcl_vector<unsigned int>::const_iterator img_idx_it = image_indices.begin();
      for (; img_idx_it != image_indices.end(); ++img_idx_it) {

        // read camera
        vcl_stringstream camera_fname_ss;
        camera_fname_ss << camera_dir << "/camera_" << vcl_setw(5) << vcl_setfill('0') << *img_idx_it << ".txt";
        vcl_string camera_filename = camera_fname_ss.str();
        // read projection matrix from the file.
        vcl_ifstream ifs(camera_filename.c_str());
        if (!ifs.is_open()) {
          vcl_cerr << "Failed to open file " << camera_filename << vcl_endl;
          return false;
        }
        vpgl_perspective_camera<double> cam;
        ifs >> cam;

        // read image
        vcl_stringstream image_fname_ss;
        image_fname_ss << image_dir << "/frame_" << vcl_setw(5) << vcl_setfill('0') << *img_idx_it << ".png";
        vcl_string image_fname = image_fname_ss.str();

        vil_image_view_base_sptr img_base = vil_load(image_fname.c_str());
        vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(img_base.ptr());

#ifdef USE_COLOR
        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_datatype> img(img_base->ni(), img_base->nj(), 1);
        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_RGB>::obs_mathtype> img_planes = vil_view_as_planes(img);
        vil_convert_stretch_range_limited(*img_byte ,img_planes, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
#else
        vil_image_view<psm_apm_traits<PSM_APM_SIMPLE_GREY>::obs_datatype> img(img_base->ni(), img_base->nj(), 1);
        vil_convert_stretch_range_limited(*img_byte, img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
#endif

        // generate image edge set
        vcl_stringstream img_id;
        img_id << "_image_" << vcl_setw(5) << vcl_setfill('0') << *img_idx_it;
        vcl_cout << "generating image edge set: img_id = " << img_id.str() << vcl_endl;
        psm_opt_generate_opt_samples<APM_T, AUX_T>(scene, cam, img, img_id.str(), false);
        img_ids.push_back(img_id.str());
      }
      vcl_cout << "done with all images." << vcl_endl;

      psm_opt_bayesian_optimizer<APM_T, AUX_T> opt(scene, img_ids);
      //if (it == 0) {
      //  vcl_cout << "initializing cells." << vcl_endl;
      //  opt.init_cells();
      //}
      vcl_cout << "optimizing cells." << vcl_endl;
      opt.optimize_cells(0.6);

      // save a debug virtual image
      vcl_stringstream vimg_fname;
      vimg_fname << "c:/research/psm/output/expected_image_bundle_r" << vcl_setw(2) << vcl_setfill('0') << refine_i << "_i" << vcl_setw(2) << vcl_setfill('0') << it << ".tiff";
      vil_image_view<psm_apm_traits<APM_T>::obs_datatype> expected_img(1280,720,1);
      vil_image_view<float> mask_img(1280,720,1);
      psm_render_expected(scene, vcam, expected_img, mask_img);
      // convert output to an 8-bit image
      vil_image_view<vxl_byte> expected_byte(expected_img.ni(),expected_img.nj(), psm_apm_traits<APM_T>::obs_dim);
#ifdef USE_COLOR
      vil_image_view<psm_apm_traits<APM_T>::obs_mathtype> expected_img_planes = vil_view_as_planes(expected_img);
      vil_convert_stretch_range_limited(expected_img_planes, expected_byte, 0.0f, 1.0f, vxl_byte(0), vxl_byte(255));
#else
      vil_convert_stretch_range_limited(expected_img, expected_byte, 0.0f, 1.0f, vxl_byte(0), vxl_byte(255));
#endif
      vil_save(expected_byte,vimg_fname.str().c_str());

    }
  }

  return 0;
}



