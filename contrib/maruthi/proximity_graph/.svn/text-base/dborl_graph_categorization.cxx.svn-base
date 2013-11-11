#include <proximity_graph/dborl_graph_categorization.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_numeric.h>
#include <vcl_map.h>
#include <vcl_cfloat.h>
#include <vnl/vnl_vector.h>
#include <boost/graph/graphml.hpp>

void dborl_graph_categorization::graph_categorize(
    vcl_string xml_graph_file,
    vcl_string exemplar_dataset_file,
    vcl_string query_dataset_file,
    vcl_string query_label_file,
    vcl_string stats_file,
    double beta)
{

   
    // read in files
    read_files(exemplar_dataset_file,
               query_dataset_file,
               query_label_file );

    // open stats file
    vcl_ofstream stats_opener(stats_file.c_str(),std::ios::app);

    // Loop over query matrix
    unsigned int num_rows = query_sim_matrix_.rows();

    for ( unsigned int rows(0) ; rows < num_rows ; ++rows )
    {

        // Clear out everything before we start
        proximity_graph_.clear();

        // clear out everthing in this vector
        vertex_objects_.clear();

        // Reread the graph in
        read_graph(xml_graph_file,queries_[rows]);

        // Grab query row
        vnl_vector<double> temp_row = query_sim_matrix_.get_row(rows);

        // Create total matrix first
        // Append a zero to query_row
        vnl_vector<double> query_row(temp_row.size()+1,0);

        //Update query_row
        query_row.update(temp_row);

        // Create total matrix
        vnl_matrix<double> total_matrix(exemplar_sim_matrix_.rows()+1,
                                        exemplar_sim_matrix_.rows()+1,
                                        0);

        // Update total matrix with exemplar matrix
        total_matrix.update(exemplar_sim_matrix_,0,0);

        // Update row and column
        total_matrix.set_row(total_matrix.rows()-1,query_row);
        total_matrix.set_column(total_matrix.cols()-1,query_row);

        // rebuild graph
        rebuild_graph(total_matrix,beta);

        // do the categorization
        vcl_string category=perform_categorization();

        // write out stats
        stats_opener<<"query: "
                    << queries_[rows]
                    <<" category: "
                    << category
                    << vcl_endl;

    }

    stats_opener.close();
    
 
}

void dborl_graph_categorization::read_files( vcl_string exemplar_dataset_file, 
                                             vcl_string query_dataset_file,
                                             vcl_string query_label_file)
{
    // Create a file stream opener
    vcl_ifstream file_opener;
    vcl_string temp;

    // get number of nodes
    file_opener.open(exemplar_dataset_file.c_str());
    
    number_of_nodes_=0;
    vcl_string line;
    while(file_opener)
    {

        getline(file_opener,line);
        number_of_nodes_++;
    }
    number_of_nodes_--;

    file_opener.close();
    
    // -------------------- Read in query labels -----------------------------
 
    // Open the file
    file_opener.open(query_label_file.c_str());

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
        
    // -------------------- Read in query data matrix ------------------------

    // Rebind stream to data matrix
    file_opener.open(query_dataset_file.c_str());
    
    // Set matrix size to number of shapes
    query_sim_matrix_.set_size(queries_.size(),number_of_nodes_);

    // Read in data matrix
    for ( unsigned int i=0; i < queries_.size() ; i++)
    {
        for ( unsigned int j=0; j < number_of_nodes_ ; j++)
        {
            file_opener>>query_sim_matrix_[i][j];
        }
    }

    file_opener.close();

    // -------------------- Read in exemp data matrix ------------------------

    // Rebind stream to data matrix
    file_opener.open(exemplar_dataset_file.c_str());
    
    // Set matrix size to number of shapes
    exemplar_sim_matrix_.set_size(number_of_nodes_,number_of_nodes_);

    // Read in data matrix
    for ( unsigned int i=0; i < number_of_nodes_ ; i++)
    {
        for ( unsigned int j=0; j < number_of_nodes_ ; j++)
        {
            file_opener>>exemplar_sim_matrix_[i][j];
        }
    }

    file_opener.close();



    
}

void dborl_graph_categorization::read_graph(vcl_string xml_graph_file,
                                            vcl_string query_name)
{

    // -------------------- Read in graph file -----------------------------
 

    //Read graph file first
    dynamic_properties dp;
    dp.property("name",get(vertex_name_t(),proximity_graph_));
    dp.property("distance",get(edge_weight_t(),proximity_graph_));
    dp.property("abs_index",get(vertex_index2_t(),proximity_graph_));
    dp.property("category",get(dborl_proximity_graph::vertex_cat_t(),
                               proximity_graph_));

    vcl_ifstream ifile(xml_graph_file.c_str());
    read_graphml(ifile,proximity_graph_,dp);
    ifile.close();

    typedef graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::vertex_iterator vertex_iter;
    vcl_pair<vertex_iter, vertex_iter> vp;

    // Loop thru all nodes
    for (vp = vertices(proximity_graph_); vp.first != vp.second; ++vp.first)
    {
        clear_vertex(*vp.first,proximity_graph_);
        vertex_objects_.push_back(*vp.first);

    }

    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_index2_t>::type
        abs_index = get(vertex_index2, proximity_graph_);
  
    // Add a vertex
    dborl_proximity_graph::Vertex query;
    query=add_vertex(proximity_graph_);
    node_name[query]=query_name;
    abs_index[query]=number_of_nodes_;


    // push back vertex object
    vertex_objects_.push_back(query);

}

void dborl_graph_categorization::rebuild_graph(
    vnl_matrix<double>& total_matrix,
    double beta)
{

    
    //For each vertex grab property map to get names and vertex index
    property_map<dborl_proximity_graph::Undirected_Graph, edge_weight_t>::type
        edge_distance = get(edge_weight, proximity_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_index2_t>::type
        abs_index = get(vertex_index2, proximity_graph_);
                                                                         
    bool flag=true;
    for ( unsigned int i = 0 ; i < vertex_objects_.size() ; ++i)
    {
        for ( unsigned int j = (i+1) ; j < vertex_objects_.size() ; ++j)
        {

            // Reset flag
            flag=true;

            // Get distance between two shapes
            double dij  = total_matrix[abs_index[vertex_objects_[i]]]
                [abs_index[vertex_objects_[j]]];

            for ( unsigned int k = 0 ; k < vertex_objects_.size() ; ++k)
            {

                if ( k != i && k != j )
                {
                    double dik  = total_matrix[abs_index[vertex_objects_[i]]]
                        [abs_index[vertex_objects_[k]]];
                    double djk  = total_matrix[abs_index[vertex_objects_[j]]]
                        [abs_index[vertex_objects_[k]]];

                    if ( beta < 1)
                    {
                        // Beta_ less than 1 case
                        if (  (dij*dij) 
                              > ((dik*dik) + 
                                 (djk*djk) +
                                 2*vcl_sqrt(1-(beta*beta))*
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
                                 ((dik*dik)*((2/beta)-1))+(djk*djk)
                                 ,(dik*dik)+(((2/beta)-1)*(djk*djk)) ))
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
                graph_traits<dborl_proximity_graph::Undirected_Graph>
                    ::edge_descriptor graph_edge;
           
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
    vcl_cout << "After Embedding  "<< vcl_endl;
    vcl_cout << "Number of Edges: "<< num_edges(proximity_graph_)<<vcl_endl;
    vcl_cout << "Number of Nodes: "<< num_vertices(proximity_graph_)<<vcl_endl;
    vcl_cout <<vcl_endl;


}

vcl_string dborl_graph_categorization::perform_categorization()
{

    //Look through all adjacent vertices of the graph
    graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::adjacency_iterator ai;
    graph_traits<dborl_proximity_graph::Undirected_Graph>
        ::adjacency_iterator ai_end;

    property_map<dborl_proximity_graph::Undirected_Graph, edge_weight_t>::type
        edge_distance = get(edge_weight, proximity_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, vertex_name_t>::type
        node_name = get(vertex_name, proximity_graph_);
    property_map<dborl_proximity_graph::Undirected_Graph, 
        dborl_proximity_graph::vertex_cat_t>::type
        cat_name = get(dborl_proximity_graph::vertex_cat_t(), proximity_graph_);

    // neighbors of edge in question
    vcl_map<vcl_string,vcl_vector<double> > neighbors;
       
    for (tie(ai, ai_end) = 
             adjacent_vertices(vertex_objects_.back(), 
                               proximity_graph_);
         ai != ai_end; ++ai)
    {
        neighbors[cat_name[*ai]].push_back(
            edge_distance[edge(vertex_objects_.back(),*ai,proximity_graph_)
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
        
    return neighbor_index;


}




