// This is breye/bvis/displayer/bvis_image_displayer.cxx

#include <bvis/displayer/bvis_image_displayer.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <bgui/bgui_image_tableau.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vgui/vgui_range_map_params.h>

//: Create a tableau if the storage object is of type image
vgui_tableau_sptr
bvis_image_displayer::make_tableau( bpro_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into an image storage object
  vidpro_image_storage_sptr image_storage;
  image_storage.vertical_cast(storage);

  // Create a new image tableau
  bgui_image_tableau_sptr image_tab = bgui_image_tableau_new();
  
  if(update_image_tableau(image_tab, image_storage))
    return image_tab;
  return NULL;
 
}


  
//: Update a tableau with a the storage object if both are of type image
bool 
bvis_image_displayer::update_tableau( vgui_tableau_sptr tab, 
                                      const bpro_storage_sptr& storage ) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return false;
    
  bgui_image_tableau_sptr image_tab = dynamic_cast<bgui_image_tableau*>(tab.ptr());
  if(!image_tab)
    return false;

  
  // Cast the storage object into an image storage object
  vidpro_image_storage_sptr image_storage;
  image_storage.vertical_cast(storage);

  return update_image_tableau(image_tab, image_storage);
}


//: Perform the update once types are known
bool 
bvis_image_displayer::update_image_tableau( bgui_image_tableau_sptr image_tab, 
                                            const vidpro_image_storage_sptr& image_storage) const
{
  static double min = 0.0, max = 255;
  static float gamma = 1.0;
  static bool invert = false;
  static bool gl_map = false;
  static bool cache = false;
  
  // Create a new image tableau
  vil_image_resource_sptr image = image_storage->get_image();
  if (!image)
    return false;
    
  switch(image->pixel_format())
  {
  case VIL_PIXEL_FORMAT_BYTE: 
    {
      if(image->nplanes() > 3){
        vil_image_view<vxl_byte> byte_image = image->get_view();
        byte_image = vil_planes(byte_image,0,1,3);
        image = vil_new_image_resource_of_view(byte_image);
      }
      image_tab->set_image_resource(image);
      break;
    }
  case VIL_PIXEL_FORMAT_UINT_16:
    {
      image_tab->set_image_resource(image);
      max =4095;
      vgui_range_map_params *rmps_=
        new vgui_range_map_params(min, max, gamma, invert,
                                  gl_map, cache);  
    
      image_tab->set_mapping(rmps_);
      break;
    }
   
  case VIL_PIXEL_FORMAT_BOOL:
    {
      vil_image_view<bool> bool_image = image->get_view();
      vil_image_view<vxl_byte> byte_image;
      vil_convert_cast(bool_image, byte_image);
      vil_math_scale_values(byte_image,255.0);
      if(byte_image.nplanes() > 3)
        byte_image = vil_planes(byte_image,0,1,3);
      image = vil_new_image_resource_of_view(byte_image);
      image_tab->set_image_resource(image);
      break;
    }
 
  default:
    {
      // stretch and convert to a byte image for display
      vil_image_view<vxl_byte> byte_image = 
        *vil_convert_stretch_range(vxl_byte(), image->get_view());
      if(byte_image.nplanes() > 3)
        byte_image = vil_planes(byte_image,0,1,3);
      image = vil_new_image_resource_of_view(byte_image);
      image_tab->set_image_resource(image);
    }
  }
  
  return true;
}

