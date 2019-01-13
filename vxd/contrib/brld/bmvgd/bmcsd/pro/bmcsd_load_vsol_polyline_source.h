// This is bmcsd_load_vsol_polyline_source.h
#ifndef bmcsd_load_vsol_polyline_source_h
#define bmcsd_load_vsol_polyline_source_h
//:
//\file
//\brief Source process associated with binary vsol file
//\author Ricardo Fabbri (rfabbri), Brown University  (@gmail.com)
//\date 08/28/2009 04:12:36 PM PDT
//

#include <vul/vul_file.h>
#include <bsold/bsold_file_io.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>


//: bprod souce process to load vsol storage from a binary file.
//
class bmcsd_load_vsol_polyline_source : public bprod_source {
public:

  bmcsd_load_vsol_polyline_source( std::string fname ) 
    : fname_(fname),
      min_samples_(0),
      min_length_(0.0),
      use_length_(true)
  {
    vsl_add_to_binary_loader(vsol_polyline_2d());
  }

  //: Used to prune the curves by enforcing a minimum number of samples.
  void set_min_samples(unsigned m) { min_samples_ = m; use_length_ = false; }

  //: Used to prune the curves by enforcing a minimum number of samples.
  void set_min_length(double m) { min_length_ = m; use_length_ = true; }

  bprod_signal execute() {
    std::string ext = vul_file::extension(fname_);
    std::vector< vsol_spatial_object_2d_sptr > base;

    if (ext == ".vsl") {
      vsl_b_ifstream bp_in(fname_.c_str());
      if (!bp_in) {
        std::cout << " Error opening file  " << fname_ << std::endl;
        return BPROD_INVALID;
      }

      std::cout << "Opened vsl file " << fname_ <<  " for reading" << std::endl;

      vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
      output_vsol->b_read(bp_in);
      bp_in.close();
      base = output_vsol->all_data();
    } else {
      bool retval = bsold_load_cem(base, fname_);
      if (!retval) {
        return BPROD_INVALID;
      }
      std::cout << "Opened cemv file " << fname_ <<  " for reading" << std::endl;
    }

    std::vector< vsol_polyline_2d_sptr > curves;
    curves.reserve(base.size());

    // Cast everything to polyline

    for (unsigned i=0; i < base.size(); ++i) {
      vsol_polyline_2d_sptr
        p = dynamic_cast<vsol_polyline_2d *> (base[i].ptr());

      if (!p) {
        std::cerr << "Non-polyline found, but only POLYLINES supported!" << std::endl;
        return BPROD_INVALID;
      }

      bool include_curve = (use_length_)? (p->length() > min_length_) : (p->size() > min_samples_);

      if (include_curve)
        curves.push_back(p);
    }

    // The swap trick reduces the excess memory used by curves
    std::vector< vsol_polyline_2d_sptr >(curves).swap(curves);
    std::cout << "Curves: #curves =  " << curves.size() << std::endl;

    output(0, curves);
    return BPROD_VALID;
  }

private:
  std::string fname_;
  unsigned min_samples_;
  double min_length_;
  bool use_length_;
};

#endif // bmcsd_load_vsol_polyline_source_h

