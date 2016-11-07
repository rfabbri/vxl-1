#ifndef dbil3d_cedt_h_
#define dbil3d_cedt_h_

#include <vil3d/vil3d_image_view.h>

//creates an unsigned distance transform from an input image 
//zero points are considered as sources, all other points as background
//the r,g,b planes of the offsets image are the x,y,z voxel offsets to the closest source
//(these values should be subtracted from an index to find the closest source)
//Note: requires single-plane contiguous image
template <class T>
void dbil3d_cedt(vil3d_image_view<T>& image, vil3d_image_view<int>& offsets, bool sqrt, bool get_offsets);

//creates a signed distance transform from an input image 
//zero points are considered as sources, all other points as background
//the r,g,b planes of the offsets image are the x,y,z voxel offsets to the closest source
//(these values should be subtracted from an index to find the closest source)
//Note: requires single-plane contiguous image
template <class T>
void dbil3d_signed_cedt(vil3d_image_view<T>& image, vil3d_image_view<int>& offsets, bool sqrt, bool get_offsets);

#endif
