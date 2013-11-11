#include <proximity_graph/dborl_knn_graph.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_map.h>
#include <boost/graph/graphml.hpp>
#include <vcl_fstream.h>

void dborl_knn_graph::construct_graph(
    vcl_string exemplar_dataset_file, 
    vcl_string exemplar_label_file, 
    unsigned int neighbors,
    vcl_string output_file_name,
    bool verbose)
{
    //Read files first
    read_files(exemplar_dataset_file,exemplar_label_file);

    //Start construcing graph
    build_knn_graph(neighbors);

    //Write out graph
    write_graph(output_file_name);

    // Print out graph
    if(verbose)
    {
        print_graph();
    }

}

void dborl_knn_graph::read_files(vcl_string dataset_file, 
                                       vcl_string node_name_files)
{

    // Open the file
    vcl_ifstream file_opener;
    file_opener.open(node_name_files.c_str());

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
    
    // Rebind stream to data matrix
    file_opener.open(dataset_file.c_str());
    
    // Read in data_matrix
    // Get number of shapes
    number_of_shapes_ = exemplars_.size();
   
    // Set matrix size to number of shapes
    exemplars_sim_matrix_.set_size(number_of_shapes_,number_of_shapes_);


    // Read in data matrix
    for ( unsigned int i=0; i < number_of_shapes_ ; i++)
    {
        for ( unsigned int j=0; j < number_of_shapes_ ; j++)
        {
            file_opener>>exemplars_sim_matrix_[i][j];
        }
    }
    file_opener.close();
}

void dborl_knn_graph::build_knn_graph(unsigned int neighbors)
{

    vcl_cout<<"Creating knn graph with "
            << neighbors 
            << " neighbors"<<vcl_endl;

    //Regardless of graph add in all nodes

    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, knn_graph_);

    // Add in each vertex
    for ( unsigned int i=0; i < number_of_shapes_; i++)
    {
     
        vertex_objects_.push_back(add_vertex(knn_graph_));
        node_name[vertex_objects_[i]]=exemplars_[i];
    }


    // Build knn graph
  
    // Creat edge objects
    property_map<dborl_proximity_graph::Undirected_Graph, edge_weight_t>::type
        edge_distance = get(edge_weight, knn_graph_);    

    for ( unsigned int i=0; i < number_of_shapes_ ; i++)
    {
        //Create a map to store names with vertex objects
        vcl_map<double,dborl_proximity_graph::Vertex> distance_map;

        for (  unsigned int j=0; j < number_of_shapes_ ; j++)
        {
            // Make sure dont add zero distance for same shape
            if ( i != j )
            {
                // Get distance between two shapes
                double dij = exemplars_sim_matrix_[i][j];
                distance_map[dij]=vertex_objects_[j];

            }
        }
        // Returns a pair gets the first element to update
        // property map
        unsigned int n=0;
        for ( vcl_map<double,dborl_proximity_graph::Vertex>::iterator 
                  it=distance_map.begin() ; it != distance_map.end(); it++ )
        {
            n++;

            if ( !edge(vertex_objects_[i],it->second,knn_graph_).second &&  
                 !edge(it->second,vertex_objects_[i],knn_graph_).second )
            {
            vcl_pair<graph_traits<
            dborl_proximity_graph::Undirected_Graph>::edge_descriptor,bool> 
                edge_pair =      add_edge(vertex_objects_[i],
                                          it->second,
                                          knn_graph_);

                edge_distance[edge_pair.first]=it->first;
            }

            if ( n == neighbors )
            {
                
                break;
            }
            
        }

    }

    vcl_cout << " Number of Nodes: "<< num_vertices(knn_graph_)<<vcl_endl;
    vcl_cout << " Number of Edges: "<< num_edges(knn_graph_)<<vcl_endl;

}

void dborl_knn_graph::write_graph(vcl_string output_file_name)
{

    // Write out graph
    dynamic_properties dp;
    dp.property("name",get(vertex_name_t(),knn_graph_));
    dp.property("distance",get(edge_weight_t(),knn_graph_));
   

    vcl_ofstream ofile(output_file_name.c_str());
    write_graphml(ofile, knn_graph_, dp);
    ofile.close();



}

void dborl_knn_graph::print_graph()
{

    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, knn_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_index_t>::type
         node_index = get(vertex_index, knn_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, edge_weight_t>::type
        edge_distance = get(edge_weight, knn_graph_);

    // Print out node information 
    typedef graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::vertex_iterator vertex_iter;
    vcl_pair<vertex_iter, vertex_iter> vp;

    vcl_cout << "Number of Nodes: "<< num_vertices(knn_graph_)<<vcl_endl;
    for (vp = vertices(knn_graph_); vp.first != vp.second; ++vp.first)
    {
        vcl_cout << "Node Name: "<< node_name[*vp.first] << "  Node Index: " 
                 << node_index[*vp.first]<<vcl_endl;
    }
   
    // Print out edge information
    vcl_cout << " Number of Edges: "<< num_edges(knn_graph_)<<vcl_endl;

    typedef graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::edge_iterator edge_iter;
    vcl_pair<edge_iter, edge_iter> ep;
    edge_iter ei, ei_end;

    // Create an adjancency matrix
    vnl_matrix<double> adjancency_matrix(number_of_shapes_, 
                                         number_of_shapes_,
                                         0.0);
    // Create an adjancency matrix
    vnl_matrix<double> dist_matrix(number_of_shapes_, 
                                   number_of_shapes_,
                                   0.0);

    for (tie(ei, ei_end) = edges(knn_graph_); ei != ei_end; ++ei)
    {

        vcl_cout << edge_distance[*ei] << vcl_endl;
        vcl_cout << "(" << source(*ei, knn_graph_) 
                 << "," << target(*ei, knn_graph_) << ")" << vcl_endl;

        adjancency_matrix(source(*ei, knn_graph_),
                          target(*ei, knn_graph_))=1;

        adjancency_matrix(target(*ei, knn_graph_),
                          source(*ei, knn_graph_))=1;

        dist_matrix(source(*ei, knn_graph_),
                    target(*ei, knn_graph_))=edge_distance[*ei];

        dist_matrix(target(*ei, knn_graph_),
                    source(*ei, knn_graph_))=edge_distance[*ei];

    }

    vcl_cout<<vcl_endl;
    vcl_cout<<"Matrix form of graph adjancency"<<vcl_endl;

    // print matrix form
    // write out matrix form
    for ( unsigned int k=0; k < number_of_shapes_ ; k++)
    {
        if ( k==0 )
        {
            vcl_cout<<"graph=[";
        }

        for ( unsigned int m=0; m < number_of_shapes_ ; m++ )
        {

            vcl_cout<<adjancency_matrix(k,m)<<" ";
        }
        if ( k == number_of_shapes_-1 )
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
    for ( unsigned int k=0; k < number_of_shapes_ ; k++)
    {
        if ( k==0 )
        {
            vcl_cout<<"graph_dist=[";
        }

        for ( unsigned int m=0; m < number_of_shapes_ ; m++ )
        {

            vcl_cout<<dist_matrix(k,m)<<" ";
        }
        if ( k == number_of_shapes_-1 )
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
