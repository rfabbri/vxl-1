// This is mw_stereo_filter.h
#ifndef mw_stereo_filter_h
#define mw_stereo_filter_h
//:
//\file
//\brief Dbpro process for curve stereo
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 09/01/2009 08:31:17 AM PDT
//
#include <dbpro/dbpro_process.h>
#include <mw/mw_discrete_corresp.h>
#include <mw/dbmcs_curve_3d_attributes.h>
#include <mw/dbmcs_stereo_views_sptr.h>
#include <mw/algo/mw_odt_curve_stereo.h>
#include <mw/pro/dbpro_load_camera_source.h>
#include <mw/pro/dbpro_load_edg_source.h>
#include <mw/pro/dbpro_load_vsol_polyline_source.h>
#include <mw/pro/dbpro_fragment_tangents.h>

//: This process takes matches 2 views using a number of confirmation views.
// Inputs:
//
// Outputs:
// - 3D curve
// - 3D curve attributes
class dbmcs_stereo_filter : public dbpro_filter {
public:

  typedef enum { CAM_ID, EDG_ID, DT_ID, LBL_ID, CVLET_ID } confirmation_input_id;
  typedef enum { CAM_ID0, FRAG_ID0, TGT_ID0,  
                 CAM_ID1, FRAG_ID1, TGT_ID1 } keyframes_input_id;

  dbmcs_stereo_filter  () 
    : has_cvlet_(false)
  {}

  dbpro_signal execute() 
  {
    get_cameras();
    get_edgemaps();
    get_curves_and_tangents();
    get_dt_label();
    if (has_cvlet_)
      get_curvelets();

    vcl_vector<dbdif_1st_order_curve_3d> crv3d;
    vcl_vector< dbmcs_curve_3d_attributes > attr;
    mw_discrete_corresp corresp;

    // TODO: set inlier views.
    if (!dbmcs_match_and_reconstruct_all_curves_attr(s_, &crv3d, &corresp, &attr)) {
      vcl_cerr << "Error: while matching all views.\n";
      return DBPRO_INVALID;
    }

    //: Fill-in remaining of attributes
    set_remaining_attributes(&attr, crv3d, corresp);

    output(0, crv3d);
    output(1, attr);
    output(2, corresp);
    return DBPRO_VALID;
  }

  void setup_inputs(
        const dbmcs_stereo_views_sptr &views,
        vcl_vector<dbpro_process_sptr> &cam_src, 
        vcl_vector<dbpro_process_sptr> &edg_src, 
        vcl_vector<dbpro_process_sptr> &edg_dt, 
        vcl_vector<dbpro_process_sptr> &frag_src,
        vcl_vector<dbpro_process_sptr> &cvlet_src,
        vcl_vector<dbpro_process_sptr> &frag_tangents
        );

  mw_odt_curve_stereo s_;
  dbmcs_stereo_views_sptr v_;

private:
  unsigned sources_per_confirmation_view_;
  unsigned confirmation_view_input_offset_;
  bool has_cvlet_;

  //: constructs an attribute data structure for each 3D curve.
  void set_remaining_attributes(
      vcl_vector< dbmcs_curve_3d_attributes > *pattr, 
      const vcl_vector<dbdif_1st_order_curve_3d> &crv3d,
      const mw_discrete_corresp &/*corresp*/
      )
  {
    vcl_vector< dbmcs_curve_3d_attributes > &a = *pattr;

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

//: Outputs the concatenation of all inputs from many dbmcs_stereo_filter
class dbmcs_stereo_jobs: public dbpro_filter {
public:

  dbmcs_stereo_jobs(unsigned num_matchers) 
    :
    num_matchers_(num_matchers)
  {}

  dbpro_signal execute() 
  {
    vcl_vector< dbdif_1st_order_curve_3d > crv3d;
    vcl_vector< dbmcs_curve_3d_attributes > attr;
    vcl_vector<mw_discrete_corresp> corresp;

    unsigned num_curves=0;
    for (unsigned i=0; i < num_matchers_; ++i) {
      assert(input_type_id(3*i) == typeid(vcl_vector< dbdif_1st_order_curve_3d >));
      num_curves += input<vcl_vector< dbdif_1st_order_curve_3d > >(3*i).size();
    }

    crv3d.reserve(num_curves);
    attr.reserve(num_curves);

    corresp.reserve(num_matchers_);

    for (unsigned i=0; i < num_matchers_; ++i) {
      assert(input_type_id(3*i) == typeid(vcl_vector< dbdif_1st_order_curve_3d >));
      assert(input_type_id(3*i+1) == typeid(vcl_vector< dbmcs_curve_3d_attributes >));
      assert(input_type_id(3*i+2) == typeid(mw_discrete_corresp));
      const vcl_vector< dbdif_1st_order_curve_3d > &crv3d_i 
        = input<vcl_vector< dbdif_1st_order_curve_3d > >(3*i);

      const vcl_vector< dbmcs_curve_3d_attributes > attr_i
        = input<vcl_vector< dbmcs_curve_3d_attributes> >(3*i + 1);

      assert(attr_i.size() == crv3d_i.size());

      crv3d.insert(crv3d.end(), crv3d_i.begin(), crv3d_i.end());
      attr.insert(attr.end(), attr_i.begin(), attr_i.end());

      corresp.push_back(input< mw_discrete_corresp >(3*i + 2));
    }

    output(0, crv3d);
    output(1, attr);
    output(2, corresp);
    return DBPRO_VALID;
  }

private:
  unsigned num_matchers_;
};

//: Stores the concatenation of all inputs from many dbmcs_stereo_jobs
class dbmcs_stereo_aggregator : public dbpro_sink {
public:

  dbmcs_stereo_aggregator(unsigned num_jobs)
    : num_jobs_(num_jobs) { }

  dbpro_signal execute()
  {
    corresp_.reserve(num_jobs_);

    unsigned inputs_per_job = 3;
    unsigned num_curves=0, num_corr=0, num_attribs=0;
    for (unsigned i=0; i < num_jobs_; ++i) {
      assert(input_type_id(inputs_per_job*i) == typeid(vcl_vector< dbdif_1st_order_curve_3d >));
      assert(input_type_id(inputs_per_job*i+1) == typeid(vcl_vector< dbmcs_curve_3d_attributes >));
      assert(input_type_id(inputs_per_job*i+2) == typeid(vcl_vector< mw_discrete_corresp >));
      num_curves += input<vcl_vector< dbdif_1st_order_curve_3d > >(inputs_per_job*i).size();
      num_attribs += input<vcl_vector< dbmcs_curve_3d_attributes > >(inputs_per_job*i+1).size();
      assert (num_curves == num_attribs);
      num_corr +=  input<vcl_vector< mw_discrete_corresp > >(inputs_per_job*i + 2).size();
    }

    crv3d_.reserve(num_curves);
    attr_.reserve(num_curves);
    corresp_.reserve(num_corr);

    for (unsigned i=0; i < num_jobs_; ++i) {
      const vcl_vector< dbdif_1st_order_curve_3d > &crv3d_i 
        = input<vcl_vector< dbdif_1st_order_curve_3d > >(inputs_per_job*i);

      const vcl_vector< dbmcs_curve_3d_attributes > &attr_i
        = input<vcl_vector< dbmcs_curve_3d_attributes> >(inputs_per_job*i + 1);

      crv3d_.insert(crv3d_.end(), crv3d_i.begin(), crv3d_i.end());
      attr_.insert(attr_.end(), attr_i.begin(), attr_i.end());

      const vcl_vector< mw_discrete_corresp > &corr_i
        = input<vcl_vector<mw_discrete_corresp> > (inputs_per_job*i + 2);
      corresp_.insert(corresp_.end(), corr_i.begin(), corr_i.end());
    }

    return DBPRO_VALID;
  }

  //: Runs the process. This is set to run serially.
  dbpro_signal run(unsigned long timestamp,
                   dbpro_debug_observer* const debug = NULL);

  vcl_vector< dbdif_1st_order_curve_3d > crv3d_;
  vcl_vector< dbmcs_curve_3d_attributes > attr_;
  vcl_vector< mw_discrete_corresp > corresp_;

private:
  unsigned num_jobs_;
};

#endif // mw_stereo_filter_h
