// This is brcv/shp/dbsk2d/pro/dbsk2d_compute_containment_graph_region_process.cxx

//:
// \file

#include <dbsk2d/pro/dbsk2d_compute_containment_graph_process.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/dbsk2d_containment_graph.h>
#include <dbsk2d/dbsk2d_transform_manager.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap_detector.h>
#include <dbsk2d/algo/dbsk2d_ishock_transform_sptr.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_loop_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap4_transform.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource.h>

#include <vgl/vgl_box_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <bsol/bsol_algs.h>

#include <vgl/vgl_distance.h>
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vul/vul_timer.h>

//: Constructor
dbsk2d_compute_containment_graph_process::
dbsk2d_compute_containment_graph_process()
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
dbsk2d_compute_containment_graph_process::
~dbsk2d_compute_containment_graph_process()
{
}

//: Clone the process
bpro1_process*
dbsk2d_compute_containment_graph_process::clone() const
{
    return new dbsk2d_compute_containment_graph_process(*this);
}

vcl_string
dbsk2d_compute_containment_graph_process::name()
{
    return "Compute Containment Graph Process";
}

vcl_vector< vcl_string >
dbsk2d_compute_containment_graph_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    to_return.push_back( "image" );
    return to_return;
}

vcl_vector< vcl_string >
dbsk2d_compute_containment_graph_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    return to_return;
}

int dbsk2d_compute_containment_graph_process::input_frames()
{
    return 1;
}

int dbsk2d_compute_containment_graph_process::output_frames()
{
    return 1;
}

bool dbsk2d_compute_containment_graph_process::execute()
{
 
    // Start timer
    vul_timer t;

    double transform_threshold(0.5);
    double path_threshold(0.2);
    vcl_string output_prefix;
    bool remove_closed(false);
    bool add_bbox(true);
    double preprocess_threshold(0.12);
    unsigned int loop_cost(1);
    double gap_distance(2.0);


    bpro1_filepath output_folder_filepath;
    this->parameters()->get_value("-output_folder", output_folder_filepath);
    vcl_string output_folder = output_folder_filepath.path;
    

    parameters()->get_value( "-transform_threshold" , transform_threshold );
    parameters()->get_value( "-path_threshold" , path_threshold );
    parameters()->get_value( "-output_prefix", output_prefix);
    parameters()->get_value( "-closed" ,         remove_closed );
    parameters()->get_value( "-add_bbox", add_bbox);
    parameters()->get_value( "-preprocess_threshold" , preprocess_threshold ); 
    parameters()->get_value( "-loop_cost" , loop_cost);
    parameters()->get_value( "-gap_distance" , gap_distance );

    bool status = true;
    
    // 1) get input storage class
    vidpro1_vsol2D_storage_sptr input_vsol;
    input_vsol.vertical_cast(input_data_[0][0]);

    //1) get input storage classes
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][1]);

    // Set other thresholds
    dbsk2d_transform_manager::Instance().set_threshold(transform_threshold);
    dbsk2d_transform_manager::Instance().set_output_frag_folder(output_folder);
    dbsk2d_transform_manager::Instance().set_output_prefix(output_prefix);

    vil_image_resource_sptr image(0);
    if ( frame_image)
    {
        image=frame_image->get_image();

        vcl_string output_binary_file = output_folder+"/"+
            output_prefix+"_regions_binary.bin";
        vcl_string output_binary_regions_contours_file = output_folder+"/"+
            output_prefix+"_regions_contours_binary.bin";
  
        dbsk2d_transform_manager::Instance().set_image(image);
        dbsk2d_transform_manager::Instance().start_binary_file(
            output_binary_file);
        dbsk2d_transform_manager::Instance().start_region_file(
            output_binary_regions_contours_file);
            
    }

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
   
    double max_gPb_value=0.0;
    vcl_vector<vgl_polygon<double> > closed_polys;

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


                    int min_fit_length = 2;
                    vgl_fit_lines_2d<double> fitter;
                    fitter.set_min_fit_length(min_fit_length);
                    fitter.set_rms_error_tol(0.05f);
                    fitter.add_curve(poly[0]);

                    vcl_vector<vgl_line_segment_2d<double> > segs;
                    fitter.fit();
                    segs= fitter.get_line_segs();

                    vgl_polygon<double> fitted_poly(1);
                    fitted_poly.push_back(segs[0].point1());
                    fitted_poly.push_back(segs[0].point2());

                    // See if segs intersects any bnd elements
                    for ( unsigned int i=1; i < segs.size() ; ++i)
                    {
                        fitted_poly.push_back(segs[i].point2());
                    }

                    closed_polys.push_back(fitted_poly);
                }
                else
                {
                    double prob=dbsk2d_transform_manager::Instance().
                        transform_probability(curve);
                    if ( prob > max_gPb_value )
                    {
                        max_gPb_value=prob;
                    }
                }
            }
        }

        
    }

    dbsk2d_transform_manager::Instance().set_normalization(max_gPb_value);

    if ( remove_closed )
    {
        // Now erase closed contours removed
        for ( unsigned int c=0; c < objects_to_erase.size(); ++c)
        {
            input_vsol->remove_object(objects_to_erase[c]);
        
        }
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

    vcl_string filename = output_folder+"/" + output_prefix +"_cgraph.dot";

    vcl_string stats_filename = output_folder+"/" + output_prefix +
        "_stats_file.txt";
    vcl_ofstream stats_filestream(stats_filename.c_str());
    stats_filestream<<"Number of Contours: "<<vsol_list.size()<<vcl_endl;
    stats_filestream<<vcl_endl;


    dbsk2d_shock_storage_sptr shock_storage;
    shock_storage.vertical_cast(shock_results[0]);

    pre_process_contours(shock_storage->get_ishock_graph(),
                         preprocess_threshold,
                         gap_distance);

    dbsk2d_containment_graph cgraph(shock_storage->get_ishock_graph(),
                                    path_threshold,
                                    loop_cost);

    cgraph.construct_graph();
    cgraph.write_stats(stats_filestream);
    // cgraph.write_graph(filename);


    for ( unsigned int c=0; c < closed_polys.size() ; ++c)
    {
        dbsk2d_transform_manager::Instance().write_output_region
            (closed_polys[c]);
        dbsk2d_transform_manager::Instance().write_output_polygon
            (closed_polys[c]);

    }

    double vox_time = t.real()/1000.0;
    t.mark(); 
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;
    stats_filestream<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;
    stats_filestream.close();
    return status;
}

bool dbsk2d_compute_containment_graph_process::finish()
{
    return true;
}

void dbsk2d_compute_containment_graph_process::
pre_process_contours(dbsk2d_ishock_graph_sptr ishock_graph,
                     double preprocess_threshold,
                     double gap_distance)
{
    
    dbsk2d_ishock_gap_detector detector(ishock_graph);
    vcl_vector<
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> > gap_pairs;
    
    detector.detect_gap1(gap_pairs);

    vcl_map<double,dbsk2d_ishock_transform_sptr> transforms;

    for ( unsigned int i=0; i < gap_pairs.size(); ++i)
    {
        int contour_id=vcl_min(gap_pairs[i].first->get_contour_id(),
                               gap_pairs[i].second->get_contour_id());

        dbsk2d_ishock_transform_sptr trans = new 
            dbsk2d_ishock_gap_transform(ishock_graph,gap_pairs[i],contour_id);
        transforms[trans->likelihood()]=trans;
    }

    vcl_map<int,vcl_vector<dbsk2d_bnd_contour_sptr> > gap_trans;
    vcl_map<double,dbsk2d_ishock_transform_sptr>::reverse_iterator it;
    for ( it = transforms.rbegin() ; it != transforms.rend() ; ++it)
    {
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> pair =
            (*it).second->get_contour_pair();
        double length_of_gap=vgl_distance(pair.first->pt(),pair.second->pt());
        if ( (pair.first->is_an_end_point() && pair.second->is_an_end_point()) 
             &&
             (pair.first->is_a_GUIelm() && pair.second->is_a_GUIelm()))
        {
            if ( this->get_contour(pair.first)->get_id() == 
                 this->get_contour(pair.second)->get_id())
            {
                dbsk2d_ishock_gap_transform* gap_transform
                    = (dbsk2d_ishock_gap_transform*)((*it).second.ptr());
                vcl_vector<vgl_point_2d<double> > gap_filler
                    = gap_transform->get_es_samples();

                dbsk2d_bnd_contour_sptr con1 = this->get_contour(pair.first);
                if ( gap_trans.count(con1->get_id()))
                {
                 
                    
                    dbsk2d_transform_manager::Instance().write_output_region
                        (gap_trans[con1->get_id()],gap_filler);
                    dbsk2d_transform_manager::Instance().write_output_polygon
                        (gap_trans[con1->get_id()],gap_filler);

                }
                else
                {
                 
                    vcl_vector<dbsk2d_bnd_contour_sptr> cons;
                    cons.push_back(con1);
                    dbsk2d_transform_manager::Instance().write_output_region
                        (cons,gap_filler);
                    dbsk2d_transform_manager::Instance().write_output_polygon
                        (cons,gap_filler);

                }

                dbsk2d_ishock_loop_transform loop_trans(
                    ishock_graph,
                    pair.first);
                loop_trans.execute_transform();
            }
            else if ( (*it).first >= preprocess_threshold && 
                      length_of_gap <= gap_distance)
            {
                dbsk2d_bnd_contour_sptr con1 = this->get_contour(pair.first);
                dbsk2d_bnd_contour_sptr con2 = this->get_contour(pair.second);

                
                int contour_id(0);
                if ( gap_trans.count(con1->get_id()))
                {
                    contour_id=con1->get_id();
                }
                else if ( gap_trans.count(con2->get_id()))
                {
                    contour_id=con2->get_id();
                }
                else
                {
                    contour_id=vcl_min(con1->get_id(),con2->get_id());
                }

                (*it).second->execute_transform();

                dbsk2d_bnd_contour_sptr gap_con = 
                    ishock_graph->boundary()->preproc_contours().back();

                if ( !gap_trans.count(contour_id))
                {
                    gap_trans[contour_id].push_back(con1);
                    gap_trans[contour_id].push_back(con2);
                    gap_trans[contour_id].push_back(gap_con);
                    gap_con->set_id(contour_id);
                    con1->set_id(contour_id);
                    con2->set_id(contour_id);
                }
                else
                {
                    gap_trans[contour_id].push_back(gap_con);
                    gap_con->set_id(contour_id);
                    if ( con1->get_id() == contour_id)
                    {
                        if ( gap_trans.count(con2->get_id()))
                        {
                            vcl_vector<dbsk2d_bnd_contour_sptr> vec =
                                gap_trans[con2->get_id()];
                            for ( unsigned int i=0; i < vec.size() ; ++i)
                            {
                                vec[i]->set_id(contour_id);
                                gap_trans[contour_id].push_back(vec[i]);
                                
                            }
                        }
                        else
                        {
                            gap_trans[contour_id].push_back(con2);
                            con2->set_id(contour_id);
                        }
                    }
                    else
                    {
                        if ( gap_trans.count(con1->get_id()))
                        {
                            vcl_vector<dbsk2d_bnd_contour_sptr> vec =
                                gap_trans[con1->get_id()];
                            for ( unsigned int i=0; i < vec.size() ; ++i)
                            {
                                vec[i]->set_id(contour_id);
                                gap_trans[contour_id].push_back(vec[i]);

                            }
                        }
                        else
                        {
                            con1->set_id(contour_id);
                            gap_trans[contour_id].push_back(con1);
                        }
                    }
                }


            }
        }
        
    }

    // {
    //     dbsk2d_ishock_transform temp_trans(ishock_graph,
    //                                        dbsk2d_ishock_transform::GAP);
    //     temp_trans.write_boundary("pre_processed_contours_gap1.bnd");
    // }
 
    transforms.clear();

    vcl_vector<
    vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> > gap4_pairs;
    detector.detect_gap4(gap4_pairs);

    for ( unsigned int i=0; i < gap4_pairs.size(); ++i)
    {
        int contour_id=this->get_contour(gap4_pairs[i].first)->get_id();
        dbsk2d_ishock_transform_sptr trans = new 
            dbsk2d_ishock_gap4_transform(ishock_graph,gap4_pairs[i],contour_id);
        transforms[trans->likelihood()]=trans;
    }

    for ( it = transforms.rbegin() ; it != transforms.rend() ; ++it)
    {
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> pair =
            (*it).second->get_contour_pair();
        dbsk2d_ishock_gap4_transform* trans=(dbsk2d_ishock_gap4_transform*)
            ((*it).second.ptr());
        double length_of_gap=trans->length_of_gap();
        if ( pair.first->is_an_end_point() )
        {
            if ( (*it).second->valid_transform() )
            {
                if ( (*it).first >= preprocess_threshold &&
                     length_of_gap <= gap_distance )
                {
                    (*it).second->execute_transform();
                }
            }
        }
        
    }

    // {
    //     dbsk2d_ishock_transform temp_trans(ishock_graph,
    //                                        dbsk2d_ishock_transform::GAP);
    //     temp_trans.write_boundary("pre_processed_contours_gap4.bnd");
    // }
    
    transforms.clear();
    gap_pairs.clear();
}

dbsk2d_bnd_contour_sptr dbsk2d_compute_containment_graph_process::
get_contour(dbsk2d_ishock_bpoint* bp)
{

    dbsk2d_bnd_vertex* vertex=bp->bnd_vertex();
    edge_list edges;
    vertex->edges(edges);

    edge_list::iterator it;
    if ( edges.size() == 2)
    {
        edges.erase(edges.begin());
    }
    it=edges.begin();

    const vcl_list< vtol_topology_object * > * 
        superiors  = (*it)->superiors_list();
    vcl_list<vtol_topology_object*>::const_iterator tit;
    tit=(*superiors).begin();

    return (dbsk2d_bnd_contour*)(*tit);

}
