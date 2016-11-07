// This is a contrib file that performs cartilage segmentation

//:
// \file
// \author Maruthi Narayanan (ntrinh@lems.brown.edu)
// \date   Nov 2, 2008
// \brief An algorithm to detect objects represented by extrinsic shock graph
// \verbatim
//    Modifications
// \endverbatim

#include <en250_project/detect_xgraph_app.h>
#include <dborl/algo/vox_detect_xgraph/vox_detect_xgraph_params.h>
#include <dborl/algo/vox_detect_xgraph/vox_detect_xgraph_params_sptr.h>

#include <dbsks/dborl_eval_det.h>
#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/dbsks_detect_xgraph.h>
#include <dbsks/xio/dbsks_xio_xshock_det.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/algo/dbsksp_screenshot.h>

#include <dborl/algo/dborl_xio_image_description.h>
#include <bpro1/bpro1_parameters.h>

#include <vsol/vsol_box_2d.h>


#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vnl/vnl_vector.h>
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <vul/vul_timer.h>



//------------------------------------------------------------------------------
//: Retrieved all input data from a parsed vox_detect_xgraph_params
bool get_input_data_from_params(const vox_detect_xgraph_params_sptr& params,
                                vcl_string& image_file,
                                vcl_string& xgraph_file,
                                vcl_string& xgraph_geom_file,
                                vcl_vector<double >& xgraph_scales,
                                int& detection_window_width,
                                int& detection_window_height)
{
    //>> Image file
    image_file = params->get_image_file(); 

    // check whether the file exists
    if (!vul_file::exists(image_file)) 
    {
        vcl_cout << "\nERROR: Couldn't find image file: " << image_file << "\n";
        return false;
    }


    //>> xgraph
    xgraph_file = params->get_xgraph_file();
    if (!vul_file::exists(xgraph_file)) 
    {
        vcl_cout << "\nERROR: Couldn't find xgraph file: " << xgraph_file << "\n";
        return false;
    }

    // scales
    xgraph_scales = params->get_xgraph_scales();
    if (xgraph_scales.empty())
    {
        vcl_cout << "Warning: no xgraph scale was chosen. Default scale will be used.\n";
    }


    //>> xgraph geometric model
    xgraph_geom_file = params->get_xgraph_geom_file();
    if (!vul_file::exists(xgraph_geom_file)) 
    {
        vcl_cout << "\nERROR: Couldn't find xgraph geomtric model file: " << xgraph_geom_file << "\n";
        return false;
    }


    //>> Detection window
    detection_window_width = params->detection_window_width_();
    detection_window_height = params->detection_window_height_();
    if (detection_window_width <= 0 || detection_window_height <= 0)
    {
        vcl_cout << "\nERROR: Detection window must have positive width and height.\n";
        return false;
    }
    return true;
}


// =============================================================================
//: VOX executable to detect objects, represented by xshock_graph, in images
int main(int argc, char *argv[]) 
{

    vul_timer timer;
    timer.mark();

    //1. Parse the input xml file /////////////////////////////////////////////

    // construct parameters with the default values;
    vox_detect_xgraph_params_sptr params = 
        new vox_detect_xgraph_params("vox_detect_xgraph");

    // parse the command line arguments
    if (!params->parse_command_line_args(argc, argv))
        return 1;

    // always print the params file if an executable to work 
    // with ORL web interface
    if (!params->print_params_xml(params->print_params_file()))
    {
        vcl_cout << "problems in writing params file to: " 
                 << params->print_params_file() << vcl_endl;
    }

    // exit if there is nothing else to do
    if (params->exit_with_no_processing() || params->print_params_only())
    {
        return 1;
    }

    //: always call this method to actually parse the input parameter file whose 
    // name is extracted from the command line
    if (!params->parse_input_xml())
    {
        return 1;
    }

    //>> Write input.xml file to output folder
    // filename format: input+applelogos_another.xml
    params->print_input_xml(params->get_output_folder() + "/" +
                            "input+" + params->input_object_name_() + ".xml");

    //////////////////////////////////////////////////////////////////////////////
    //2. Retrieve input data from parsed parameters
    vcl_string image_file;
    vcl_string xgraph_file;
    vcl_string xgraph_geom_file;
    vcl_vector<double > xgraph_scales;
    int det_window_width;
    int det_window_height;

    // parse everything in one function
    if (!get_input_data_from_params(params, image_file, xgraph_file, 
                                    xgraph_geom_file,   xgraph_scales, 
                                    det_window_width, det_window_height))
    {
        return EXIT_FAILURE;
    }


    //////////////////////////////////////////////////////////////////////////////
    //3. Detect objects in image
  
    // some announcements
    vcl_cout << "\n>>Run object detection on image " << 
        vul_file::strip_directory(image_file)<< " using xgraph model " 
             << vul_file::strip_directory(xgraph_file) << vcl_endl;

    vil_image_resource_sptr image_resource = 0;
    vcl_vector<dbsks_det_desc_xgraph_sptr > det_list;

    // Only use appearance cost
    bool success = detect_xgraph_using_app(image_file,   
                                           xgraph_file, 
                                           xgraph_geom_file, 
                                           xgraph_scales,
                                           det_window_width,
                                           det_window_height,
                                           det_list);


    if (!success)
    {

        vcl_cout << "\nERROR: Xgraph detection failed. No results created." 
                 << vcl_endl;
        return 1;

    }

    // sort the detection based on confidence
    vcl_sort(det_list.begin(), det_list.end(), dbsks_decreasing_confidence);
    vcl_cout << "\n  Final number of detections= " << det_list.size() << vcl_endl;

    /////////////////////////////////////////////////////////////////////////////
    // OUTPUT ///////////////////////////////////////////////////////////////////
    vcl_string output_file_prefix = params->get_output_folder() + "/";

    //>> Write detection results to a file in VOC2008 format
    vcl_string det_result_file = output_file_prefix + "VOC_det_result+" 
        + params->input_xgraph_name_()
        + "+" + params->input_object_name_() + ".txt";
    vcl_cout << "\n>>Save detection results in VOC2008 format to file " 
             << det_result_file << "...";
    {
        // \todo turn this into a function
        vcl_ofstream ofs(det_result_file.c_str(), vcl_ios::out);
        for (unsigned i =0; i < det_list.size(); ++i)
        {
            vsol_box_2d_sptr bbox = det_list[i]->bbox();
            ofs << params->input_object_name_() << " "
                << det_list[i]->confidence() << " "
                << bbox->get_min_x() << " "
                << bbox->get_min_y() << " "
                << bbox->get_max_x() << " "
                << bbox->get_max_y() << "\n";
        }
        ofs.close();
    }

    //////////////////////////////////////////////////////////////////////////////
    vcl_cout << "\n>>Save each individual detected xgraph to output folder " 
             << output_file_prefix << vcl_endl;

    // limit the number printed out
    unsigned num_sols = det_list.size();
    vcl_vector<vcl_string > xgraph_filenames(num_sols, "");
    vcl_vector<vcl_string > screenshot_filenames(num_sols, "");
  
    for (unsigned i =0; i < num_sols; ++i)
    {
        if (det_list[i]->is_a() != "dbsks_det_desc_xgraph")
        {
            vcl_cout << "\nWarning: detection " << i << " is not of type `dbsks_det_desc_xgraph'. No screenshot saved\n";
        }
        dbsks_det_desc_xgraph_sptr det = det_list[i];
        dbsksp_xshock_graph_sptr xgraph = det->xgraph();

        // shock graph file
        vcl_string output_xml_filename = vul_sprintf("%s+%s.%d.xml", params->input_xgraph_name_().c_str(),
                                                     params->input_object_name_().c_str(), 
                                                     i);
        vcl_string output_xml_file = output_file_prefix + output_xml_filename;


        vcl_cout << "\n  Saving xgraph file " << output_xml_file << "...";
        if (x_write(output_xml_file, xgraph))
        {
            vcl_cout << "done.\n";
        }
        else
        {
            vcl_cout << "failed.\n";
        }

        // // screenshot file
        // vcl_string output_screenshot_filename = vul_sprintf("%s+%s.%d.png", params->input_xgraph_name_().c_str(),
        //   params->input_object_name_().c_str(), i);
        // vcl_string output_screenshot_file = output_file_prefix + output_screenshot_filename;
        // vcl_cout << "\n  Saving screenshot file " << output_screenshot_file << "...";

        // // convert image_resource to image_view
        // vil_image_view<vxl_byte > source_image = *vil_convert_cast(vxl_byte(), image_resource->get_view());

        // // draw the shock graph on top of the source image
        // vil_image_view<vxl_byte > screenshot_image;
        // dbsksp_screenshot(source_image, xgraph, screenshot_image);

        // // save image
        // if (!vil_save(screenshot_image, output_screenshot_file.c_str()))
        // {
        //   vcl_cout << "failed.\n";
        // } 
        // else
        // {
        //   vcl_cout << "done.\n";
        // }

        // record the names
        xgraph_filenames[i] = output_xml_filename;
        // screenshot_filenames[i] = output_screenshot_filename;
    }





    //////////////////////////////////////////////////////////////////////////////
    // Print out each detection record to XML format

    vcl_string det_xml_record_file = output_file_prefix + "xml_det_record+" + params->input_xgraph_name_()
        + "+" + params->input_object_name_() + ".txt";

    vcl_cout << "\n>>Save detection results as XML record file " << det_xml_record_file << vcl_endl;
    {

        vcl_ofstream ofs(det_xml_record_file.c_str(), vcl_ios::out);
        for (unsigned i =0; i < det_list.size(); ++i)
        {
            ofs << "<xshock_det>\n";
            ofs << "  <object_name>" << params->input_object_name_() <<"</object_name>\n";
            ofs << "  <model_category>" << params->input_model_category_() <<"</model_category>\n";
            ofs << "  <screenshot>&lt;img src=\"" << "./" << screenshot_filenames[i] <<"\"&gt;</screenshot>\n";
            ofs << "  <xgraph_xml>\"" << xgraph_filenames[i] <<"\"</xgraph_xml>\n";
            ofs << "  <confidence>" << det_list[i]->confidence() <<"</confidence>\n";
            ofs << "  <xgraph_scale>" << vcl_sqrt(det_list[i]->xgraph()->area()) <<"</xgraph_scale>\n";
      

            vsol_box_2d_sptr bbox = det_list[i]->bbox();
            ofs << "  <bbox_xmin>" << bbox->get_min_x() <<"</bbox_xmin>\n"
                << "  <bbox_ymin>" << bbox->get_min_y() <<"</bbox_ymin>\n"
                << "  <bbox_xmax>" << bbox->get_max_x() <<"</bbox_xmax>\n"
                << "  <bbox_ymax>" << bbox->get_max_y() <<"</bbox_ymax>\n";
            ofs << "</xshock_det>\n";
        }
        ofs.close();


        // save the record with complete xml header
        vcl_vector<dbsks_xshock_det_record_sptr > xshock_det_list;
        for (unsigned i =0; i < det_list.size(); ++i)
        {
            dbsks_xshock_det_record_sptr record = dbsks_xshock_det_record_new();
      
            // fill in data for the record
            record->set_value("object_name", params->input_object_name_());
            record->set_value("model_category", params->input_model_category_());
      
            vcl_string screenshot_str = vcl_string("&lt;img src=\"") + "./" + //<img src=./"
                screenshot_filenames[i] + "\"&gt;"; //screenshot.png">
            record->set_value("screenshot", screenshot_str);
      
            record->set_value("xgraph_xml", "\"" + xgraph_filenames[i] + "\"");
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
        x_write(vul_file::strip_extension(det_xml_record_file) + ".xml", xshock_det_list);
    }
  

    //////////////////////////////////////////////////////////////////////////////
    //>>  Evaluate the detection results to plot the P-R curve
    vcl_cout <<">>EVALUATE DETECTION RESULTS\n";
  
    vcl_string image_desc_file = params->input_object_dir_() + "/" +
        params->input_object_name_() + ".xml";

    // check whether the file exists
    if (!vul_file::exists(image_desc_file)) 
    {
        vcl_cout << "\nERROR: Couldn't find image description file: " << image_desc_file << "\n";
        return 1;
    }

    // Load image description from file
    vcl_cout << "\n>>  Loading image description file" << image_desc_file << "...";
    dborl_image_description_sptr image_desc;
    if (!x_read(image_desc_file, image_desc))
    {
        vcl_cout << "Failed.\n";
        return 1;
    }
    else
    {
        vcl_cout << "Succeeded.\n";
    }

    // Evaluate the detection results against objects in the image description
    vcl_cout << "\n>>  Evaluate the detection results against ground truth data in image description file ...";
    double min_required_overlap = 0.5;
    vcl_vector<double > confidence_vec;
    vcl_vector<int > FP_vec;
    vcl_vector<int > TP_vec;
    int num_pos = -1;

    // Clone the detection list to its parent class
    vcl_vector<dborl_det_desc_sptr > borl_det_list;
    borl_det_list.reserve(det_list.size());
    for (unsigned i =0; i < det_list.size(); ++i)
    {
        borl_det_list.push_back(det_list[i].ptr());
    }
    dborl_VOC_eval_det(image_desc, params->input_model_category_(), min_required_overlap, borl_det_list,
                       confidence_vec, FP_vec, TP_vec, num_pos);
    vcl_cout << "done." << vcl_endl;

    //>> Print out the evaluation results (PR) curve
    vcl_string eval_file = output_file_prefix + "eval_det+" +
        params->input_xgraph_name_() + "+" + 
        params->input_object_name_() + ".txt";

    vcl_cout << "\n>> Print out evaluation results to file" << eval_file << vcl_endl;

    {
        vcl_ofstream ofs(eval_file.c_str(), vcl_ios::out);
        ofs << "num_pos " << num_pos << "\n";
        for (unsigned i =0; i < confidence_vec.size(); ++i)
        {
            ofs << params->input_object_name_() << " "
                << params->input_model_category_() << " "
                << confidence_vec[i] << " "
                << TP_vec[i] << " "
                << FP_vec[i] << "\n";
        }
        ofs.close();
    }

    // Display total time spent
    vcl_cout << "\n\n Total time spent for the task: " << timer.all() / 1000 << " seconds.\n" << vcl_endl;  return 0;
};

 
