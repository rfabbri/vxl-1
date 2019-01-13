// This is bmcsd_load_cemv_source.h
#ifndef bmcsd_load_cemv_source_h
#define bmcsd_load_cemv_source_h
//:
//\file
//\brief Source process associated with binary vsol file
//\author Ricardo Fabbri (rfabbri), Brown University  (@gmail.com)
//\date 08/28/2009 04:12:36 PM PDT
//

#include <bsold/bsold_file_io.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>



//: bprod souce process to load vsol storage from a binary file.
//
class bmcsd_load_cemv_source : public bprod_source {
public:

  bmcsd_load_cemv_source( std::string fname ) 
    : fname_(fname),
      min_samples_(0)
  {
    vsl_add_to_binary_loader(vsol_polyline_2d());
  }

  //: Used to prune the curves by enforcing a minimum number of samples.
  void set_min_samples(unsigned m) { min_samples_ = m; }

  bprod_signal execute() {
    std::vector< vsol_spatial_object_2d_sptr > base;
    bsold_load_cem(contours, fname_);

    // create the output storage class
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

      if (p->size() >= min_samples_ ) 
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
};

#endif // bmcsd_load_cemv_source_h
