// This is bmcsdstereo_filter_e.h
#ifndef bmcsdstereo_filter_e_h
#define bmcsdstereo_filter_e_h
//:
//\file
//\brief bprod process for curve stereo
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
#include <iomanip>

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

  void load_inputs(const bmcsd_stereo_views_sptr &views,
		   std::vector<bdifd_camera> &cams,
		   std::vector<sdet_edgemap_sptr> &em,
		   std::vector<std::vector< vsol_polyline_2d_sptr > > &curves,
		   std::vector<std::vector<std::vector<double> > > &tangents,
		   std::vector<vil_image_view<vxl_uint_32> > &dts,
		   std::vector<vil_image_view<unsigned> > &labels
		   )
  {
    v_ = views;
    s_->set_nviews(2 + v_->num_confirmation_views());

    s_->set_cams(cams);
    s_->set_all_edgemaps(em);
    s_->set_all_dt_label(dts, labels);

    //Anil: Record the original sizes of each image curve in each view
    //Also initialize the vector that stored flags for used curve samples
    std::vector<std::vector<unsigned> > original_sizes(s_->nviews());

    for(unsigned v=0; v<s_->nviews(); ++v){    
      original_sizes[v].resize(curves[v].size());
      for(unsigned c=0; c<curves[v].size(); ++c)
        original_sizes[v][c] = curves[v][c]->size();
    }

    std::vector<std::vector<std::vector<bool> > > usedSamples;
    usedSamples.resize(2);

    usedSamples[0].resize(curves[0].size());
    usedSamples[1].resize(curves[1].size());

    for(unsigned c=0; c<curves[0].size(); ++c){
      usedSamples[0][c].resize(curves[0][c]->size());
      std::fill(usedSamples[0][c].begin(), usedSamples[0][c].end(), false);
    }
    
    for(unsigned c=0; c<curves[1].size(); ++c){
      usedSamples[1][c].resize(curves[1][c]->size());
      std::fill(usedSamples[1][c].begin(), usedSamples[1][c].end(), false);
    }

    //s_->usedSamples_ = usedSamples;

    //std::vector<dbbl_subsequence_set> sseq;
    s_->set_curves(curves);
    s_->set_tangents(tangents);
    bmcsd_odt_curve_stereo_e *os = dynamic_cast<bmcsd_odt_curve_stereo_e *> (s_);
    os->set_original_curve_sizes(original_sizes);
    os->set_num_image_curves_v0(curves[0].size());
    //s_->break_into_episegs_and_replace_curve(&sseq);
    os->usedSamples_ = usedSamples;
    //s_->usedCurves_ = this->usedCurves_;

    /*std::vector<vsol_spatial_object_2d_sptr> brokenCurves_v0, brokenCurves_v1;

    for(unsigned i=0; i<s_->num_curves(0); ++i)
        brokenCurves_v0.push_back(dynamic_cast<vsol_spatial_object_2d*>(s_->curves(0,i).ptr()));
    dbsol_save_cem(brokenCurves_v0,std::string("after_breakup_v0.cemv"));

    for(unsigned i=0; i<s_->num_curves(1); ++i)
        brokenCurves_v1.push_back(dynamic_cast<vsol_spatial_object_2d*>(s_->curves(1,i).ptr()));
        dbsol_save_cem(brokenCurves_v1,std::string("after_breakup_v1.cemv"));  */

    //s_->set_sseq(sseq);
  }
    
  bprod_signal execute() override
  {
    /* Anil modified this part to be called by load_inputs directly in the
     * stereo driver
    bprod_signal sig = bmcsd_stereo_filter_base::execute();
    if (sig != BPROD_VALID)
      return sig;
    */

    //--------------------------------------------------------------------------

    get_edge_to_curve_index();

    //vul_timer breaking;
    std::vector<bbld_subsequence_set> sseq;
    s_->break_into_episegs_and_replace_curve(&sseq);
    bmcsd_odt_curve_stereo_e *os = dynamic_cast<bmcsd_odt_curve_stereo_e *> (s_);
    os->set_sseq(sseq);
    os->usedCurves_ = usedCurves_;
    os->matchCount_ = 0;
    os->reconCount_ = 0;

    //--------------------------------------------------------------------------

    std::vector<bdifd_1st_order_curve_3d> crv3d;
    std::vector< bmcsd_curve_3d_attributes_e > attr;
    bmcsd_discrete_corresp_e corresp;

    // std::cout << "#3a BREAKING: " << breaking.real() << std::endl;

    // TODO: set inlier views.
    if (!bmcsd_match_and_reconstruct_all_curves_attr_using_mates(
          *static_cast<bmcsd_odt_curve_stereo_e *>(s_), &crv3d, &corresp, &attr, mate_curves_v1_, isFirstRun_, true)) {
      std::cerr << "Error: while matching all views.\n";
      return BPROD_INVALID;
    }

    //: Fill-in remaining of attributes
    set_remaining_attributes(&attr, crv3d, corresp);

    //
    //    std::ofstream test_file;
    //    std::string test_fname = "num_operations.txt";
    //    test_file.open(test_fname.c_str(), std::ofstream::app);
    //    test_file << v_->stereo0() << " " << v_->stereo1() << " " << s_->matchCount_ << " " << s_->reconCount_ << std::endl;
    //    test_file.close();
      

    output(0, crv3d);
    output(1, attr);
    output(2, corresp);
    return BPROD_VALID;
  }

  //Anil: Alternative run function that does not work with inputs
  //This is to be used when inputs are given from the mcs executable
  bprod_signal run(unsigned long timestamp,
                  bprod_debug_observer* const debug = NULL) override;

  //Anil: Flag indicating whether this is an iteration run for elongation
  //True means it's the first pass, false means it's an iteration run
  bool isFirstRun_;
  //Anil: If this is an iteration run for elongation, mate curve IDs in v1()
  //for each image curve
  std::vector<std::set<int> > mate_curves_v1_;
  //Anil: Curve IDs that are already used in a previous run
  std::vector<std::vector<unsigned> > usedCurves_;
  //Anil: Count for the number of curve sample matching operations
  unsigned matchCount_;
  //Anil: Count for the number of curve sample reconstruction operations
  unsigned reconCount_;

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

  //Anil: Method to read in edge to curve association from a text file
  void get_edge_to_curve_index();
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

void bmcsd_stereo_filter_e::
get_edge_to_curve_index()
{
  std::cout << "Reading in edge-curve index" << std::endl;
  bmcsd_odt_curve_stereo_e *os = dynamic_cast<bmcsd_odt_curve_stereo_e *> (s_);
  os->edge_curve_index_.clear();
  os->edge_curve_index_.resize(v_->num_confirmation_views());

  for (unsigned i=0; i < v_->num_confirmation_views(); ++i) {
    std::string prefix = "./edge-curve/";
    std::ostringstream filestream;
    filestream << std::setw(8) << std::setfill('0'); // <view_i>.txt
    filestream << v_->confirmation_view(i);

    std::string filename = prefix + filestream.str() + std::string(".txt");

    std::ifstream file(filename.c_str());
    std::string line;
    getline(file,line);
    std::stringstream buffer(line);

    unsigned numEdges;
    buffer >> numEdges;              // line i --> confirmation view i [ numEdges edge_id1 edge_id12]
                                                    
    std::vector<int> curEdges;       // 

    for (unsigned e=0; e<numEdges; ++e)
    {
      int ID;
      buffer >> ID;
      curEdges.push_back(ID);
    }
    os->edge_curve_index_[i] = curEdges; // edge_curve_index[i] = std::std::vector encoding line i of txt
  }
}

//: Runs the filter
//Anil: Alternative run function that does not work with inputs
//This is to be used when inputs are given from the mcs executable
//
// this is just the code from bprod_filter::run 
// but with somethings removed and minor changes such as request_inputs,
bprod_signal
bmcsd_stereo_filter_e::run(unsigned long timestamp,
                  bprod_debug_observer* const debug)
{
  // notify the debugger if available
  if (debug) debug->notify_enter(this, timestamp);

  update_mutex_.lock();
  
  if(timestamp > this->timestamp_) {
    this->timestamp_ = timestamp;
    this->last_signal_ = BPROD_VALID;
    if (debug) {
      debug->notify_pre_exec(this);
      this->last_signal_ = this->execute();
      debug->notify_post_exec(this);
    }
    else
      this->last_signal_ = this->execute();
    this->notify_observers(this->last_signal_);
  }
  
  update_mutex_.unlock();

  // notify the debugger if available
  if (debug) debug->notify_exit(this, timestamp);

  return this->last_signal_;
}

#endif // bmcsdstereo_filter_e_h
