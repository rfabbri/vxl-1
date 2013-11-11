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

#include <mw/pro/dbpro_load_camera_source.h>
#include <mw/pro/dbpro_load_edg_source.h>
#include <mw/pro/dbpro_load_vsol_polyline_source.h>

template <class T>
class view_aggregator : public dbpro_sink
{
 public:
  view_aggregator() {} 

  //: Execute the process.
  // Input 0 : camera
  // Input 1 : edgemap
  // Input 2,3 : distance transform and label.
  dbpro_signal execute()
  {
    //: Camera
    assert(input_type_id(0) == typeid(vpgl_perspective_camera<T>));
    cam_ = &(input<vpgl_perspective_camera<T> >(0));

    //: Edge map
    assert(input_type_id(1) == typeid(dbdet_edgemap_sptr));
    em_ = input<dbdet_edgemap_sptr>(1);

    //: Distance transform of the pixelized edge map
    assert(input_type_id(2) == typeid(vil_image_view<vxl_uint_32>));
    dt_ = input<vil_image_view<vxl_uint_32> >(2);

    //: Label
    assert(input_type_id(3) == typeid(vil_image_view<unsigned>));
    label_ = input<vil_image_view<unsigned> >(3);

    //: Curve frags
    assert(input_type_id(4) == typeid(vcl_vector< vsol_polyline_2d_sptr >));
    curves_ = input<vcl_vector< vsol_polyline_2d_sptr > >(4);

    return DBPRO_VALID;
  }

  const vpgl_perspective_camera<T> *cam_;
  vil_image_view<vxl_uint_32> dt_;
  vil_image_view<unsigned> label_;
  dbdet_edgemap_sptr em_;
  vcl_vector< vsol_polyline_2d_sptr > curves_;
};

MAIN( test_multiview_dbpro_process )
{
  START ("multiview dbpro processes");

  vcl_string datapath("/usr/local/moredata/subset");
  vcl_string img_fname = datapath + vcl_string("/frame_00066.png");
  vcl_string edg_fname = datapath + vcl_string("/frame_00066-grad_thresh5.edg");
  vcl_string frag_fname = datapath + vcl_string("/frame_00066.vsl");

  // 1 Cam loader
  dbpro_process_sptr 
    cam_src = new dbpro_load_camera_source<double>(
        img_fname, dbpro_load_camera_source<double>::MW_INTRINSIC_EXTRINSIC);

  // 1 Edge map loader
  bool bSubPixel = true;
  double scale=1.0;
  dbpro_process_sptr 
    edg_src = new dbpro_load_edg_source(edg_fname, bSubPixel, scale);

  // 1 dt and label loader
  dbpro_process_sptr 
    edg_dt = new dbpro_edg_dt();

  edg_dt->connect_input(0, edg_src, 0);

  // 1 curve fragment loader
  dbpro_process_sptr
    frag_src = new dbpro_load_vsol_polyline_source(frag_fname);

  // Node that commands all others and collects their outputs
  view_aggregator<double> *out_ptr = new view_aggregator<double>;
  dbpro_process_sptr out(out_ptr);
  out->connect_input(0, cam_src, 0);
  out->connect_input(1, edg_src, 0);
  out->connect_input(2, edg_dt, 0);
  out->connect_input(3, edg_dt, 1);
  out->connect_input(4, frag_src, 0);

  out->run(1);

  vcl_cout << "========\nResulting Camera:\n\n";
  vcl_cout << *(out_ptr->cam_) << "\n";
  vcl_cout << vcl_endl;

  vcl_cout << "Resulting edgemap output:\n";
  vcl_cout << "Edgemap size: " <<  out_ptr->em_->num_edgels() << "\n\n\n";

  vcl_cout << "Resulting distance transforms:\n";
  vcl_cout << "DT : " <<  out_ptr->dt_ << "\n\n\n";
  vcl_cout << "Label : " <<  out_ptr->label_ << "\n\n\n";

  vil_image_view<vxl_byte> dt_byte, label_byte;

  vil_convert_cast(out_ptr->dt_, dt_byte);
  vil_convert_cast(out_ptr->label_, label_byte);

  vil_save(dt_byte,"test-dt.pbm");
  vil_save(label_byte,"test-label.pbm");

  vcl_cout << "Resulting curves:\n";
  vcl_cout << "Curve frags size: " <<  out_ptr->curves_.size() << "\n\n\n";

  SUMMARY();
}
