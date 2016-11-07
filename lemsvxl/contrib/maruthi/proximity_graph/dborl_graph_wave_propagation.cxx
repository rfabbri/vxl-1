#include <proximity_graph/dborl_graph_wave_propagation.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_numeric.h>
#include <boost/graph/graphml.hpp>

void dborl_graph_wave_propagation::find_nn(
    vcl_string xml_graph_file,
    vcl_string dataset_file,
    vcl_string query_file,
    vcl_string seed_file,
    vcl_string stats_file,
    dborl_graph_wave_propagation::Search_type search_flag,
    double tau,
    bool verbose)
{

    // Save off print flag
    verbose_=verbose;

    // Save off of tau for extended neighborhood
    tau_ = tau;

    //Read graph file first
    dynamic_properties dp;
    dp.property("name",get(vertex_name_t(),proximity_graph_));
    dp.property("distance",get(edge_weight_t(),proximity_graph_));
    dp.property("category",get(dborl_proximity_graph::vertex_cat_t(),
                               proximity_graph_));

    vcl_ifstream ifile(xml_graph_file.c_str());
    read_graphml(ifile,proximity_graph_,dp);
    ifile.close();

    //Reset number of shapes
    number_of_shapes_ = num_vertices(proximity_graph_);

    // Read in files first
    read_files(dataset_file,query_file,seed_file);

    // Find vertex set of seeds
    find_vertex_seeds();

    // Initialize stats file
    stats_file_ = stats_file;

    // Wave propagation on graph
    for (unsigned int i=0; i <queries_.size() ; i++)
    {
        initialize_visitor_status();
        if (search_flag == dborl_graph_wave_propagation::Grad_Descent)
        {
            gradient_descent(i);
        }
        else
        {
            wave_propagate(i);
        }

        print_statistics(i);
    }


}


void dborl_graph_wave_propagation::test_reachability(
    vcl_string xml_graph_file,
    vcl_string dataset_file,
    vcl_string stats_file,
    dborl_graph_wave_propagation::Search_type search_flag,
    double tau,
    bool verbose)
{

    // Save off verbose flag
    verbose_ = verbose;

    // Save off of tau for extended neighborhood
    tau_ = tau;

    //Read graph file first
    dynamic_properties dp;
    dp.property("name",get(vertex_name_t(),proximity_graph_));
    dp.property("distance",get(edge_weight_t(),proximity_graph_));
    dp.property("category",get(dborl_proximity_graph::vertex_cat_t(),
                               proximity_graph_));


    vcl_ifstream ifile(xml_graph_file.c_str());
    read_graphml(ifile,proximity_graph_,dp);
    ifile.close();

    // Initialize stats file
    stats_file_ = stats_file;

    //Reset number of shapes
    number_of_shapes_ = num_vertices(proximity_graph_);
    number_of_queries_= number_of_shapes_;

    // Rebind stream to data matrix
    // Create a file stream opener
    vcl_ifstream file_opener;
    file_opener.open(dataset_file.c_str());
    
    // Set matrix size to number of shapes
    query_sim_matrix_.set_size(number_of_shapes_,number_of_shapes_);

    // Read in data matrix
    for ( unsigned int i=0; i < number_of_shapes_ ; i++)
    {
        for ( unsigned int j=0; j < number_of_shapes_ ; j++)
        {
            file_opener>>query_sim_matrix_[i][j];
        }
    }
    file_opener.close();

    // Grab nodes name
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);

    // Print out node information 
    typedef graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::vertex_iterator vertex_iter;
    vcl_pair<vertex_iter, vertex_iter> vp;
    vcl_pair<vertex_iter, vertex_iter> vp_seeds;

    //Define a statistics data structure
    vcl_vector<unsigned int> stats;

    //Create a queries list
    for (vp = vertices(proximity_graph_); vp.first != vp.second; ++vp.first)
    {
        queries_.push_back(node_name[*vp.first]);
    }

    
    vcl_ofstream stats_opener(stats_file_.c_str(),std::ios::app);

    // Keep track of queries reached
    unsigned int queries_reached(0);

    // Keep track of query that reaches maximum reached
    vcl_string max_node_string;
    unsigned int max_reached=queries_reached;

    //Loop over seeds and push back seed then loop over queries
    for (vp_seeds = vertices(proximity_graph_); 
         vp_seeds.first != vp_seeds.second; ++vp_seeds.first)
    {
        // push back seeds name
        seeds_.push_back(node_name[*vp_seeds.first]);

        // Find vertex set of seeds
        find_vertex_seeds();

        // Wave propagation on graph
        for (unsigned int i=0; i <queries_.size() ; i++)
        {

            initialize_visitor_status();

            if (search_flag == dborl_graph_wave_propagation::Grad_Descent)
            {
                gradient_descent(i);
            }
            else
            {
                wave_propagate(i);
            }

            if( node_name[nearest_neighbor_] == queries_[i] )
            {
                queries_reached++;
                
            }
            
            if ( queries_reached > max_reached)
            {
                max_reached=queries_reached;
                max_node_string=seeds_[0];
                


            }
            
        }

        stats.push_back(queries_reached);

        seeds_.clear();

        //Set temp back to zero
        queries_reached=0;
        
        //Clear out vertex seeds   
        vertex_seeds_.erase(vertex_seeds_.begin(),vertex_seeds_.end());

    }


    //Find number of nodes reached 
    unsigned int numb_nodes_reached=
        static_cast<unsigned int>(vcl_count(stats.begin(),
                                            stats.end(),
                                            number_of_shapes_));

    //Find average nodes reached from any start node
    double average_reached=
        vcl_accumulate(stats.begin(),stats.end(),0)/
        static_cast<double>(stats.size());

    stats_opener<<"total_queries_reached: "
                << numb_nodes_reached
                <<" avg_queries_reached: "
                << average_reached
                <<" max_node: "
                <<max_node_string
                <<" nodes_reached: "
                <<max_reached
                <<" edges: "
                << num_edges(proximity_graph_)
                <<" vertices: "
                << number_of_shapes_
                <<vcl_endl;
    

    stats_opener.close();
}

void dborl_graph_wave_propagation::optimal_seed_selection(
    vcl_string xml_graph_file,
    vcl_string dataset_file,
    vcl_string stats_file,
    dborl_graph_wave_propagation::Search_type search_flag,
    double tau,
    bool verbose)
{

    // Save off verbose flag
    verbose_ = verbose;

    // Save off of tau for extended neighborhood
    tau_ = tau;

    //Read graph file first
    dynamic_properties dp;
    dp.property("name",get(vertex_name_t(),proximity_graph_));
    dp.property("distance",get(edge_weight_t(),proximity_graph_));
    dp.property("category",get(dborl_proximity_graph::vertex_cat_t(),
                               proximity_graph_));

    vcl_ifstream ifile(xml_graph_file.c_str());
    read_graphml(ifile,proximity_graph_,dp);
    ifile.close();

    // Initialize stats file
    stats_file_ = stats_file;

    //Reset number of shapes
    number_of_shapes_ = num_vertices(proximity_graph_);
    number_of_queries_= number_of_shapes_;

    // Rebind stream to data matrix
    // Create a file stream opener
    vcl_ifstream file_opener;
    file_opener.open(dataset_file.c_str());
    
    // Set matrix size to number of shapes
    query_sim_matrix_.set_size(number_of_shapes_,number_of_shapes_);

    // Read in data matrix
    for ( unsigned int i=0; i < number_of_shapes_ ; i++)
    {
        for ( unsigned int j=0; j < number_of_shapes_ ; j++)
        {
            file_opener>>query_sim_matrix_[i][j];
        }
    }
    file_opener.close();

    // Grab nodes name
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);

    // Print out node information 
    typedef graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::vertex_iterator vertex_iter;
    vcl_pair<vertex_iter, vertex_iter> vp;
    vcl_pair<vertex_iter, vertex_iter> vp_seeds;

    //Keep track of neighbors reached by greatest
    vcl_map< vcl_string,vcl_vector<vcl_string> > stats_neighbors_reached;

    // List of visit status
    vcl_map<vcl_string,dborl_graph_wave_propagation::Visit_Status> 
        queries_visitor_status;

    // Stats file to write results
    vcl_ofstream stats_opener(stats_file_.c_str(),std::ios::app);

    //Create a queries list
    for (vp = vertices(proximity_graph_); vp.first != vp.second; ++vp.first)
    {
        queries_.push_back(node_name[*vp.first]);
        queries_visitor_status[node_name[*vp.first]]=
            dborl_graph_wave_propagation::NOT_VISITED;

    }
    
    // Keep track of queries reached
    unsigned int queries_reached(0);

    // Keep track of query that reaches maximum reached
    vcl_string max_node_string;
    unsigned int max_reached=queries_reached;

    // Visited
    unsigned int visited = 0;

    while( visited != num_vertices(proximity_graph_) )
    {
        // Wave propagation on graph
        for (unsigned int i=0; i <queries_.size() ; i++)
        {

            // create seed name
            vcl_string seed_name = queries_[i];

            if ( queries_visitor_status[queries_[i]]
                 ==dborl_graph_wave_propagation::
                 NOT_VISITED)
            {
                // push back seeds name
                seeds_.push_back(queries_[i]);

                // Find vertex set of seeds
                find_vertex_seeds();
            }
           
            // Wave propagation on graph
            for (unsigned int k=0; k <queries_.size() ; k++)
            {
                if ( queries_visitor_status[queries_[k]]
                     ==dborl_graph_wave_propagation::
                     NOT_VISITED)
                {
                    initialize_visitor_status();

                    if (search_flag == dborl_graph_wave_propagation
                        ::Grad_Descent)
                    {
                        gradient_descent(k);
                    }
                    else
                    {
                        wave_propagate(k);
                    }

                    if( node_name[nearest_neighbor_] == queries_[k] )
                    {
                        queries_reached++;
                        stats_neighbors_reached[seed_name].
                            push_back(queries_[k]);
                    }
            
                    if ( queries_reached > max_reached)
                    {
                        max_reached=queries_reached;
                        max_node_string=seed_name;
                
                    }

                }
            
            }
            
            // Reset everything
            seeds_.clear();

            //Set temp back to zero
            queries_reached=0;
        
            //Clear out vertex seeds   
            vertex_seeds_.erase(vertex_seeds_.begin(),vertex_seeds_.end());

        }

        //We have found best seed lets reset everyting
        //Lets redo query list
        //remove elements out of query that have names
        // Grab maximum element

        vcl_vector<vcl_string> max_node=stats_neighbors_reached
            [max_node_string];
   
        vcl_cout<<"best node: "<<max_node_string<<" neighbors reached: "
                <<max_node.size()<<vcl_endl;
   
        stats_opener<<max_node_string<<vcl_endl;

    
        // Insert seeds
        for(unsigned int j=0; j < max_node.size() ; j++)
        {
         
            queries_visitor_status[max_node[j]]=
                                   dborl_graph_wave_propagation::VISITED;
            visited++;
        }

        

        queries_reached=0;
        max_reached=queries_reached;

        // Reset everything
        seeds_.clear();
        
        //Clear out vertex seeds   
        vertex_seeds_.erase(vertex_seeds_.begin(),vertex_seeds_.end());
        
     
        //Erase map
        stats_neighbors_reached.erase(stats_neighbors_reached.begin(),
                                       stats_neighbors_reached.end());

    }

    stats_opener.close();
}

void dborl_graph_wave_propagation::initialize_visitor_status()
{

    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);

    //Reset best distance
    best_distance_=1e10;

    // Print out node information 
    typedef graph_traits<dborl_proximity_graph::Undirected_Graph>
         ::vertex_iterator vertex_iter;
    vcl_pair<vertex_iter, vertex_iter> vp;

    for (vp = vertices(proximity_graph_); vp.first != vp.second; ++vp.first)
    {
        visitor_status_[node_name[*vp.first]]=
            dborl_graph_wave_propagation::NOT_VISITED;
            
    
    }

    //Reclear wavefront
    wavefront_.erase(wavefront_.begin(),wavefront_.end());

}

void dborl_graph_wave_propagation::read_files(vcl_string dataset_file,
                                              vcl_string query_file,
                                              vcl_string seed_file)
{
    // Create a file stream opener
    vcl_ifstream file_opener;
    vcl_string temp;

    //--------------------  Open queries ----------------------------------

    // Open the file
    file_opener.open(query_file.c_str());

    // Read each exemplar name
    while(file_opener)
    {
        getline(file_opener,temp);
        queries_.push_back(temp);
    }

    //Delete last element
    queries_.pop_back();
    
    //Close file
    file_opener.close();
    
    // -------------------- Open seeds ----------------------------------

    // Open the file
    file_opener.open(seed_file.c_str());

    // Read each exemplar name
    while(file_opener)
    {
        getline(file_opener,temp);
        seeds_.push_back(temp);
    }

    //Delete last element
    seeds_.pop_back();
   
    vcl_cout<<"Number of seeds: "<<seeds_.size()<<vcl_endl;

    //Close file
    file_opener.close();
    
    // -------------------- Read in a data matrix ------------------------

    // Rebind stream to data matrix
    file_opener.open(dataset_file.c_str());
    
    // Read in data_matrix
    // Get number of shapes
    number_of_queries_ = queries_.size();

    // Set matrix size to number of shapes
    query_sim_matrix_.set_size(number_of_queries_,number_of_shapes_);

    // Read in data matrix
    for ( unsigned int i=0; i < number_of_queries_ ; i++)
    {
        for ( unsigned int j=0; j < number_of_shapes_ ; j++)
        {
            file_opener>>query_sim_matrix_[i][j];
        }
    }

    file_opener.close();
}

void dborl_graph_wave_propagation::find_vertex_seeds()
{

    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_index_t>::type
         node_index = get(vertex_index, proximity_graph_);
  
    // Print out node information 
    typedef graph_traits<dborl_proximity_graph::Undirected_Graph>
         ::vertex_iterator vertex_iter;
    vcl_pair<vertex_iter, vertex_iter> vp;

    vcl_string seed_name;
    for ( unsigned int i(0) ; i < seeds_.size() ; ++i )
    {
        seed_name=seeds_[i];
        for (vp = vertices(proximity_graph_); vp.first != vp.second; ++vp.first)
        {
            if ( node_name[*vp.first] == seed_name)
            { 
                vertex_seeds_[seed_name]=*vp.first;
            }
        }

    }


}

void dborl_graph_wave_propagation::wave_propagate(unsigned int query_index)
{

    // Initialize wavefront with seeds
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_index_t>::type
         node_index = get(vertex_index, proximity_graph_);
    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
 
    // Insert seeds
    for(vcl_map<vcl_string,dborl_proximity_graph::Vertex>::const_iterator 
            it = vertex_seeds_.begin(); it != vertex_seeds_.end(); ++it)
    {
        wavefront_insert
            (it->second,
             query_sim_matrix_[query_index][node_index[it->second]]);
    
        visitor_status_[node_name[it->second]]=
                dborl_graph_wave_propagation::IN_LIST;
    
    }

    
    //Keep track of minimum distance in front
    //Define iterator of wavefront
    best_distance_  = (wavefront_.begin())->second;

    double front_distance  = best_distance_;
    unsigned init=1;

    // Keep track of front node before we pop
    dborl_proximity_graph::Vertex incoming_node=wavefront_.begin()->first;

    //Grab best neighbors
    nearest_neighbor_=incoming_node;

    while(init || ((best_distance_ > front_distance/tau_) 
                   && !wavefront_.empty()))
    {

        // Re init
        init=0;

        // Printing
        if ( verbose_ )
        {

            vcl_cout<<"-------------"<<vcl_endl;
            print_queue();

        }

        // pop off wavefront
        wavefront_.pop_front();

        //Burn wave
        propagate_nodes(incoming_node,
                        query_index);

        //Grab incoming node
        if ( !wavefront_.empty())
        {
            incoming_node=wavefront_.begin()->first;
        
            //Grab front distance
            front_distance = query_sim_matrix_[query_index]
                [node_index[incoming_node]];
        }
    }
 
    if ( verbose_ )
    {

        vcl_cout<<"The nearest neighbor is "<<
            node_name[nearest_neighbor_]<<vcl_endl;

    }
}

void dborl_graph_wave_propagation::propagate_nodes(
    dborl_proximity_graph::Vertex incoming_node,
    unsigned int query_index)
{
    
    //Look through all adjacent vertices of the graph
    graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::adjacency_iterator ai;
    graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::adjacency_iterator ai_end;

    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_index_t>::type
         node_index = get(vertex_index, proximity_graph_);
 
    // Before we pop node off of list, lets say that we visited it
    // Pop node of off wavefront
    visitor_status_[node_name[incoming_node]]=
        dborl_graph_wave_propagation::VISITED;

    // Keep track of front of wavefront
    // Grab nearest neighbor before we pop off

    double d = query_sim_matrix_[query_index][node_index[incoming_node]];

    if ( d < best_distance_ )
    {
        nearest_neighbor_=incoming_node;
        best_distance_ = d;

    }

    // Insert into wavefront
    for (tie(ai, ai_end) = adjacent_vertices(incoming_node, proximity_graph_);
            ai != ai_end; ++ai)
    {
        
        // If node has been visited before dont bother revisiting
        if ( visitor_status_[node_name[*ai]] 
             == dborl_graph_wave_propagation::NOT_VISITED )
        {
            double temp_distance = 
                query_sim_matrix_[query_index][node_index[*ai]];
            visitor_status_[node_name[*ai]]=
                dborl_graph_wave_propagation::IN_LIST;
            wavefront_insert(*ai,temp_distance);
        }
     
    }
 
}

void dborl_graph_wave_propagation::gradient_descent(unsigned int query_index)
{

    vcl_string shape_name;

    // Find seed of shape to start at
    shape_name = find_start_seed(query_index);
    
    // Grab vertex in graph of start seed
    dborl_proximity_graph::Vertex prev_shape=
        vertex_seeds_[shape_name];

    // Save a temp shape
    dborl_proximity_graph::Vertex temp_shape;

    // Start off from seed distance
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_index_t>::type
         node_index = get(vertex_index, proximity_graph_);
    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
 
    double prev_distance = 
        query_sim_matrix_[query_index][node_index[prev_shape]];
    
    // Set up next distance
    double next_distance=prev_distance;

    if (verbose_ )
    {
        vcl_cout<<"---------------------------------------------------"
                <<vcl_endl;
        vcl_cout<<"Finding nearest neighbor of "<<queries_[query_index]
                <<vcl_endl;
        vcl_cout<<"The start seed of the wave is "
            <<node_name[prev_shape]<<" -------> " << next_distance <<vcl_endl;
    }

    // Seed computation
    unsigned distance_computations(0);

    do{

        // Swap distances
        prev_distance=next_distance;

        // Keep track of nearest neighbor
        nearest_neighbor_=prev_shape;

        // Keep track of old shape

        // Grab start distance
        prev_shape = closest_vertex_neighbor(prev_shape,query_index);
        
        // Grab next distance
        next_distance=query_sim_matrix_[query_index][node_index[prev_shape]];

    }while(next_distance < tau_*prev_distance);

    if ( verbose_ )
    {

        vcl_cout<<"The closest shape is "<< node_name[nearest_neighbor_]
                <<vcl_endl;
        vcl_cout<<"---------------------------------------------------"
                <<vcl_endl;

    }

}


dborl_proximity_graph::Vertex
dborl_graph_wave_propagation::closest_vertex_neighbor
(dborl_proximity_graph::Vertex incoming_node,unsigned int query_index)
{

    //Look through all adjacent vertices of the graph
    graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::adjacency_iterator ai;
    graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::adjacency_iterator ai_end;

    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_index_t>::type
         node_index = get(vertex_index, proximity_graph_);
    
    // Loop over all vertices of node
    // Save index
    unsigned int next_node_index=node_index[incoming_node];
    vcl_string next_node_name=node_name[incoming_node];

    // Keep track of distances
    double distance_seed_query=5000;

    // Lets say we visited this node
    visitor_status_[node_name[incoming_node]]=
        dborl_graph_wave_propagation::VISITED;

    for (tie(ai, ai_end) = adjacent_vertices(incoming_node, proximity_graph_);
            ai != ai_end; ++ai)
    {

        //Calculate distance and update numb computations
        double temp_distance = 
            query_sim_matrix_[query_index][node_index[*ai]];
        
        if ( temp_distance < distance_seed_query )
        {
            distance_seed_query=temp_distance;
            next_node_index=node_index[*ai];
            next_node_name=node_name[*ai];
        } 

        visitor_status_[node_name[*ai]]=
            dborl_graph_wave_propagation::IN_LIST;

    }

    if ( verbose_)
    {
        vcl_cout<<"The next node to visit is index "<<next_node_index<<
            " its name is "<<
            next_node_name<<" --------> distance "
                <<distance_seed_query<<vcl_endl;
    }


    return vertex(next_node_index,proximity_graph_);
}

vcl_string dborl_graph_wave_propagation::
find_start_seed(unsigned int query_index)
{

    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_index_t>::type
         node_index = get(vertex_index, proximity_graph_);

    // Keep track of distances
    double distance_seed_query=5000;
    
    // Keep track of shape name that is minimum
    vcl_string start_seed;

    for(vcl_map<vcl_string,dborl_proximity_graph::Vertex>::const_iterator 
            it = vertex_seeds_.begin(); it != vertex_seeds_.end(); ++it)
    {
        double temp_distance = 
            query_sim_matrix_[query_index][node_index[it->second]];

        if ( temp_distance < distance_seed_query )
        {
            distance_seed_query=temp_distance;
            start_seed=node_name[it->second];
        } 
        if ( verbose_ )
        {
            vcl_cout<<"The distance from "
                    << queries_[query_index]<<" to shape " 
                    << node_name[it->second]<< " is " 
                    << temp_distance<<vcl_endl;
        }


        visitor_status_[node_name[it->second]]=
            dborl_graph_wave_propagation::IN_LIST;
       
    }
    
    if ( verbose_ )
    {
        vcl_cout<<"The start seed is "<<start_seed<<vcl_endl;
    }

    return start_seed;

}

// Insert ordered queue of elements
void dborl_graph_wave_propagation::
wavefront_insert(dborl_proximity_graph::Vertex element,
                 double distance_to_query)
{
    //Define iterator of wavefront
    vcl_list< vcl_pair<dborl_proximity_graph::Vertex,double> >::iterator iter 
        = wavefront_.begin();

    // Find place where to insert new vertex element
    while(iter!=wavefront_.end() && iter->second < distance_to_query)
    {
      iter++;
    }
    
    // Insert ordered element
    wavefront_.insert
        (iter,
         vcl_pair<dborl_proximity_graph::Vertex,double>
         (element,distance_to_query));

}

void dborl_graph_wave_propagation::print_queue()
{

    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
  
    //Define iterator to printstuff
    vcl_list< vcl_pair<dborl_proximity_graph::Vertex,double> >::iterator iterp 
        = wavefront_.begin();
 
    // Find place where to insert new vertex element
    while(iterp!=wavefront_.end())
    {
        vcl_cout<<"The distance from query to shape " 
                << node_name[iterp->first]<< " is " 
                << iterp->second<<vcl_endl;
        
        iterp++;
    }


}

void dborl_graph_wave_propagation::print_statistics(unsigned int query_index)
{

    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_index_t>::type
        node_index = get(vertex_index, proximity_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, 
        dborl_proximity_graph::vertex_cat_t>::type
        cat_name = get(dborl_proximity_graph::vertex_cat_t(),proximity_graph_);

    vcl_ofstream stats_opener(stats_file_.c_str(),std::ios::app);

    // Determine number of nodes visited
    unsigned int visit=0;
    unsigned int not_visit=0;

    for(vcl_map<vcl_string,dborl_graph_wave_propagation::Visit_Status>
            ::const_iterator 
            it = visitor_status_.begin(); it != visitor_status_.end(); ++it)
    {
        if(it->second == dborl_graph_wave_propagation::NOT_VISITED )
        {

            not_visit++;
        }
        else
        {
            visit++;
        }
    }


    stats_opener<<queries_[query_index]
                <<" & "
                <<node_name[nearest_neighbor_]
                <<" & "
                <<cat_name[nearest_neighbor_]
                <<" & "
                << query_sim_matrix_[query_index][node_index[nearest_neighbor_]]
                <<" Visited: "
                << visit
                <<" Not Visited: "
                << not_visit<<vcl_endl;


    stats_opener.close();
}
