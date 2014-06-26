// This is brcv/shp/dbskfg/pro/dbskfg_form_containment_graph_region_process.cxx

//:
// \file

#include <con_graph/pro/dbskfg_form_containment_graph_region_process.h>
#include <con_graph/dbskfg_containment_graph_sptr.h>
#include <con_graph/dbskfg_containment_graph.h>

#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <bsol/bsol_algs.h>

#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/algo/dbskfg_transform_manager.h>
#include <dbskfg/algo/dbskfg_composite_graph_fileio.h>

#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/pro/dbskfg_form_composite_graph_process.h>
#include <dbskfg/algo/dbskfg_detect_transforms.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage_sptr.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <vul/vul_timer.h>

#include <vcl_numeric.h>
#include <vcl_algorithm.h>

//: Constructor
dbskfg_form_containment_graph_region_process::
dbskfg_form_containment_graph_region_process()
{
    if ( 
        !parameters()->add( "Region id:" , "-region_id" , (int) 265 ) ||
        !parameters()->add( "Expand Single:" , 
                            "-expand_single" , (bool) false ) ||
        !parameters()->add( "Add bbox:" , 
                            "-add_bbox", (bool) true) ||
        !parameters()->add( "Expansion Type: Implicit(1) Explicit(0)" , 
                            "-expand_type" , (int) 1 ) ||
        !parameters()->add( "Threshold on Probability of Transform:" , 
                            "-transform_threshold" , 
                            (double) 0.2 ) ||
        !parameters()->add( "Threshold on Probability of Path:" , 
                            "-path_threshold" , 
                            (double) 0.2 ) ||
        !parameters()->add( "Threshold on Preprocessing transforms:" , 
                            "-preprocess_threshold" , 
                            (double) 0.12 ) ||
        !parameters()->add( "Minimum Gap distance:" , 
                            "-gap_distance" , 
                            (double) 2.0 ) ||
        !parameters()->add( "Euler Sprial Completion" , "-ess" , 
                            (double)0.25) ||
        !parameters()->add( "Remove closed contours" , "-closed" , 
                            (bool)false) ||
        !parameters()->add( "Loop cost type (0,1)" , "-loop_cost" , 
                            (unsigned int) 1) ||
        !parameters()->add( "Expand Outside" , "-outside" , 
                            (bool) false) ||
        !parameters()->add( "Train" , "-train" , 
                            (bool) false) ||
        !parameters()->add( "Output folder:" , 
                            "-output_folder", bpro1_filepath("", "")) ||
        !parameters()->add( "Output file prefix:" , 
                            "-output_prefix", vcl_string(""))

        )
    {
         vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;

    }

}

//: Destructor
dbskfg_form_containment_graph_region_process::
~dbskfg_form_containment_graph_region_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_form_containment_graph_region_process::clone() const
{
    return new dbskfg_form_containment_graph_region_process(*this);
}

vcl_string
dbskfg_form_containment_graph_region_process::name()
{
    return "Compute Containment Graph From Region";
}

vcl_vector< vcl_string >
dbskfg_form_containment_graph_region_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    to_return.push_back( "image" );
    return to_return;
}

vcl_vector< vcl_string >
dbskfg_form_containment_graph_region_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    return to_return;
}

int dbskfg_form_containment_graph_region_process::input_frames()
{
    return 1;
}

int dbskfg_form_containment_graph_region_process::output_frames()
{
    return 1;
}

bool dbskfg_form_containment_graph_region_process::execute()
{
 
    // Start timer
    vul_timer t;

    int region_id(0);
    int expand_type(1);
    double transform_threshold(0.5);
    double path_threshold(0.2);
    vcl_string output_prefix;
    double ess(0.25);
    bool remove_closed(false);
    bool expand_single(false);
    bool add_bbox(true);

    bpro1_filepath output_folder_filepath;
    this->parameters()->get_value("-output_folder", output_folder_filepath);
    vcl_string output_folder = output_folder_filepath.path;
    

    parameters()->get_value( "-transform_threshold" , transform_threshold );
    parameters()->get_value( "-path_threshold" , path_threshold );
    parameters()->get_value( "-region_id" , region_id );
    parameters()->get_value( "-expand_type", expand_type);
    parameters()->get_value( "-output_prefix", output_prefix);
    parameters()->get_value( "-ess" ,         ess );
    parameters()->get_value( "-closed" ,         remove_closed );
    parameters()->get_value( "-expand_single" ,  expand_single );
    parameters()->get_value( "-add_bbox", add_bbox);

    bool status = true;
    
    // 1) get input storage class
    vidpro1_vsol2D_storage_sptr input_vsol;
    input_vsol.vertical_cast(input_data_[0][0]);

    //1) get input storage classes
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][1]);

    // Redo input vsol storage

    // 2) Set id equivalent to what they are
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = 
        input_vsol->all_data();

    // 3) Keep track of objects to remove
    vcl_vector< vsol_spatial_object_2d_sptr > objects_to_erase;

    vsol_box_2d_sptr bbox=0;

    // 4) Adding bounding box if needed
    if ( add_bbox )
    {
       
        // create new bounding box
        bbox = new vsol_box_2d();

        // Grab image to see boundaries
        vil_image_resource_sptr image = frame_image->get_image();
        
        // Enlarge bounding box from size
        // Calculate xcenter, ycenter
        double xcenter = image->ni()/2.0;
        double ycenter = image->nj()/2.0;

        // Translate to center and scale
        double xmin_scaled = ((0-xcenter)*2)+xcenter;
        double ymin_scaled = ((0-ycenter)*2)+ycenter;
        double xmax_scaled = ((image->ni()-xcenter)*2)+xcenter;
        double ymax_scaled = ((image->nj()-ycenter)*2)+ycenter;
        
        vcl_cout<<xmin_scaled<<" "<<ymin_scaled<<vcl_endl;
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
        input_vsol->add_object(box_poly->cast_to_spatial_object());
        box_poly->print(vcl_cout);
        bbox=0;
    }
   
    for ( unsigned int i=0; i < vsol_list.size() ; ++i)
    {   
        if( vsol_list[i]->cast_to_curve())
        {
            if( vsol_list[i]->cast_to_curve()->cast_to_polyline() )
            {
                vsol_polyline_2d_sptr curve = 
                    vsol_list[i]->cast_to_curve()->cast_to_polyline();
                
                if ( curve->p0()->get_p() == curve->p1()->get_p())
                {
                    // This represents a closed curve
                    // Lets write it out and not use it

                    // Do not include this in the contour set
                    objects_to_erase.push_back(vsol_list[i]);

                    // Grab all points from vsol
                    vcl_vector<vgl_point_2d<double> > points;
                    for (unsigned p=0; p<curve->size(); p++)
                    {
                        points.push_back( curve->vertex(p)->get_p());
                    }

                    // Create polygon
                    vgl_polygon<double> poly(points);

                    vcl_stringstream streamer;
                    streamer<<i;

                    // Create node name
                    vcl_string filename = output_folder+"/" + output_prefix +
                        "_cgraph_closed_"+streamer.str()+"_mask.png";

                    dbskfg_utilities::save_image_mask(poly,
                                                      frame_image->get_image(),
                                                      filename);
                }
            }
        }

        
    }

    if ( remove_closed )
    {
        // Now erase closed contours removed
        for ( unsigned int c=0; c < objects_to_erase.size(); ++c)
        {
            input_vsol->remove_object(objects_to_erase[c]);
        
        }
    }

    // 3) Grab new vsol list
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list_new = 
        input_vsol->all_data();

    // Reset ids
    for ( unsigned int i=0; i < vsol_list_new.size() ; ++i)
    {  
        vsol_list_new[i]->set_id(i);
    }
  
    /*********************** Shock Compute **********************************/
    // Grab output from shock computation
    vcl_vector<bpro1_storage_sptr> shock_results;
    {
        // 3) Create shock pro process and assign inputs 
        dbsk2d_compute_ishock_process shock_pro;

        shock_pro.clear_input();
        shock_pro.clear_output();

        shock_pro.add_input(frame_image);
        shock_pro.add_input(input_vsol);

        // Set params
        shock_pro.parameters()->set_value("-exist_ids",true);
        status = shock_pro.execute();

        if ( status == false)
        {
            return status;
        }

        shock_pro.finish();

        shock_results = shock_pro.get_output();

        // Clean up after ourselves
        shock_pro.clear_input();
        shock_pro.clear_output();

    }
  
    /*********************** Compute Composite Graph ************************/
    vcl_vector<bpro1_storage_sptr> cg_results;
    {
        // Lets vertical cast to shock stroge
        // Holds shock storage
        dbsk2d_shock_storage_sptr shock_storage;
        shock_storage.vertical_cast(shock_results[0]);

        dbskfg_form_composite_graph_process cg_pro;

        cg_pro.clear_input();
        cg_pro.clear_output();
        
        // Set params
        cg_pro.parameters()->set_value("-locus",false);

        cg_pro.add_input(shock_storage);
        cg_pro.add_input(frame_image);

        status = cg_pro.execute();

        cg_pro.finish();

        if ( status == false)
        {
            return status;
        }
  
        cg_results = cg_pro.get_output();

        cg_pro.clear_input();
        cg_pro.clear_output();
    }
 
    // Delete shock storage
    shock_results.clear();
    vsol_list.clear();

    // Grab composite graph storage
    dbskfg_composite_graph_storage_sptr cg_storage;
    cg_storage.vertical_cast(cg_results[0]);

    // Holds local_graph
    dbskfg_composite_graph_sptr local_graph;
    local_graph = cg_storage->get_composite_graph();

    // Grab a rag node for an id
    dbskfg_rag_graph_sptr rag_graph = 
        cg_storage->get_rag_graph();

    // Keep track of current id
    //   dbskfg_rag_node_sptr rag_node = rag_graph->rag_node(region_id);
  
    // Set up transform manager
    dbskfg_transform_manager::Instance().set_rag_graph(
        rag_graph);
    dbskfg_transform_manager::Instance().set_cgraph(
        local_graph);

    // Set other thresholds
    dbskfg_transform_manager::Instance().set_threshold(transform_threshold);
    dbskfg_transform_manager::Instance().set_output_frag_folder(output_folder);
    dbskfg_transform_manager::Instance().set_output_prefix(output_prefix);
    dbskfg_transform_manager::Instance().set_ess_completion(ess);

    vil_image_resource_sptr image(0);
    if ( frame_image)
    {
        image=frame_image->get_image();

        vcl_string output_binary_file = output_folder+"/"+
            output_prefix+"_regions_binary.bin";
        vcl_string output_region_file = output_folder+"/"+
            output_prefix+"_regions_contours_binary.bin";
  
        dbskfg_transform_manager::Instance().set_image(image);
        dbskfg_transform_manager::Instance().start_binary_file(
            output_binary_file);
        dbskfg_transform_manager::Instance().start_region_file(
            output_region_file);
            
    }
     
    // Implicit
    if ( expand_type == 1 )
    {
    
        dbskfg_detect_transforms detector(dbskfg_transform_manager::Instance()
                                          .get_cgraph(),
                                          dbskfg_transform_manager::Instance()
                                          .get_image());

        detector.detect_transforms_simple(true,
                                          true,
                                          ess,
                                          1);


    }

    // Determine expansion type
    dbskfg_containment_node::ExpandType expand_flag;

    if ( expand_type == 1 )
    {
        expand_flag = dbskfg_containment_node::IMPLICIT;
    }
    else
    {
        expand_flag = dbskfg_containment_node::EXPLICIT;
    }

    vcl_string filename = output_folder+"/" + output_prefix +"_cgraph.dot";

    vcl_string stats_filename = output_folder+"/" + output_prefix +
        "_stats_file.txt";
    vcl_ofstream stats_filestream(stats_filename.c_str());
    stats_filestream<<"Number of Contours: "<<vsol_list_new.size()<<vcl_endl;
    stats_filestream<<vcl_endl;

    // Kick of containment graph building
    dbskfg_containment_graph_sptr cgraph =
        new dbskfg_containment_graph(input_vsol,
                                     expand_flag,
                                     path_threshold);
    
    cgraph->set_output_filename(filename);

    vcl_vector<double> times_per_node;

    // Start timer
    vul_timer t2;

    for (dbskfg_rag_graph::vertex_iterator vit = 
             rag_graph->vertices_begin(); 
         vit != rag_graph->vertices_end(); ++vit)
    {

                     
            dbskfg_rag_node_sptr rag_node = *vit;

            vcl_cout<<"Working on rag_node , id: "<<rag_node->id()<<vcl_endl;
            if ( expand_single )
            {
                if ( rag_node->id() == region_id )
                {
                    // Create node name
                    vcl_string node_name = dbskfg_transform_manager::Instance().
                        get_output_prefix()+"_cgraph_node_1_0";

                    // Print region
                    dbskfg_composite_graph_fileio file;
                    file.write_out(rag_node,
                                   dbskfg_transform_manager::Instance()
                                   .get_image(),
                                   1.0,
                                   node_name,
                                   dbskfg_transform_manager::Instance().
                                   get_output_frag_folder());

                    bool flag=cgraph->expand_region_root(rag_node);
                    
                    break;
    
                }
            }
            else
            {

                if ( !rag_node->endpoint_spawned_node() && 
                     rag_node->get_shock_links().size() >  1 &&
                     rag_node->contour_ratio() >= 0.4 &&
                     rag_node->is_rag_node_within_image
                     (dbskfg_transform_manager::Instance().get_image()->ni(),
                      dbskfg_transform_manager::Instance().get_image()->nj()))
                {


                    // Start timer
                    vul_timer t3;

                    // Create node name
                    vcl_string node_name = dbskfg_transform_manager::Instance().
                        get_output_prefix()+"_cgraph_node_1_0";

                    // Print region
                    dbskfg_composite_graph_fileio file;
                    file.write_out(rag_node,
                                   dbskfg_transform_manager::Instance()
                                   .get_image(),
                                   1.0,
                                   node_name,
                                   dbskfg_transform_manager::Instance().
                                   get_output_frag_folder());
    
                    bool flag = cgraph->expand_region_root(rag_node);
                   
                    double vox_time = t3.real()/1000.0;
                    t3.mark();

                    if ( flag)
                    {
                        times_per_node.push_back(vox_time);
                    }

                }


            }

    }

    double vox_time2 = t2.real()/1000.0;
    t2.mark();

    double average_node_time = vcl_accumulate(times_per_node.begin(),
                                          times_per_node.end(),
                                          0.0);

    stats_filestream<<"Depth: "<<0<<" Node: "<<times_per_node.size()<<vcl_endl;
    stats_filestream<<"Average Node Time: "
                    <<average_node_time/times_per_node.size()
                    <<vcl_endl;
    stats_filestream<<"Time: "<<vox_time2<<" sec"<<vcl_endl;
    stats_filestream<<vcl_endl;

    cgraph->expand_tree(stats_filestream);

    double vox_time = t.real()/1000.0;
    t.mark(); 
    stats_filestream<<"Total Nodes: "<<cgraph->number_of_vertices()<<vcl_endl;
    stats_filestream<<"Total Edges: "<<cgraph->number_of_edges()<<vcl_endl;
    stats_filestream<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;

    cgraph->print(vcl_cout);
    cgraph->clear();
    cgraph=0;

    stats_filestream.close();

 
    input_vsol->clear_all();
    rag_graph=0;
    local_graph=0;
    dbskfg_transform_manager::Instance().destroy_singleton();

    return status;
}

bool dbskfg_form_containment_graph_region_process::finish()
{
    return true;
}


