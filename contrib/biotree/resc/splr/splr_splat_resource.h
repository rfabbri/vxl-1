#if !defined(SPLR_SPLAT_RESOURCE_H_)
#define SPLR_SPLAT_RESOURCE_H_

#include <biob/biob_worldpt_roster.h>
#include <splr/splr_splat_collection.h>
#include <xscan/xscan_scan.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <biob/biob_worldpt_box.h>


template <class T>
class splr_splat_resource {
 public:
   biob_worldpt_roster * roster();  //should return type be const?
   splr_splat_collection * splat_collection();//should return type be const?
   virtual void initialize(const xscan_scan &scan,
      const xmvg_composite_filter_3d<T> &filter_3d) = 0;
 private:
   biob_worldpt_roster * roster_;
   splat_collection * splat_collection_;
};

#endif
