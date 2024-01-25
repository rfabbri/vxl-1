// This is bmcsdstereo_filter.h
#ifndef bmcsdstereo_filter_h
#define bmcsdstereo_filter_h
//:
//\file
//\brief Dbpro process for curve stereo
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date 09/01/2009 08:31:17 AM PDT
//
#include <bprod/bprod_process.h>
#include <bmcsd/bmcsd_discrete_corresp.h>
#include <bmcsd/bmcsd_curve_3d_attributes.h>
#include <bmcsd/bmcsd_stereo_views_sptr.h>
#include <bmcsd/algo/bmcsd_odt_curve_stereo.h>
#include <bmcsd/pro/bmcsd_load_camera_source.h>
#include <bmcsd/pro/bmcsd_load_edg_source.h>
#include <bmcsd/pro/bmcsd_load_vsol_polyline_source.h>
#include <bmcsd/pro/bmcsd_fragment_tangents_filter.h>
#include <bmcsd/pro/bmcsd_stereo_filter_base.h>

//: This process takes matches 2 views using a number of confirmation views.
// Inputs:
//
// Outputs:
// - 3D curve
// - 3D curve attributes
class bmcsd_stereo_filter : public bmcsd_stereo_filter_base {
public:

  bmcsd_stereo_filter() 
    bmcsd_stereo_filter_base(new bmcsd_stereo_odt_filter)
  {
  }

  bprod_signal execute() override
  {
    bprod_signal sig = bmcsd_stereo_filter_base::execute();
    if (sig != BBPROD_VALID)
      return sig;

    std::vector<bdifd_1st_order_curve_3d> crv3d;
    std::vector< bmcsd_curve_3d_attributes > attr;
    bmcsd_discrete_corresp corresp;
    // TODO: set inlier views.
    if (!bmcsd_match_and_reconstruct_all_curves_attr(s_, &crv3d, &corresp, &attr)) {
      std::cerr << "Error: while matching all views.\n";
      return BPROD_INVALID;
    }

    //: Fill-in remaining of attributes
    set_remaining_attributes(&attr, crv3d, corresp);

    output(0, crv3d);
    output(1, attr);
    output(2, corresp);
    return BPROD_VALID;
  }

  void setup_inputs(
        const bmcsd_stereo_views_sptr &views,
        std::vector<bprod_process_sptr> &cam_src, 
        std::vector<bprod_process_sptr> &edg_src, 
        std::vector<bprod_process_sptr> &edg_dt, 
        std::vector<bprod_process_sptr> &frag_src,
        std::vector<bprod_process_sptr> &cvlet_src,
        std::vector<bprod_process_sptr> &frag_tangents
        );

private:
  unsigned sources_per_confirmation_view_;
  unsigned confirmation_view_input_offset_;
  bool has_cvlet_;

  //: constructs an attribute data structure for each 3D curve.
  void set_remaining_attributes(
      std::vector< bmcsd_curve_3d_attributes > *pattr, 
      const std::vector<bdifd_1st_order_curve_3d> &crv3d,
      const bmcsd_discrete_corresp &/*corresp*/
      )
  {
    std::vector< bmcsd_curve_3d_attributes > &a = *pattr;

    assert(a.size() == crv3d.size());
    for (unsigned i=0; i < a.size(); ++i) {
      a[i].set_views(v_);
    }
  }

  void get_cameras();
  void get_edgemaps();
  void get_curves_and_tangents();
  void get_dt_label();
  void get_curvelets();
};

//: Outputs the concatenation of all inputs from many bmcsd_stereo_filter
class bmcsd_stereo_jobs: public bprod_filter {
public:

  bmcsd_stereo_jobs(unsigned num_matchers) 
    :
    num_matchers_(num_matchers)
  {}

  bprod_signal execute() 
  {
    std::vector< bdifd_1st_order_curve_3d > crv3d;
    std::vector< bmcsd_curve_3d_attributes > attr;
    std::vector<bmcsd_discrete_corresp> corresp;

    unsigned num_curves=0;
    for (unsigned i=0; i < num_matchers_; ++i) {
      assert(input_type_id(3*i) == typeid(std::vector< bdifd_1st_order_curve_3d >));
      num_curves += input<std::vector< bdifd_1st_order_curve_3d > >(3*i).size();
    }

    crv3d.reserve(num_curves);
    attr.reserve(num_curves);
    corresp.reserve(num_matchers_);
    for (unsigned i=0; i < num_matchers_; ++i) {
      assert(input_type_id(3*i) == typeid(std::vector< bdifd_1st_order_curve_3d >));
      assert(input_type_id(3*i+1) == typeid(std::vector< bmcsd_curve_3d_attributes >));
      assert(input_type_id(3*i+2) == typeid(bmcsd_discrete_corresp));
      const std::vector< bdifd_1st_order_curve_3d > &crv3d_i 
        = input<std::vector< bdifd_1st_order_curve_3d > >(3*i);

      const std::vector< bmcsd_curve_3d_attributes > attr_i
        = input<std::vector< bmcsd_curve_3d_attributes> >(3*i + 1);

      assert(attr_i.size() == crv3d_i.size());
      crv3d.insert(crv3d.end(), crv3d_i.begin(), crv3d_i.end());
      attr.insert(attr.end(), attr_i.begin(), attr_i.end());
      corresp.push_back(input< bmcsd_discrete_corresp >(3*i + 2));
    }

    output(0, crv3d);
    output(1, attr);
    output(2, corresp);
    return BPROD_VALID;
  }

private:
  unsigned num_matchers_;
};

//: Stores the concatenation of all inputs from many bmcsd_stereo_jobs
class bmcsd_stereo_aggregator : public bprod_sink {
public:

  bmcsd_stereo_aggregator(unsigned num_jobs)
    : num_jobs_(num_jobs) { }

  bprod_signal execute()
  {
    corresp_.reserve(num_jobs_);
    unsigned inputs_per_job = 3;
    unsigned num_curves=0, num_corr=0, num_attribs=0;
    for (unsigned i=0; i < num_jobs_; ++i) {
      assert(input_type_id(inputs_per_job*i) == typeid(std::vector< bdifd_1st_order_curve_3d >));
      assert(input_type_id(inputs_per_job*i+1) == typeid(std::vector< bmcsd_curve_3d_attributes >));
      assert(input_type_id(inputs_per_job*i+2) == typeid(std::vector< bmcsd_discrete_corresp >));
      num_curves += input<std::vector< bdifd_1st_order_curve_3d > >(inputs_per_job*i).size();
      num_attribs += input<std::vector< bmcsd_curve_3d_attributes > >(inputs_per_job*i+1).size();
      assert (num_curves == num_attribs);
      num_corr +=  input<std::vector< bmcsd_discrete_corresp > >(inputs_per_job*i + 2).size();
    }

    crv3d_.reserve(num_curves);
    attr_.reserve(num_curves);
    corresp_.reserve(num_corr);

    for (unsigned i=0; i < num_jobs_; ++i) {
      const std::vector< bdifd_1st_order_curve_3d > &crv3d_i 
        = input<std::vector< bdifd_1st_order_curve_3d > >(inputs_per_job*i);

      const std::vector< bmcsd_curve_3d_attributes > &attr_i
        = input<std::vector< bmcsd_curve_3d_attributes> >(inputs_per_job*i + 1);

      crv3d_.insert(crv3d_.end(), crv3d_i.begin(), crv3d_i.end());
      attr_.insert(attr_.end(), attr_i.begin(), attr_i.end());

      const std::vector< bmcsd_discrete_corresp > &corr_i
        = input<std::vector<bmcsd_discrete_corresp> > (inputs_per_job*i + 2);
      corresp_.insert(corresp_.end(), corr_i.begin(), corr_i.end());
    }

    return BPROD_VALID;
  }

  //: Runs the process. This is set to run serially.
  bprod_signal run(unsigned long timestamp,
                   bprod_debug_observer* const debug = NULL) override;

  std::vector< bdifd_1st_order_curve_3d > crv3d_;
  std::vector< bmcsd_curve_3d_attributes > attr_;
  std::vector< bmcsd_discrete_corresp > corresp_;

private:
  unsigned num_jobs_;
};

#endif // bmcsdstereo_filter_h
