#include "imgr_scan_images_resource.h"
#include <dbil/dbil_bounded_image_view.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/algo/vpgl_project.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_image_resource.h>
#include "imgr_bounded_image_view_3d.h"

//:Load the image data into a set of in-memory views that hold 
//just the bounded part of the image. 
imgr_image_view_3d_base_sptr 
imgr_scan_images_resource:: get_bounded_view(vgl_box_3d<double> const& bounds) const
{
  //containers for the two likely image data types
  vcl_vector<dbil_bounded_image_view<unsigned char>* > char_views_2d;
  vcl_vector<dbil_bounded_image_view<unsigned short>* > short_views_2d;
  vcl_vector<dbil_bounded_image_view<double>* > double_view_2d;

  xscan_scan scan = get_scan();

  // FOR DEBUGGING
  // scan.set_scan_size(1);
  // FOR DEBUGGING

  vcl_vector<vil_image_resource_sptr> resources = get_images();

  unsigned long nk = scan.scan_size();
  enum vil_pixel_format format = VIL_PIXEL_FORMAT_UNKNOWN;
 
  //Fill the view_3d with image_view_2d(s)
  for(orbit_index i=0; i<nk; ++i)
    {
      //compute the image bounds for each image by projecting the 3-d box
      xmvg_perspective_camera<double> cam = scan(i);
      vgl_box_2d<double> box_2d = 
        vpgl_project::project_bounding_box(cam, bounds);
      //extract the bounds
      unsigned i0 = (unsigned)box_2d.min_x(); 
      unsigned j0 = (unsigned)box_2d.min_y();
      unsigned imax = (unsigned)box_2d.max_x(); 
      unsigned jmax = (unsigned)box_2d.max_y(); 
      unsigned bni = imax-i0+1;   unsigned bnj = jmax-j0+1;

      //The file-based resource
      vil_image_resource_sptr r = resources[i];

      format = r->pixel_format();
      //check for the two reasonable formats, can add more later
      if(format==VIL_PIXEL_FORMAT_BYTE)
        {
          vil_image_view<unsigned char> v = r->get_view(i0, bni, j0, bnj);
          dbil_bounded_image_view<unsigned char>* bview =
            new dbil_bounded_image_view<unsigned char>(v, i0, j0,
                                                      r->ni(), r->nj());
          char_views_2d.push_back(bview);
        }
      else if(format==VIL_PIXEL_FORMAT_UINT_16)
        {
          vil_image_view<unsigned short> v = r->get_view(i0, bni, j0, bnj);
          dbil_bounded_image_view<unsigned short>* bview =
            new dbil_bounded_image_view<unsigned short>(v, i0, j0,
                                                      r->ni(), r->nj());
          short_views_2d.push_back(bview);
        }
    }
  //Return the appropriate 3-d image view
  if(format==VIL_PIXEL_FORMAT_BYTE)
    return new imgr_bounded_image_view_3d<unsigned char>(char_views_2d, bounds); 
  else if(format==VIL_PIXEL_FORMAT_UINT_16)
    return new imgr_bounded_image_view_3d<unsigned short>(short_views_2d, bounds);
  else
    return 0;

}
