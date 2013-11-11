//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date May 16, 2008
// \brief An algorithm to run shock computation on input vsols
//        This is a wrapper around 
//        brcv/shp/dbsk2d/pro/dbsk2d_compute_ishock_process
//
// \verbatim
//    Modifications
//      Nhon Trinh  Oct 24, 2008 : renamed from dborl_compute_ishock 
//                                 to vox_compute_ishock
//      Maruthi Narayanan June 07, 2009: Total rework
//                                 fixed to properly do shock computation
// \endverbatim


#include "vox_compute_ishock_params.h"
#include "vox_compute_ishock_params_sptr.h"
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vul/vul_file_iterator.h>
#include <bbas/bsol/bsol_algs.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vil/vil_load.h>

#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_gap_transform_process.h>
#include <dbsk2d/pro/dbsk2d_sample_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_save_esf_process.h>

int main(int argc, char *argv[]) 
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    // construct parameters with the default values;
    vox_compute_ishock_params_sptr params = 
        new vox_compute_ishock_params("dborl_compute_ishock");  
  
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

    //Determine which input object we are going to use
    //Either from the input_object_dir or the associated file
    //The associated file always takes precendence
    vcl_string input_vsol_fn;

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

        input_vsol_fn = params->input_assoc_dir_() + "/" + assoc_filename;

    }
    else
    {
        // Use the database entries
        input_vsol_fn = params->input_object_dir_() + "/" +
            params->input_object_name_() + params->input_contour_extension_();

    }

    if (!vul_file::exists(input_vsol_fn)) 
    {
        vcl_cerr << "Cannot find contour file: " << input_vsol_fn << vcl_endl;
        return 1;
    }

    vcl_string input_contour_extension = vul_file::extension(input_vsol_fn);

    // Create output storage
    vcl_vector<bpro1_storage_sptr> vsol_contour;

    //Call appropriate process to load file
    if ( input_contour_extension == ".cem" ||
         input_contour_extension == ".cemv" )
    {
        
        // Call vidpro load cem process
        vidpro1_load_cem_process load_pro_cem;

        bpro1_filepath input(input_vsol_fn,input_contour_extension);

        load_pro_cem.parameters()->set_value("-ceminput",input);

        // Before we start the process lets clean input output
        load_pro_cem.clear_input();
        load_pro_cem.clear_output();

        // Pass in input vsol string
        bool load_cem_status = load_pro_cem.execute();
        load_pro_cem.finish();

        // Grab output from symbolic edge linking
        if ( load_cem_status )
        {
            vsol_contour = load_pro_cem.get_output();
        }

        //Clean up after ourselves
        load_pro_cem.clear_input();
        load_pro_cem.clear_output();

    }
    else if ( input_contour_extension == ".con")
    {

        // Call vidpro load con process
        vidpro1_load_con_process load_pro_con;

        bpro1_filepath input(input_vsol_fn,input_contour_extension);

        load_pro_con.parameters()->set_value("-coninput",input);

        // Before we start the process lets clean input output
        load_pro_con.clear_input();
        load_pro_con.clear_output();

        // Pass in input vsol string
        bool load_con_status = load_pro_con.execute();
        load_pro_con.finish();

        // Grab output from symbolic edge linking
        if ( load_con_status )
        {
            vsol_contour = load_pro_con.get_output();
        }

        //Clean up after ourselves
        load_pro_con.clear_input();
        load_pro_con.clear_output();

    }
    else
    {

        vcl_cerr << "Unknown input type: " << 
            input_contour_extension << " Quit now" <<vcl_endl;
        return 1;

    }


    if ( vsol_contour.size() != 1)
    {
        vcl_cerr<<"Problem loading file, "<<input_vsol_fn<<
            "Could not get vsol data structure"<<vcl_endl;
        
        return 1;
    }

   
    // Grab the underlying contours
    vidpro1_vsol2D_storage_sptr vsol_contour_storage = 
        vidpro1_vsol2D_storage_new();
    vsol_contour_storage.vertical_cast(vsol_contour[0]);

    if ( params->add_bbox_())
    {
        vcl_cout<<"************  Compute  Bbox  *************"<<vcl_endl;

        // Grab the underlying contours
        vidpro1_vsol2D_storage_sptr vsol_contour_storage;
        vsol_contour_storage.vertical_cast(vsol_contour[0]);

        // create new bounding box
        vsol_box_2d_sptr bbox = new vsol_box_2d();

        // Determine largest bounding box 
        vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = 
            vsol_contour_storage->all_data();
    
        for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
        {
            bbox->grow_minmax_bounds(vsol_list[b]->get_bounding_box());
        }

        // Enlarge bounding box from size
        // Calculate xcenter, ycenter
        double xcenter = bbox->width()/2.0;
        double ycenter = bbox->height()/2.0;

        // Translate to center and scale
        double xmin_scaled = ((bbox->get_min_x()-xcenter)*2)+xcenter;
        double ymin_scaled = ((bbox->get_min_y()-ycenter)*2)+ycenter;
        double xmax_scaled = ((bbox->get_max_x()-xcenter)*2)+xcenter;
        double ymax_scaled = ((bbox->get_max_y()-ycenter)*2)+ycenter;
        
        bbox->add_point(xmin_scaled,ymin_scaled);
        bbox->add_point(xmax_scaled,ymax_scaled);

        vcl_cout << "bbox (minx, miny) (maxx, maxy) (width, height): " 
                 << "("   << bbox->get_min_x() << ", " << bbox->get_min_y() 
                 << ") (" << bbox->get_max_x() << ", " << bbox->get_max_y() 
                 << ") (" 
                 << bbox->width() << ", " 
                 << bbox->height() << ")"<<vcl_endl;
 
        // Add to vidpro storage this new bounding box
        vsol_polygon_2d_sptr box_poly = bsol_algs::poly_from_box(bbox);
        vsol_contour_storage->add_object(box_poly->cast_to_spatial_object());

    }

    
    //******************** IShock Computation *******************************
    vcl_cout<<"************ Computing Shock *************"<<vcl_endl;

    dbsk2d_compute_ishock_process shock_pro;
    set_process_parameters_of_bpro1(*params, 
                                    shock_pro, 
                                    params->tag_compute_ishock_);  
    
    // Before we start the process lets clean input output
    shock_pro.clear_input();
    shock_pro.clear_output();

    // Create empty image stroage
    vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();

    // Use input from edge detection
    shock_pro.add_input(image_storage);
    shock_pro.add_input(vsol_contour_storage);
    bool ishock_status = shock_pro.execute();
    shock_pro.finish();

    // Grab output from symbolic edge linking
    vcl_vector<bpro1_storage_sptr> shock_results;

    // If ishock status is bad we will keep iterating with noise till we get 
    // a valid shock computation otherwise call it quits
    if (!ishock_status)
    {
        // Add noise to parameter set
        shock_pro.parameters()->set_value("-b_noise",true);

        // Clean up before we start running
        shock_pro.clear_input();
        shock_pro.clear_output();

        unsigned int i(0);
        unsigned int num_iterations = params->num_iter_();

        for ( ; i < num_iterations; ++i)
        {
            vcl_cout<<vcl_endl;
            vcl_cout<<"************ Retry Compute Shock,iter: "
                    <<i+1<<" *************"<<vcl_endl;
          
            // Add inputs
            shock_pro.add_input(image_storage);
            shock_pro.add_input(vsol_contour_storage);

            // Kick off process again
            ishock_status = shock_pro.execute();
            shock_pro.finish();

            if ( ishock_status )
            {
                // We have produced valid shocks lets quit
                break;
                
            }

            // Clean up after ourselves
            shock_pro.clear_input();
            shock_pro.clear_output();

        }
    }
        
    if ( ishock_status )
    {
        shock_results = shock_pro.get_output();

        // Clean up after ourselves
        shock_pro.clear_input();
        shock_pro.clear_output();

    }

   
    if (shock_results.size() != 1) 
    {
        vcl_cerr << "Shock computation failed after "<<params->num_iter_()
                 <<" iterations"
                 << vcl_endl;
        return 1;
    }

    //********************   Gap Transform    *******************************

    // Perform Gap Transform on Intrinsinc Shock Graph
    vcl_vector<bpro1_storage_sptr> shock_gt_results;

    if ( params->gap_transform_())
    {

        vcl_cout<<"************  Gap Transform  *************"<<vcl_endl;

        dbsk2d_gap_transform_process gt_pro;
        set_process_parameters_of_bpro1(*params, 
                                        gt_pro, 
                                        params->tag_gap_transform_);


        // Before we start the process lets clean input output
        gt_pro.clear_input();
        gt_pro.clear_output();

        // Grab the underlying contours
        dbsk2d_shock_storage_sptr output_shock = dbsk2d_shock_storage_new();
        output_shock.vertical_cast(shock_results[0]);

        // Open up image for gap transform
 
        //load the input image
        vcl_string input_img = params->input_object_dir_() + "/" 
            + params->input_object_name_() + params->input_image_extension_();

        if (!vul_file::exists(input_img)) 
        {
            vcl_cerr << "Cannot find image for gap transform: " 
                     << input_img << vcl_endl;
            return 1;
        }

        // Grab image
        vil_image_resource_sptr img_sptr = 
            vil_load_image_resource(input_img.c_str());
        
        if (!img_sptr) 
        {
            vcl_cerr << "Cannot load image for gap transform: " << 
                input_img << vcl_endl;
            return 1;
        }
        
        output_shock->set_image(img_sptr);
        
        // Use input from ishock computation
        gt_pro.add_input(output_shock);
        bool gt_status = gt_pro.execute();
        gt_pro.finish();

        // Grab output from symbolic edge linking
        if ( gt_status )
        {
            shock_gt_results = gt_pro.get_output();
        }

        //Clean up after ourselves
        gt_pro.clear_input();
        gt_pro.clear_output();

        // It has two outputs
        if (shock_gt_results.size() != 2) 
        {
            vcl_cerr << "Shock after gap transform is Invalid!"
                     << vcl_endl;
            return 1;
        }

    }

    //******************** Sample Shocks  ********************************
    vcl_cout<<"************  Sampling Shock *************"<<vcl_endl;
    
    dbsk2d_sample_ishock_process sample_sg_pro;
    set_process_parameters_of_bpro1(*params, 
                                    sample_sg_pro, 
                                    params->tag_sample_shock_);


    // Before we start the process lets clean input output
    sample_sg_pro.clear_input();
    sample_sg_pro.clear_output();

    // Use input from either ishock computation or gap_transform
    if ( params->gap_transform_())
    {
        sample_sg_pro.add_input(shock_gt_results[0]);
    }
    else
    {
        sample_sg_pro.add_input(shock_results[0]);
    }

    // Kick of process
    bool sample_status = sample_sg_pro.execute();
    sample_sg_pro.finish();

    // Grab output from sampling
    vcl_vector<bpro1_storage_sptr> sample_shock_results;
    if ( sample_status )
    {
        sample_shock_results   = sample_sg_pro.get_output();
    }

    //Clean up after ourselves
    sample_sg_pro.clear_input();
    sample_sg_pro.clear_output();

    if (sample_shock_results.size() != 1) 
    {
        vcl_cerr << "Sampling of Intrinsinc Shock Computation Failed"
                 << vcl_endl;
        return 1;
    }

    
    //******************** Save Shocks   ********************************
    vcl_cout<<"************   Saving  Shock *************"<<vcl_endl;    

    dbsk2d_save_esf_process save_sg_pro;
    
    vcl_string output_file;
    if (params->save_to_object_folder_())
    { 
        output_file = params->output_shock_folder_() + "/";
    }
    else 
    {
        output_file = params->input_object_dir_() + "/";
    }
        
    if (!vul_file::exists(output_file)) 
    {
        vul_file::make_directory(output_file);
        
    }
    
    output_file = output_file + params->input_object_name_()+
        params->output_extension_();
       
    bpro1_filepath output(output_file,params->output_extension_());

    save_sg_pro.parameters()->set_value("-esfoutput",output);

    // Before we start lets clean input output
    save_sg_pro.clear_input();
    save_sg_pro.clear_output();

    save_sg_pro.add_input(sample_shock_results[0]);
    bool status = save_sg_pro.execute();
    save_sg_pro.finish();

    //Clean up after ourselves
    save_sg_pro.clear_input();
    save_sg_pro.clear_output();

    if ( !status )
    {
        vcl_cerr << "Problems in saving extrinsinc shock file: " 
                 << output_file << vcl_endl;
        return 1;

    }

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;

    return 0;
}

