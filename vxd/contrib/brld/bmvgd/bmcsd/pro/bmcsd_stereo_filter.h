// This is mw_stereo_filter.h
#ifndef mw_stereo_filter_h
#define mw_stereo_filter_h
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
// This is bmcsd_stereo_driver_e.h
#ifndef bmcsd_stereo_driver_e_h
#define bmcsd_stereo_driver_e_h
//:
//\file
//\brief Class to run bmcsd_*_curve_stereo in a multiview sequence
//\author Ricardo Fabbri, Brown University and Rio de Janeiro State U. (rfabbri.github.io)
//\date 08/12/2009 02:19:28 PM PDT
//

#include <bmcsd/bmcsd_curve_3d_attributes_e.h>
#include <bmcsd/bmcsd_curve_3d_sketch_e.h>
#include <bmcsd/algo/bmcsd_odt_curve_stereo_e.h>
#include <bmcsd/pro/bmcsd_stereo_filter.h>

//: Performs multiview stereo on a video sequence, by running many instances of
// two-view stereo in subsets of the frames.
class bmcsd_stereo_driver_e : public bmcsd_stereo_driver_base {
public: 
  //: The resulting 3D reconstruction after run();
  void get_curve_sketch(bmcsd_curve_3d_sketch_e *csk) const
    { csk->set(*crv3d_, *attr_); }

  //: The correspondence structures between the first two views, for each
  // instance (not necessarily in order of instances when run in parallel, but
  // you can recover the instance from the bmcsd_curve_3d_attributes).
  const bmcsd_discrete_corresp_e &corresp(unsigned i) const
    { return (*corresp_)[i]; }

  unsigned num_corresp() const
    { return corresp_->size(); }

  //: The number of computed results.
  unsigned num_outputted_data() const { 
      assert (corresp_->size() == crv3d_->size()); 
      assert (attr_->size() == corresp_->size()); 
      return attr_->size();
    }

protected:
  //: Outputs
  bmcsd_curve_3d_sketch_e csk_;
  std::vector< bmcsd_curve_3d_attributes_e > *attr_;
  std::vector<bmcsd_discrete_corresp_e> *corresp_;
};

class bmcsd_concurrent_stereo_driver_e : public bmcsd_stereo_driver_e {
public:
  bmcsd_concurrent_stereo_driver(
      const bmcsd_curve_stereo_data_path &dpath, 
      const bmcsd_stereo_instance_views &frames_to_match)
    :
      bmcsd_stereo_driver_e(dpath, frames_to_match),
      max_concurrent_matchers_(2)
  {
  }

  //: The maximum numner of matchers to run simultaneously. Roughly equals the
  // number of cores in a computer.
  void set_max_concurrent_matchers(unsigned n) { max_concurrent_matchers_ = n; }

  //: Initializes the processing, e.g. setting up a processing graph, computing
  // tangents in the curve fragments, etc.
  virtual bool init();

  //: Runs all instances of the 2-view matching and reconstruction
  virtual bool run(unsigned long timestamp=1);

  //: Updates the basic stereo processors with newly set parameters.
  // This must be called every time new parameters are set.
  void update_stereo_params();

private:
  unsigned max_concurrent_matchers_;

  //: pool of processors.
  std::vector<bprod_process_sptr> cam_src_;
  std::vector<bprod_process_sptr> edg_src_;
  std::vector<bprod_process_sptr> edg_dt_;
  std::vector<bprod_process_sptr> frag_src_;
  std::vector<bprod_process_sptr> cvlet_src_;
  std::vector<bprod_process_sptr> frag_tangents_;
  std::vector<bprod_process_sptr> curve_stereo_;
  std::vector<bprod_process_sptr> curve_stereo_jobs_;
  bprod_process_sptr output_job_;
};

#endif // bmcsd_stereo_driver_e_h
#include <bmcsd/pro/bmcsd_load_vsol_polyline_source.h>
#include <bmcsd/pro/bmcsd_fragment_tangents_filter.h>

//: This process takes matches 2 views using a number of confirmation views.
// Inputs:
//
// Outputs:
// - 3D curve
// - 3D curve attributes
class bmcsd_stereo_filter : public bprod_filter {
public:

  typedef enum { CAM_ID, EDG_ID, DT_ID, LBL_ID, CVLET_ID } confirmation_input_id;
  typedef enum { CAM_ID0, FRAG_ID0, TGT_ID0,  
                 CAM_ID1, FRAG_ID1, TGT_ID1 } keyframes_input_id;

  bmcsd_stereo_filter  () 
    : has_cvlet_(false)
  {}

  bprod_signal execute() 
  {
    get_cameras();
    get_edgemaps();
    get_curves_and_tangents();
    get_dt_label();
    if (has_cvlet_)
      get_curvelets();

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

  bmcsd_odt_curve_stereo s_;
  bmcsd_stereo_views_sptr v_;

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
                   bprod_debug_observer* const debug = NULL);

  std::vector< bdifd_1st_order_curve_3d > crv3d_;
  std::vector< bmcsd_curve_3d_attributes > attr_;
  std::vector< bmcsd_discrete_corresp > corresp_;

private:
  unsigned num_jobs_;
};

#endif // mw_stereo_filter_h
