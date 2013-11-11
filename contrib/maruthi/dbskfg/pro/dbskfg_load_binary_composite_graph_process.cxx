// This is brcv/shp/dbskfg/pro/dbskfg_load_binary_composite_graph_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_load_binary_composite_graph_process.h>
#include <vsol/vsol_line_2d.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_box_2d.h>
#include <bsol/bsol_algs.h>


#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>


#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/pro/dbskfg_form_composite_graph_process.h>

#include <vcl_algorithm.h>

//: Constructor
dbskfg_load_binary_composite_graph_process::dbskfg_load_binary_composite_graph_process():image_ni_(0),image_nj_(0)
{
    if( !parameters()->add( "Input file <filename...>" , "-cginput" , 
                            bpro1_filepath("","*.bin") ) )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_load_binary_composite_graph_process::
~dbskfg_load_binary_composite_graph_process()
{
    cgraphs_.clear();
    frags_removed_.clear();
}

//: Clone the process
bpro1_process*
dbskfg_load_binary_composite_graph_process::clone() const
{
    return new dbskfg_load_binary_composite_graph_process(*this);
}

vcl_string
dbskfg_load_binary_composite_graph_process::name()
{
    return "Load Binary Composite Graph";
}

vcl_vector< vcl_string >
dbskfg_load_binary_composite_graph_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    return to_return;

}

vcl_vector< vcl_string >
dbskfg_load_binary_composite_graph_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    //to_return.push_back("composite_graph");
    return to_return;
}

int dbskfg_load_binary_composite_graph_process::input_frames()
{
    return 1;
}

int dbskfg_load_binary_composite_graph_process::output_frames()
{
    return 1;
}

bool dbskfg_load_binary_composite_graph_process::execute()
{
    // get input file path
    bpro1_filepath input;
    parameters()->get_value( "-cginput" , input);
    vcl_string input_file_path = input.path;

    int num_of_files = 0;

    output_data_.clear();

    // make sure that input_file_path is sane
    if (input_file_path == "") 
    { return false; }

    vcl_map<unsigned int,vcl_vector< vsol_spatial_object_2d_sptr > > geoms;
    vcl_map<unsigned int,vcl_set<unsigned int> > con_ids;
    read_binary_file(input_file_path,geoms,con_ids);
    vcl_cout<<"Loaded "<<geoms.size()<<" Fragments "<<vcl_endl;

    // create new bounding box
    vsol_box_2d_sptr bbox = new vsol_box_2d();
    
    // Enlarge bounding box from size
    // Calculate xcenter, ycenter
    double xcenter = image_ni_/2.0;
    double ycenter = image_nj_/2.0;
    
    // Translate to center and scale
    double xmin_scaled = ((0-xcenter)*5)+xcenter;
    double ymin_scaled = ((0-ycenter)*5)+ycenter;
    double xmax_scaled = ((image_ni_-xcenter)*5)+xcenter;
    double ymax_scaled = ((image_nj_-ycenter)*5)+ycenter;
    
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
    box_poly->set_id(10e6);
    box_poly->print(vcl_cout);
    bbox=0;


    vcl_map<unsigned int,vcl_vector< vsol_spatial_object_2d_sptr > >::iterator 
        git;
    unsigned int g=0;
    for ( git=geoms.begin() ; git != geoms.end() ; ++git)
    {
        vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
        output_vsol->add_objects((*git).second);
        output_vsol->add_object(box_poly->cast_to_spatial_object());
        bool status=compute_composite_graph(output_vsol, con_ids[(*git).first]);
        if ( status == false)
        {
            frags_removed_.push_back(g);
        }
        g++;
    }


    return true;
}

bool dbskfg_load_binary_composite_graph_process::finish()
{
    return true;
}


void dbskfg_load_binary_composite_graph_process::read_binary_file
(vcl_string input_file,
 vcl_map<unsigned int,
 vcl_vector< vsol_spatial_object_2d_sptr > >& geoms,
 vcl_map<unsigned int,vcl_set<unsigned int> >& con_ids)
{
    
    vcl_ifstream file (input_file.c_str(), 
                       vcl_ios::in|vcl_ios::binary|vcl_ios::ate);

    double* memblock(0);
    unsigned int size_cons=0;
    if (file.is_open())
    {
        vcl_ifstream::pos_type size = file.tellg();
        size_cons=size/sizeof(double);
        memblock = new double[size_cons];
        file.seekg (0, vcl_ios::beg);
        file.read ((char *) memblock, size);
        file.close();

    }
    
    image_ni_=memblock[0];
    image_nj_=memblock[1];
    
    vcl_cout<<"Image size: "<<image_ni_<<" by "<<image_nj_<<vcl_endl;
    vcl_cout<<"Read in size_cons data: "<<size_cons<<vcl_endl;
    unsigned int c=2;
    unsigned int id=0;
    while ( c < size_cons)
    {
        unsigned int numb_contours=memblock[c];
        c++;
        unsigned int stop=numb_contours+c;
        unsigned int index=0;
        while ( c < stop )
        {
            vgl_point_2d<double> p1(memblock[c],memblock[c+1]);
            vgl_point_2d<double> p2(memblock[c+2],memblock[c+3]);
            vsol_spatial_object_2d_sptr line=new vsol_line_2d(p1,p2);
            line->set_id(index);
            geoms[id].push_back(line);
            con_ids[id].insert(index);
            c=c+5;
            index++;
        }
        id++;

    }
}

bool dbskfg_load_binary_composite_graph_process::
compute_graph(vcl_vector<vsol_spatial_object_2d_sptr>& contours,
              vcl_set<unsigned int>& contour_ids,
              vcl_pair<unsigned int,unsigned int>& image_size,
              bool prune_degree_three_nodes)
{
   
    // create new bounding box
    vsol_box_2d_sptr bbox = new vsol_box_2d();
    
    // Enlarge bounding box from size
    // Calculate xcenter, ycenter
    double xcenter = image_size.first/2.0;
    double ycenter = image_size.second/2.0;
    
    // Translate to center and scale
    double xmin_scaled = ((0-xcenter)*2)+xcenter;
    double ymin_scaled = ((0-ycenter)*2)+ycenter;
    double xmax_scaled = ((image_size.first-xcenter)*2)+xcenter;
    double ymax_scaled = ((image_size.second-ycenter)*2)+ycenter;
    
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
    box_poly->set_id(10e6);
    box_poly->print(vcl_cout);
    bbox=0;


    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    output_vsol->add_objects(contours);
    output_vsol->add_object(box_poly->cast_to_spatial_object());
    bool flag=compute_composite_graph(output_vsol,
                                      contour_ids,
                                      prune_degree_three_nodes);

    return flag;
}

bool dbskfg_load_binary_composite_graph_process::
compute_composite_graph(vidpro1_vsol2D_storage_sptr input_vsol,
                        vcl_set<unsigned int>& cons,
                        bool prune_degree_three_nodes)
{
    
    /*********************** Shock Compute **********************************/
    // Grab output from shock computation
    bool status=true;
    // Create empty image stroage
    vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();
    
    vcl_vector<bpro1_storage_sptr> shock_results;
    {
        // 3) Create shock pro process and assign inputs 
        dbsk2d_compute_ishock_process shock_pro;

        shock_pro.clear_input();
        shock_pro.clear_output();


        shock_pro.add_input(image_storage);
        shock_pro.add_input(input_vsol);

        // Set params
        shock_pro.parameters()->set_value("-exist_ids",true);
        shock_pro.parameters()->set_value("-fit_lines",false);
        shock_pro.parameters()->set_value("-b_prune",true);
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

        cg_pro.add_input(shock_storage);
        cg_pro.add_input(image_storage);

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


    // Grab composite graph storage
    dbskfg_composite_graph_storage_sptr cg_storage;
    cg_storage.vertical_cast(cg_results[0]);

    dbskfg_rag_node_sptr retain_region(0);
    int int_size=0;
    dbskfg_rag_graph_sptr rag_graph = cg_storage->get_rag_graph();
    for (dbskfg_rag_graph::vertex_iterator vit = rag_graph->vertices_begin(); 
         vit != rag_graph->vertices_end(); ++vit)
    {
        dbskfg_rag_node_sptr node = *vit;
        if ( node->get_shock_links().size() == 0 || 
             node->endpoint_spawned_node())
        {
            continue;
        }
        
        vcl_set<unsigned int> rag_ids;
        node->rag_contour_ids(rag_ids);
        
        vcl_set<unsigned int>::iterator it=rag_ids.find(10e6);
        if ( it != rag_ids.end())
        {
            continue;
        }

        vcl_vector<unsigned int> intersection(10000);
        vcl_vector<unsigned int>::iterator start_iterator;
        start_iterator=intersection.begin();
        vcl_vector<unsigned int>::iterator out_iterator;
        
        out_iterator=vcl_set_intersection(cons.begin(),
                                          cons.end(),
                                          rag_ids.begin(),
                                          rag_ids.end(),
                                          start_iterator);
        int temp = int(out_iterator-start_iterator);

        if ( temp > int_size )
        {
            retain_region=node;
            int_size=temp;
        }

    }

    if ( !retain_region )
    {
        vcl_cerr<<"Fragment is outside image"<<vcl_endl;
        cg_storage=0;
        return false;

    }

    dbskfg_composite_graph_sptr cgraph = cg_storage->get_composite_graph();
    vcl_vector<dbskfg_rag_node_sptr> regions_to_remove;

    for (dbskfg_rag_graph::vertex_iterator vit = rag_graph->vertices_begin(); 
         vit != rag_graph->vertices_end(); ++vit)
    {
        dbskfg_rag_node_sptr node = *vit;

        if ( node->id() != retain_region->id() )
        {
            vcl_map<unsigned int,dbskfg_shock_link*> shock_links=
                node->get_shock_links();
            vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
            for ( it= shock_links.begin() ; it != shock_links.end(); ++it)
            {

                vcl_vector<dbskfg_composite_link_sptr> left_links=
                    (*it).second->left_contour_links();

                for ( unsigned int k=0; k < left_links.size() ; ++k)
                {
                    dbskfg_contour_link* clink=
                        dynamic_cast<dbskfg_contour_link*>(&(*left_links[k]));
                    clink->delete_shock((*it).second->id());    
                    
                }

                vcl_vector<dbskfg_composite_link_sptr> right_links=
                    (*it).second->right_contour_links();

                for ( unsigned int k=0; k < right_links.size() ; ++k)
                {
                    dbskfg_contour_link* clink=
                        dynamic_cast<dbskfg_contour_link*>(&(*right_links[k]));
                    clink->delete_shock((*it).second->id());    
                    
                }

                dbskfg_contour_node* left_point=(*it).second->get_left_point();
                dbskfg_contour_node* right_point=
                    (*it).second->get_right_point();
                
                if ( left_point )
                {
                    left_point->delete_shock((*it).second->id());

                }

                if ( right_point )
                {
                    right_point->delete_shock((*it).second->id());

                }

                cgraph->remove_edge((*it).second);
                
            }
            regions_to_remove.push_back(node);
        }
        else
        {
            vcl_map<unsigned int,dbskfg_shock_node*> wavefront=
                node->get_wavefront();
            vcl_map<unsigned int,dbskfg_shock_node*>::iterator wit;
            for ( wit = wavefront.begin() ; wit != wavefront.end() ; ++wit)
            {
                (*wit).second->set_virtual(true);
                (*wit).second->set_composite_degree(1);
            }
        }
        
    }
    
    for ( unsigned int i=0; i < regions_to_remove.size() ; ++i)
    {

        rag_graph->remove_vertex(regions_to_remove[i]);
    }

    unsigned int numb_shock_edges=0;
    vcl_vector<dbskfg_composite_link_sptr> contours_to_erase;
    vcl_set<unsigned int> shock_nodes;
    for (dbskfg_composite_graph::edge_iterator eit = cgraph->edges_begin(); 
         eit != cgraph->edges_end(); ++eit)
    {
        if ( (*eit)->link_type() == dbskfg_composite_link::CONTOUR_LINK)
        {
            dbskfg_contour_link* clink=dynamic_cast<dbskfg_contour_link*>
                (&*(*eit));
            if ( clink->contour_id() == 10e6 )
            {
                contours_to_erase.push_back(*eit);
            }
        }
        else
        {
            numb_shock_edges++;
            shock_nodes.insert((*eit)->source()->id());
            shock_nodes.insert((*eit)->target()->id());
        }

        

    }

    for ( unsigned int c=0; c < contours_to_erase.size() ; ++c)
    {
        cgraph->remove_edge(contours_to_erase[c]);
    }

    cgraph->purge_isolated_vertices();
    dbskfg_utilities::classify_nodes(cgraph);

    unsigned int degree_three_nodes=0;
    unsigned int numb_shock_nodes=0;

    for (dbskfg_composite_graph::vertex_iterator vit = 
             cgraph->vertices_begin(); 
         vit != cgraph->vertices_end(); ++vit)
    {
        if ( (*vit)->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            if ( (*vit)->get_composite_degree() == 3 )
            {
                degree_three_nodes++;
            }
        }
    }

    if ( numb_shock_edges > (shock_nodes.size()-1) )
    {
        vcl_cerr<<"Fragment has a cycle"<<vcl_endl;
        vcl_cerr<<"Numb shock edges: "<<numb_shock_edges<<vcl_endl;
        vcl_cerr<<"Numb shock nodes: "<<shock_nodes.size()<<vcl_endl;
        shock_results.clear();
        cg_storage=0;
        return false;
    }

    if ( prune_degree_three_nodes )
    {
        if ( degree_three_nodes < 1 )
        {
            vcl_cerr<<"Fragment has zero degree three nodes! "<<vcl_endl;
            vcl_cerr<<"Numb shock edges: "<<numb_shock_edges<<vcl_endl;
            vcl_cerr<<"Numb shock nodes: "<<shock_nodes.size()<<vcl_endl;
            vcl_cerr<<"Degree three nodes: "<<degree_three_nodes<<vcl_endl;
            shock_results.clear();
            cg_storage=0;
            return false;

        }

    }

    unsigned int id=cgraphs_.size();
    cgraphs_[id]=cgraph;

    // clear_output();
    // output_data_[0].push_back(cg_storage);

    return status;
}
