#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include <vil/vil_image_view.h>
#include <dbil/dbil_bounded_image_view.h>
#include <imgr/imgr_bounded_image_view_3d.h>
#include <imgr/imgr_scan_resource.h>
#include <xmvg/xmvg_perspective_camera.h>
#include <vpgl/algo/vpgl_project.h>

imgr_scan_resource::imgr_scan_resource(xscan_scan const& scan,
                                       vcl_vector<vil_image_resource_sptr> const& resources)
  : resources_(resources),scan_(scan)
{
  vil_image_resource_sptr r = resources_[0];
  if(!r)
    {
      ni_ = 0;
      nj_ = 0;
      nk_ = 0;
      nplanes_ = 1;
      format_ = VIL_PIXEL_FORMAT_UNKNOWN;
      return;
    }
  ni_ = r->ni();
  nj_ = r->nj();
  nk_ = resources.size();
  nplanes_ = r->nplanes();
  format_ = r->pixel_format();
}

//:Load the image data into a set of in-memory views that hold 
//just the bounded part of the image. 
imgr_image_view_3d_base_sptr imgr_scan_resource::
get_bounded_view(vgl_box_3d<double> const& bounds)
{
  unsigned i0, j0, imax, jmax, bni, bnj;

  //containers for the two likely image data types
  vcl_vector<dbil_bounded_image_view<unsigned char>* > char_views_2d;
  vcl_vector<dbil_bounded_image_view<unsigned short>* > short_views_2d;

  //Fill the view_3d with image_view_2d(s)
  for(orbit_index i=0; i<nk_; ++i)
    {
      //compute the image bounds for each image by projecting the 3-d box
      xmvg_perspective_camera<double> cam = scan_(i);
      //The file-based resource
      vil_image_resource_sptr r = resources_[i];
      vgl_box_2d<double> box_2d = 
        vpgl_project::project_bounding_box(cam, bounds);
      
      //extract the bounds
      // make sure that the bounds are not out of image plane, 
      // if so, crop the 2d box
      crop_bound_2d(box_2d, i0, j0, imax, jmax, r->ni(), r->nj());         
      bni = imax-i0+1;   bnj = jmax-j0+1;

      //check for the two reasonable formats, can add more later
      if(format_==VIL_PIXEL_FORMAT_BYTE)
        {
          vil_image_view<unsigned char> v = r->get_view(i0, bni, j0, bnj);
          dbil_bounded_image_view<unsigned char>* bview =
            new dbil_bounded_image_view<unsigned char>(v, i0, j0,
                                                      r->ni(), r->nj());
          char_views_2d.push_back(bview);
        }
      else if(format_==VIL_PIXEL_FORMAT_UINT_16)
        {
          vil_image_view<unsigned short> v = r->get_view(i0, bni, j0, bnj);
          dbil_bounded_image_view<unsigned short>* bview =
            new dbil_bounded_image_view<unsigned short>(v, i0, j0,
                                                      r->ni(), r->nj());
          short_views_2d.push_back(bview);
        }
    }
  //Return the appropriate 3-d image view
  if(format_==VIL_PIXEL_FORMAT_BYTE)
    return new imgr_bounded_image_view_3d<unsigned char>(char_views_2d, bounds); 
  else if(format_==VIL_PIXEL_FORMAT_UINT_16)
    return new imgr_bounded_image_view_3d<unsigned short>(short_views_2d, bounds);
  else
    return 0;

}

void imgr_scan_resource::crop_bound_2d(vgl_box_2d<double> const &box_2d, 
                                       unsigned &i0, unsigned &j0, 
                                       unsigned &imax, unsigned &jmax,
                                       unsigned int ni, unsigned int nj)
                                       
{
        
  //extract the bounds
  // make sure that the bounds are not out of image plane, 
  // if so, crop the 2d box
  if (box_2d.min_x() < 0){
    i0 = 0;
  } else if (box_2d.min_x() > ni) {
    i0 = ni-1;
  } else {
    i0 = (unsigned)box_2d.min_x(); 
  }

  if (box_2d.min_y() < 0){
    j0 = 0;
  } else if (box_2d.min_y() > nj) {
    j0 = (unsigned)nj-1;
  } else {
    j0 = (unsigned)box_2d.min_y(); 
  }

  if (box_2d.max_x() < 0){
    imax = 0;
  } else if (box_2d.max_x() > ni) {
    imax = (unsigned)ni-1;
  } else {
    imax = (unsigned)box_2d.max_x(); 
  }

  if (box_2d.max_y() < 0){
    jmax = 0;
  } else if (box_2d.max_y() > nj) {
    jmax = (unsigned)nj-1;
  } else {
    jmax = (unsigned)box_2d.max_y(); 
  }
}

