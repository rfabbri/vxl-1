//:
// \file
// \author Maruthi Narayanan ( mn@lems.brown.edu )
// \date July 13th, 2009
// \brief An algorithm to detect bbox within model shock patchex to query
//        shock patches
//
//
// \verbatim
// \endverbatim


#include "vox_detect_shock_patches_params.h"
#include "vox_detect_shock_patches_params_sptr.h"
#include <dbskr/pro/dbskr_detect_shock_patches_process.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <dbxml/dbxml_algos.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vcl_cmath.h>

int main(int argc, char *argv[]) 
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    // construct parameters with the default values;
    vox_detect_shock_patches_params_sptr params = 
        new vox_detect_shock_patches_params("dborl_detect_shock_patches");  
  
    // parse the command line arguments
    if (!params->parse_command_line_args(argc, argv))
        return 1;

    //: always print the params file if an executable to work with ORL web 
    // interface
    if (!params->print_params_xml(params->print_params_file()))
    {
        vcl_cerr << "problems in writing params file to: " 
                 << params->print_params_file() << vcl_endl;
    }

    // exit if there is nothing else to do
    if (params->exit_with_no_processing() || params->print_params_only())
    {
        return 0;
    }

    //: always call this method to actually parse the input parameter file 
    // whose name is extracted from the command line
    if (!params->parse_input_xml())
    {
        return 1;
    }

    //Grab the query shock patch storage file
    vcl_string query_file =  params->query_object_dir_()+"/"+
        params->query_object_name_()+
        params->input_shock_patch_extension_()+"/"+
        params->query_object_name_()+
        "-patch_strg.bin";

    //Determine which input object we are going to use
    //Either from the input_object_dir or the associated file
    //The associated file always takes precendence
    vcl_string match_file;

    // Use associated file
    if ( vul_file::exists(params->input_assoc_dir_()))
    {
        // associated filename
        vcl_string assoc_filename;

        // Iterate over all files in directory
        vul_file_iterator fn(params->input_assoc_dir_()+"/*");
        for ( ; fn ; ++fn) 
        {
            //To deal with hidden files need to check if directories
            if (!vul_file::is_directory(fn.filename()))
            {
                assoc_filename=fn.filename();
            }
        }

        match_file = params->input_assoc_dir_() + "/" + assoc_filename;

    }
    else
    {
        // Use the database entries
        // Create a vul file iterator to find .bin file
        vcl_string glob=params->query_object_dir_()+"/"+"*"+
            params->query_object_name_()+"_match.bin";
        vul_file_iterator fn(glob);
        match_file = params->query_object_dir_()+"/"+fn.filename();
    }
    
    // Do some error checking
    bool match_status = vul_file::exists(match_file);
    if (!match_status ) 
    {
        vcl_cerr << "Cannot find shock patch match (.bin) file: " << 
            match_file << vcl_endl;

        return 1;
    }

    bool query_status = vul_file::exists(query_file);
    if ( !query_status ) 
    {
        vcl_cerr << "Cannot find query shock storage (.bin) file: " << 
            query_file << vcl_endl;

        return 1;
    }

    // Lets create directory of where output shock patches will go
    vcl_string output_file;
    if (params->save_to_object_folder_())
    { 
        output_file = params->output_shock_patch_detect_folder_() + "/";
    }
    else 
    {
        output_file = params->query_object_dir_() + "/";
    }

    if (!vul_file::exists(output_file)) 
    {
        vul_file::make_directory(output_file);
        
    }

    output_file += params->query_object_name_() + "_detect_results.xml";

    //******************** Extract Shock Patches ****************************
    vcl_cout<<"************  Detecting Shock Patches  *************"<<vcl_endl;
    
    // Set up process
    dbskr_detect_shock_patches_process detect_pro;

    // Set parameters of this process
    bpro1_filepath shock_patch_match(match_file,".bin");
    bpro1_filepath query_patch_storage(query_file,".bin");

    detect_pro.parameters()->set_value("-match_sim_filepath", 
                                       shock_patch_match);
    detect_pro.parameters()->set_value("-query_filepath", 
                                       query_patch_storage);
    detect_pro.parameters()->set_value("-top_N",params->model_N_());
    detect_pro.parameters()->set_value("-con_k",params->top_k_());

    
    double long  threshold_comp=    
        ((params->max_threshold_() - params->min_threshold_())/
         params->threshold_inc_())+1.0f;
    vcl_stringstream conv_stream;
    conv_stream<<threshold_comp;

    unsigned int num_thresholds;
    conv_stream>>num_thresholds;
    double threshold        = params->min_threshold_();
    double inc_threshold    = params->threshold_inc_();
    
    // create storage for all results
    // create a vector of vector of results
    vcl_vector< vcl_vector<bpro1_storage_sptr> > detect_results;

    // Store off thresholds
    vcl_vector<vcl_string> threshold_vec;

    unsigned int thresh_index(0);
    for ( ; thresh_index < num_thresholds ; ++thresh_index )
    {

        // Set the threshold of the process
        detect_pro.parameters()->set_value("-thres",threshold);

        // Before we start the process lets clean input output
        detect_pro.clear_input();
        detect_pro.clear_output();

        // No input needed just call process
        bool detect_status = detect_pro.execute();
        detect_pro.finish();
        if ( detect_status )
        {
            detect_results.push_back(detect_pro.get_output());
        }

        // Clean up after ourselves
        detect_pro.clear_input();
        detect_pro.clear_output();
  
        if ( !detect_status )
        {
            vcl_cerr<<" Error in shock patch detection!"<<vcl_endl;

        }
        
        // Push back a string
        vcl_stringstream str_stream;
        str_stream<<threshold;
        threshold_vec.push_back(str_stream.str());

        threshold += inc_threshold;
    }
 
    //******************** Write Detection Results ****************************
    vcl_cout<<"************  Write Detection Results  *************"<<vcl_endl;
   
    // Xml Tree: detect_results->query->threshold->bbox

    // Create root element
    bxml_document  doc;
    bxml_data_sptr root     = new bxml_element("detect_results");
    bxml_element*  root_elm = dbxml_algos::
        cast_to_element(root,"detect_results");
    doc.set_root_element(root_elm); 
    root_elm->append_text("\n   ");

    // Create query element
    bxml_data_sptr query     = new bxml_element("query");
    bxml_element*  query_elm = dbxml_algos::cast_to_element(query,"query");
    root_elm->append_data(query);
    root_elm->append_text("\n");
    query_elm->set_attribute("name",params->query_object_name_());
    query_elm->append_text("\n      ");

    // Set up coordinates
    vcl_string xmincoord("0");
    vcl_string ymincoord("0");
    vcl_string xmaxcoord("0");
    vcl_string ymaxcoord("0");
    
    // Loop over vector of bounding boxes
    unsigned int i(0);
    for ( ; i < detect_results.size() ; ++i)
    {

        bxml_data_sptr threshold     = new bxml_element("threshold");
        bxml_element*  threshold_elm = dbxml_algos::
            cast_to_element(threshold,"threshold");

        if ( i > 0 )
        {
            query_elm->append_text("   ");
        }
        query_elm->append_data(threshold);
        query_elm->append_text("\n   ");
        threshold_elm->set_attribute("value",threshold_vec[i]);
        
        bxml_data_sptr bbox = new bxml_element("bndbox");
        bxml_element *bbox_elm = dbxml_algos::cast_to_element(bbox,"bndbox");
        threshold_elm->append_text("\n         ");
        threshold_elm->append_data(bbox);
        
        if ( detect_results[i].size() )
        {
            vidpro1_vsol2D_storage_sptr bbox_vsol;
            bbox_vsol.vertical_cast(detect_results[i][0]);

            // parse through all the vsol classes and save only polygons
            // again we know the result will only be polygon 2d
            vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = 
                bbox_vsol->all_data();

            // Update this later to deal with mulitple detections
            // For now just pick the first element in the vector
            // Now grab each coordinate and convert to string stream
            vcl_stringstream xminstream;
            xminstream<<vsol_list[0]->get_min_x();
            xmincoord=xminstream.str();

            vcl_stringstream yminstream;
            yminstream<<vsol_list[0]->get_min_y();
            ymincoord=yminstream.str();

            vcl_stringstream xmaxstream;
            xmaxstream<<vsol_list[0]->get_max_x();
            xmaxcoord=xmaxstream.str();

            vcl_stringstream ymaxstream;
            ymaxstream<<vsol_list[0]->get_max_y();
            ymaxcoord=ymaxstream.str();
        }

        // Append coordinates
        bxml_data_sptr xmin = new bxml_element("xmin");
        bxml_data_sptr ymin = new bxml_element("ymin");
        bxml_data_sptr xmax = new bxml_element("xmax");
        bxml_data_sptr ymax = new bxml_element("ymax");
        
        bxml_element* xmin_elm=dbxml_algos::cast_to_element(xmin,"xmin");
        bxml_element* ymin_elm=dbxml_algos::cast_to_element(ymin,"ymin");
        bxml_element* xmax_elm=dbxml_algos::cast_to_element(xmax,"xmax");
        bxml_element* ymax_elm=dbxml_algos::cast_to_element(ymax,"ymax");

        bbox_elm->append_text("\n            ");
        bbox_elm->append_data(xmin);
        xmin_elm->append_text(xmincoord);
 
        bbox_elm->append_text("\n            ");
        bbox_elm->append_data(ymin);
        ymin_elm->append_text(ymincoord);
 
        bbox_elm->append_text("\n            ");
        bbox_elm->append_data(xmax);
        xmax_elm->append_text(xmaxcoord);

        bbox_elm->append_text("\n            ");
        bbox_elm->append_data(ymax);
        ymax_elm->append_text(ymaxcoord);
 
        bbox_elm->append_text("\n         ");

        threshold_elm->append_text("\n      ");

    }

    bxml_write(output_file,doc);

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;


    return 0;
}

