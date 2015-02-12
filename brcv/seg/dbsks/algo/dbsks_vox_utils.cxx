// This is dbsks/algo/dbsks_vox_utils.cxx

//:
// \file

#include "dbsks_vox_utils.h"


#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/xio/dbsks_xio_xshock_det.h>

#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <bpro1/bpro1_parameters.h>

#include <vsol/vsol_box_2d.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_sprintf.h>


//------------------------------------------------------------------------------
//: Save a list of xshock detections to a folder
bool dbsks_save_detections_to_folder(const vcl_vector<dbsks_det_desc_xgraph_sptr >& det_list, 
                                     const vcl_string& image_name,
                                     const vcl_string& model_category,
                                     const vcl_string& det_group_id,
                                     const vil_image_view<vxl_byte >& source_image,
                                     const vcl_string& output_dir,
                                     const vcl_string& det_record_dirname)
{
  // limit the max number of screenshot to write out
  unsigned max_number_screenshots = 500;
  
  // location where all detected xgraphs and their screenshots will be saved
  vcl_string det_record_dir = output_dir + "/" + det_record_dirname;
  
  // create this folder if it has not been created yet
  if (!vul_file::is_directory(det_record_dir))
  {
    vul_file::make_directory(det_record_dir);
  }

  vcl_vector<vcl_string > xgraph_paths(det_list.size(), "" );
  for (unsigned i =0; i < det_list.size(); ++i)
  {
    if (det_list[i]->is_a() != "dbsks_det_desc_xgraph")
    {
      vcl_cout << "\nWarning: detection " << i << " is not of type `dbsks_det_desc_xgraph'. No screenshot saved\n";
    }
    dbsks_det_desc_xgraph_sptr det = det_list[i];
    dbsksp_xshock_graph_sptr xgraph = det->xgraph();

    vcl_string xgraph_filename = vul_sprintf("%s.%d.xml", det_group_id.c_str(), i);
    vcl_string xgraph_file = det_record_dir + "/" + xgraph_filename;

    vcl_cout << "\n  Saving xgraph file " << xgraph_file << "...";
    if (x_write(xgraph_file, xgraph))
    {
      vcl_cout << "done.\n";
    }
    else
    {
      vcl_cout << "failed.\n";
    }

    // record the names
    xgraph_paths[i] = "/" + det_record_dirname + "/" + xgraph_filename;
  }

 
  // limit the max number of screenshots to write out
  vcl_vector<vcl_string > screenshot_paths(det_list.size(), "");
  unsigned num_screenshots = vnl_math::min(unsigned(det_list.size()), max_number_screenshots);
  for (unsigned i =0; i < num_screenshots; ++i)
  {
    // screenshot file
    vcl_string screenshot_filename = vul_sprintf("%s.%d.png", det_group_id.c_str(), i);
    vcl_string screenshot_file = det_record_dir + "/" + screenshot_filename;
    vcl_cout << "\n  Saving screenshot file " << screenshot_file << "...";

    // draw the shock graph on top of the source image
    vil_image_view<vxl_byte > screenshot_image;

    vil_rgb<vxl_byte> contour_color(0, 0, 255); // blue
    vil_rgb<vxl_byte> padding_color(255, 255, 0); // yellow
    dbsksp_screenshot(source_image, det_list[i]->xgraph(), screenshot_image, 
      1, 1, contour_color, padding_color);

    // save image
    if (!vil_save(screenshot_image, screenshot_file.c_str()))
    {
      vcl_cout << "failed.\n";
    } 
    else
    {
      vcl_cout << "done.\n";
    }

    // record the names
    screenshot_paths[i] = "/" + det_record_dirname + "/" + screenshot_filename;
 
	// only save once
	if(source_image.nplanes()>1)
	{
	// save the binary shape, in case to use in latter dynamic programming for optimal sequence of pose tracking
		vil_image_view<vxl_byte > screenshot_binary;
		vcl_vector<vgl_point_2d<int > > region_pts;
		if(!dbsks_fill_in_silhouette(det_list[i]->xgraph(), source_image, region_pts, screenshot_binary))
			vcl_cout << "failed in fill the silhouette.\n";
		else
		{
			vcl_cout << "\n  Number of shape points: " << region_pts.size() << "...";	
			vcl_string screenshot_filename2 = vul_sprintf("%s.%d_binary.png", det_group_id.c_str(), i);
			vcl_string screenshot_file2 = det_record_dir + "/" + screenshot_filename2;
			vcl_cout << "\n  Saving screenshot file " << screenshot_file2 << "...";		
			if (!vil_save(screenshot_binary, screenshot_file2.c_str()))
			{
			  vcl_cout << "failed.\n";
			} 
			else
			{
			  vcl_cout << "done.\n";
			}


	/*
			vcl_string appear_id_filename1 = vul_sprintf("%s.%d_appearance_1.txt", det_group_id.c_str(), i);
			vcl_string appear_id_file1 = det_record_dir + "/" + appear_id_filename1;
			vcl_string appear_id_filename2 = vul_sprintf("%s.%d_appearance_2.txt", det_group_id.c_str(), i);
			vcl_string appear_id_file2 = det_record_dir + "/" + appear_id_filename2;
			// compute the appearance id matrix
			vcl_vector<vnl_matrix<double> > appearance_id_matrix = dbsks_compute_appearance_id_matrix( region_pts, source_image);

			vcl_ofstream ofs1(appear_id_file1.c_str()), ofs2(appear_id_file2.c_str());
			appearance_id_matrix[0].print(ofs1);
			appearance_id_matrix[1].print(ofs2);
			ofs1.close();
			ofs2.close();
	*/
		
		}
	}

  }


  //////////////////////////////////////////////////////////////////////////////
  // Print out each detection record to XML format

  vcl_string det_xml_record_file = output_dir + 
    "/" + ("xml_det_record+" + det_group_id + ".xml");

  vcl_cout << "\n>>Save detection results as XML record file " << det_xml_record_file << vcl_endl;
  {
    // Build a detection record list
    vcl_vector<dbsks_xshock_det_record_sptr > xshock_det_list;
    for (unsigned i =0; i < det_list.size(); ++i)
    {
      dbsks_xshock_det_record_sptr record = dbsks_xshock_det_record_new();
      
      // fill in data for the record
      record->set_value("object_name", image_name);
      record->set_value("model_category", model_category);
      
      vcl_string screenshot_str = vcl_string("&lt;img src=\"") + "." + //<img src=."
        screenshot_paths[i] + "\"&gt;"; //screenshot.png">
      record->set_value("screenshot", screenshot_str);
      
      record->set_value("xgraph_xml", "\"" + xgraph_paths[i] + "\"");
      record->set_value("confidence",  det_list[i]->confidence());
      record->set_value("xgraph_scale", vcl_sqrt(det_list[i]->xgraph()->area()));
      
      vsol_box_2d_sptr bbox = det_list[i]->bbox();
      record->set_value("bbox_xmin", bbox->get_min_x());
      record->set_value("bbox_ymin", bbox->get_min_y());
      record->set_value("bbox_xmax", bbox->get_max_x());
      record->set_value("bbox_ymax", bbox->get_max_y());
      
      // add record to the overall list
      xshock_det_list.push_back(record);
    }

    // write all detection to one xml file
    x_write(det_xml_record_file, xshock_det_list);
  }
  return true;
}


//------------------------------------------------------------------------------
//: Save a list of xshock detections to a folder
bool dbsks_load_detections_from_folder(const vcl_string& storage_folder,
                                       vcl_vector<dbsks_det_desc_xgraph_sptr >& records)
{
  records.clear();

  // Load detection list from existing folder
  vcl_string regexp = storage_folder + "/" + "xml_det_record+*.xml";
  int num_xml_det_record_files = 0;
  for (vul_file_iterator fn = regexp; fn; ++fn)
  {
    // increment the number of files
    ++num_xml_det_record_files;

    //: load a list of xshock detection
    vcl_vector<dbsks_xshock_det_record_sptr > det_record_list;
    if (x_read(fn(), det_record_list))
    {
      // construct detection descriptors from the records
      vcl_vector<dbsks_det_desc_xgraph_sptr > dets;
      dets.reserve(det_record_list.size());
      for (unsigned dd =0; dd < det_record_list.size(); ++dd)
      {
        dbsks_xshock_det_record_sptr det_record = det_record_list[dd];

        //> Retrieve the detection xgraph
        vcl_string det_xgraph_filename = "";
        det_record->get_value("xgraph_xml", det_xgraph_filename);

        // full path to the xgraph file
        vcl_string det_xgraph_file = storage_folder + "/" + det_xgraph_filename;

        // load the xgraph
        dbsksp_xshock_graph_sptr det_xgraph = 0;
        if (!x_read(det_xgraph_file, det_xgraph))
        {
          continue;
        }

        //> Retrieve confidence level
        double det_confidence; // note data type 
        det_record->get_value("confidence", det_confidence);

        dets.push_back(new dbsks_det_desc_xgraph(det_xgraph, det_confidence));
      } // for i - det_record_list
      records.insert(records.end(), dets.begin(), dets.end());      
    } // if x_read(...)
  } // file_iterator

  // return false if there is no xml_det_record_... files
  return (num_xml_det_record_files > 0);
}



