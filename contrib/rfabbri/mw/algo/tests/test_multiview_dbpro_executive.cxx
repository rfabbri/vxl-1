//:
//\file
//\brief File for me to experiment with dbpro.
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//
#include <testlib/testlib_test.h>
#include <dbpro/dbpro_process.h>
#include <vcl_iostream.h>
#include <dbpro/dbpro_observer.h>
#include <dbpro/tests/dbpro_sample_processes.h>
#include <vcl_vector.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>

#include <mw/algo/mw_data.h>
#include <mw/pro/dbpro_load_camera_source.h>
#include <mw/pro/dbpro_load_edg_source.h>
#include <mw/pro/dbpro_load_vsol_polyline_source.h>

class views_aggregator : public dbpro_sink
{
 public:

  typedef enum { CAM_ID, EDG_ID, DT_ID, LBL_ID, CRVS_ID } input_id;
  static const unsigned sources_per_view = 5;

  views_aggregator(unsigned nviews) : nviews_(nviews) {
    cam_.reserve(nviews_);
    dt_.reserve(nviews_);
    label_.reserve(nviews_);
    em_.reserve(nviews_);
    curves_.reserve(nviews_);
  }

  //: Execute the process.
  // Input 0 : camera
  // Input 1 : edgemap
  // Input 2,3 : distance transform and label.
  dbpro_signal execute()
  {
    for (unsigned i=0; i < nviews_; ++i) {
      //: Camera
      assert(input_type_id(i*sources_per_view +CAM_ID) == typeid(vpgl_perspective_camera<double>));
      cam_.push_back(input<vpgl_perspective_camera<double> >(CAM_ID));

      //: Edge map
      assert(input_type_id(i*sources_per_view +EDG_ID) == typeid(dbdet_edgemap_sptr));
      em_.push_back(input<dbdet_edgemap_sptr>(EDG_ID));

      //: Distance transform of the pixelized edge map
      assert(input_type_id(i*sources_per_view +DT_ID) == typeid(vil_image_view<vxl_uint_32>));
      dt_.push_back(input<vil_image_view<vxl_uint_32> >(DT_ID));

      //: Label
      assert(input_type_id(i*sources_per_view +LBL_ID) == typeid(vil_image_view<unsigned>));
      label_.push_back(input<vil_image_view<unsigned> >(LBL_ID));

      //: Curve frags
      assert(input_type_id(i*sources_per_view +CRVS_ID) == typeid(vcl_vector< vsol_polyline_2d_sptr >));
      curves_.push_back(input<vcl_vector< vsol_polyline_2d_sptr > >(CRVS_ID));
    }

    return DBPRO_VALID;
  }

  void setup_inputs(
      unsigned view,
      dbpro_process_sptr cam_src, 
      dbpro_process_sptr edg_src, 
      dbpro_process_sptr edg_dt, 
      dbpro_process_sptr frag_src) 
  {
    edg_dt->connect_input(0, edg_src, 0);
    connect_input(view*sources_per_view + CAM_ID, cam_src, 0);
    connect_input(view*sources_per_view + EDG_ID, edg_src, 0);
    connect_input(view*sources_per_view + DT_ID, edg_dt, 0);
    connect_input(view*sources_per_view + LBL_ID, edg_dt, 1);
    connect_input(view*sources_per_view + CRVS_ID, frag_src, 0);
  }

  unsigned nviews_;
  vcl_vector<vpgl_perspective_camera<double> > cam_;
  vcl_vector<vil_image_view<vxl_uint_32> > dt_;
  vcl_vector<vil_image_view<unsigned> > label_;
  vcl_vector<dbdet_edgemap_sptr > em_;
  vcl_vector<vcl_vector< vsol_polyline_2d_sptr > > curves_;
};

#define PARALLEL_RUN 1

#ifdef PARALLEL_RUN
MAIN( test_multiview_dbpro_process )
{
  START ("multiview dbpro processes");

  //: A rough estimate of how many cores to use at a time
  static const unsigned max_num_active_frames = 6;

  mw_curve_stereo_data_path dpath;
  mw_data::get_capitol_building_data_subset(&dpath);

  unsigned nviews =  vcl_min(dpath.nviews(), max_num_active_frames);
  assert(nviews != 0);


  dbpro_executive graph;
#ifndef NDEBUG
  graph.enable_debug();
#endif

  views_aggregator *out_ptr = new views_aggregator(nviews);
  graph["out"] = out_ptr;

  //: Setup the processing nodes
  for (unsigned v=0; v < nviews; ++v) {
    // Attach sources to files -----


    // XXX here then is how it is not clear how to assign a name easily.
    // 1 Cam loader
    dbpro_process_sptr 
      cam_src = new dbpro_load_camera_source<double>(
          dpath[v].cam_full_path(), dpath[v].cam_file_type());

    process_pool.push_back(cam_src);

    // 1 Edge map loader
    static const bool my_bSubPixel = true;
    static const double my_scale=1.0;
    dbpro_process_sptr 
      edg_src = new dbpro_load_edg_source(dpath[v].edg_full_path(), my_bSubPixel, my_scale);

    process_pool.push_back(edg_src);

    // 1 dt and label loader
    dbpro_process_sptr 
      edg_dt = new dbpro_edg_dt;

    process_pool.push_back(edg_dt);

    // 1 curve fragment loader
    dbpro_process_sptr
      frag_src = new dbpro_load_vsol_polyline_source(dpath[v].frag_full_path());

    process_pool.push_back(frag_src);

    // Connect inputs -----

    out_ptr->setup_inputs(v, cam_src, edg_src, edg_dt, frag_src);
  }

  out->run(1);

  for (unsigned v=0; v < nviews; ++v) {
    vcl_cout << "======== view[" << v << "]\nResulting Camera:\n\n";
    vcl_cout << out_ptr->cam_[v] << "\n";
    vcl_cout << vcl_endl;

    vcl_cout << "Resulting edgemap output:\n";
    vcl_cout << "Edgemap size: " <<  out_ptr->em_[v]->num_edgels() << "\n\n\n";

    vcl_cout << "Resulting distance transforms:\n";
    vcl_cout << "DT : " <<  out_ptr->dt_[v] << "\n\n\n";
    vcl_cout << "Label : " <<  out_ptr->label_[v] << "\n\n\n";

    bool write_image = false;
    if (write_image) {
      vil_image_view<vxl_byte> dt_byte, label_byte;

      vil_convert_cast(out_ptr->dt_[v], dt_byte);
      vil_convert_cast(out_ptr->label_[v], label_byte);

      vil_save(dt_byte,"test-dt.pbm");
      vil_save(label_byte,"test-label.pbm");
    }

    vcl_cout << "Resulting curves:\n";
    vcl_cout << "Curve frags size: " <<  out_ptr->curves_[v].size() << "\n\n\n";
  }

  SUMMARY();
}

#else // PARALLEL_RUN not defined

// Just to test serial loading:
MAIN( test_multiview_dbpro_process )
{
  mw_curve_stereo_data_path dpath;
  mw_data::get_capitol_building_data_subset(&dpath);

  static const bool my_bSubPixel = true;
  static const double my_scale=1.0;

  for (unsigned v=0; v < dpath.nviews(); ++v) {
    dbpro_process_sptr 
      edg_src = new dbpro_load_edg_source(dpath[v].edg_full_path(), my_bSubPixel, my_scale);

    // 1 Cam loader
    dbpro_process_sptr 
      cam_src = new dbpro_load_camera_source<double>(
          dpath[v].cam_full_path(), dpath[v].cam_file_type());

    cam_src->run(1);

    // 1 dt and label loader
    dbpro_process_sptr 
      edg_dt = new dbpro_edg_dt;
    edg_dt->connect_input(0, edg_src, 0);
    edg_dt->run(1);

    // 1 curve fragment loader
    dbpro_process_sptr
      frag_src = new dbpro_load_vsol_polyline_source(dpath[v].frag_full_path());
    frag_src->run(1);
  }
  SUMMARY();
}
#endif
