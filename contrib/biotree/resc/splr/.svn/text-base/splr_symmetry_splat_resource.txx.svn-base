#if !defined(SPLR_SYMMETRY_SPLAT_RESOURCE_TXX_)
#define SPLR_SYMMETRY_SPLAT_RESOURCE_TXX_

#include <splr/splr_symmetry_splat_resource.h>

template <class T>
void splr_symmetry_splat_resource::set_symmetry(splr_symmetry * symmetry){
  symmetry_ = symmetry;
}

template <class T>
void splr_symmetry_splat_resource::initialize(const xscan_scan &scan,
      const xmvg_composite_filter_3d<T> &filter_3d){
  roster_ = symmetry_->roster();
  splat_collection_ 
     = new splr_symmetry_splat_collection(scan, filter_3d, roster_, symmetry_);
}
                         
template <class T>
  ~splr_symmetry_splat_resource(){
     delete splat_collection_;
     delete roster_;
     delete symmetry_;
}

#endif
