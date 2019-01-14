//:
//\file
//\brief File for me to experiment with bprod.
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//
#include <testlib/testlib_test.h>
#include <bprod/bprod_process.h>
#include <iostream>
#include <bprod/bprod_observer.h>
#include <bprod/tests/bprod_sample_processes.h>
#include <vector>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>

#include <mw/pro/bprod_load_camera_source.h>
#include <mw/pro/bprod_load_edg_source.h>
#include <mw/pro/bprod_load_vsol_polyline_source.h>

template <class T>
class view_aggregator : public bprod_sink
{
 public:
  view_aggregator() {} 

  //: Execute the process.
  // Input 0 : camera
  // Input 1 : edgemap
  // Input 2,3 : distance transform and label.
  bprod_signal execute()
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
    assert(input_type_id(4) == typeid(std::vector< vsol_polyline_2d_sptr >));
    curves_ = input<std::vector< vsol_polyline_2d_sptr > >(4);

    return BPROD_VALID;
  }

  const vpgl_perspective_camera<T> *cam_;
  vil_image_view<vxl_uint_32> dt_;
  vil_image_view<unsigned> label_;
  dbdet_edgemap_sptr em_;
  std::vector< vsol_polyline_2d_sptr > curves_;
};

MAIN( test_multiview_bprod_process )
{
  START ("multiview bprod processes");

  std::string datapath("/usr/local/moredata/subset");
  std::string img_fname = datapath + std::string("/frame_00066.png");
  std::string edg_fname = datapath + std::string("/frame_00066-grad_thresh5.edg");
  std::string frag_fname = datapath + std::string("/frame_00066.vsl");

  // 1 Cam loader
  bprod_process_sptr 
    cam_src = new bprod_load_camera_source<double>(
        img_fname, bprod_load_camera_source<double>::MW_INTRINSIC_EXTRINSIC);

  // 1 Edge map loader
  bool bSubPixel = true;
  double scale=1.0;
  bprod_process_sptr 
    edg_src = new bprod_load_edg_source(edg_fname, bSubPixel, scale);

  // 1 dt and label loader
  bprod_process_sptr 
    edg_dt = new bprod_edg_dt();

  edg_dt->connect_input(0, edg_src, 0);

  // 1 curve fragment loader
  bprod_process_sptr
    frag_src = new bprod_load_vsol_polyline_source(frag_fname);

  // Node that commands all others and collects their outputs
  view_aggregator<double> *out_ptr = new view_aggregator<double>;
  bprod_process_sptr out(out_ptr);
  out->connect_input(0, cam_src, 0);
  out->connect_input(1, edg_src, 0);
  out->connect_input(2, edg_dt, 0);
  out->connect_input(3, edg_dt, 1);
  out->connect_input(4, frag_src, 0);

  out->run(1);

  std::cout << "========\nResulting Camera:\n\n";
  std::cout << *(out_ptr->cam_) << "\n";
  std::cout << std::endl;

  std::cout << "Resulting edgemap output:\n";
  std::cout << "Edgemap size: " <<  out_ptr->em_->num_edgels() << "\n\n\n";

  std::cout << "Resulting distance transforms:\n";
  std::cout << "DT : " <<  out_ptr->dt_ << "\n\n\n";
  std::cout << "Label : " <<  out_ptr->label_ << "\n\n\n";

  vil_image_view<vxl_byte> dt_byte, label_byte;

  vil_convert_cast(out_ptr->dt_, dt_byte);
  vil_convert_cast(out_ptr->label_, label_byte);

  vil_save(dt_byte,"test-dt.pbm");
  vil_save(label_byte,"test-label.pbm");

  std::cout << "Resulting curves:\n";
  std::cout << "Curve frags size: " <<  out_ptr->curves_.size() << "\n\n\n";

  SUMMARY();
}
