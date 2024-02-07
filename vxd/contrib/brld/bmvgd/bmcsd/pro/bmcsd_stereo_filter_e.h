// This is bmcsdstereo_filter_e.h
#ifndef bmcsdstereo_filter_e_h
#define bmcsdstereo_filter_e_h
//:
//\file
//\brief Dbpro process for curve stereo
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date 09/01/2009 08:31:17 AM PDT
//
#include <bprod/bprod_process.h>
#include <bmcsd/bmcsd_discrete_corresp_e.h>
#include <bmcsd/bmcsd_curve_3d_attributes_e.h>
#include <bmcsd/bmcsd_stereo_views_sptr.h>
#include <bmcsd/algo/bmcsd_odt_curve_stereo_e.h>
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
class bmcsd_stereo_filter_e : public bmcsd_stereo_filter_base {
public:

  bmcsd_stereo_filter_e() :
    bmcsd_stereo_filter_base(new bmcsd_odt_curve_stereo_e)
  {
  }


  bprod_signal execute() override
  {
    bprod_signal sig = bmcsd_stereo_filter_base::execute();
    if (sig != BPROD_VALID)
      return sig;

    std::vector<bdifd_1st_order_curve_3d> crv3d;
    std::vector< bmcsd_curve_3d_attributes_e > attr;
    bmcsd_discrete_corresp_e corresp;
    // TODO: set inlier views.
    if (!bmcsd_match_and_reconstruct_all_curves_attr_e(
          *static_cast<bmcsd_odt_curve_stereo_e *>(s_), &crv3d, &corresp, &attr)) {
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

private:
  //: constructs an attribute data structure for each 3D curve.
  void set_remaining_attributes(
      std::vector< bmcsd_curve_3d_attributes_e > *pattr, 
      const std::vector<bdifd_1st_order_curve_3d> &crv3d,
      const bmcsd_discrete_corresp_e &/*corresp*/
      )
  {
    std::vector< bmcsd_curve_3d_attributes_e > &a = *pattr;
    assert(a.size() == crv3d.size());
    for (unsigned i=0; i < a.size(); ++i) {
      a[i].set_views(v_);
    }
  }
};

//: Outputs the concatenation of all inputs from many bmcsd_stereo_filter_e
class bmcsd_stereo_jobs_e: public bprod_filter {
public:

  bmcsd_stereo_jobs_e(unsigned num_matchers) 
    :
    num_matchers_(num_matchers)
  {}

  bprod_signal execute() 
  {
    std::vector< bdifd_1st_order_curve_3d > crv3d;
    std::vector< bmcsd_curve_3d_attributes_e > attr;
    std::vector<bmcsd_discrete_corresp_e> corresp;

    unsigned num_curves=0;
    for (unsigned i=0; i < num_matchers_; ++i) {
      assert(input_type_id(3*i) == typeid(std::vector< bdifd_1st_order_curve_3d >));
      num_curves += input<std::vector< bdifd_1st_order_curve_3d > >(3*i).size();
    }

    crv3d.reserve(num_curves);
    attr.reserve(num_curves);
    corresp.reserve(num_matchers_);
    for (unsigned i=0; i < num_matchers_; ++i) {
      assert(input_type_id(3*i)   == typeid(std::vector< bdifd_1st_order_curve_3d >));
      assert(input_type_id(3*i+1) == typeid(std::vector< bmcsd_curve_3d_attributes_e >));
      assert(input_type_id(3*i+2) == typeid(bmcsd_discrete_corresp_e));
      const std::vector< bdifd_1st_order_curve_3d > &crv3d_i 
        = input<std::vector< bdifd_1st_order_curve_3d > >(3*i);

      const std::vector< bmcsd_curve_3d_attributes_e > attr_i
        = input<std::vector< bmcsd_curve_3d_attributes_e > >(3*i + 1);

      assert(attr_i.size() == crv3d_i.size());
      crv3d.insert(crv3d.end(), crv3d_i.begin(), crv3d_i.end());
      attr.insert(attr.end(), attr_i.begin(), attr_i.end());
      corresp.push_back(input< bmcsd_discrete_corresp_e >(3*i + 2));
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
class bmcsd_stereo_aggregator_e : public bmcsd_stereo_aggregator_base {
public:
  using bmcsd_stereo_aggregator_base::bmcsd_stereo_aggregator_base;

  bprod_signal execute()
  {
    corresp_.reserve(num_jobs_);
    unsigned inputs_per_job = 3;
    unsigned num_curves=0, num_corr=0, num_attribs=0;
    for (unsigned i=0; i < num_jobs_; ++i) {
      assert(input_type_id(inputs_per_job*i) == typeid(std::vector< bdifd_1st_order_curve_3d >));
      assert(input_type_id(inputs_per_job*i+1) == typeid(std::vector< bmcsd_curve_3d_attributes_e >));
      assert(input_type_id(inputs_per_job*i+2) == typeid(std::vector< bmcsd_discrete_corresp_e >));
      num_curves  += input<std::vector< bdifd_1st_order_curve_3d >  >(inputs_per_job*i).size();
      num_attribs += input<std::vector< bmcsd_curve_3d_attributes_e > >(inputs_per_job*i+1).size();
      assert (num_curves == num_attribs);
      num_corr +=  input<std::vector< bmcsd_discrete_corresp_e > >(inputs_per_job*i + 2).size();
    }

    crv3d_.reserve(num_curves);
    attr_.reserve(num_curves);
    corresp_.reserve(num_corr);
    for (unsigned i=0; i < num_jobs_; ++i) {
      const std::vector< bdifd_1st_order_curve_3d > &crv3d_i 
        = input<std::vector< bdifd_1st_order_curve_3d > >(inputs_per_job*i);

      const std::vector< bmcsd_curve_3d_attributes_e > &attr_i
        = input<std::vector< bmcsd_curve_3d_attributes_e> >(inputs_per_job*i + 1);

      crv3d_.insert(crv3d_.end(), crv3d_i.begin(), crv3d_i.end());
      attr_.insert(attr_.end(), attr_i.begin(), attr_i.end());

      const std::vector< bmcsd_discrete_corresp_e > &corr_i
        = input<std::vector<bmcsd_discrete_corresp_e> > (inputs_per_job*i + 2);
      corresp_.insert(corresp_.end(), corr_i.begin(), corr_i.end());
    }

    return BPROD_VALID;
  }

  std::vector< bmcsd_curve_3d_attributes_e > attr_;
  std::vector< bmcsd_discrete_corresp_e > corresp_;
};

#endif // bmcsdstereo_filter_e_h
