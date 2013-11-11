#include "xmvg_composite_filter_descriptor.h"

xmvg_composite_filter_descriptor::xmvg_composite_filter_descriptor(vcl_vector<xmvg_filter_descriptor> const& fds)
:fds_(fds)

    {
calculate_bounding_box();
    }

// finding the convex union of the boxes in order to obtain the bounding box
// encompassing all the filters

void xmvg_composite_filter_descriptor::calculate_bounding_box(void)
{
  for ( unsigned int i = 0;i<fds_.size();i++)
  {
    bounding_box_.add(fds_[i].box());
  }
}
