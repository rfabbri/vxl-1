// This is brcv/shp/dbskfg/dbskfg_compute_composite_graph.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/algo/dbskfg_compute_composite_graph.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <dbskfg/algo/dbskfg_region_growing_transforms.h>
// dbsk2d headers
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_ishock_graph.h>
#include <dbsk2d/dbsk2d_ishock_belm.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
// vcl headers
#include <vcl_sstream.h>
#include <vcl_limits.h>
// vsol headers
#include <vsol/vsol_point_2d_sptr.h>
// vgl headers
#include <vgl/vgl_lineseg_test.h>
#include <vgl/vgl_distance.h>



dbskfg_compute_composite_graph::dbskfg_compute_composite_graph
( 
    dbsk2d_ishock_graph_sptr ishock_graph,
    dbsk2d_shock_graph_sptr coarse_shock_graph,
    dbskfg_composite_graph_sptr composite_graph,
    dbskfg_rag_graph_sptr rag_graph
)
 :composite_graph_(composite_graph),
  shock_graph_(coarse_shock_graph),
  ishock_graph_(ishock_graph),
  rag_graph_(rag_graph)
{
//    double*  ptr= const_cast<double *>(&vgl_tolerance<double>::position);
//    *ptr=1e-8;
  
}

dbskfg_compute_composite_graph::~dbskfg_compute_composite_graph()
{


}

bool dbskfg_compute_composite_graph::compile_composite_graph()
{

    // Keep track of status
    bool status_flag = true;

    // clear all maps before we start
    isnodes_to_composite_snodes_.clear();
    contour_node_map_.clear();

    // 1. Lets get all nodes shock/contour and shock rays
    compile_nodes_and_shock_rays(status_flag);

    if ( status_flag == false)
    {
        return status_flag;
    }

    vcl_cout<<"Finished compiling nodes and shock rays"<<vcl_endl;

    // 2. Lets compile all contour links
    compile_contour_and_shock_links(status_flag);

    vcl_cout<<"Finished compiling contours and shock links"<<vcl_endl;

    //3: Lets classify nodes
    classify_nodes();

    vcl_cout<<"Finished classifying nodes"<<vcl_endl;

    //4. Lets compile all shock fragments
    compile_shock_fragments();
    
    dbskfg_region_growing_transforms region_growing(rag_graph_);
    region_growing.grow_regions();

    vcl_cout<<"Printing out vertices: "<<
        composite_graph_->number_of_vertices()<<vcl_endl;

    vcl_cout<<"Printing out edges: "<<composite_graph_->number_of_edges()
            <<vcl_endl;

    return status_flag;
}

void dbskfg_compute_composite_graph::compile_nodes_and_shock_rays(bool& 
                                                                  status_flag)
{

    // Always look at raw graph
    dbsk2d_ishock_graph::vertex_iterator vit;

    vcl_string lstring,rstring;

    // Keep a local thing to see if shock nodes repeat
    for ( vit = ishock_graph_->all_nodes().begin(); 
          vit != ishock_graph_->all_nodes().end();
          ++vit)
    {
        dbsk2d_ishock_node* node = *vit;

        // Node has either a parent or child
        if(node->degree())
        {
            
            compile_nodes_and_shock_rays_helper(node,status_flag);
            if ( status_flag == false)
            {
                break;
            }
                                      

        }
        
    }

}

                
void dbskfg_compute_composite_graph::compile_nodes_and_shock_rays_helper
(
    dbsk2d_ishock_node* node,
    bool& status_flag
)
{

    // In case of dual node need to keep shock string
    vgl_point_2d<double> shock_origin = node->origin();
    vcl_stringstream shock_streamer;
    shock_streamer<<shock_origin;
    vcl_string shock_string=shock_streamer.str();

    // Create the node we may or may not add to the graph
    // TODO: Add shock intrinsinc parameters
    dbskfg_composite_node_sptr shock_node=
        new dbskfg_shock_node(
            composite_graph_->next_available_id(),
            node->origin(),
            node->startTime());
    composite_graph_->add_vertex(shock_node);

    // Keep a temp map
    vcl_map<vcl_string,vcl_pair<vgl_point_2d<double>,int> > contact_point_map;

    // Grab list of parent shocks
    ishock_edge_list& list=node->pShocks();

    // Go through list of ishock edges
    for ( ishock_edge_list_iter it = list.begin(); it != list.end(); ++it)
    {
        dbsk2d_ishock_edge* edge=*it;

        double tau=edge->eTau();

        vgl_point_2d<double> leftstpt    = edge->getLFootPt(tau);
        vgl_point_2d<double> rightstpt   = edge->getRFootPt(tau);

        vcl_stringstream lstreamer,rstreamer;
        lstreamer<<leftstpt;
        rstreamer<<rightstpt; 
                
        vcl_string temp_lstring=lstreamer.str();
        vcl_string temp_rstring=rstreamer.str();

        vcl_pair<int,int> contour_ids = dbskfg_utilities::
            get_contour_id_from_shock_edge(edge);

        vcl_pair<vgl_point_2d<double>,int> left_pair_combo =
            vcl_make_pair(leftstpt,contour_ids.first);
        vcl_pair<vgl_point_2d<double>,int> right_pair_combo =
            vcl_make_pair(rightstpt,contour_ids.second);  

        contact_point_map[temp_lstring]=left_pair_combo;
        contact_point_map[temp_rstring]=right_pair_combo;

    }

    dbsk2d_ishock_edge* cshock(0);

    // Now look through child shocks
    if ( node->cShock() )
    {
        cshock = node->cShock();
        
        double tau=cshock->sTau();

        vgl_point_2d<double> leftstpt    = cshock->getLFootPt(tau);
        vgl_point_2d<double> rightstpt   = cshock->getRFootPt(tau);

        vcl_stringstream lstreamer,rstreamer;
        lstreamer<<leftstpt;
        rstreamer<<rightstpt; 
                
        vcl_string temp_lstring=lstreamer.str();
        vcl_string temp_rstring=rstreamer.str();

        vcl_pair<int,int> contour_ids = dbskfg_utilities::
            get_contour_id_from_shock_edge(cshock);

        vcl_pair<vgl_point_2d<double>,int> left_pair_combo =
            vcl_make_pair(leftstpt,contour_ids.first);
        vcl_pair<vgl_point_2d<double>,int> right_pair_combo =
            vcl_make_pair(rightstpt,contour_ids.second);  

        contact_point_map[temp_lstring]=left_pair_combo;
        contact_point_map[temp_rstring]=right_pair_combo;
        
    }

    if ( node->cShock2() )
    {

        cshock = node->cShock2();
        
        double tau=cshock->sTau();

        vgl_point_2d<double> leftstpt    = cshock->getLFootPt(tau);
        vgl_point_2d<double> rightstpt   = cshock->getRFootPt(tau);

        vcl_stringstream lstreamer,rstreamer;
        lstreamer<<leftstpt;
        rstreamer<<rightstpt; 

        vcl_string temp_lstring=lstreamer.str();
        vcl_string temp_rstring=rstreamer.str();
      
        vcl_pair<int,int> contour_ids = dbskfg_utilities::
            get_contour_id_from_shock_edge(cshock);

        vcl_pair<vgl_point_2d<double>,int> left_pair_combo =
            vcl_make_pair(leftstpt,contour_ids.first);
        vcl_pair<vgl_point_2d<double>,int> right_pair_combo =
            vcl_make_pair(rightstpt,contour_ids.second);  

        contact_point_map[temp_lstring]=left_pair_combo;
        contact_point_map[temp_rstring]=right_pair_combo;
      

    } 

   
    // Keep track of keys that we missed
    vcl_map<vcl_string,vcl_string> keys_missed;

    if ( contact_point_map.size() != node->degree(true) )
    {
   
        // We need to create a temp map before we delete everything
        vcl_map<vcl_string,vcl_pair<vgl_point_2d<double>,int> > temp_map;
        
        // Keep another tempory map
        vcl_map<vcl_string,vcl_string> string_temp_map;

        //Loop over map
        vcl_map<vcl_string,vcl_pair<vgl_point_2d<double>,int > >::iterator mit;
        for ( mit = contact_point_map.begin(); mit != contact_point_map.end() ; 
              ++mit)
        {

            vcl_stringstream fixed_streamer,norm_streamer;
            fixed_streamer<<vcl_fixed;
            fixed_streamer.precision(1);
            
            fixed_streamer<<"<vgl_point_2d "
                          << (*mit).second.first.x() 
                          << ','
                          << (*mit).second.first.y() 
                          << "> ";

            norm_streamer << (*mit).second.first;

            if ( string_temp_map.count(fixed_streamer.str())==0)
            {
                string_temp_map[fixed_streamer.str()]=norm_streamer.str();
                temp_map[norm_streamer.str()]=(*mit).second;
            }
            else
            {
                keys_missed[string_temp_map[fixed_streamer.str()]]=
                    norm_streamer.str();

                if ( contour_node_map_.
                     count(keys_missed[string_temp_map[fixed_streamer.str()]]))
                {
                    // The mapped key exists already
                    contour_node_map_[string_temp_map[fixed_streamer.str()]]=
                        contour_node_map_[keys_missed
                                          [string_temp_map
                                           [fixed_streamer.str()]]];
                    
                }
            }
        }

        // Clear out contact_point map
        contact_point_map.clear();

        // Reset map
        contact_point_map = temp_map;

    }

    if ( contact_point_map.size() != node->degree(true) )
    {
        status_flag=false;
        return;
    }


    //Loop over map
    vcl_map<vcl_string,vcl_pair<vgl_point_2d<double>,int > >::iterator mit;
    for ( mit = contact_point_map.begin(); mit != contact_point_map.end() ; 
          ++mit)
    {
        vcl_string key=(*mit).first;

        // Insert contour node
        if ( (contour_node_map_.count(key) == 0) )
        {
           
            vcl_pair< vgl_point_2d<double>,int> contour_pair = (*mit).second;
            dbskfg_composite_node_sptr cnode = 
                new dbskfg_contour_node(
                    composite_graph_->next_available_id(),
                    contour_pair.first,
                    contour_pair.second);
            contour_node_map_[key]=dynamic_cast<dbskfg_contour_node*>
                (&(*cnode));
            nodes_contour_[contour_pair.second].push_back(
                dynamic_cast<dbskfg_contour_node*>(&(*cnode)));
            composite_graph_->add_vertex(cnode);

        }

        //Lets delete the shock node
        if ( key == shock_string )
        {

            isnodes_to_composite_snodes_[node->id()]=contour_node_map_[key];
            composite_graph_->remove_vertex(shock_node);
    
        }

        // Add in shock ray
        if ( shock_string != key )
        {
            // Add the shock link into the graph
            dbskfg_composite_link_sptr shock_ray =
                new dbskfg_shock_link(
                    shock_node,
                    contour_node_map_[key],
                    composite_graph_->next_available_id(),
                    dbskfg_shock_link::SHOCK_RAY);

            // Add in edges
            composite_graph_->
                add_edge(shock_ray);
        
            // Add in links
            shock_node->add_outgoing_edge(shock_ray);
            contour_node_map_[key]->add_incoming_edge(shock_ray);

            isnodes_to_composite_snodes_[node->id()]=
                reinterpret_cast<dbskfg_composite_node*>(&(*shock_node));
   

        }
  
    }

    // Take care of keys missed
    vcl_map<vcl_string,vcl_string>::iterator sit;
    for ( sit = keys_missed.begin() ; sit != keys_missed.end() ; ++sit)
    {
        if ( contour_node_map_.count((*sit).second)== 0 )
        {
            contour_node_map_[(*sit).second]=contour_node_map_[(*sit).first];
        }
    }

}


void dbskfg_compute_composite_graph::compile_contour_and_shock_links
(bool& status_flag)
{
    
    // Keep a map to make sure edges we have seen we dont see again
    vcl_map<vcl_pair<unsigned int, unsigned int>,
        dbskfg_composite_link_sptr> edge_map;

    // Iterate thru all edges
    dbsk2d_ishock_graph::edge_iterator eit;
    for ( eit = ishock_graph_->all_edges().begin(); 
          eit != ishock_graph_->all_edges().end();
          ++eit)
    {
        dbsk2d_ishock_edge* edge = *eit;
    
        if ( edge->cSNode() != 0  && edge->pSNode() != 0 )
        {
  
            dbskfg_composite_node_sptr source_node =
                isnodes_to_composite_snodes_[edge->pSNode()->id()];
            dbskfg_composite_node_sptr target_node =
                isnodes_to_composite_snodes_[edge->cSNode()->id()];

            dbskfg_utilities::Fragment_Type frag_type;

            frag_type = 
                ( source_node->node_type() == 
                  dbskfg_composite_node::CONTOUR_NODE ||
                 target_node->node_type() == 
                  dbskfg_composite_node::CONTOUR_NODE )
                ? dbskfg_utilities::DEGENERATE : dbskfg_utilities::REGULAR ;

            // Add the shock link into the graph
            dbskfg_composite_link_sptr shock_edge =
                new dbskfg_shock_link(
                    source_node,
                    target_node,
                    composite_graph_->next_available_id(),
                    dbskfg_shock_link::SHOCK_EDGE,
                    frag_type);

            dbskfg_shock_link* shock_link = 
                dynamic_cast<dbskfg_shock_link*>(&(*shock_edge));
     
            bool flag = composite_graph_->
                add_edge(shock_edge);
     
            source_node->add_outgoing_edge(shock_edge);
            target_node->add_incoming_edge(shock_edge);

            compile_contour_and_shock_links_helper(
                 edge,
                 shock_link,
                 edge_map,
                 dbskfg_utilities::LEFT,
                 status_flag);

            compile_contour_and_shock_links_helper(
                edge,
                shock_link,
                edge_map,
                dbskfg_utilities::RIGHT,
                status_flag);

        }
        if ( status_flag == false)
        {
            break;
        }
        

    }


}

void dbskfg_compute_composite_graph::compile_contour_and_shock_links_helper(
    dbsk2d_ishock_edge* edge,
    dbskfg_shock_link* shock_link,
    vcl_map< vcl_pair<unsigned int,unsigned int>, dbskfg_composite_link_sptr>& 
    edge_map,
    dbskfg_utilities::Orientation type,
    bool& status_flag)
{

    vgl_point_2d<double> startpt,endpt;

    if ( type == dbskfg_utilities::LEFT )
    {
        startpt    = edge->getLFootPt(edge->sTau());
        endpt      = edge->getLFootPt(edge->eTau());
    }
    else
    {
        startpt   = edge->getRFootPt(edge->sTau());
        endpt     = edge->getRFootPt(edge->eTau());
    }       
    vcl_stringstream start_streamer;
    vcl_stringstream end_streamer;  
  
    start_streamer << startpt;
    end_streamer   << endpt;
  
    vcl_string start_string   = start_streamer.str();
    vcl_string end_string     = end_streamer.str();


    if ( type == dbskfg_utilities::LEFT )
    {
        dbsk2d_ishock_belm* elm = edge->lBElement();
        if ( elm->is_a_line() )
        {
            dbsk2d_ishock_bline* line=dynamic_cast<dbsk2d_ishock_bline*>
                (elm);
            if ( start_string == end_string )
            {
                status_flag=false;
            }
        }
        
    }
    else
    {
        dbsk2d_ishock_belm* elm = edge->rBElement();
        if ( elm->is_a_line() )
        {
            dbsk2d_ishock_bline* line=dynamic_cast<dbsk2d_ishock_bline*>
                (elm);
            if ( start_string == end_string )
            {
                status_flag=false;
            }
        }
    }
     
    unsigned int start_id = contour_node_map_[start_string]->id();
    unsigned int end_id   = contour_node_map_[end_string]->id();
 
    vcl_pair<unsigned int,unsigned int> contour_pair_f=
        vcl_make_pair(start_id,end_id);
    vcl_pair<unsigned int,unsigned int> contour_pair_r=
        vcl_make_pair(contour_pair_f.second,contour_pair_f.first);

    vcl_pair<int,int> contour_ids = dbskfg_utilities::
        get_contour_id_from_shock_edge(edge);
    int orig_con_id(0);
    if ( type == dbskfg_utilities::LEFT )
    {
        orig_con_id = contour_ids.first;
    }
    else
    {
        orig_con_id = contour_ids.second;
    }       

    // If not equal indicates line or arc
    if ( start_string != end_string )
    {
       
        // Create line
        vgl_line_segment_2d<double> line(startpt,endpt);
           
        // Determine points between these
        vcl_vector<dbskfg_contour_node*> points_on_line=
            find_points_on_line(line,
                                contour_node_map_[start_string]
                                ->contour_id(),
                                contour_node_map_[end_string]
                                ->contour_id());

        if ( points_on_line.size() == 0 )
        {
      
            if ( (edge_map.count(contour_pair_f) == 0) && 
                 (edge_map.count(contour_pair_r) == 0) )
            {
                // Create first link
                dbskfg_composite_link_sptr contour_link(0);
                contour_link =
                    new dbskfg_contour_link(
                        contour_node_map_[start_string],
                        contour_node_map_[end_string],
                        composite_graph_->next_available_id());
                composite_graph_->add_edge(contour_link);
                
                contour_node_map_[start_string]->
                    add_outgoing_edge(contour_link);
                contour_node_map_[end_string]->
                    add_incoming_edge(contour_link);
                
                edge_map[contour_pair_f]=contour_link;
                edge_map[contour_pair_r]=contour_link;

                if ( type == dbskfg_utilities::LEFT )
                {
                    shock_link->add_to_left_contour
                        (edge_map[contour_pair_f]);
                }
                else
                {
                    shock_link->add_to_right_contour
                        (edge_map[contour_pair_f]);
                }

            }
            else
            {
                // We have seen this before do not need to add a link
                // but add d a pointer
                if ( type == dbskfg_utilities::LEFT )
                {
                    shock_link->add_to_left_contour
                        (edge_map[contour_pair_f]);
                }
                else
                {
                    shock_link->add_to_right_contour
                        (edge_map[contour_pair_f]);
                }
                    

            }

            dbskfg_contour_link* clink = 
                dynamic_cast<dbskfg_contour_link*>
                (&(*edge_map[contour_pair_f]));
            clink->shock_links_affected(shock_link);
            clink->set_original_contour_id(orig_con_id);
    
        }
        else
        {
                 
            // create target node
            dbskfg_composite_node_sptr target_node(
                points_on_line.front());
            dbskfg_composite_link_sptr contour_link(0);

            vcl_pair<unsigned int,unsigned int> front_pair_f=
                vcl_make_pair(start_id,target_node->id());
            vcl_pair<unsigned int,unsigned int> front_pair_r=
                vcl_make_pair(front_pair_f.second,front_pair_f.first);
      
            if ( (edge_map.count(front_pair_f) == 0) && 
                 (edge_map.count(front_pair_r) == 0) )
            {
                        
                // Create first link                        
                contour_link =
                    new dbskfg_contour_link(
                        contour_node_map_[start_string],
                        target_node,
                        composite_graph_->next_available_id());
                composite_graph_->add_edge(contour_link);
                        
                contour_node_map_[start_string]->
                    add_outgoing_edge(contour_link);
                target_node->
                    add_incoming_edge(contour_link);
             
                edge_map[front_pair_f]=contour_link;
                edge_map[front_pair_r]=contour_link;

                if ( type == dbskfg_utilities::LEFT )
                {
                    shock_link->add_to_left_contour
                        (edge_map[front_pair_f]);
                }
                else
                {
                    shock_link->add_to_right_contour
                        (edge_map[front_pair_f]);
                }
         
                    
            }
            else
            {

                if ( type == dbskfg_utilities::LEFT )
                {
                    shock_link->add_to_left_contour
                        (edge_map[front_pair_f]);
                }
                else
                {
                    shock_link->add_to_right_contour
                        (edge_map[front_pair_f]);
                }


            }

            dbskfg_contour_link* clink = 
                dynamic_cast<dbskfg_contour_link*>
                (&(*edge_map[front_pair_f]));
            clink->shock_links_affected(shock_link);
            clink->set_original_contour_id(orig_con_id);

            vcl_pair<unsigned int, unsigned int> middle_pair_f;
            vcl_pair<unsigned int, unsigned int> middle_pair_r;

            // Loop over vector and create links
            for ( unsigned int i=0; i < 
                      (points_on_line.size()-1); ++i)
            {

                unsigned int index=i;

                dbskfg_composite_node_sptr 
                    start_node(points_on_line[index]);
                index=index+1;
                dbskfg_composite_node_sptr 
                    stop_node(points_on_line[index]);

                middle_pair_f.first=start_node->id();
                middle_pair_f.second=stop_node->id();

                middle_pair_r.first  = middle_pair_f.second;
                middle_pair_r.second = middle_pair_f.first;

                if ( (edge_map.count(middle_pair_f) == 0) &&
                     (edge_map.count(middle_pair_r) == 0) )
                {
                    contour_link =
                        new dbskfg_contour_link(
                            start_node,
                            stop_node,
                            composite_graph_->next_available_id());
                    composite_graph_->add_edge(contour_link);

                    start_node->
                        add_outgoing_edge(contour_link);
                    stop_node->
                        add_incoming_edge(contour_link);
            
                    edge_map[middle_pair_f]=contour_link;
                    edge_map[middle_pair_r]=contour_link;

                    if ( type == dbskfg_utilities::LEFT )
                    {
                        shock_link->add_to_left_contour
                            (edge_map[middle_pair_f]);
                    }
                    else
                    {
                        shock_link->add_to_right_contour
                            (edge_map[middle_pair_f]);
                    }                    

                }
                else
                {
                    if ( type == dbskfg_utilities::LEFT )
                    {
                        shock_link->add_to_left_contour
                            (edge_map[middle_pair_f]);
                    }
                    else
                    {
                        shock_link->add_to_right_contour
                            (edge_map[middle_pair_f]);
                    }                    

                }

                clink = 
                    dynamic_cast<dbskfg_contour_link*>
                    (&(*edge_map[middle_pair_f]));
                clink->shock_links_affected(shock_link);
                clink->set_original_contour_id(orig_con_id);
            }

            // Add last link
            dbskfg_composite_node_sptr 
                last_node(points_on_line.back());

            vcl_pair<unsigned int,unsigned int> back_pair_f=
                vcl_make_pair(last_node->id(),end_id);
            vcl_pair<unsigned int,unsigned int> back_pair_r=
                vcl_make_pair(back_pair_f.second,back_pair_f.first);
                        
            if ( (edge_map.count(back_pair_f) == 0) &&
                 (edge_map.count(back_pair_r) == 0) )
            {
                    
                contour_link =
                    new dbskfg_contour_link(
                        last_node,
                        contour_node_map_[end_string],
                        composite_graph_->next_available_id());
                composite_graph_->add_edge(contour_link);

                last_node->
                    add_outgoing_edge(contour_link);
                contour_node_map_[end_string]->
                    add_incoming_edge(contour_link);
            
                edge_map[back_pair_f]=contour_link;
                edge_map[back_pair_r]=contour_link;

                if ( type == dbskfg_utilities::LEFT )
                {
                    shock_link->add_to_left_contour
                        (edge_map[back_pair_f]);
                }
                else
                {
                    shock_link->add_to_right_contour
                        (edge_map[back_pair_f]);
                }                    

                    
            }
            else
            {
                
                if ( type == dbskfg_utilities::LEFT )
                {
                    shock_link->add_to_left_contour
                        (edge_map[back_pair_f]);
                }
                else
                {
                    shock_link->add_to_right_contour
                        (edge_map[back_pair_f]);
                }                    

            }

            clink = 
                dynamic_cast<dbskfg_contour_link*>
                (&(*edge_map[back_pair_f]));
            clink->shock_links_affected(shock_link);
            clink->set_original_contour_id(orig_con_id);
        }
                
    }
    else
    {
        if ( type == dbskfg_utilities::LEFT )
        {
            shock_link->add_left_point(contour_node_map_[start_string]);
        }
        else
        {
            shock_link->add_right_point(contour_node_map_[start_string]);
        }                    

        dbskfg_contour_node* cnode = 
            dynamic_cast<dbskfg_contour_node*>
            (&(*contour_node_map_[start_string]));
        cnode->shock_links_affected(shock_link);
       
    }



}

void dbskfg_compute_composite_graph::compile_shock_fragments()
{

  for (dbskfg_composite_graph::edge_iterator eit =
           composite_graph_->edges_begin();
       eit != composite_graph_->edges_end(); ++eit)
  {
      dbskfg_composite_link_sptr link = *eit;

      if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK )
      {
  
          dbskfg_shock_link* shock_link=
              dynamic_cast<dbskfg_shock_link*>(&(*link));
          if ( shock_link->shock_link_type() == dbskfg_shock_link::SHOCK_EDGE )
          {
       
              shock_link->form_shock_fragment();

              dbskfg_rag_node_sptr rag_node = new dbskfg_rag_node(
                  rag_graph_->next_available_id());

              rag_node->add_shock_link(shock_link);

              rag_graph_->add_vertex(rag_node);
        
              shock_link->set_rag_node(&(*rag_node));
          }
      }
  }
}

vcl_vector< dbskfg_contour_node* > 
dbskfg_compute_composite_graph::
find_points_on_line(vgl_line_segment_2d<double>& line,int s_id,int e_id)
{
    // Create strings for left and right points
    vgl_point_2d<double> stpt    = line.point1();
    vgl_point_2d<double> endpt   = line.point2();
            
    vcl_stringstream st_streamer,end_streamer;
    
    st_streamer  << stpt;
    end_streamer << endpt;

    vcl_string st_string   = st_streamer.str();
    vcl_string end_string  = end_streamer.str();
    
    // Keep the result of processing
    vcl_vector<dbskfg_contour_node*> intersecting_points;

    // Loop over graph and find all nodes with this contour id
    vcl_vector<dbskfg_contour_node* > contour_segs;
    
    // Loop over map of this contour id
    vcl_vector<dbskfg_contour_node*> contour_elements_start=
        nodes_contour_[s_id];

    // Loop over map of possibly other contour id
    vcl_vector<dbskfg_contour_node*> contour_elements_end=
        nodes_contour_[e_id];

    for ( unsigned int i =0 ;i < contour_elements_start.size() ; ++i)
    {

        dbskfg_contour_node* contour_piece = 
            contour_elements_start[i];

        vcl_stringstream cp_s;
        cp_s<<contour_piece->pt();

        if ( (st_string != cp_s.str()) &&
             (end_string != cp_s.str()) )
        {
           
            contour_segs.push_back(contour_piece);
        }

    }

    if ( s_id != e_id )
    {

        for ( unsigned int i =0 ;i < contour_elements_end.size() ; ++i)
        {

            dbskfg_contour_node* contour_piece = 
                contour_elements_end[i];

            vcl_stringstream cp_s;
            cp_s<<contour_piece->pt();

            if ( (st_string != cp_s.str()) &&
                 (end_string != cp_s.str()) )
            {
           
                contour_segs.push_back(contour_piece);
            }

        }


    }


       

    // We have found all nodes on this contour, lets now see which ones 
    // exist in this line 
    vcl_map<double,unsigned int> sort_map;    
    for ( unsigned int i = 0 ; i < contour_segs.size() ; ++i)
    {
        if ( vgl_lineseg_test_point(contour_segs[i]->pt(),line) )
        {
            sort_map[vgl_distance(line.point1(),contour_segs[i]->pt())]
                = i;
        }
   
    }

    // Define iterator
    vcl_map<double,unsigned int>::iterator it;
    for ( it = sort_map.begin() ; it != sort_map.end() ; ++it)
    {
                                   
       intersecting_points.push_back(contour_segs[(*it).second]);
       
    }
    return intersecting_points;

}

void dbskfg_compute_composite_graph::classify_nodes()
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
