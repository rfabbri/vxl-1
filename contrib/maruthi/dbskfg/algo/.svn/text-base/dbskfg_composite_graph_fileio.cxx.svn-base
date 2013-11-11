// This is file shp/dbskfg/dbskfg_composite_graph_fileio.cxx

//:
// \file

#include <dbskfg/algo/dbskfg_composite_graph_fileio.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_shock_link_boundary.h>
#include <dbskfg/algo/dbskfg_prune_composite_graph.h>
#include <bxml/bxml_write.h>
#include <dbxml/dbxml_algos.h>

#include <vul/vul_psfile.h>
#include <vul/vul_file.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>

#include <vil/vil_load.h>

// vsol headers
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>

#include <vgl/vgl_convex.h>

#include <dbskfg/algo/dbskfg_transform_manager.h>
#include <dbsk2d/dbsk2d_file_io.h>
// ----------------------------------------------------------------------------
//: Constructor
dbskfg_composite_graph_fileio::dbskfg_composite_graph_fileio():
composite_graph_(0)
{

}

// ----------------------------------------------------------------------------
//: Destructor
dbskfg_composite_graph_fileio::~dbskfg_composite_graph_fileio()
{

}

// ----------------------------------------------------------------------------
//: load method
void dbskfg_composite_graph_fileio::
load_composite_graph(dbskfg_composite_graph_sptr composite_graph,
                     vcl_string filename)
{
    // Set composite_graph
    composite_graph_ = composite_graph;

    // read document
    bxml_document doc_in = bxml_read(filename);
    bxml_data_sptr root_xml = doc_in.root_element();
    
    compile_nodes(root_xml);

    compile_links(root_xml);

    classify_nodes();

    vcl_cout<<"Printing out vertices: "<<
        composite_graph_->number_of_vertices()<<vcl_endl;

    vcl_cout<<"Printing out edges: "<<composite_graph_->number_of_edges()
            <<vcl_endl;
}

// ----------------------------------------------------------------------------
//: Write out specific rag node
void dbskfg_composite_graph_fileio::write_out(
    const dbskfg_rag_graph_sptr& rag_graph,
    const vil_image_resource_sptr& image,
    unsigned int id_to_extract,
    double prune_threshold,
    vcl_string output_prefix,
    vcl_string output_folder
)
{
  
    for (dbskfg_rag_graph::vertex_iterator vit = rag_graph->vertices_begin(); 
             vit != rag_graph->vertices_end(); ++vit)
    {
        // Create filename
        vcl_stringstream xml_filename,image_filename;
        xml_filename   << "frag_graph_"<<(*vit)->id()<<".xml";
        image_filename << "frag_image_"<<(*vit)->id();
        
        vcl_string output_string_xml = output_folder+"/"+
            output_prefix+"_"+xml_filename.str();
        vcl_string output_string_image = output_folder+"/"+
            output_prefix+"_"+image_filename.str();

        if ( (*vit)->id() == id_to_extract )
        {
                                
            // Grab shock links from rag node
            vcl_map<unsigned int,dbskfg_shock_link*> shock_links;
            prune_rag_node(shock_links,*vit,prune_threshold);
            
            if ( shock_links.size() )
            {

                vcl_cout<<"Writing out "<<output_string_xml<<vcl_endl;
                write_rag_node(*vit,shock_links,output_string_xml);

                if ( image )
                {
                    write_rag_node_image(*vit,shock_links,image,
                                         output_string_image);
                }
            }
        }
    }    
}


// ----------------------------------------------------------------------------
//: Write out specific rag node
void dbskfg_composite_graph_fileio::write_out(
    const dbskfg_rag_node_sptr& rag_node,
    const vil_image_resource_sptr& image,
    double prune_threshold,
    vcl_string output_prefix,
    vcl_string output_folder
)
{
    
    dbskfg_transform_manager::Instance().
               write_output_region(rag_node);

    // Create filename
    // vcl_stringstream xml_filename,image_filename;
    // xml_filename   << "frag_graph_"<<rag_node->id()<<".xml";
    // image_filename << "frag_image_"<<rag_node->id();
    
    // vcl_string output_string_xml = output_folder+"/"+
    //     output_prefix+"_"+xml_filename.str();
    // vcl_string output_string_image = output_folder+"/"+
    //     output_prefix+"_"+image_filename.str();
                                
    // Grab shock links from rag node
    // vcl_map<unsigned int,dbskfg_shock_link*> shock_links;
    // prune_rag_node(shock_links,rag_node,prune_threshold);
            
    // if ( shock_links.size() )
    // {

    //     // grab polygon from rag node
    //     vgl_polygon<double> poly;
    //     rag_node->fragment_boundary(poly);

    //     dbskfg_transform_manager::Instance().
    //         write_output_polygon(poly);

    //     write_rag_node_one_file(rag_node,shock_links);


        // vcl_cout<<"Writing out "<<output_string_xml<<vcl_endl;
        // //  write_rag_node(rag_node,shock_links,output_string_xml);
        
        // if ( image )
        // {
        //     write_rag_node_image(rag_node,shock_links,image,
        //                          output_string_image);
        // }

        // dbskfg_transform_manager::Instance().
        //       write_output_polygon(poly);

        // dbskfg_utilities::save_image_mask(poly,
        //                                   image,
        //                                   output_string_image+"_mask.png");

        // }

//    }
}


// ----------------------------------------------------------------------------
//: Write out transform
void dbskfg_composite_graph_fileio::write_out_transform(
    const dbskfg_transform_descriptor_sptr& transform,
    const vil_image_resource_sptr& image,
    vcl_string filename)
{

    // ************ Determine polygon for image ***************************

    // Keep a vector all points
    vcl_vector<vgl_point_2d<double> > points;

    // Convert all contours to vsol objects for easy rendering
    vcl_vector<dbskfg_composite_link_sptr>::iterator lit;

    for ( lit = transform->contours_affected_.begin() ; 
          lit != transform->contours_affected_.end() ; ++lit )
    {
        points.push_back((*lit)->source()->pt());
        points.push_back((*lit)->target()->pt());
   
    }

    for ( unsigned int i=0; i < transform->all_gaps_.size() ; ++i )
    {
        vcl_pair<dbskfg_composite_node_sptr,dbskfg_composite_node_sptr>
            gap = transform->all_gaps_[i];
        
        dbskfg_composite_node_sptr node = gap.first;

        dbskfg_composite_node::edge_iterator srit;
       
        // ************************ In Edges ***************************
        for ( srit = node->in_edges_begin() ; 
              srit != node->in_edges_end() 
                  ; ++srit)
        {
            points.push_back((*srit)->opposite(node)->pt());
        }
        
        // ************************ Out Edges ***************************
        for ( srit = node->out_edges_begin() ; 
              srit != node->out_edges_end() 
                  ; ++srit)
        {
            points.push_back((*srit)->opposite(node)->pt());
        }

        node = gap.second;

        // ************************ In Edges ***************************
        for ( srit = node->in_edges_begin() ; 
              srit != node->in_edges_end() 
                  ; ++srit)
        {
            points.push_back((*srit)->opposite(node)->pt());
        }
        
        // ************************ Out Edges ***************************
        for ( srit = node->out_edges_begin() ; 
              srit != node->out_edges_end() 
                  ; ++srit)
        {
            points.push_back((*srit)->opposite(node)->pt());
        }
      
    }

    // create a ps file object
    vcl_string psfile_name = filename;
    vul_psfile psfile(psfile_name.c_str(), false);


    vgl_polygon<double> image_poly = vgl_convex_hull(points);

    dbskfg_utilities::save_image_mask(image_poly,
                                      image,
                                      "training_temp.png",
                                      true);
    // Grab image
    vil_image_resource_sptr img_sptr = 
        vil_load_image_resource("training_temp.png");


    // Get image view
    vil_image_view<vxl_byte> img= img_sptr->get_view();

    if ( img.nplanes() == 1 )
    {
        psfile.print_greyscale_image(img.top_left_ptr(),
                                     img.ni(),
                                     img.nj());

    }
    else
    {
        psfile.print_color_image(img.top_left_ptr(),
                                 img.ni(),
                                 img.nj());
    }

    psfile.set_line_width(2.0f);
    psfile.set_fg_color(0.0f,0.0f,1.0f);

    for ( unsigned int i=0; i < transform->new_contours_spatial_objects_.size()
              ;++i)
    {
        
        vsol_spatial_object_2d_sptr object = 
            transform->new_contours_spatial_objects_[i];

        if ( object->cast_to_curve()->cast_to_line())
        {
            vsol_line_2d_sptr line = object->
                cast_to_curve()->cast_to_line();
            psfile.line(line->p0()->x(),line->p0()->y(),
                        line->p1()->x(),line->p1()->y());
        }
        else
        {
            vsol_polyline_2d_sptr polyline = 
                object->cast_to_curve()->cast_to_polyline();
            
            // collect vertices of the polyline
            vcl_vector<vgl_point_2d<double > > pts;
            
            for (unsigned int i = 1; i < polyline->size(); i++)
            {
                psfile.line(
                    polyline->vertex(i-1)->x(),
                    polyline->vertex(i-1)->y(),
                    polyline->vertex(i  )->x(),
                    polyline->vertex(i  )->y());
            }
            
            
        }
    }

    psfile.set_line_width(2.0f);
    psfile.set_fg_color(1.0f,0.0f,0.0f);


    for ( unsigned int i=0; i < transform->all_gaps_.size() ; ++i )
    {
        vcl_pair<dbskfg_composite_node_sptr,dbskfg_composite_node_sptr>
            gap = transform->all_gaps_[i];
        
        dbskfg_composite_node_sptr node = gap.first;

        dbskfg_composite_node::edge_iterator srit;
       
        // ************************ In Edges ***************************
        for ( srit = node->in_edges_begin() ; 
              srit != node->in_edges_end() 
                  ; ++srit)
        {
            vgl_point_2d<double> pt = (*srit)->opposite(node)->pt();
            psfile.line(node->pt().x(),node->pt().y(),
                        pt.x(),pt.y());
        }
        
        // ************************ Out Edges ***************************
        for ( srit = node->out_edges_begin() ; 
              srit != node->out_edges_end() 
                  ; ++srit)
        {
            vgl_point_2d<double> pt = (*srit)->opposite(node)->pt();
            psfile.line(node->pt().x(),node->pt().y(),
                        pt.x(),pt.y());
        }

        node = gap.second;

        // ************************ In Edges ***************************
        for ( srit = node->in_edges_begin() ; 
              srit != node->in_edges_end() 
                  ; ++srit)
        {
            vgl_point_2d<double> pt = (*srit)->opposite(node)->pt();
            psfile.line(node->pt().x(),node->pt().y(),
                        pt.x(),pt.y());
        }
        
        // ************************ Out Edges ***************************
        for ( srit = node->out_edges_begin() ; 
              srit != node->out_edges_end() 
                  ; ++srit)
        {
            vgl_point_2d<double> pt = (*srit)->opposite(node)->pt();
            psfile.line(node->pt().x(),node->pt().y(),
                        pt.x(),pt.y());
        }
      
    }

    for ( lit = transform->contours_affected_.begin() ; 
          lit != transform->contours_affected_.end() ; ++lit )
    {
        psfile.line((*lit)->source()->pt().x(),(*lit)->source()->pt().y(),
                    (*lit)->target()->pt().x(),(*lit)->target()->pt().y());   
    }

    vul_file::delete_file_glob("training_temp.png");
}

// ----------------------------------------------------------------------------
//: Write out all rag nodes
void dbskfg_composite_graph_fileio::write_out(
    const dbskfg_rag_graph_sptr& rag_graph,
    const vil_image_resource_sptr& image,
    vcl_string output_prefix,
    vcl_string output_folder,
    double contour_threshold,
    double prune_threshold)
{

    // Keep a vector and write a file for all fragments
    vcl_vector<vcl_string> fragment_list;

    unsigned int frags_write=0;
    for (dbskfg_rag_graph::vertex_iterator vit = rag_graph->vertices_begin(); 
             vit != rag_graph->vertices_end(); ++vit)
    {
        // Create filename
        vcl_stringstream xml_filename,image_filename;
        xml_filename   << "frag_graph_"<<(*vit)->id()<<".xml";
        image_filename << "frag_image_"<<(*vit)->id();
        
        vcl_string output_string_xml = output_folder+"/"+
            output_prefix+"_"+xml_filename.str();
        vcl_string output_string_image = output_folder+"/"+
            output_prefix+"_"+image_filename.str();


        if ( (*vit)->is_rag_node_within_image(image->ni(),image->nj() ))
        {
            if (  !(*vit)->endpoint_spawned_node() )
            {
                if ( (*vit)->contour_ratio() >= contour_threshold )
                {
                    
                    // Grab shock links from rag node
                    vcl_map<unsigned int,dbskfg_shock_link*> shock_links;
                    prune_rag_node(shock_links,*vit,prune_threshold);

                    if ( shock_links.size() )
                    {
                        vcl_cout<<"Writing out "<<output_string_xml<<vcl_endl;
                        write_rag_node(*vit,shock_links,output_string_xml);
                    
                        if ( image )
                        {
                            write_rag_node_image(
                                *vit,shock_links,image,output_string_image);
                        }

                        frags_write++;

                        fragment_list.push_back(output_string_xml);
                    }
                }
            }
        }
      
    }

    // write a file of all fragments
    vcl_string fragment_file_name = output_folder + "/" + output_prefix+
        "_fragment_list.txt";
    vcl_ofstream fragment(fragment_file_name.c_str());
    for ( unsigned int c=0; c < fragment_list.size() ; ++c)
    {
        fragment<<fragment_list[c]<<vcl_endl;
    }
    fragment.close();


    vcl_cout<<"Writing out "<<frags_write<<" fragments out of a total of "
            << rag_graph->number_of_vertices()<<vcl_endl;
}


// ----------------------------------------------------------------------------
//: Write out all rag nodes
void dbskfg_composite_graph_fileio::write_out(
    const dbskfg_rag_graph_sptr& rag_graph,
    const vil_image_resource_sptr& image,
    vcl_string output_prefix,
    vcl_string output_folder,
    double contour_threshold,
    double prune_threshold,
    vgl_box_2d<double> bbox)
{

    // Keep a vector and write a file for all fragments
    vcl_vector<vcl_string> fragment_list;

    unsigned int frags_write=0;
    for (dbskfg_rag_graph::vertex_iterator vit = rag_graph->vertices_begin(); 
             vit != rag_graph->vertices_end(); ++vit)
    {
        // Create filename
        vcl_stringstream xml_filename,image_filename;
        xml_filename   << "frag_graph_"<<(*vit)->id()<<".xml";
        image_filename << "frag_image_"<<(*vit)->id();
        
        vcl_string output_string_xml = output_folder+"/"+
            output_prefix+"_"+xml_filename.str();
        vcl_string output_string_image = output_folder+"/"+
            output_prefix+"_"+image_filename.str();


        if ( (*vit)->is_rag_node_within_image(image->ni(),image->nj() ))
        {
            if ( (*vit)->intersection_bbox(bbox))
            {
                if (  !(*vit)->endpoint_spawned_node() )
                {
                    if ( (*vit)->contour_ratio() >= contour_threshold )
                    {
                    
                        // Grab shock links from rag node
                        vcl_map<unsigned int,dbskfg_shock_link*> shock_links;
                        prune_rag_node(shock_links,*vit,prune_threshold);

                        if ( shock_links.size() )
                        {
                            vcl_cout<<"Writing out "<<
                                output_string_xml<<vcl_endl;
                            write_rag_node(*vit,shock_links,output_string_xml);
                    
                            if ( image )
                            {
                                write_rag_node_image(
                                    *vit,shock_links,image,
                                    output_string_image);
                            }

                            frags_write++;

                            fragment_list.push_back(output_string_xml);
                        }
                    }
                }
            }
        }
      
    }

    // write a file of all fragments
    vcl_string fragment_file_name = output_folder + "/" + output_prefix+
        "_fragment_list.txt";
    vcl_ofstream fragment(fragment_file_name.c_str());
    for ( unsigned int c=0; c < fragment_list.size() ; ++c)
    {
        fragment<<fragment_list[c]<<vcl_endl;
    }
    fragment.close();


    vcl_cout<<"Writing out "<<frags_write<<" fragments out of a total of "
            << rag_graph->number_of_vertices()<<vcl_endl;
}


// ----------------------------------------------------------------------------
//: Compile all nodes for new composite graph
void dbskfg_composite_graph_fileio::compile_nodes(
    const bxml_data_sptr& root_xml)
{
    vcl_vector<bxml_data_sptr> contour_points; // contour_points
    vcl_vector<bxml_data_sptr> shock_nodes;    // shock_nodes

    // Lets find contour points
    bxml_element* head=dbxml_algos::find_by_name(root_xml,"points");
    dbxml_algos::find_all_elems_by_name(head,"contour_node",contour_points);
    
    // Lets find shock_nodes
    bxml_element* head2=dbxml_algos::find_by_name(root_xml,"nodes");
    dbxml_algos::find_all_elems_by_name(head2,"shock_node",shock_nodes);

    //Loop over contour_points
    for ( unsigned int c=0; c < contour_points.size() ; ++c)
    {
        bxml_element* data_elm = static_cast<bxml_element*>
            ((contour_points[c]).ptr());
        
        // Grab all attributes
        unsigned int id;
        double x,y;
        
        data_elm->get_attribute("id",id);
        data_elm->get_attribute("x",x);
        data_elm->get_attribute("y",y);

        // Create point
        vgl_point_2d<double> location(x,y);

        dbskfg_composite_node_sptr cnode = new dbskfg_contour_node(
            composite_graph_->next_available_id(),
            location,
            2 );
        composite_graph_->add_vertex(cnode);

        all_nodes_[id]=cnode;
    }

    //Loop over contour_points
    for ( unsigned int s=0; s < shock_nodes.size() ; ++s)
    {
        bxml_element* data_elm = static_cast<bxml_element*>
            ((shock_nodes[s]).ptr());
        
        // Grab all attributes
        unsigned int id;
        double x,y,radius;
        bool virtual_flag;

        data_elm->get_attribute("id",id);
        data_elm->get_attribute("radius",radius);
        data_elm->get_attribute("x",x);
        data_elm->get_attribute("y",y);
        data_elm->get_attribute("virtual",virtual_flag);

        // Create point
        vgl_point_2d<double> location(x,y);

        dbskfg_composite_node_sptr cnode = new dbskfg_shock_node(
            composite_graph_->next_available_id(),
            location,
            radius );
        composite_graph_->add_vertex(cnode);

        if ( virtual_flag)
        {
            cnode->set_virtual(virtual_flag);
        }

        all_nodes_[id]=cnode;
    }
}

// ----------------------------------------------------------------------------
//: Compile all nodes for new composite graph
void dbskfg_composite_graph_fileio::compile_links(
    const bxml_data_sptr& root_xml)
{
    vcl_vector<bxml_data_sptr> contour_links; // contour_points
    vcl_vector<bxml_data_sptr> shock_links;    // shock_nodes

    // Lets find contour points
    bxml_element* head=dbxml_algos::find_by_name(root_xml,"lines");
    dbxml_algos::find_all_elems_by_name(head,"contour_link",contour_links);
    
    // Lets find shock_links
    bxml_element* head2=dbxml_algos::find_by_name(root_xml,"links");
    dbxml_algos::find_all_elems_by_name(head2,"shock_link",shock_links);

    //Loop over contour_links
    for ( unsigned int c=0; c < contour_links.size() ; ++c)
    {
        bxml_element* data_elm = static_cast<bxml_element*>
            ((contour_links[c]).ptr());
        
        // get all attributes
        unsigned int id,start,stop;
        data_elm->get_attribute("id",id);
        data_elm->get_attribute("start",start);
        data_elm->get_attribute("stop",stop);

        dbskfg_composite_link_sptr clink = new dbskfg_contour_link(
            all_nodes_[start],
            all_nodes_[stop],
            composite_graph_->next_available_id());
        composite_graph_->add_edge(clink);
        all_nodes_[start]->add_outgoing_edge(clink);
        all_nodes_[stop]->add_incoming_edge(clink);

        contour_links_[id]=clink;

    }
 
    //Loop over shock_links
    for ( unsigned int s=0; s < shock_links.size() ; ++s)
    {
        bxml_element* data_elm = static_cast<bxml_element*>
            ((shock_links[s]).ptr());
        
        // Grab all attributes
        unsigned start,stop;
        data_elm->get_attribute("start",start);
        data_elm->get_attribute("stop",stop);

        dbskfg_composite_link_sptr slink = new dbskfg_shock_link(
            all_nodes_[start],
            all_nodes_[stop],
            composite_graph_->next_available_id(),
            dbskfg_shock_link::SHOCK_EDGE);
        composite_graph_->add_edge(slink);
        all_nodes_[start]->add_outgoing_edge(slink);
        all_nodes_[stop]->add_incoming_edge(slink);

        // Look for left and right info 
        bxml_element* left  = dbxml_algos::find_by_name(data_elm,"left");
        bxml_element* right = dbxml_algos::find_by_name(data_elm,"right");

        
        unsigned int left_index,right_index,leftp_index,rightp_index;

        dbskfg_shock_link_boundary left_boundary,right_boundary;

        dbskfg_shock_link* sedge = dynamic_cast<dbskfg_shock_link*>
            (&(*slink));
    
        // Work on left first
        if ( left->get_attribute("line",left_index))
        {
            // Grab the contour link
            dbskfg_composite_link_sptr clink = contour_links_[left_index];
            vgl_point_2d<double> start(clink->source()->pt().x(),
                                       clink->source()->pt().y());
            vgl_point_2d<double> stop(clink->target()->pt().x(),
                                      clink->target()->pt().y());
            left_boundary.contour_.push_back(start);
            left_boundary.contour_.push_back(stop);
            sedge->add_to_left_contour(clink);
        }
        else
        {
            left->get_attribute("point",leftp_index);

            // Grab the contour link
            dbskfg_composite_node_sptr cnode = all_nodes_[leftp_index];
            left_boundary.point_ = cnode->pt();

            dbskfg_contour_node* ccnode = dynamic_cast<dbskfg_contour_node*>
                (&(*cnode));
            sedge->add_left_point(ccnode);
        }

        // Work on right first
        if ( right->get_attribute("line",right_index))
        {
            
            // Grab the contour link
            dbskfg_composite_link_sptr clink = contour_links_[right_index];
            vgl_point_2d<double> start(clink->source()->pt().x(),
                                       clink->source()->pt().y());
            vgl_point_2d<double> stop(clink->target()->pt().x(),
                                      clink->target()->pt().y());
            right_boundary.contour_.push_back(start);
            right_boundary.contour_.push_back(stop);
            sedge->add_to_right_contour(clink);
        }
        else
        {
            right->get_attribute("point",rightp_index);

            // Grab the contour link
            dbskfg_composite_node_sptr cnode = all_nodes_[rightp_index];
            right_boundary.point_ = cnode->pt();
            
            dbskfg_contour_node* ccnode = dynamic_cast<dbskfg_contour_node*>
                (&(*cnode));
            sedge->add_right_point(ccnode);
        }

        sedge->set_left_boundary(left_boundary);
        sedge->set_right_boundary(right_boundary);
        sedge->construct_locus();

    }
}

// ----------------------------------------------------------------------------
//: Write out image of rag node
void dbskfg_composite_graph_fileio::write_rag_node_image(
    const dbskfg_rag_node_sptr& rag_node,
    vcl_map<unsigned int,dbskfg_shock_link*>& 
    shock_links,                     
    const vil_image_resource_sptr& image ,
    vcl_string filename)
{
    // create a ps file object
    vcl_string psfile_name = filename+".ps";
    vul_psfile psfile(psfile_name.c_str(), false);

    // Get image view
    vil_image_view<vxl_byte> img= image->get_view();

    if ( img.nplanes() == 1 )
    {
        psfile.print_greyscale_image(img.top_left_ptr(),
                                     img.ni(),
                                     img.nj());

    }
    else
    {
        psfile.print_color_image(img.top_left_ptr(),
                                 img.ni(),
                                 img.nj());
    }

  
    // Draw shock graph first
    psfile.set_line_width(1.0f);
    psfile.set_fg_color(0.0f,1.0f,0.0f);

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it=shock_links.begin() ; it != shock_links.end() ; ++it)
    {
        dbskfg_shock_link* slink = (*it).second;
        for( unsigned int ii = 1 ; ii < slink->ex_pts().size() ; ii++ ) 
        {
            psfile.line(slink->ex_pts()[ii-1].x(),slink->ex_pts()[ii-1].y(),
                        slink->ex_pts()[ii  ].x(),slink->ex_pts()[ii  ].y());

        }

    }

    psfile.set_line_width(2.0f);
    psfile.set_fg_color(1.0f,0.0f,0.0f);

    // grab polygon from rag node
    vgl_polygon<double> poly;
    rag_node->fragment_boundary(poly);

    for (unsigned int s = 0; s < poly.num_sheets(); ++s)
    { 
        for (unsigned int ii = 1; ii < poly[s].size(); ++ii)
        {
            psfile.line(poly[s][ii-1].x(),poly[s][ii-1].y(),
                        poly[s][ii  ].x(),poly[s][ii  ].y());
  
        }

        psfile.line(poly[s][poly[s].size()-1].x(),
                    poly[s][poly[s].size()-1].y(),
                    poly[s][0].x(), poly[s][0].y());
 
    }

    // Draw shock rays
    vcl_vector< vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > >
        shock_rays = rag_node->determine_shock_rays(poly);
   
    psfile.set_line_width(2.0f);
    psfile.set_fg_color(0.0f,0.0f,1.0f);
    
     
    for ( unsigned int k=0; k <shock_rays.size() ; ++k)
    {
        psfile.line(shock_rays[k].first.x(),
                    shock_rays[k].first.y(),
                    shock_rays[k].second.x(),
                    shock_rays[k].second.y());
    }

    dbskfg_utilities::save_image_poly(poly,
                                      image,
                                      filename+".png");

    dbskfg_utilities::save_image_mask(poly,
                                      image,
                                      filename+"_mask.png");

}

// ----------------------------------------------------------------------------
//: Prune rag node
void dbskfg_composite_graph_fileio::
prune_rag_node(vcl_map<unsigned int,dbskfg_shock_link*>& shock_links,
               dbskfg_rag_node_sptr rag_node,
               double prune_threshold)
{

    // Grab shock links from rag node
    vcl_map<unsigned int,dbskfg_shock_link*> prune_links=
        rag_node->get_shock_links();

    dbskfg_prune_composite_graph pruner(rag_node);

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator mits;
    for ( mits=prune_links.begin() ; mits != prune_links.end() ; ++mits)
    {
        if ( pruner.splice_cost((*mits).first) > prune_threshold)
        {
            shock_links[(*mits).first]=(*mits).second;
        }
    }

}

// ----------------------------------------------------------------------------
//: Write out all rag nodes
void dbskfg_composite_graph_fileio::write_rag_node(
    const dbskfg_rag_node_sptr& rag_node,
    vcl_map<unsigned int,dbskfg_shock_link*>& shock_links,
    vcl_string filename)
{

    // Create root element
    bxml_document  doc;
    bxml_data_sptr root     = new bxml_element("composite_graph");
    bxml_element*  root_elm = dbxml_algos::
        cast_to_element(root,"composite_graph");
    doc.set_root_element(root_elm); 
    root_elm->append_text("\n   ");
    
    // Create contour set
    bxml_data_sptr contour     = new bxml_element("contour");
    bxml_element*  contour_elm = dbxml_algos::cast_to_element(contour,
                                                              "contour");
    root_elm->append_data(contour);
    root_elm->append_text("\n   ");
    contour_elm->append_text("\n      ");
   
    // Create contour points within contour set
    bxml_data_sptr points     = new bxml_element("points");
    bxml_element*  points_elm = dbxml_algos::cast_to_element(points,
                                                             "points");

    // Create line set within countour set
    bxml_data_sptr lines     = new bxml_element("lines");
    bxml_element*  lines_elm = dbxml_algos::cast_to_element(lines,
                                                            "lines");

    // Add points and lines
    contour_elm->append_data(points);  
    contour_elm->append_text("\n      ");
    contour_elm->append_data(lines);
    contour_elm->append_text("\n");
    
    // Create shock set
    bxml_data_sptr shock     = new bxml_element("shock");
    bxml_element*  shock_elm = dbxml_algos::cast_to_element(shock,
                                                            "shock");
  
    root_elm->append_data(shock);
    root_elm->append_text("\n");
    shock_elm->append_text("\n      ");
    
    // Create contour points within contour set
    bxml_data_sptr nodes     = new bxml_element("nodes");
    bxml_element*  nodes_elm = dbxml_algos::cast_to_element(nodes,
                                                            "nodes");

    // Create line set within countour set
    bxml_data_sptr links     = new bxml_element("links");
    bxml_element*  links_elm = dbxml_algos::cast_to_element(links,
                                                            "links");

    // Add points and lines
    shock_elm->append_data(nodes);  
    shock_elm->append_text("\n      ");
    shock_elm->append_data(links);
    shock_elm->append_text("\n");
     
    // Close out brackets
    contour_elm->append_text("   ");
    shock_elm->append_text("   ");

    // Grab all points to put in contour nodes
    vcl_map<vcl_string,vcl_pair<unsigned int,vgl_point_2d<double> > > 
        contour_map;
    vcl_vector<vcl_pair<unsigned int,unsigned int> > contour_lines;
    vcl_map<unsigned int,vcl_pair<unsigned int,dbskfg_composite_node_sptr> >
        shock_map;
    vcl_vector<vcl_pair<unsigned int,unsigned int> > shock_lines;
    
    // Create a mapping of each shock id to its corresponiding line pair
    // pair first left, second right
    vcl_map<unsigned int,vcl_pair<unsigned int,unsigned int> > 
        shock_to_contour;
    
    unsigned int node_id=0;

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it=shock_links.begin() ; it != shock_links.end() ; ++it)
    {
        dbskfg_shock_link *slink = (*it).second;

        // Grab left and right boundary for shock
        dbskfg_shock_link_boundary left_boundary = 
            slink->get_left_boundary();
        dbskfg_shock_link_boundary right_boundary = 
            slink->get_right_boundary();
        if ( slink->source()->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            if ( shock_map.count(slink->source()->id())== 0)
            {
                shock_map[slink->source()->id()]=
                    vcl_make_pair(node_id,
                                  slink->source());
                node_id++;
            }
        }

        if ( slink->target()->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            if ( shock_map.count(slink->target()->id())== 0)
            {
                shock_map[slink->target()->id()]=
                    vcl_make_pair(node_id,
                                  slink->target());
                node_id++;
            }
        }
        
        if ( slink->shock_compute_type() == dbskfg_utilities::RLLP )
        {
            vgl_point_2d<double> r_first_point= right_boundary.contour_.front();
            vgl_point_2d<double> r_last_point = right_boundary.contour_.back();
         
            vcl_stringstream rstream_first,rstream_last,p_stream;
            rstream_first<< r_first_point;
            rstream_last << r_last_point;
            p_stream     << left_boundary.point_;

            // See if we should add right first point
            if ( contour_map.count(rstream_first.str())==0)
            {
                contour_map[rstream_first.str()]=vcl_make_pair(node_id,
                                                               r_first_point);
                node_id++;
            }

            if ( contour_map.count(rstream_last.str())==0)
            {
                contour_map[rstream_last.str()]=vcl_make_pair(node_id,
                                                              r_last_point);
                node_id++;
            }
       
            if ( contour_map.count(p_stream.str())==0)
            {
                contour_map[p_stream.str()]=vcl_make_pair(
                    node_id,left_boundary.point_);
                node_id++;
            }

            contour_lines.push_back(vcl_make_pair
                                    (contour_map[rstream_first.str()].first,
                                     contour_map[rstream_last.str()].first));

            shock_to_contour[slink->id()].first = 
                contour_map[p_stream.str()].first;        
            shock_to_contour[slink->id()].second=
                contour_lines.size()-1;

        }
        else if ( slink->shock_compute_type() == dbskfg_utilities::LLRP )
        {
            vgl_point_2d<double> l_first_point = left_boundary.contour_.front();
            vgl_point_2d<double> l_last_point  = left_boundary.contour_.back();

            vcl_stringstream lstream_first,lstream_last,p_stream;
            lstream_first<< l_first_point;
            lstream_last << l_last_point;
            p_stream     << right_boundary.point_;

            // See if we should add right first point
            if ( contour_map.count(lstream_first.str())==0)
            {
                contour_map[lstream_first.str()]=vcl_make_pair(node_id,
                                                               l_first_point);
                node_id++;
            }
            
            if ( contour_map.count(lstream_last.str())==0)
            {
                contour_map[lstream_last.str()]=vcl_make_pair(node_id,
                                                              l_last_point);
                node_id++;
            }

            if ( contour_map.count(p_stream.str())==0)
            {
                contour_map[p_stream.str()]= vcl_make_pair(
                    node_id,right_boundary.point_);

                node_id++;
            }

            contour_lines.push_back(
                vcl_make_pair(contour_map[lstream_first.str()].first,
                              contour_map[lstream_last.str()].first));

            shock_to_contour[slink->id()].first=
                contour_lines.size()-1;
            shock_to_contour[slink->id()].second = 
                contour_map[p_stream.str()].first;
           
        }
        else if ( slink->shock_compute_type() == dbskfg_utilities::LL )
        {
            vgl_point_2d<double> r_first_point= right_boundary.contour_.front();
            vgl_point_2d<double> r_last_point = right_boundary.contour_.back();
            vgl_point_2d<double> l_first_point= left_boundary.contour_.front();
            vgl_point_2d<double> l_last_point = left_boundary.contour_.back();

            vcl_stringstream lstream_first,lstream_last;
            vcl_stringstream rstream_first,rstream_last;

            lstream_first<< l_first_point;
            lstream_last << l_last_point;
            rstream_first<< r_first_point;
            rstream_last << r_last_point;

            // See if we should add right first point
            if ( contour_map.count(lstream_first.str())==0)
            {
                contour_map[lstream_first.str()]=vcl_make_pair(node_id,
                                                               l_first_point);
                node_id++;
            }
            
            if ( contour_map.count(lstream_last.str())==0)
            {
                contour_map[lstream_last.str()]=vcl_make_pair(node_id,
                                                              l_last_point);
                node_id++;
            }

            contour_lines.push_back(vcl_make_pair
                                    (contour_map[lstream_first.str()].first,
                                     contour_map[lstream_last.str()].first));

            shock_to_contour[slink->id()].first = contour_lines.size()-1;


            // See if we should add right first point
            if ( contour_map.count(rstream_first.str())==0)
            {
                contour_map[rstream_first.str()]=vcl_make_pair(node_id,
                                                               r_first_point);
                node_id++;
            }

            if ( contour_map.count(rstream_last.str())==0)
            {
                contour_map[rstream_last.str()]=vcl_make_pair(node_id,
                                                              r_last_point);
                node_id++;
            }

            contour_lines.push_back(vcl_make_pair
                                    (contour_map[rstream_first.str()].first,
                                     contour_map[rstream_last.str()].first));

            shock_to_contour[slink->id()].second = contour_lines.size()-1;


        }
        else if ( slink->shock_compute_type() == dbskfg_utilities::PP )
        {
            vcl_stringstream r_pstream,l_pstream;
            l_pstream     << left_boundary.point_;
            r_pstream     << right_boundary.point_;

            if ( contour_map.count(l_pstream.str())==0)
            {
                contour_map[l_pstream.str()]= vcl_make_pair(
                    node_id,left_boundary.point_);

                node_id++;
            }

            if ( contour_map.count(r_pstream.str())==0)
            {
                contour_map[r_pstream.str()]= vcl_make_pair(
                    node_id,right_boundary.point_);

                node_id++;

            }

            shock_to_contour[slink->id()].first = 
                contour_map[l_pstream.str()].first;
            shock_to_contour[slink->id()].second = 
                contour_map[r_pstream.str()].first;
          
        }

        // Figure out shock links
        unsigned int source_shock_id,target_shock_id;

        if ( slink->source()->node_type() == dbskfg_composite_node::
             CONTOUR_NODE )
        {
            
            vcl_stringstream stream;
            stream<<slink->source()->pt();
           
            source_shock_id = contour_map[stream.str()].first;

        } 
        else
        {
            source_shock_id = shock_map[slink->source()->id()].first;
        }

        if ( slink->target()->node_type() == dbskfg_composite_node::
             CONTOUR_NODE )
        {   
            vcl_stringstream stream;
            stream<<slink->target()->pt();
            
            target_shock_id = contour_map[stream.str()].first;

        } 
        else
        {
            target_shock_id = shock_map[slink->target()->id()].first;
        }
        
        shock_lines.push_back(vcl_make_pair(source_shock_id,target_shock_id));


    }

    // Append into xml file
    points_elm->append_text("\n            ");

    // Write out all points
    vcl_map<vcl_string,vcl_pair<unsigned int,vgl_point_2d<double> > >::iterator 
        mit;

    unsigned int size=0;
    for ( mit = contour_map.begin() ; mit != contour_map.end() ; ++mit)
    {
        unsigned int id = (*mit).second.first;
        vgl_point_2d<double> point = (*mit).second.second;

        bxml_data_sptr cnode = new bxml_element("contour_node");
        bxml_element *cnode_elm = dbxml_algos::cast_to_element(
            cnode,"contour_node");

        points_elm->append_data(cnode_elm);
        cnode_elm->set_attribute("id",id);

        vcl_stringstream xstream,ystream;
        xstream<<vcl_fixed; ystream<<vcl_fixed;
        xstream.precision(10); ystream.precision(10);
        xstream<<point.x();  ystream<<point.y();
        
        cnode_elm->set_attribute("x",xstream.str());
        cnode_elm->set_attribute("y",ystream.str());
        
        if ( size == contour_map.size() - 1 )
        {
            points_elm->append_text("\n      ");
        }
        else
        {
            points_elm->append_text("\n            ");
            
        }
        size++;
        
    }

    // Append into xml file
    lines_elm->append_text("\n            ");
    size=0;
    // Write out contour lines
    for ( unsigned int k=0; k < contour_lines.size() ; ++k)
    {
        vcl_pair<unsigned int,unsigned int> line=
            contour_lines[k];
        bxml_data_sptr lnode = new bxml_element("contour_link");
        bxml_element *lnode_elm = dbxml_algos::cast_to_element(
            lnode,"contour_link");

        lines_elm->append_data(lnode_elm);
        lnode_elm->set_attribute("id",k);
        lnode_elm->set_attribute("start",line.first);
        lnode_elm->set_attribute("stop", line.second);    

        if ( size == contour_lines.size() - 1 )
        {
            lines_elm->append_text("\n      ");
        }
        else
        {
            lines_elm->append_text("\n            ");
        }
        size++;
        
    }

    nodes_elm->append_text("\n            ");
    size=0;
    vcl_map<unsigned int,vcl_pair<unsigned int,dbskfg_composite_node_sptr> >
        ::iterator cit;
    for ( cit = shock_map.begin() ; cit != shock_map.end() ; ++cit)
    {
        dbskfg_shock_node* shock_node =
            dynamic_cast<dbskfg_shock_node*>(&(*(*cit).second.second));
        unsigned int id = (*cit).second.first;
        bxml_data_sptr snode = new bxml_element("shock_node");
        bxml_element *snode_elm = dbxml_algos::cast_to_element(
            snode,"shock_node");

        nodes_elm->append_data(snode_elm);
        snode_elm->set_attribute("id",id);

        // Grad x,y points
        vcl_stringstream xstream,ystream;
        xstream<<vcl_fixed; ystream<<vcl_fixed;
        xstream.precision(10); ystream.precision(10);
        xstream<<shock_node->pt().x();  ystream<<shock_node->pt().y();

        snode_elm->set_attribute("x",xstream.str());
        snode_elm->set_attribute("y",ystream.str());

        // Grab radius
        vcl_stringstream rad_stream;
        rad_stream<<vcl_fixed; 
        rad_stream.precision(10);
        rad_stream<<shock_node->get_radius();

        snode_elm->set_attribute("radius",rad_stream.str());
        snode_elm->set_attribute("theta",0);
        snode_elm->set_attribute("phi",0);
        snode_elm->set_attribute("virtual",rag_node->part_of_wavefront
                                 (shock_node->id()));

        if ( size == shock_map.size() - 1 )
        {
            nodes_elm->append_text("\n      ");
        }
        else
        {
            nodes_elm->append_text("\n            ");
        }
        size++;

    }
    
    links_elm->append_text("\n            ");
    size=0;
    it = shock_links.begin();
    for ( unsigned int b=0 ; b < shock_lines.size() ; ++b)
    {
        vcl_pair<unsigned int,unsigned int> lines=
            shock_lines[b];
        bxml_data_sptr slink = new bxml_element("shock_link");
        bxml_element *slink_elm = dbxml_algos::cast_to_element(
            slink,"shock_link");

        links_elm->append_data(slink_elm);
        slink_elm->set_attribute("start",lines.first);
        slink_elm->set_attribute("stop", lines.second);
        slink_elm->append_text("\n                  ");

        if ( (*it).second->shock_compute_type() == dbskfg_utilities::RLLP)
        {
            bxml_data_sptr left  = new bxml_element("left");
            bxml_data_sptr right = new bxml_element("right");  
            bxml_element *left_elm = dbxml_algos::cast_to_element(
                left,"left");
            bxml_element *right_elm = dbxml_algos::cast_to_element(
                right,"right");

            slink_elm->append_data(left_elm);
            left_elm->set_attribute("point",shock_to_contour[
                                        (*it).second->id()].first);
            
            slink_elm->append_text("\n                  ");
            slink_elm->append_data(right_elm);
            right_elm->set_attribute("line",shock_to_contour[
                                         (*it).second->id()].second);
              
      
        }
        else if ( (*it).second->shock_compute_type() == dbskfg_utilities::LLRP)
        {
            bxml_data_sptr left  = new bxml_element("left");
            bxml_data_sptr right = new bxml_element("right");  
            bxml_element *left_elm = dbxml_algos::cast_to_element(
                left,"left");
            bxml_element *right_elm = dbxml_algos::cast_to_element(
                right,"right");

            slink_elm->append_data(left_elm);
            left_elm->set_attribute("line",shock_to_contour[
                                        (*it).second->id()].first);
            
            slink_elm->append_text("\n                  ");
            slink_elm->append_data(right_elm);
            right_elm->set_attribute("point",shock_to_contour[
                                         (*it).second->id()].second);

        }
        else if ( (*it).second->shock_compute_type() == dbskfg_utilities::LL)
        {
            bxml_data_sptr left  = new bxml_element("left");
            bxml_data_sptr right = new bxml_element("right");  
            bxml_element *left_elm = dbxml_algos::cast_to_element(
                left,"left");
            bxml_element *right_elm = dbxml_algos::cast_to_element(
                right,"right");

            slink_elm->append_data(left_elm);
            left_elm->set_attribute("line",shock_to_contour[
                                        (*it).second->id()].first);
            
            slink_elm->append_text("\n                  ");
            slink_elm->append_data(right_elm);
            right_elm->set_attribute("line",shock_to_contour[
                                         (*it).second->id()].second);

        }
        else
        {
            bxml_data_sptr left  = new bxml_element("left");
            bxml_data_sptr right = new bxml_element("right");  
            bxml_element *left_elm = dbxml_algos::cast_to_element(
                left,"left");
            bxml_element *right_elm = dbxml_algos::cast_to_element(
                right,"right");

            slink_elm->append_data(left_elm);
            left_elm->set_attribute("point",shock_to_contour[
                                        (*it).second->id()].first);
            
            slink_elm->append_text("\n                  ");
            slink_elm->append_data(right_elm);
            right_elm->set_attribute("point",shock_to_contour[
                                         (*it).second->id()].second);
            

        }
        slink_elm->append_text("\n            ");

        if ( size == shock_lines.size() - 1 )
        {
            links_elm->append_text("\n      ");
        }
        else
        {
            links_elm->append_text("\n            ");
        }
        size++;
        ++it;

    }

    bxml_write(filename,doc);

}

// ----------------------------------------------------------------------------
//: Write out all rag nodes
void dbskfg_composite_graph_fileio::write_rag_node_one_file(
    const dbskfg_rag_node_sptr& rag_node,
    vcl_map<unsigned int,dbskfg_shock_link*>& shock_links)
{
   
    
    // grab root_element
    bxml_document& doc=dbskfg_transform_manager::Instance().get_xml_file();
    bxml_data_sptr root     = doc.root_element();
    bxml_element*  root_elm = dbxml_algos::cast_to_element(
        root,"medial_fragments");
    root_elm->append_text("\n   ");

    // Create root element
    bxml_data_sptr cgraph     = new bxml_element("composite_graph");
    bxml_element*  cgraph_elm = dbxml_algos::cast_to_element(
        cgraph,"composite_graph");
    root_elm->append_data(cgraph);
    
    // Create contour set
    bxml_data_sptr contour     = new bxml_element("contour");
    bxml_element*  contour_elm = dbxml_algos::cast_to_element(contour,
                                                              "contour");
    cgraph_elm->append_data(contour);
    cgraph_elm->append_text("\n   ");
    contour_elm->append_text("\n      ");
   
    // Create contour points within contour set
    bxml_data_sptr points     = new bxml_element("points");
    bxml_element*  points_elm = dbxml_algos::cast_to_element(points,
                                                             "points");

    // Create line set within countour set
    bxml_data_sptr lines     = new bxml_element("lines");
    bxml_element*  lines_elm = dbxml_algos::cast_to_element(lines,
                                                            "lines");

    // Add points and lines
    contour_elm->append_data(points);  
    contour_elm->append_text("\n      ");
    contour_elm->append_data(lines);
    contour_elm->append_text("\n");
    
    // Create shock set
    bxml_data_sptr shock     = new bxml_element("shock");
    bxml_element*  shock_elm = dbxml_algos::cast_to_element(shock,
                                                            "shock");
  
    cgraph_elm->append_data(shock);
    cgraph_elm->append_text("\n");
    shock_elm->append_text("\n      ");
    
    // Create contour points within contour set
    bxml_data_sptr nodes     = new bxml_element("nodes");
    bxml_element*  nodes_elm = dbxml_algos::cast_to_element(nodes,
                                                            "nodes");

    // Create line set within countour set
    bxml_data_sptr links     = new bxml_element("links");
    bxml_element*  links_elm = dbxml_algos::cast_to_element(links,
                                                            "links");

    // Add points and lines
    shock_elm->append_data(nodes);  
    shock_elm->append_text("\n      ");
    shock_elm->append_data(links);
    shock_elm->append_text("\n");
     
    // Close out brackets
    contour_elm->append_text("   ");
    shock_elm->append_text("   ");

    // Grab all points to put in contour nodes
    vcl_map<vcl_string,vcl_pair<unsigned int,vgl_point_2d<double> > > 
        contour_map;
    vcl_vector<vcl_pair<unsigned int,unsigned int> > contour_lines;
    vcl_map<unsigned int,vcl_pair<unsigned int,dbskfg_composite_node_sptr> >
        shock_map;
    vcl_vector<vcl_pair<unsigned int,unsigned int> > shock_lines;
    
    // Create a mapping of each shock id to its corresponiding line pair
    // pair first left, second right
    vcl_map<unsigned int,vcl_pair<unsigned int,unsigned int> > 
        shock_to_contour;
    
    unsigned int node_id=0;

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it=shock_links.begin() ; it != shock_links.end() ; ++it)
    {
        dbskfg_shock_link *slink = (*it).second;

        // Grab left and right boundary for shock
        dbskfg_shock_link_boundary left_boundary = 
            slink->get_left_boundary();
        dbskfg_shock_link_boundary right_boundary = 
            slink->get_right_boundary();
        if ( slink->source()->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            if ( shock_map.count(slink->source()->id())== 0)
            {
                shock_map[slink->source()->id()]=
                    vcl_make_pair(node_id,
                                  slink->source());
                node_id++;
            }
        }

        if ( slink->target()->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            if ( shock_map.count(slink->target()->id())== 0)
            {
                shock_map[slink->target()->id()]=
                    vcl_make_pair(node_id,
                                  slink->target());
                node_id++;
            }
        }
        
        if ( slink->shock_compute_type() == dbskfg_utilities::RLLP )
        {
            vgl_point_2d<double> r_first_point= right_boundary.contour_.front();
            vgl_point_2d<double> r_last_point = right_boundary.contour_.back();
         
            vcl_stringstream rstream_first,rstream_last,p_stream;
            rstream_first<< r_first_point;
            rstream_last << r_last_point;
            p_stream     << left_boundary.point_;

            // See if we should add right first point
            if ( contour_map.count(rstream_first.str())==0)
            {
                contour_map[rstream_first.str()]=vcl_make_pair(node_id,
                                                               r_first_point);
                node_id++;
            }

            if ( contour_map.count(rstream_last.str())==0)
            {
                contour_map[rstream_last.str()]=vcl_make_pair(node_id,
                                                              r_last_point);
                node_id++;
            }
       
            if ( contour_map.count(p_stream.str())==0)
            {
                contour_map[p_stream.str()]=vcl_make_pair(
                    node_id,left_boundary.point_);
                node_id++;
            }

            contour_lines.push_back(vcl_make_pair
                                    (contour_map[rstream_first.str()].first,
                                     contour_map[rstream_last.str()].first));

            shock_to_contour[slink->id()].first = 
                contour_map[p_stream.str()].first;        
            shock_to_contour[slink->id()].second=
                contour_lines.size()-1;

        }
        else if ( slink->shock_compute_type() == dbskfg_utilities::LLRP )
        {
            vgl_point_2d<double> l_first_point = left_boundary.contour_.front();
            vgl_point_2d<double> l_last_point  = left_boundary.contour_.back();

            vcl_stringstream lstream_first,lstream_last,p_stream;
            lstream_first<< l_first_point;
            lstream_last << l_last_point;
            p_stream     << right_boundary.point_;

            // See if we should add right first point
            if ( contour_map.count(lstream_first.str())==0)
            {
                contour_map[lstream_first.str()]=vcl_make_pair(node_id,
                                                               l_first_point);
                node_id++;
            }
            
            if ( contour_map.count(lstream_last.str())==0)
            {
                contour_map[lstream_last.str()]=vcl_make_pair(node_id,
                                                              l_last_point);
                node_id++;
            }

            if ( contour_map.count(p_stream.str())==0)
            {
                contour_map[p_stream.str()]= vcl_make_pair(
                    node_id,right_boundary.point_);

                node_id++;
            }

            contour_lines.push_back(
                vcl_make_pair(contour_map[lstream_first.str()].first,
                              contour_map[lstream_last.str()].first));

            shock_to_contour[slink->id()].first=
                contour_lines.size()-1;
            shock_to_contour[slink->id()].second = 
                contour_map[p_stream.str()].first;
           
        }
        else if ( slink->shock_compute_type() == dbskfg_utilities::LL )
        {
            vgl_point_2d<double> r_first_point= right_boundary.contour_.front();
            vgl_point_2d<double> r_last_point = right_boundary.contour_.back();
            vgl_point_2d<double> l_first_point= left_boundary.contour_.front();
            vgl_point_2d<double> l_last_point = left_boundary.contour_.back();

            vcl_stringstream lstream_first,lstream_last;
            vcl_stringstream rstream_first,rstream_last;

            lstream_first<< l_first_point;
            lstream_last << l_last_point;
            rstream_first<< r_first_point;
            rstream_last << r_last_point;

            // See if we should add right first point
            if ( contour_map.count(lstream_first.str())==0)
            {
                contour_map[lstream_first.str()]=vcl_make_pair(node_id,
                                                               l_first_point);
                node_id++;
            }
            
            if ( contour_map.count(lstream_last.str())==0)
            {
                contour_map[lstream_last.str()]=vcl_make_pair(node_id,
                                                              l_last_point);
                node_id++;
            }

            contour_lines.push_back(vcl_make_pair
                                    (contour_map[lstream_first.str()].first,
                                     contour_map[lstream_last.str()].first));

            shock_to_contour[slink->id()].first = contour_lines.size()-1;


            // See if we should add right first point
            if ( contour_map.count(rstream_first.str())==0)
            {
                contour_map[rstream_first.str()]=vcl_make_pair(node_id,
                                                               r_first_point);
                node_id++;
            }

            if ( contour_map.count(rstream_last.str())==0)
            {
                contour_map[rstream_last.str()]=vcl_make_pair(node_id,
                                                              r_last_point);
                node_id++;
            }

            contour_lines.push_back(vcl_make_pair
                                    (contour_map[rstream_first.str()].first,
                                     contour_map[rstream_last.str()].first));

            shock_to_contour[slink->id()].second = contour_lines.size()-1;


        }
        else if ( slink->shock_compute_type() == dbskfg_utilities::PP )
        {
            vcl_stringstream r_pstream,l_pstream;
            l_pstream     << left_boundary.point_;
            r_pstream     << right_boundary.point_;

            if ( contour_map.count(l_pstream.str())==0)
            {
                contour_map[l_pstream.str()]= vcl_make_pair(
                    node_id,left_boundary.point_);

                node_id++;
            }

            if ( contour_map.count(r_pstream.str())==0)
            {
                contour_map[r_pstream.str()]= vcl_make_pair(
                    node_id,right_boundary.point_);

                node_id++;

            }

            shock_to_contour[slink->id()].first = 
                contour_map[l_pstream.str()].first;
            shock_to_contour[slink->id()].second = 
                contour_map[r_pstream.str()].first;
          
        }

        // Figure out shock links
        unsigned int source_shock_id,target_shock_id;

        if ( slink->source()->node_type() == dbskfg_composite_node::
             CONTOUR_NODE )
        {
            
            vcl_stringstream stream;
            stream<<slink->source()->pt();
           
            source_shock_id = contour_map[stream.str()].first;

        } 
        else
        {
            source_shock_id = shock_map[slink->source()->id()].first;
        }

        if ( slink->target()->node_type() == dbskfg_composite_node::
             CONTOUR_NODE )
        {   
            vcl_stringstream stream;
            stream<<slink->target()->pt();
            
            target_shock_id = contour_map[stream.str()].first;

        } 
        else
        {
            target_shock_id = shock_map[slink->target()->id()].first;
        }
        
        shock_lines.push_back(vcl_make_pair(source_shock_id,target_shock_id));


    }

    // Append into xml file
    points_elm->append_text("\n            ");

    // Write out all points
    vcl_map<vcl_string,vcl_pair<unsigned int,vgl_point_2d<double> > >::iterator 
        mit;

    unsigned int size=0;
    for ( mit = contour_map.begin() ; mit != contour_map.end() ; ++mit)
    {
        unsigned int id = (*mit).second.first;
        vgl_point_2d<double> point = (*mit).second.second;

        bxml_data_sptr cnode = new bxml_element("contour_node");
        bxml_element *cnode_elm = dbxml_algos::cast_to_element(
            cnode,"contour_node");

        points_elm->append_data(cnode_elm);
        cnode_elm->set_attribute("id",id);

        vcl_stringstream xstream,ystream;
        xstream<<vcl_fixed; ystream<<vcl_fixed;
        xstream.precision(10); ystream.precision(10);
        xstream<<point.x();  ystream<<point.y();
        
        cnode_elm->set_attribute("x",xstream.str());
        cnode_elm->set_attribute("y",ystream.str());
        
        if ( size == contour_map.size() - 1 )
        {
            points_elm->append_text("\n      ");
        }
        else
        {
            points_elm->append_text("\n            ");
            
        }
        size++;
        
    }

    // Append into xml file
    lines_elm->append_text("\n            ");
    size=0;
    // Write out contour lines
    for ( unsigned int k=0; k < contour_lines.size() ; ++k)
    {
        vcl_pair<unsigned int,unsigned int> line=
            contour_lines[k];
        bxml_data_sptr lnode = new bxml_element("contour_link");
        bxml_element *lnode_elm = dbxml_algos::cast_to_element(
            lnode,"contour_link");

        lines_elm->append_data(lnode_elm);
        lnode_elm->set_attribute("id",k);
        lnode_elm->set_attribute("start",line.first);
        lnode_elm->set_attribute("stop", line.second);    

        if ( size == contour_lines.size() - 1 )
        {
            lines_elm->append_text("\n      ");
        }
        else
        {
            lines_elm->append_text("\n            ");
        }
        size++;
        
    }

    nodes_elm->append_text("\n            ");
    size=0;
    vcl_map<unsigned int,vcl_pair<unsigned int,dbskfg_composite_node_sptr> >
        ::iterator cit;
    for ( cit = shock_map.begin() ; cit != shock_map.end() ; ++cit)
    {
        dbskfg_shock_node* shock_node =
            dynamic_cast<dbskfg_shock_node*>(&(*(*cit).second.second));
        unsigned int id = (*cit).second.first;
        bxml_data_sptr snode = new bxml_element("shock_node");
        bxml_element *snode_elm = dbxml_algos::cast_to_element(
            snode,"shock_node");

        nodes_elm->append_data(snode_elm);
        snode_elm->set_attribute("id",id);

        // Grad x,y points
        vcl_stringstream xstream,ystream;
        xstream<<vcl_fixed; ystream<<vcl_fixed;
        xstream.precision(10); ystream.precision(10);
        xstream<<shock_node->pt().x();  ystream<<shock_node->pt().y();

        snode_elm->set_attribute("x",xstream.str());
        snode_elm->set_attribute("y",ystream.str());

        // Grab radius
        vcl_stringstream rad_stream;
        rad_stream<<vcl_fixed; 
        rad_stream.precision(10);
        rad_stream<<shock_node->get_radius();

        snode_elm->set_attribute("radius",rad_stream.str());
        snode_elm->set_attribute("theta",0);
        snode_elm->set_attribute("phi",0);
        snode_elm->set_attribute("virtual",rag_node->part_of_wavefront
                                 (shock_node->id()));

        if ( size == shock_map.size() - 1 )
        {
            nodes_elm->append_text("\n      ");
        }
        else
        {
            nodes_elm->append_text("\n            ");
        }
        size++;

    }
    
    links_elm->append_text("\n            ");
    size=0;
    it = shock_links.begin();
    for ( unsigned int b=0 ; b < shock_lines.size() ; ++b)
    {
        vcl_pair<unsigned int,unsigned int> lines=
            shock_lines[b];
        bxml_data_sptr slink = new bxml_element("shock_link");
        bxml_element *slink_elm = dbxml_algos::cast_to_element(
            slink,"shock_link");

        links_elm->append_data(slink_elm);
        slink_elm->set_attribute("start",lines.first);
        slink_elm->set_attribute("stop", lines.second);
        slink_elm->append_text("\n                  ");

        if ( (*it).second->shock_compute_type() == dbskfg_utilities::RLLP)
        {
            bxml_data_sptr left  = new bxml_element("left");
            bxml_data_sptr right = new bxml_element("right");  
            bxml_element *left_elm = dbxml_algos::cast_to_element(
                left,"left");
            bxml_element *right_elm = dbxml_algos::cast_to_element(
                right,"right");

            slink_elm->append_data(left_elm);
            left_elm->set_attribute("point",shock_to_contour[
                                        (*it).second->id()].first);
            
            slink_elm->append_text("\n                  ");
            slink_elm->append_data(right_elm);
            right_elm->set_attribute("line",shock_to_contour[
                                         (*it).second->id()].second);
              
      
        }
        else if ( (*it).second->shock_compute_type() == dbskfg_utilities::LLRP)
        {
            bxml_data_sptr left  = new bxml_element("left");
            bxml_data_sptr right = new bxml_element("right");  
            bxml_element *left_elm = dbxml_algos::cast_to_element(
                left,"left");
            bxml_element *right_elm = dbxml_algos::cast_to_element(
                right,"right");

            slink_elm->append_data(left_elm);
            left_elm->set_attribute("line",shock_to_contour[
                                        (*it).second->id()].first);
            
            slink_elm->append_text("\n                  ");
            slink_elm->append_data(right_elm);
            right_elm->set_attribute("point",shock_to_contour[
                                         (*it).second->id()].second);

        }
        else if ( (*it).second->shock_compute_type() == dbskfg_utilities::LL)
        {
            bxml_data_sptr left  = new bxml_element("left");
            bxml_data_sptr right = new bxml_element("right");  
            bxml_element *left_elm = dbxml_algos::cast_to_element(
                left,"left");
            bxml_element *right_elm = dbxml_algos::cast_to_element(
                right,"right");

            slink_elm->append_data(left_elm);
            left_elm->set_attribute("line",shock_to_contour[
                                        (*it).second->id()].first);
            
            slink_elm->append_text("\n                  ");
            slink_elm->append_data(right_elm);
            right_elm->set_attribute("line",shock_to_contour[
                                         (*it).second->id()].second);

        }
        else
        {
            bxml_data_sptr left  = new bxml_element("left");
            bxml_data_sptr right = new bxml_element("right");  
            bxml_element *left_elm = dbxml_algos::cast_to_element(
                left,"left");
            bxml_element *right_elm = dbxml_algos::cast_to_element(
                right,"right");

            slink_elm->append_data(left_elm);
            left_elm->set_attribute("point",shock_to_contour[
                                        (*it).second->id()].first);
            
            slink_elm->append_text("\n                  ");
            slink_elm->append_data(right_elm);
            right_elm->set_attribute("point",shock_to_contour[
                                         (*it).second->id()].second);
            

        }
        slink_elm->append_text("\n            ");

        if ( size == shock_lines.size() - 1 )
        {
            links_elm->append_text("\n      ");
        }
        else
        {
            links_elm->append_text("\n            ");
        }
        size++;
        ++it;

    }

    dbskfg_transform_manager::Instance().write_xml_file();

}

// ----------------------------------------------------------------------------
//: classify all nodes
void dbskfg_composite_graph_fileio::classify_nodes()
{
    for (dbskfg_composite_graph::vertex_iterator vit = 
             composite_graph_->vertices_begin(); 
         vit != composite_graph_->vertices_end(); ++vit)
    {
        dbskfg_composite_node_sptr node = *vit;
        if ( node->node_type() == dbskfg_composite_node::CONTOUR_NODE )
        {
            unsigned int contour_degree(0);
            dbskfg_composite_node::edge_iterator eit;

            vcl_vector<dbskfg_composite_link_sptr> edges;

            for (eit = (*vit)->out_edges_begin(); 
                 eit != (*vit)->out_edges_end() ; ++eit)
            {

                if ( (*eit)->link_type() 
                     == dbskfg_composite_link::CONTOUR_LINK)
                {
                    contour_degree++;
                }
            }

            for (eit = (*vit)->in_edges_begin(); 
                 eit != (*vit)->in_edges_end() ; ++eit)
            {

                if ( (*eit)->link_type() 
                     == dbskfg_composite_link::CONTOUR_LINK)
                {
                    contour_degree++;
                }
            }

            (*vit)->set_composite_degree(contour_degree);
        }
        else
        {
            unsigned int shock_degree(0);
            dbskfg_composite_node::edge_iterator eit;
            
            for (eit = (*vit)->out_edges_begin(); 
                 eit != (*vit)->out_edges_end() ; ++eit)
            {

                if ( (*eit)->link_type() 
                     == dbskfg_composite_link::SHOCK_LINK)
                {
                    dbskfg_composite_link_sptr link=*eit;
                    dbskfg_shock_link* slink=
                        dynamic_cast<dbskfg_shock_link*>(&(*link));
                    if ( slink->shock_link_type() 
                         == dbskfg_shock_link::SHOCK_EDGE)
                    {
                        shock_degree++;
                    }
                }
            }

            for (eit = (*vit)->in_edges_begin(); 
                 eit != (*vit)->in_edges_end() ; ++eit)
            {

                if ( (*eit)->link_type() 
                     == dbskfg_composite_link::SHOCK_LINK)
                {
                    dbskfg_composite_link_sptr link=*eit;
                    dbskfg_shock_link* slink=
                        dynamic_cast<dbskfg_shock_link*>(&(*link));
                    if ( slink->shock_link_type() 
                         == dbskfg_shock_link::SHOCK_EDGE)
                    {
                        shock_degree++;
                    }
                }
            }
            
            (*vit)->set_composite_degree(shock_degree);

        }
    }

}

// ----------------------------------------------------------------------------
//: Write out specific rag node
void dbskfg_composite_graph_fileio::write_contour_composite_graph(
    dbskfg_composite_graph_sptr composite_graph,
    vcl_string filename) 
{

    vcl_vector<vsol_spatial_object_2d_sptr> contour_objects;
    vcl_vector<vsol_spatial_object_2d_sptr> shock_objects;
    for (dbskfg_composite_graph::edge_iterator eit =
             composite_graph->edges_begin();
         eit != composite_graph->edges_end(); ++eit)
    {

        vgl_point_2d<double> start = (*eit)->source()->pt();
        vgl_point_2d<double> stop =  (*eit)->target()->pt();


        // Add in contours for front 
        vsol_spatial_object_2d_sptr obj=
                new vsol_line_2d(start,
                                 stop);
        if ( (*eit)->link_type() == dbskfg_composite_link::SHOCK_LINK)
        {
            shock_objects.push_back(obj);
        }
        else
        {
            contour_objects.push_back(obj);
        }
        


    }

    vcl_string shock_filename=filename + "_shock.bnd";
    vcl_string contour_filename=filename + "_contour.bnd";
    dbsk2d_file_io::save_bnd_v3_0(shock_filename,contour_objects);
    dbsk2d_file_io::save_bnd_v3_0(contour_filename,shock_objects);


}
