// This is dbdet/xio/dbdet_xio_seg3d_info.cxx


#include "dbdet_xio_seg3d_info.h"

#include <vcl_cstdio.h>
#include <vsl/vsl_basic_xml_element.h>

#include <dbdet/xio/dbdet_xio_seg3d_info_parser.h>

// ============================================================================
//: parse an xml file
bool x_read(vcl_string fname, 
           const dbdet_seg3d_info_sptr& s) 
{
  vcl_FILE *xmlFile;

  if (fname.size() == 0)
  {
    vcl_cout << "File not specified" << vcl_endl;
    return false;
  }

  xmlFile = vcl_fopen(fname.c_str(), "r");
  if (!xmlFile)
  {
    vcl_cout << fname << "-- error on opening" << vcl_endl;
    return false;
  }

  dbdet_seg3d_info_xml_parser parser;
  parser.set_seg3d_info(s);

  if (!parser.parseFile(xmlFile)) 
  {
    vcl_cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << vcl_endl;
     return false;
  }
  return true;
}



// ============================================================================
//: write xml file
void x_write(vcl_ostream& os, 
             const dbdet_seg3d_info_sptr& s)
{
  vsl_basic_xml_element xml_element("volume_segmentation");
  xml_element.x_write_open(os);

  // image folder
  vsl_basic_xml_element image_folder_element("image_folder");
  image_folder_element.append_cdata(s->image_folder());
  image_folder_element.x_write(os);

  // contour folder
  vsl_basic_xml_element contour_folder_element("contour_folder");
  contour_folder_element.append_cdata(s->contour_folder());
  contour_folder_element.x_write(os);

  for (int i=0; i < s->num_frames(); ++i)
  {
    dbdet_seg3d_info_frame frame = s->frame(i);
    
    // start frame id
    vsl_basic_xml_element xml_frame("frame");
    xml_frame.add_attribute("frame_index", i);
    xml_frame.x_write_open(os);

    // image file
    vsl_basic_xml_element xml_image("image_file");
    xml_image.append_cdata(frame.image_file);
    xml_image.x_write(os);

    // contour files
    for (unsigned int k=0; k<frame.contour_file_list.size(); ++k)
    {
      vsl_basic_xml_element xml_contour("contour_file");
      xml_contour.add_attribute("contour_index", (int)k);
      xml_contour.append_cdata(frame.contour_file_list[k]);
      xml_contour.x_write(os);
    }

    xml_frame.x_write_close(os);
  }
  xml_element.x_write_close(os);
}

