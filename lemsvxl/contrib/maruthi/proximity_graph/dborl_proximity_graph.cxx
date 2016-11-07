#include <proximity_graph/dborl_proximity_graph.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>
#include <boost/graph/graphml.hpp>
#include <vcl_fstream.h>
#include <vnl/vnl_math.h>
#include <vcl_cfloat.h>
#include <vcl_numeric.h>

void dborl_proximity_graph::construct_graph(
    vcl_string exemplar_dataset_file, 
    vcl_string exemplar_label_file,
    vcl_string exemplar_category_file,
    vcl_string output_graph,
    double beta,
    bool verbose,
    bool perform_wilson_edit,
    bool perform_thinning)
{

    // Save double beta
    beta_ = beta;

    //Read files first
    read_files(exemplar_dataset_file,exemplar_label_file,
               exemplar_category_file);

    // Preprocess graph
    preprocess_graph();

    //Start construcing graph
    build_beta_graph();

    // Perform wilson editing
    if ( perform_wilson_edit )
    {
        wilson_edit();
    }

    // Perform thinning
    if ( perform_thinning )
    {
        thinning();
    }

    //Write out graph
    write_graph(output_graph);

    // Print out graph
    if(verbose) 
    {
        print_graph();
    }

}

void dborl_proximity_graph::read_files(vcl_string dataset_file, 
                                       vcl_string node_name_file,
                                       vcl_string category_name_file)
{

    // Open the file
    vcl_ifstream file_opener;
    file_opener.open(node_name_file.c_str());

    // Read each exemplar name
    vcl_string temp;
    while(file_opener)
    {
        getline(file_opener,temp);
        exemplars_.push_back(temp);
    }

    //Delete last element
    exemplars_.pop_back();
       
    //Close file
    file_opener.close();
    
    // Read in category names
    // Rebind stream
    file_opener.open(category_name_file.c_str());

    while(file_opener)
    {
        getline(file_opener,temp);
        categories_.push_back(temp);
    }

    // Delete last element
    categories_.pop_back();

    // Close file
    file_opener.close();

    // Rebind stream to data matrix
    file_opener.open(dataset_file.c_str());
    
    // Read in data_matrix
    // Get number of shapes
    number_of_nodes_ = exemplars_.size();
   
    // Set matrix size to number of shapes
    exemplars_sim_matrix_.set_size(number_of_nodes_,number_of_nodes_);


    // Read in data matrix
    for ( unsigned int i=0; i < number_of_nodes_ ; i++)
    {
        for ( unsigned int j=0; j < number_of_nodes_ ; j++)
        {
            file_opener>>exemplars_sim_matrix_[i][j];
           
            
        }
    }

    file_opener.close();
}

void dborl_proximity_graph::preprocess_graph()
{
    //Regardless of graph add in all nodes

    //For each vertex grab property map to get names and vertex index
    property_map<Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
    property_map<Undirected_Graph, vertex_index2_t>::type
        abs_index = get(vertex_index2, proximity_graph_);
    property_map<Undirected_Graph, vertex_cat_t>::type
        cat_name = get(vertex_cat_t(), proximity_graph_);

    // Add in each vertex
    for ( unsigned int i=0; i < number_of_nodes_; i++)
    {
     
        vertex_objects_.push_back(add_vertex(proximity_graph_));
        node_name[vertex_objects_[i]]=exemplars_[i];
        cat_name[vertex_objects_[i]]=categories_[i];
        abs_index[vertex_objects_[i]]=i;
    }


}

void dborl_proximity_graph::build_beta_graph()
{

    vcl_cout<<"Creating Proximity Graph with beta: "<< beta_ <<vcl_endl;

    // Build gabriel graph brute force computation O(N^3)

    // Creat edge objects
    property_map<Undirected_Graph, edge_weight_t>::type
        edge_distance = get(edge_weight, proximity_graph_);    

    bool flag=true;
    for ( unsigned int i=0; i < number_of_nodes_ ; i++)
    {
        for (  unsigned int j=(i+1); j < number_of_nodes_ ; j++)
        {
            // Reset flag
            flag=true;

            // Get distance between two shapes
            double dij = exemplars_sim_matrix_[i][j];
            for ( unsigned int k=0; k < number_of_nodes_ ; k++)
            {
                // Make sure we are not comparing against the two shapes
                if ( k != i && k != j )
                {
                    double dik = exemplars_sim_matrix_[i][k];
                    double djk = exemplars_sim_matrix_[j][k];

                    if ( beta_ < 1)
                    {
                        // Beta_ less than 1 case
                        if (  (dij*dij) 
                            > ((dik*dik) + 
                               (djk*djk) +
                               2*vcl_sqrt(1-(beta_*beta_))*
                               (dik*djk)  ))
                        {
                            flag=false;
                            break;
                        }
                    }
                    else
                    {
                        // Beta_ greater than 1 case
                        if ( dij*dij > vcl_max(
                                 ((dik*dik)*((2/beta_)-1))+(djk*djk)
                                 ,(dik*dik)+(((2/beta_)-1)*(djk*djk)) ))
                        {

                            flag=false;
                            break;
                
                        }

                    }

                }

            }

            if(flag)
            {
                //Create edge object
                graph_traits<Undirected_Graph>::edge_descriptor graph_edge;
           
                // Returns a pair gets the first element to update
                // property map
                graph_edge =      add_edge(vertex_objects_[i],
                                           vertex_objects_[j],
                                           proximity_graph_).first;
                edge_distance[graph_edge]=dij;
            }
        }


    }
    
    // Print out minimal information
    vcl_cout << "Number of Edges: "<< num_edges(proximity_graph_)<<vcl_endl;
    vcl_cout << "Number of Nodes: "<< num_vertices(proximity_graph_)<<vcl_endl;

}

void dborl_proximity_graph::wilson_edit()
{
    
    // property maps to get values of noes
    property_map<Undirected_Graph, vertex_cat_t>::type
        cat_name = get(vertex_cat_t(), proximity_graph_);
    property_map<Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
   property_map<Undirected_Graph, edge_weight_t>::type
        edge_distance = get(edge_weight, proximity_graph_);
 
    // Define a vector of vertex of objects to delete
    vcl_vector<vcl_string> vertex_to_delete;

    // Iterate through graph
    typedef graph_traits<Undirected_Graph>::vertex_iterator vertex_iter;
    vcl_pair<vertex_iter, vertex_iter> vp;

    // Loop thru all nodes
    for (vp = vertices(proximity_graph_); vp.first != vp.second; ++vp.first)
    {
        
        // neighbors of edge in question
        vcl_map<vcl_string,vcl_vector<double> > neighbors;

        //Look through all adjacent vertices of the graph
        graph_traits<dborl_proximity_graph::Undirected_Graph>
            ::adjacency_iterator ai;
        graph_traits<dborl_proximity_graph::Undirected_Graph>
            ::adjacency_iterator ai_end;

        Vertex incoming_node = *vp.first;
        
        for (tie(ai, ai_end) = 
                 adjacent_vertices(incoming_node, 
                                   proximity_graph_);
             ai != ai_end; ++ai)
        {
            neighbors[cat_name[*ai]].push_back(
                edge_distance[edge(incoming_node,*ai,proximity_graph_)
                              .first]);

        }

        //create iterator
        unsigned int count_neighbors(0);
        vcl_string neighbor_index;
        double mean(DBL_MAX);

        vcl_map<vcl_string,vcl_vector<double> >::iterator it;
        for (it = neighbors.begin() ; it != neighbors.end() ; it++)
        {
        
            unsigned int temp = (it->second).size();
            double temp_mean = vcl_accumulate((it->second).begin(),
                                          (it->second).end(),
                                          0.0f)/(it->second).size();

            if ( temp > count_neighbors)
            {
                
                count_neighbors=temp;
                mean=temp_mean;
                neighbor_index=it->first;
                
            } 
            else if ( temp == count_neighbors )
            {
                if ( temp_mean < mean )
                {

                    mean=temp_mean;
                    neighbor_index=it->first;
                }


            }
        }

        // Compare categories
        // misclassified
        if ( !(neighbor_index == cat_name[incoming_node]) )
        {
            vertex_to_delete.push_back(node_name[incoming_node]);
        }

    }

 
    // Delete flagged nodes
    for (unsigned int d(0) ; d < vertex_to_delete.size() ; d++)
    {
        delete_node(vertex_to_delete[d]);
    }


    // Print out minimal information
    vcl_cout << "Wilson Editing "<< vcl_endl;
    vcl_cout << "Number of Edges: "<< num_edges(proximity_graph_)<<vcl_endl;
    vcl_cout << "Number of Nodes: "<< num_vertices(proximity_graph_)<<vcl_endl;
    
}

 
void dborl_proximity_graph::thinning()
{
    // property maps to get values of noes
    property_map<Undirected_Graph, vertex_cat_t>::type
        node_category = get(vertex_cat_t(), proximity_graph_);
    property_map<Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);

    // Define a vector of vertex of objects to delete
    vcl_vector<vcl_string> vertex_to_delete;

    // Iterate through graph
    typedef graph_traits<Undirected_Graph>::vertex_iterator vertex_iter;
    vcl_pair<vertex_iter, vertex_iter> vp;

    // Loop thru all nodes
    for (vp = vertices(proximity_graph_); vp.first != vp.second; ++vp.first)
    {
        
        // neighbors of edge in question
        vcl_map<vcl_string,unsigned int> neighbors;

        //Look through all adjacent vertices of the graph
        graph_traits<dborl_proximity_graph::Undirected_Graph>
            ::adjacency_iterator ai;
        graph_traits<dborl_proximity_graph::Undirected_Graph>
            ::adjacency_iterator ai_end;

        Vertex incoming_node = *vp.first;

        for (tie(ai, ai_end) = 
                 adjacent_vertices(incoming_node, 
                                   proximity_graph_);
             ai != ai_end; ++ai)
        {
            neighbors[node_category[*ai]]=0;
        }


        // Remove redundant data
        if ( neighbors.size() == 0)
        {
            vertex_to_delete.push_back(node_name[incoming_node]);
        }


    }

 
    // Delete flagged nodes
    for (unsigned int d(0) ; d < vertex_to_delete.size() ; d++)
    {
        delete_node(vertex_to_delete[d]);
    }


    // Print out minimal information
    vcl_cout << "Proximity Graph Thinning "<< vcl_endl;
    vcl_cout << "Number of Edges: "<< num_edges(proximity_graph_)<<vcl_endl;
    vcl_cout << "Number of Nodes: "<< num_vertices(proximity_graph_)<<vcl_endl;


}

void dborl_proximity_graph::write_graph(vcl_string output_graph)
{

    // Write out graph
    dynamic_properties dp;
    dp.property("name",get(vertex_name_t(),proximity_graph_));
    dp.property("distance",get(edge_weight_t(),proximity_graph_));
    dp.property("category",get(vertex_cat_t(),proximity_graph_));
    dp.property("abs_index",get(vertex_index2_t(),proximity_graph_));

    vcl_ofstream ofile(output_graph.c_str());
    write_graphml(ofile, proximity_graph_, dp);
    ofile.close();



}

void dborl_proximity_graph::delete_node(vcl_string node_to_delete)
{
    //For each vertex grab property map to get names and vertex index
    property_map<Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
    // Print out node information 
    typedef graph_traits<Undirected_Graph>::vertex_iterator vertex_iter;
    vcl_pair<vertex_iter, vertex_iter> vp;

    for (vp = vertices(proximity_graph_); vp.first != vp.second; ++vp.first)
    {
        if ( node_to_delete == node_name[*vp.first] )
        {

            break;
        }
    }

    // clear out edges of vertex
    clear_vertex(*vp.first,proximity_graph_);

    // Remove vertex
    remove_vertex(*vp.first,proximity_graph_);

}

void dborl_proximity_graph::print_graph()
{

    //For each vertex grab property map to get names and vertex index
    property_map<Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
    property_map<Undirected_Graph, vertex_index_t>::type
        node_index = get(vertex_index, proximity_graph_);
    property_map<Undirected_Graph, vertex_cat_t>::type
        node_category = get(vertex_cat_t(), proximity_graph_);
    property_map<Undirected_Graph, edge_weight_t>::type
        edge_distance = get(edge_weight, proximity_graph_);
    property_map<Undirected_Graph, vertex_index2_t>::type
        abs_index = get(vertex_index2, proximity_graph_);
 
    // Print out node information 
    typedef graph_traits<Undirected_Graph>::vertex_iterator vertex_iter;
    vcl_pair<vertex_iter, vertex_iter> vp;

    vcl_cout << "Number of Nodes: "<< num_vertices(proximity_graph_)<<vcl_endl;
    for (vp = vertices(proximity_graph_); vp.first != vp.second; ++vp.first)
    {
        vcl_cout << "Node Name: "<< node_name[*vp.first] << "  Node Index: " 
                 << node_index[*vp.first]<<" abs index: "<< 
                  abs_index[*vp.first]<<vcl_endl << " Node Category: "
                 << node_category[*vp.first] << vcl_endl;
    }
   
    // Print out edge information
    vcl_cout << " Number of Edges: "<< num_edges(proximity_graph_)<<vcl_endl;

    typedef graph_traits<Undirected_Graph>::edge_iterator edge_iter;
    vcl_pair<edge_iter, edge_iter> ep;
    edge_iter ei, ei_end;

    // Create an adjancency matrix
    vnl_matrix<double> adjancency_matrix(number_of_nodes_, 
                                         number_of_nodes_,
                                         0.0);
    // Create an adjancency matrix
    vnl_matrix<double> dist_matrix(number_of_nodes_, 
                                   number_of_nodes_,
                                   0.0);

    for (tie(ei, ei_end) = edges(proximity_graph_); ei != ei_end; ++ei)
    {

        vcl_cout << edge_distance[*ei] << vcl_endl;
        vcl_cout << "(" << source(*ei, proximity_graph_) 
                 << "," << target(*ei, proximity_graph_) << ")" << vcl_endl;

        adjancency_matrix(source(*ei, proximity_graph_),
                          target(*ei, proximity_graph_))=1;

        adjancency_matrix(target(*ei, proximity_graph_),
                          source(*ei, proximity_graph_))=1;

        dist_matrix(source(*ei, proximity_graph_),
                    target(*ei, proximity_graph_))=edge_distance[*ei];

        dist_matrix(target(*ei, proximity_graph_),
                    source(*ei, proximity_graph_))=edge_distance[*ei];

    }

    vcl_cout<<vcl_endl;
    vcl_cout<<"Matrix form of graph adjancency"<<vcl_endl;

    // print matrix form
    // write out matrix form
    for ( unsigned int k=0; k < number_of_nodes_ ; k++)
    {
        if ( k==0 )
        {
            vcl_cout<<"graph=[";
        }

        for ( unsigned int m=0; m < number_of_nodes_ ; m++ )
        {

            vcl_cout<<adjancency_matrix(k,m)<<" ";
        }
        if ( k == number_of_nodes_-1 )
        {
            vcl_cout<<"];"<<vcl_endl;
        }
        else
        {
            vcl_cout<<"; ..."<<vcl_endl;
        }
    }

    vcl_cout<<vcl_endl;
    vcl_cout<<"Matrix form of graph distances"<<vcl_endl;

    // print matrix form
    // write out matrix form
    for ( unsigned int k=0; k < number_of_nodes_ ; k++)
    {
        if ( k==0 )
        {
            vcl_cout<<"graph_dist=[";
        }

        for ( unsigned int m=0; m < number_of_nodes_ ; m++ )
        {

            vcl_cout<<dist_matrix(k,m)<<" ";
        }
        if ( k == number_of_nodes_-1 )
        {
            vcl_cout<<"];"<<vcl_endl;
        }
        else
        {
            vcl_cout<<"; ..."<<vcl_endl;
        }
    }

    vcl_cout<<vcl_endl;
}
