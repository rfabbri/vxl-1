// This is 

// \author Nhon Trinh
// \date May 25, 2005

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <dbdet/xio/dbdet_xio_seg3d_info.h>



void test_xio_seg3d_info()
{
  //volume_segmentation_xml_parser parser;

  dbdet_seg3d_info_sptr s = new dbdet_seg3d_info();

  s->set_image_folder("D:/vision/data/971-tiff");
  s->set_contour_folder("D:/vision/data/971-con");

  dbdet_seg3d_info_frame frame1;
  frame1.image_file = "knee_971_011.tif";
  frame1.contour_file_list.push_back("971_con_011_0.con");
  frame1.contour_file_list.push_back("971_con_011_1.con");

  dbdet_seg3d_info_frame frame2;
  frame2.image_file = "knee_971_012.tif";
  frame2.contour_file_list.push_back("971_con_012_0.con");
  frame2.contour_file_list.push_back("971_con_012_1.con");

  s->add_frame(frame1);
  s->add_frame(frame2);

  vcl_string xmlname = "test_xml.xml";
  vcl_ofstream xml_file(xmlname.c_str());
  x_write(xml_file,s);
  xml_file.close();


  dbdet_seg3d_info_sptr s2 = new dbdet_seg3d_info();
  x_read(xmlname, s2);

  vcl_string xmlname2 = "test_xml_copy.xml";
  xml_file.open(xmlname2.c_str());
  x_write(xml_file, s2);
  xml_file.close();
}


MAIN( test_xml_write_read )
{
  START (" Test writing and reading an xml file");
  test_xio_seg3d_info();
  SUMMARY();
}

