// This is breye1/bvis1/displayer/dbknee_dicom_image_displayer.cxx

#include "dbknee_dicom_image_displayer.h"
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <bgui/bgui_image_tableau.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vgui/vgui_range_map_params.h>

#include <vil/vil_math.h>



//: Perform the update once types are known
bool dbknee_dicom_image_displayer::
update_image_tableau( bgui_image_tableau_sptr image_tab, 
                     const vidpro1_image_storage_sptr& image_storage) const
{
  // special treatment for dicom images

  // check and map its dynamic range to [0 255]
  vil_image_resource_sptr image = image_storage->get_image();
  
  if (image->file_format() && (vcl_string(image->file_format()) == "dicom") && 
    image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    image_tab->set_image_resource(image);

    // create a range map for image_tab corresponding to the dynamic range of the dicom file
    float gamma = 1.0;
    bool invert = false;
    bool gl_map = false;
    bool cache = false;

    vxl_uint_16 min_value, max_value;

    // for large enough dicom image, ignore the boundary when computing the dynamic range
    // because they're often not accurate
    unsigned int i0 = 4;
    unsigned int j0 = 4;
    unsigned int ni = image->ni()-8;
    unsigned int nj = image->nj()-8;

    if (image->ni() <= 32 || image->nj() <= 32)
    {
      i0 = 0; j0 = 0; ni = image->ni(); nj = image->nj();
    }
    
    vil_image_view<vxl_uint_16 > image_view(image->get_view(i0, ni, j0, nj));
    vil_math_value_range(image_view, min_value, max_value);

    vgui_range_map_params *rmps_= new vgui_range_map_params(
      double(min_value), double(max_value), gamma, invert, gl_map, cache);  
    image_tab->set_mapping(rmps_);
    return true;
  }

  // for other formats, use the standard method
  return bvis1_image_displayer::update_image_tableau(image_tab, image_storage);
}

