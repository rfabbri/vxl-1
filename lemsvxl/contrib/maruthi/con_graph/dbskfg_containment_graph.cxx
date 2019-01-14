// This is file shp/dbskfg/dbskfg_containment_graph.cxx

//:
// \file

// con_graph headers
#include <con_graph/dbskfg_containment_graph.h>
#include <con_graph/dbskfg_containment_node_sptr.h>
#include <con_graph/dbskfg_containment_link_sptr.h>
// dbskfg headers
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <dbskfg/algo/dbskfg_transform_manager.h>
#include <dbskfg/algo/dbskfg_detect_transforms.h>
#include <dbskfg/dbskfg_utilities.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <dbskfg/dbskfg_shock_node.h>
// breye1 headers
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
// vsol headers
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
// vul headers
#include <vul/vul_psfile.h>
#include <vul/vul_timer.h>
// vcl headers
#include <sstream>
#include <numeric>
#include <algorithm>
#include <vnl/vnl_random.h>

#include <vgl/vgl_polygon_scan_iterator.h>

// ----------------------------------------------------------------------------
//: Constructor
dbskfg_containment_graph::
dbskfg_containment_graph(vidpro1_vsol2D_storage_sptr contour_map,
                         dbskfg_containment_node::ExpandType expansion,
                         double path_threshold): 
    dbgrl_graph<dbskfg_containment_node, dbskfg_containment_link>(),
    next_available_id_(0),
    expand_type_(expansion),
    path_threshold_(path_threshold),
    output_filename_()
{


    // 1) Set id equivalent to what they are
    std::vector< vsol_spatial_object_2d_sptr > vsol_list = 
        contour_map->all_data();

    for ( unsigned int i=0; i < vsol_list.size() ; ++i)
    {
        original_list_[i].push_back(vsol_list[i]);
        original_list_[i].front()->set_id(i);
    }

    // Create initial set of attributes
    std::map<unsigned int,std::vector<vsol_spatial_object_2d_sptr> >::iterator it;
    for ( it = original_list_.begin() ; it != original_list_.end() ; ++it)
    {
        std::vector<vsol_spatial_object_2d_sptr> cons
            = (*it).second;
        initial_attributes_[(*it).first]=true;

    }
}

unsigned int dbskfg_containment_graph::next_available_id()
{
    next_available_id_++;
    return next_available_id_;

}

// ----------------------------------------------------------------------------
//: Destructor
dbskfg_containment_graph::~dbskfg_containment_graph()
{
    original_list_.clear();
    depth_map_.clear();
    depth_nodes_.clear();
    node_cache_.clear();
}

// ----------------------------------------------------------------------------
//: compute region root
void dbskfg_containment_graph::compute_region_root(
    dbskfg_rag_node_sptr rag_node)
{
    
    // Grab the original contours that bound region
    std::set<unsigned int> rag_con_ids;
    rag_node->rag_contour_ids(rag_con_ids);

    // Detect transforms affecting this region
    std::vector<dbskfg_transform_descriptor_sptr> results; 
    {
        dbskfg_detect_transforms transforms(
            dbskfg_transform_manager::Instance().get_cgraph(),
            dbskfg_transform_manager::Instance().get_image());

        transforms.transform_affects_region(rag_node,rag_con_ids);
        results = transforms.objects();
    }
   
    // Create root node
    dbskfg_containment_node_sptr root_node
        = new dbskfg_containment_node(this->next_available_id(),
                                      rag_node->id(),
                                      0);

    this->add_vertex(root_node);

    // Grab initial wavefront to create
    std::map<unsigned int, dbskfg_shock_node*> wavefront
        = rag_node->get_wavefront();

    std::set<std::string> rag_node_wavefront;

    std::map<unsigned int, dbskfg_shock_node*>::iterator it;
    for ( it = wavefront.begin(); it != wavefront.end() ; ++it)
    {
        vgl_point_2d<double> point = (*it).second->pt();
        std::stringstream stream;
        stream<<point;
        rag_node_wavefront.insert(stream.str());
    }
    
    // Create a vector of all size one
    // Create initial set of attributes
    root_node->set_attributes(initial_attributes_);
    root_node->set_rag_con_ids(rag_con_ids);
    root_node->set_rag_wavefront(rag_node_wavefront);

    std::map<std::string,unsigned int>& gap_map =
        dbskfg_transform_manager::Instance().gap_map();
    std::vector<std::vector<unsigned int> > total_contours;

    // Determine all new contours to be entered
    for ( unsigned int i(0) ; i < results.size()  ; ++i)
    {
        // For each transform lets compute polygon
        std::vector<unsigned int> contours_affected =
            results[i]->contour_ids_affected();

        // Determine transform type
        dbskfg_transform_descriptor::TransformType ttype =
            results[i]->transform_type_;
        
        // Get next id
        unsigned int next_id = original_list_.size();

        if ( ttype == dbskfg_transform_descriptor::GAP )
        {
            std::vector<vsol_spatial_object_2d_sptr> 
                gap_contours = results[i]->new_contours_spatial_objects_;
            std::pair<std::string,std::string> gap_strings =
                results[i]->gap_string();

            gap_map[gap_strings.first]=next_id;
            gap_map[gap_strings.second]=next_id;

            for ( unsigned int c=0; c < gap_contours.size() ; ++c)
            {
                original_list_[next_id].push_back(gap_contours[c]);

            }

            results[i]->gap_id_ = next_id;
            contours_affected.push_back(next_id);
        
        }
     
        total_contours.push_back(contours_affected);
        
    }
  
    for ( unsigned int j(0) ; j < results.size()  ; ++j)
    {

        // Determine next node
        dbskfg_containment_node_sptr next_node(0);

        // Determine what type of expansion we are doing
        if ( expand_type_ == dbskfg_containment_node::EXPLICIT )
        {
            next_node = new dbskfg_containment_node(
                this->next_available_id(),
                *dbskfg_transform_manager::Instance().get_cgraph(),
                *dbskfg_transform_manager::Instance().get_rag_graph(),
                1);
        }
        else
        {
            next_node = new dbskfg_containment_node(
                this->next_available_id(),
                rag_node->id(),
                1);

        }
          
        this->add_vertex(next_node);
        
        dbskfg_containment_link_sptr link(0);
        link = new dbskfg_containment_link(root_node,
                                           next_node,
                                           results[j]);

        if ( expand_type_ == dbskfg_containment_node::IMPLICIT )
        {
            link->determine_group_transform();
        }

        link->set_contours_affected(total_contours[j]);
        this->add_edge(link);
        

        root_node->add_outgoing_edge(link);
        next_node->add_incoming_edge(link);
        next_node->update_attributes();
        next_node->set_prob_of_fragment(link->cost());
    }
  
    results.clear();
    total_contours.clear();
   
    // expand_tree();
    print(std::cout);
  
}


// ----------------------------------------------------------------------------
//: compute region root
bool dbskfg_containment_graph::expand_region_root(
    dbskfg_rag_node_sptr rag_node)
{
    bool flag=true;

    // Create root node
    dbskfg_containment_node_sptr root_node
        = new dbskfg_containment_node(this->next_available_id(),
                                      rag_node->id(),
                                      0);

    this->add_vertex(root_node);


    std::set<std::string> rag_wavefront;
    std::set<unsigned int> rag_con_ids;
    rag_node->rag_contour_ids(rag_con_ids);
    rag_node->wavefront_string(rag_wavefront);

    vgl_polygon<double> root_poly;
    rag_node->fragment_boundary(root_poly);

    root_node->set_polygon(root_poly);

    root_node->set_rag_con_ids(rag_con_ids);
    root_node->set_rag_wavefront(rag_wavefront);
    root_node->set_attributes(initial_attributes_);
    root_node->expand_root_node(original_list_,*this,rag_node);
                        

    if ( root_node->out_degree() == 0 )
    {
        // This node has nothing
        this->remove_vertex(root_node);

        flag=false;
    }

    return flag;
}

void dbskfg_containment_graph::print(std::ostream& os)
{
    os<<"Containment Graph Vertices: "<<this->number_of_vertices()
            <<std::endl;
    os<<"Number of Edges "<<this->number_of_edges()<<std::endl;


    for (dbskfg_containment_graph::vertex_iterator vit = 
             this->vertices_begin(); 
         vit != this->vertices_end(); ++vit)
    {
        os<<"Number of Outgoing Edges "<<(*vit)->out_degree()<<std::endl;
        os<<"Number of Incoming Edges "<<(*vit)->in_degree()<<std::endl;
        (*vit)->print(os);


    }

    std::stringstream file_stream;

    std::string output_string;

    file_stream<<"cgraph_"<<this->number_of_vertices()<<".dot";
    if ( output_filename_.size() )
    {
        output_string=output_filename_;
    }
    else
    {
        output_string=file_stream.str();
    }

    std::ofstream file(output_string.c_str());

    file<<"digraph G {"<<std::endl;
    for (dbskfg_containment_graph::vertex_iterator vit = 
             this->vertices_begin(); 
         vit != this->vertices_end(); ++vit)
    {
  

       
        std::stringstream filename;
        filename<<"Node"<<(*vit)->id()<<".ps";
        std::string filestring=filename.str();
        print_node((*vit)->get_attributes(),filestring,(*vit)->get_polygon());

         dbskfg_containment_node::edge_iterator srit;

        // Move to left in and out
        for ( srit = (*vit)->out_edges_begin() ; 
              srit != (*vit)->out_edges_end() 
                  ; ++srit)
        {
            std::string transform_string;
            if ((*srit)->transform_type() == 
                dbskfg_transform_descriptor::GAP) 
            {
                std::vector<unsigned int> contours_affected = 
                    (*srit)->get_contours_affected();
                std::stringstream sstream;
                sstream<<"G={";
                for ( unsigned int b=0; b < contours_affected.size() ; b++)
                {
                    if ( b != contours_affected.size() - 1 )
                    {
                        sstream<<contours_affected[b]<<",";
                    }
                    else
                    {
                        sstream<<contours_affected[b]<<"}";
                    }
                }
                
                transform_string=sstream.str();

            }
            else
            {
                std::vector<unsigned int> contours_affected = 
                    (*srit)->get_contours_affected();
                std::stringstream sstream;
                sstream<<"L={";
                for ( unsigned int b=0; b < contours_affected.size() ; b++)
                {
                    if ( b != contours_affected.size() - 1 )
                    {
                        sstream<<contours_affected[b]<<",";
                    }
                    else
                    {
                        sstream<<contours_affected[b]<<"}";
                    }
                }
                transform_string=sstream.str();

            }
            
            file<<"\t"<<(*vit)->id()<<" -> "
                <<(*srit)->target()->id()<<" [ label=\""<<transform_string
                <<" cost= "
                <<(*srit)->cost()
                <<" dist= "
                <<(*srit)->get_distance()
                <<"\" ];"
                <<std::endl; 
        }

        

    }
    file<<"}"<<std::endl;
    file.close();


}

void dbskfg_containment_graph::expand_tree(std::ofstream& stream)
{

    unsigned int depth=1;
    
    do
    {
        // Start timer
        vul_timer t;

        std::vector<dbskfg_containment_node_sptr> queue=
            depth_nodes_[depth];

        stream<<"Depth: "<<depth<<" Node: "<<queue.size()<<std::endl;
        depth_map_[depth]=queue.size();

        std::cout<<"*************************"<<std::endl;
        std::cout<<"EXPANDING DEPTH : "<<depth<<std::endl;
        std::cout<<"*************************"<<std::endl;

        expand_tree_level(queue,stream);

        queue.clear();
  
        depth++;

        double vox_time = t.real()/1000.0;
        t.mark();
      
        stream<<"Time: "<<vox_time<<" sec"<<std::endl;
        stream<<std::endl;


    }while(depth_nodes_.count(depth));

    
}

void dbskfg_containment_graph::expand_tree_level(
    std::vector<dbskfg_containment_node_sptr>& queue,
    std::ofstream& stream)
{
    std::vector<double> times_per_node;
    std::map<unsigned int,dbskfg_containment_node_sptr> nodes;

    while(!queue.empty())
    {
        // Pop of stack
        dbskfg_containment_node_sptr node=queue.back();

        // Start timer
        vul_timer t;
   
        if ( node->get_prob_of_fragment() > path_threshold_ )
        {
            if ( expand_type_ == dbskfg_containment_node::IMPLICIT )
            {
                // Expand this node
                node->expand_node_implicit_multi(original_list_,
                                                 *this);
            }
            else
            {
                // Expand this node
                node->expand_node_explicit(nodes,
                                           original_list_,
                                           *this);

            }
        }
      
        double vox_time = t.real()/1000.0;
        t.mark();
       
        times_per_node.push_back(vox_time);

        // clean up stack
        queue.pop_back();

    }

    double average_node_time = std::accumulate(times_per_node.begin(),
                                              times_per_node.end(),
                                              0.0);

    stream<<"Average Node Time: "<<average_node_time/times_per_node.size()
            <<std::endl;
    

    nodes.clear();
}

void dbskfg_containment_graph::print_node(std::map<unsigned int,bool> attributes,
                                          std::string filename,
                                          vgl_polygon<double>& vgl_poly)
{
    // create a ps file object
    vul_psfile psfile(filename.c_str(), false);

    psfile.set_line_width(1.0f);
    psfile.set_fg_color(1.0f,0.0f,0.0f);
    psfile.set_bg_color(1.0f,1.0f,1.0f);

    vnl_random rand_object;
     
    psfile.set_fg_color(rand_object.drand64(0.0,1.0),
                        rand_object.drand64(0.0,1.0),
                        rand_object.drand64(0.0,1.0));
    
    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(vgl_poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            
            if (x < 0 || y < 0)
            {
                continue;
            }
            psfile.point(x,y,1.0f);
        }
    }

    std::map<unsigned int,std::vector<vsol_spatial_object_2d_sptr> >::iterator it;
    for ( it = original_list_.begin() ; it != original_list_.end() ; ++it)
    {
        if (attributes[(*it).first] == true )
        {
            std::vector<vsol_spatial_object_2d_sptr> cons
                = (*it).second;

            for ( unsigned int k=0; k < cons.size() ; ++k)
            {
        
                vsol_spatial_object_2d_sptr object = cons[k];

                if ( initial_attributes_.count((*it).first) == 0 &&
                     object->get_id() == 0 )
                {
                    psfile.set_fg_color(0.0f,0.0f,1.0f);
                }
                else
                {
                    psfile.set_fg_color(1.0f,0.0f,0.0f);
                }

                if ( object->cast_to_region() )
                {
                    if ( object->cast_to_region()->cast_to_polygon() )
                    {
                        vsol_polygon_2d_sptr poly =
                            object->cast_to_region()->cast_to_polygon();

                        // collect vertices of the polyline
                        for (unsigned int i = 1; i < poly->size(); i++)
                        {
                            psfile.line(
                                poly->vertex(i-1)->x(),
                                poly->vertex(i-1)->y(),
                                poly->vertex(i  )->x(),
                                poly->vertex(i  )->y());
                        }

                        psfile.line(
                            poly->vertex(poly->size()-1)->x(),
                            poly->vertex(poly->size()-1)->y(),
                            poly->vertex(0  )->x(),
                            poly->vertex(0  )->y());

                    }
                }
                else
                {
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
                        std::vector<vgl_point_2d<double > > pts;

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
            }
        }
    
    }



}

void dbskfg_containment_graph::print_node_cache()
{

    // Keep a map of all nodes for easy cache
    std::map< std::vector<bool>, std::vector< std::pair<std::set<std::string>
        ,dbskfg_containment_node_sptr > > >::iterator bit;
    for ( bit = node_cache_.begin(); bit != node_cache_.end() ; ++bit)
    {
        std::vector<bool> attr = (*bit).first;
        for ( unsigned int k=0; k < attr.size() ; ++k)
        {
            std::cout<<" Contour ( "<<k <<" ) "<< attr[k]<<std::endl;
        }
        std::vector< std::pair<std::set<std::string>, 
            dbskfg_containment_node_sptr > > nodes_to_consider = 
            (*bit).second;

        std::vector< std::pair<std::set<std::string>, 
            dbskfg_containment_node_sptr > >::iterator it;

        for ( it = nodes_to_consider.begin() ; it != nodes_to_consider.end(); 
              ++it)
        {
            std::pair<std::set<std::string>, dbskfg_containment_node_sptr >
              node_pair   = (*it);

            node_pair.second->print(std::cout);
        }
        

    }
}

//: Return node if
dbskfg_containment_node_sptr dbskfg_containment_graph::node_merge(
    std::map<unsigned int,bool>& attr,
    std::set<std::string>& wavefront)
{
    dbskfg_containment_node_sptr node=0;

    unsigned int string1_size  = (*attr.rbegin()).first+1;
    std::vector<bool> bit_set1(string1_size,false);

    for  ( unsigned int i=0; i < bit_set1.size() ; ++i) 
    {
        if ( attr.count(i) && attr[i] )
        {
            bit_set1[i]=true;
        }
    }

    std::vector< std::pair<std::set<std::string>, 
        dbskfg_containment_node_sptr > >::iterator it; 
        
    if ( node_cache_.count(bit_set1))
    {
        std::vector< std::pair<std::set<std::string>, 
            dbskfg_containment_node_sptr > > nodes_to_consider = 
            node_cache_[bit_set1];



        for ( it = nodes_to_consider.begin() ; it != nodes_to_consider.end(); 
              ++it)
        {
            std::pair<std::set<std::string>, dbskfg_containment_node_sptr >
              node_pair   = (*it);

            if ( wavefront.size() == node_pair.first.size())
            {

                std::set<std::string> regions_to_compare =
                    node_pair.first;

                std::vector<std::string> intersection(100);
                std::vector<std::string>::iterator start_iterator;
                start_iterator=intersection.begin();
                std::vector<std::string>::iterator out_iterator;

                out_iterator=std::set_intersection(wavefront.begin(),
                                                  wavefront.end(),
                                                  regions_to_compare.begin(),
                                                  regions_to_compare.end(),
                                                  start_iterator);
                int int_size = int(out_iterator-start_iterator);

                if ( int_size == wavefront.size())
                {
                    node=node_pair.second;
                    break;
                }


            }
        }
    }

    return node;
}

//: Insert node
void dbskfg_containment_graph::insert_node(
    dbskfg_containment_node_sptr node)
{
    std::map<unsigned int,bool> attr = node->get_attributes();
    std::set<std::string> wavefront   = node->get_rag_wavefront();

    std::pair<std::set<std::string>,dbskfg_containment_node_sptr> pair
        = std::make_pair(wavefront,node);

    unsigned int string1_size  = (*attr.rbegin()).first+1;
    std::vector<bool> bit_set1(string1_size,false);
    
    for  ( unsigned int i=0; i < bit_set1.size() ; ++i) 
    {
        if ( attr.count(i) && attr[i] )
        {
            bit_set1[i]=true;
        }
    }

    node_cache_[bit_set1].push_back(pair);
    depth_nodes_[node->depth()].push_back(node);
}
