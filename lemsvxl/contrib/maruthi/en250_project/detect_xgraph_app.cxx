#include <en250_project/detect_xgraph_app.h>
#include <en250_project/dbsks_xshock_app_likelihood.h>
#include <dbsks/dbsks_xio_xshock.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/dbsks_det_nms.h>
#include <en250_project/dbsks_xshock_detector.h>
#include <dbsks/dbsks_xshock_utils.h>
#include <dbsks/dbsks_detect_xgraph.cxx>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vcl_iostream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <vul/vul_timer.h>


//: Detect an object, represented as a shock graph, in an image by apperance
bool detect_xgraph_using_app(const vcl_string& image_file,
                             const vcl_string& xgraph_file,
                             const vcl_string& xgraph_geom_file,
                             const vcl_vector<double >& xgraph_scales,
                             int det_window_width,
                             int det_window_height,
                             vcl_vector<dbsks_det_desc_xgraph_sptr >
                             & output_det_list)
{

    //>>> Preliminary checks
    // load shock graph, and geometry file
    // if any fail bad news

 
    //>>> Preliminary checks to open shock graph
    // Load the shock graph
    dbsksp_xshock_graph_sptr xgraph = 0;

    vcl_cout << "Loading xshock_graph XML file: " << xgraph_file << "...";
    if ( x_read(xgraph_file, xgraph) )
    {
        vcl_cout << "Succeeded.\n";
    }
    else
    {
        vcl_cout << "Failed.\n";
        return false;
    } 

    // Load the xgraph geometric model
    dbsks_xgraph_geom_model_sptr xgraph_geom = 0;
    if (!x_read(xgraph_geom_file, xgraph_geom))
    {
        vcl_cout << "ERROR: Couldn't load xgraph geometric model from file " 
                 << xgraph_geom_file << vcl_endl;
        return false;
    }

    output_det_list.clear();
    if (xgraph_scales.empty())
    {
        vcl_cout << "\nERROR: no scale was specified. Nothing was run.\n";
        return false;
    }


    //////////////////////////////////////////////////////////////////////////
    //>>> Detect xgraphs
    vcl_cout << ">> Detecting xgraph in image at multiple scales...";
    vcl_vector<dbsks_det_desc_xgraph_sptr > raw_det_list;
    double def_scale = vcl_sqrt(xgraph->area());
  
    for (unsigned i =0; i < xgraph_scales.size(); ++i)
    {
        vcl_cout << "\n>>>> Processing xgraph scale = " 
                 << xgraph_scales[i] << vcl_endl;

        double xgraph_scale = xgraph_scales[i];

        ////////////////////////////////////////////////////////////////////
        // scale the edgemap up so that the xgraph size is "standard", i.e. 130
        //double standard_scale = 130; 
        //160 for mugs because it occupies more space. For giraffes, swans, 
        //and bottles: 130;
        //double scaled_up_factor = 1.0;

        vcl_vector<dbsks_det_desc_xgraph_sptr > dets;
        detect_xgraph_using_app(image_file,
                                xgraph,
                                xgraph_geom,
                                xgraph_scale,
                                det_window_width,
                                det_window_height,
                                dets);

       
    
    
        // scale the detection down to original size of the image
        // for (unsigned m =0; m < dets.size(); ++m)
        // {
        //   dets[m]->xgraph()->scale_up(0, 0, 1.0/scaled_up_factor);

        //   // recompute bounding box as a result of scaling the xgraph
        //   dets[m]->compute_bbox();
        // }

        raw_det_list.insert(raw_det_list.end(), dets.begin(), dets.end());
    }
    vcl_cout << "\n    Number of detection before non-max suppression: " 
             << raw_det_list.size() << vcl_endl;

    //>>> Non-max suppression across detection results form different scales
    double min_overlap_ratio_for_rejection = 0.3;
    dbsks_det_nms_using_polygon(raw_det_list, output_det_list, 
                                min_overlap_ratio_for_rejection);

    vcl_cout << "\n----Number of detection after non-max suppression: " 
             << output_det_list.size() << vcl_endl; 



    return true;

}




bool detect_xgraph_using_app(const vcl_string& image_file,
                             const dbsksp_xshock_graph_sptr& xgraph,
                             const dbsks_xgraph_geom_model_sptr& xgraph_geom,
                             double xgraph_scale,
                             int det_window_width,
                             int det_window_height,
                             vcl_vector<dbsks_det_desc_xgraph_sptr >
                             & output_det_list)
{

    // ///////////////////////////////////////////////////////////////////////
    // confidence threshold to reject a detection
    double confidence_lower_threshold = -10000;

    // if one bbox overlaps with another bbox with higher confidence more than 
    // "min_overlap_ratio_for_rejection" of its area then that bbox is rejected.
    double min_overlap_ratio_for_rejection = 0.3;

    vul_timer timer;
    timer.mark();

    /////////////////////////////////////////////////////////////////////
    //>> Build a likelihood calculator  
    dbsks_xshock_app_likelihood app_like(image_file);

    //>> Scale the shock graph to desired scale
    vcl_cout << "\n>>Scaling the xgraph to specified scale ...";
    double cur_scale = vcl_sqrt(xgraph->area());
    dbsksp_xshock_graph_sptr scaled_xgraph = 
        new dbsksp_xshock_graph(*xgraph);
    scaled_xgraph->similarity_transform( 
        (*scaled_xgraph->vertices_begin())->pt(), 0, 0, 0, xgraph_scale 
        / cur_scale);
    scaled_xgraph->update_all_degree_1_nodes();
    vcl_cout << "done\n";


    //>> Compute all windows (rectangular boxes) necessary to 
    // cover the whole image
    vcl_cout << 
     "\n>>Computing sliding (rectangular) windows to cover the whole image ...";
  
    // maximum size for a detection window
    int max_width = det_window_width; //512; 
    int max_height = det_window_height; //512;

    // list of windows to run detection on
    vcl_vector<vgl_box_2d<int > > windows;
    dbsks_compute_detection_windows(max_width, 
                                    max_height, 
                                    512, 
                                    512, 
                                    windows);


    // Print out list of windows
    vcl_cout << "\n  Total number of windows to run on = " << 
        windows.size() << "\n";
    for (unsigned iw =0; iw < windows.size(); ++iw)
    {
        vgl_box_2d<int > window = windows[iw];
        vcl_cout << "  window " << iw << ": xmin=" << window.min_x() 
                 << " ymin=" << window.min_y() 
                 << " xmax=" << window.max_x()
                 << " ymax=" << window.max_y() << "\n";
    }
    vcl_cout.flush();

    //>> Detect objects within each window
    vcl_cout << "\n>> Detecting objects in all computed windows ...";
    vcl_vector<dbsks_det_desc_xgraph_sptr > det_list;
    det_list.clear();

    for (unsigned iw =0; iw < windows.size(); ++iw)
    {
        vgl_box_2d<int > window = windows[iw];
        vcl_cout << "\n>>>> Window index = " << iw 
                 << " xmin=" << window.min_x() 
                 << " ymin=" << window.min_y() 
                 << " xmax=" << window.max_x()
                 << " ymax=" << window.max_y() << vcl_endl;
    
        // xshock detection engine
        dbsks_xshock_detector engine;
        engine.app_like_ = &app_like;
        engine.xgraph_geom_ = xgraph_geom;
        engine.set_xgraph(scaled_xgraph);

        ///////////////////////
        engine.detect(window);
        ///////////////////////

        // construct a vector of detection descriptor
        for (unsigned i =0; i < engine.list_solutions_.size(); ++i)
        {
            dbsksp_xshock_graph_sptr sol_xgraph = engine.list_solutions_[i];
      
            double confidence = -engine.list_solution_costs_[i];
            double real_confidence = -engine.list_solution_real_costs_[i];

            // only consider detetion with at least minimal confidence level
            if (real_confidence > confidence_lower_threshold)
            {
                dbsks_det_desc_xgraph_sptr det = 
                    new dbsks_det_desc_xgraph(sol_xgraph, real_confidence);
                det->compute_bbox();
                det_list.push_back(det);
            }
            /////////////////////////////////////////////////////
        } // solution
    } // iw
    vcl_cout << "\n    Number of detection before non-max suppression: " 
             << det_list.size() << vcl_endl;

    //>> Non-max supression on the boundary polygon
    vcl_cout << "\n    Non-max suppression based on boundary polygon box: ...";
    dbsks_det_nms_using_polygon(det_list, 
                                output_det_list, 
                                min_overlap_ratio_for_rejection);

  
    vcl_cout << "\n    Number of detection after non-max suppression: " 
             << output_det_list.size() << vcl_endl; 

    return true;


}

