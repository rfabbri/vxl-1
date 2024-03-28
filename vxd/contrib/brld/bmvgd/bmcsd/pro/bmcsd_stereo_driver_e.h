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
#include <bmcsd/pro/bmcsd_stereo_driver_base.h>

//: Performs multiview stereo on a video sequence, by running many instances of
// two-view stereo in subsets of the frames.
class bmcsd_stereo_driver_e : virtual public bmcsd_stereo_driver_base {
public: 
  bmcsd_stereo_driver_e(
      const bmcsd_curve_stereo_data_path &dpath, 
      const bmcsd_stereo_instance_views &frames_to_match)
    : bmcsd_stereo_driver_base(dpath, frames_to_match)
  {}

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
  //Anil: Flag indicating whether this is an iteration run for elongation
  //True means it's the first pass, false means it's an iteration run
  bool isFirstRun_;

  //Anil: If this is an iteration run for elongation, mate curve IDs is v1()
  std::vector<std::set<int> > mate_curves_v1_;

  //Anil: Flags for curves on the anchor views that are already used during a previous run
  std::vector<std::vector<unsigned> > usedCurves_;
};

class bmcsd_concurrent_stereo_driver_e : public bmcsd_concurrent_stereo_driver_base, public bmcsd_stereo_driver_e {
public:
  bmcsd_concurrent_stereo_driver_e(
      const bmcsd_curve_stereo_data_path &dpath, 
      const bmcsd_stereo_instance_views &frames_to_match)
    : // why repeated contructors: https://stackoverflow.com/a/9908015/1489510
      bmcsd_stereo_driver_base(dpath, frames_to_match), 
      bmcsd_concurrent_stereo_driver_base(dpath, frames_to_match),
      bmcsd_stereo_driver_e(dpath, frames_to_match)
  {
  }

  //: Anil: Setter for the mate curve id set
  void set_mate_curves_v1(std::vector<std::set<int> > mate_curves_v1)
  { mate_curves_v1_ = mate_curves_v1; }

  //: Anil: Setter for the used curve sample id vector
  void set_usedCurves(std::vector<std::vector<unsigned> > usedCurves)
  { usedCurves_ = usedCurves; }

  //: Anil: Setter for the iteration run flag
  void set_isFirstRun(bool isFirstRun)
  { isFirstRun_ = isFirstRun; }
  

  //: Initializes the processing, e.g. setting up a processing graph, computing
  // tangents in the curve fragments, etc.
  virtual bool init();


  //: Anil: Initializes the processing from precomputed data
  bool init(std::vector<bdifd_camera> &cams,
	    std::vector<sdet_edgemap_sptr> &em,
	    std::vector<std::vector< vsol_polyline_2d_sptr > > &curves,
	    std::vector<std::vector<std::vector<double> > > &tangents,
	    std::vector<vil_image_view<vxl_uint_32> > &dts,
	    std::vector<vil_image_view<unsigned> > &labels);

  //: Runs all instances of the 2-view matching and reconstruction
  virtual bool run(unsigned long timestamp=1);
};

#endif // bmcsd_stereo_driver_e_h
