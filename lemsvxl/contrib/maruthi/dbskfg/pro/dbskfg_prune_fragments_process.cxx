// This is brcv/shp/dbskfg/pro/dbskfg_prune_fragments_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_prune_fragments_process.h>
#include <dbskfg/dbskfg_rtree_poly_box_2d.h>

// vidpro1 headers
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

// vul headers
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>

// vil headers
#include <vil/vil_image_list.h>
#include <vil/vil_image_resource.h>

#include <vcl_cstdlib.h>

#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_matrix.h>

// vgl headers
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>
#include <vgl/algo/vgl_rtree.h>

#include <vul/vul_timer.h>
#include <vcl_fstream.h>
#include <dbskfg/algo/fastcluster.cpp>

//: Constructor
dbskfg_prune_fragments_process::dbskfg_prune_fragments_process()
{
    if (
        !parameters()->add( "Input Binary File:" , 
                            "-input_binary_file", 
                            bpro1_filepath("", "*.bin")) ||
        !parameters()->add( "Output folder:" , 
                            "-output_folder", bpro1_filepath("", "")) ||
        !parameters()->add( "Output file prefix:" , 
                            "-output_prefix", vcl_string("")) ||
        !parameters()->add( "Threshold:" , 
                            "-threshold", (double) 0.4) ||
        !parameters()->add( "Scale:", 
                            "-scale", (double) 2.0)

        )

    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_prune_fragments_process::~dbskfg_prune_fragments_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_prune_fragments_process::clone() const
{
    return new dbskfg_prune_fragments_process(*this);
}

vcl_string
dbskfg_prune_fragments_process::name()
{
    return "Prune Fragments";
}

vcl_vector< vcl_string >
dbskfg_prune_fragments_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    return to_return;
    
}

vcl_vector< vcl_string >
dbskfg_prune_fragments_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
}

int dbskfg_prune_fragments_process::input_frames()
{
    return 1;
}

int dbskfg_prune_fragments_process::output_frames()
{
    return 1;
}

bool dbskfg_prune_fragments_process::execute()
{

 
    bpro1_filepath input;
    this->parameters()->get_value("-input_binary_file", input);
    vcl_string input_file_path = input.path;

    bpro1_filepath output_folder_filepath;
    this->parameters()->get_value("-output_folder", output_folder_filepath);
    vcl_string output_folder = output_folder_filepath.path;

    vcl_string output_prefix;
    parameters()->get_value( "-output_prefix", output_prefix);

    double threshold=0.0;
    parameters()->get_value( "-threshold",threshold);

    double scale=0.0;
    parameters()->get_value( "-scale",scale);

    // make sure that input_file_path is sane
    if (input_file_path == "") 
    { return false; }


    // Create names
    vcl_string output_file = output_folder+"/"+
            output_prefix+"_pruned_fragments.bin";
    vcl_string output_dendrogram_complete = output_folder+"/"+
            output_prefix+"_dendrogram_complete.bin";
    vcl_string output_dendrogram_average = output_folder+"/"+
            output_prefix+"_dendrogram_average.bin";

    // Let time how long this takes
    // Start timer
    vul_timer t;

    vcl_ifstream input_binary_file(input_file_path.c_str(),vcl_ios::in|
                                   vcl_ios::binary);



    // Read in image coordinates
    double image_size[2];
    input_binary_file.read(reinterpret_cast<char*>(
                               image_size),
                           sizeof(double)*2);

    polygons_.clear();

    while(input_binary_file.good()) 
    {
        double polygon_size[1];
        input_binary_file.read(reinterpret_cast<char*>(
                                   polygon_size),
                               sizeof(double));
        if ( input_binary_file.eof())
        {
            break;
        }

        vgl_bbox_2d<double> box;
        vcl_vector<vgl_point_2d<double> > coords(polygon_size[0]);
        for ( unsigned int i=0; i < coords.size() ; ++i)
        {
            double coordinates[2];
            input_binary_file.read(reinterpret_cast<char*>(
                                       coordinates),
                                   sizeof(double)*2);

            coords[i].set(coordinates[0]*scale,coordinates[1]*scale);
            box.add(coords[i]);
        }
        
        vgl_polygon<double> poly(coords);
        polygons_.push_back(poly);
       
       
    }
    vcl_cout<<"Read in "<<polygons_.size()
            <<" fragments and resized with factor "<< scale <<vcl_endl;
    input_binary_file.close();

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time to read file: "<<vox_time<<" sec"<<vcl_endl;

    
    // Let time how long this takes
    // Start timer
    vul_timer t2;
    linear_scan(output_file,threshold);

    vox_time = t2.real()/1000.0;
    t2.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time linear scan: "<<vox_time<<" sec"<<vcl_endl;

    clear_output();
    polygons_.clear();

    return true;
}

bool dbskfg_prune_fragments_process::finish()
{
    return true;
}

void dbskfg_prune_fragments_process::linear_scan(
    vcl_string output_distance_matrix,double threshold)
{
    unsigned int new_frags=0;
    if (prototype_list_.size() == 0 )
    {
        prototype_list_.push_back(polygons_.back());
        polygons_.pop_back();
        new_frags++;
    }


    for ( unsigned int p=0; p < polygons_.size() ; ++p)
    {
        vgl_polygon<double> new_proto= polygons_[p];
        double min_distance=1.0e6;
        vcl_map<vcl_pair<unsigned int,unsigned int>, double> local_map;

        for ( unsigned int i=0; i < prototype_list_.size() ; ++i)
        {

            int value;

            vgl_polygon<double> intersect_poly = vgl_clip(
                new_proto,               // p1
                prototype_list_[i],       // p2
                vgl_clip_type_intersect, // p1 U p2
                &value);                 // test if success
            vcl_pair<unsigned int,unsigned int> key
                    (i,prototype_list_.size());

            if ( intersect_poly.num_sheets() )
            {
                vgl_polygon<double> union_poly = vgl_clip(
                    new_proto,              // p1
                    prototype_list_[i],      // p2
                    vgl_clip_type_union,    // p1 U p2
                    &value);                // test if success


                double union_poly_area = vgl_area(union_poly);
                double intersect_poly_area = vgl_area(intersect_poly);
                double jaccard_index = 1-intersect_poly_area/union_poly_area;
              
                local_map[key]=jaccard_index;

                if ( jaccard_index < min_distance )
                {
                    min_distance=jaccard_index;
                }

            }
            else
            {
                local_map[key]=1.0;
                if ( 1.0 < min_distance )
                {
                    min_distance=1.0;
                }

            }

            
        }

        if ( min_distance >= threshold )
        {
            prototype_list_.push_back(new_proto);
            new_frags++;
            vcl_map< vcl_pair<unsigned int,unsigned int>, double>::iterator it;
            for ( it = local_map.begin(); it != local_map.end() ; ++it)
            {
                distance_matrix_[(*it).first]=(*it).second;
            }
        }
        

    }
    vcl_cout<<"Protos added in this run: "<<new_frags<<vcl_endl;
    vcl_cout<<"Total size of prototype list: "<<
        prototype_list_.size()<<vcl_endl;

}

void dbskfg_prune_fragments_process::brute_force_computation(
    vcl_string output_distance_matrix,
    vcl_string output_dendrogram_complete,
    vcl_string output_dendrogram_average)
{

    unsigned int size=((polygons_.size())*(polygons_.size()-1))/2;
    double* compressed_distance_matrix=new double[size];
    double* orig_distance_matrix=new double[size];

    unsigned int index=0;

    // Let time how long this takes
    // Start timer
    vul_timer t2;

    
    // Create distance matrix
    for ( unsigned int i=0; i < polygons_.size() ; ++i)
    {
        for ( unsigned int j=i+1; j < polygons_.size() ; ++j)
        {

            // Compute Intersection and Union for jacard index
            
            //Take union of two polygons_
            int value;

            vgl_polygon<double> intersect_poly = vgl_clip(
                polygons_[i],             // p1
                polygons_[j],             // p2
                vgl_clip_type_intersect, // p1 U p2
                &value);                 // test if success

            if ( intersect_poly.num_sheets() )
            {
                vgl_polygon<double> union_poly = vgl_clip(
                    polygons_[i],            // p1
                    polygons_[j],            // p2
                    vgl_clip_type_union,    // p1 U p2
                    &value);                // test if success


                double union_poly_area = vgl_area(union_poly);
                double intersect_poly_area = vgl_area(intersect_poly);

                double jacard_index = intersect_poly_area/union_poly_area;

                compressed_distance_matrix[index]=1.0-jacard_index;      
                orig_distance_matrix[index]=1.0-jacard_index;      
            }
            else
            {
                compressed_distance_matrix[index]=1.0;
                orig_distance_matrix[index]=1.0;
            }
            index++;
        }
       
    }

    double vox_time = t2.real()/1000.0;
    t2.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time to compute distance matrix: "
            <<vox_time<<" sec"<<vcl_endl;
  
    // Let time how long this takes
    // Start timer
    vul_timer t3;

    write_distance_matrix(static_cast<double>(size),
                          orig_distance_matrix,
                          output_distance_matrix);

    double* cluster_results_complete= new double[(polygons_.size()-1)*4];
    cluster_data_complete(polygons_.size(),compressed_distance_matrix,
                          cluster_results_complete);

    double* cluster_results_average=new double[(polygons_.size()-1)*4];
    cluster_data_average(polygons_.size(),orig_distance_matrix,
                         cluster_results_average);

    vox_time = t3.real()/1000.0;
    t3.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time to cluster data: "
            <<vox_time<<" sec"<<vcl_endl;


    write_dendrogram((polygons_.size()-1)*4,
                     cluster_results_complete,
                     output_dendrogram_complete);

    write_dendrogram((polygons_.size()-1)*4,
                     cluster_results_average,
                     output_dendrogram_average);

    delete [] cluster_results_average;
    delete [] cluster_results_complete;
    delete [] orig_distance_matrix;
    delete [] compressed_distance_matrix;



}

void dbskfg_prune_fragments_process::cluster_data_complete(
    unsigned int N,
    double* D,
    double* Z)
{
    cluster_result Z2(N-1);
    NN_chain_core<METHOD_METR_COMPLETE, t_index>(N, D, NULL, Z2);

    union_find nodes;
    std::stable_sort(Z2[0], Z2[N-1]);
    nodes.init(N);
   
    
    linkage_output output(Z);
    t_index node1, node2;

    for (t_index i=0; i<N-1; i++) 
    {
       
        // Find the cluster identifiers for these points.
        node1 = nodes.Find(Z2[i]->node1);
        node2 = nodes.Find(Z2[i]->node2);
        // Merge the nodes in the union-find data structure by making them
        // children of a new node.
        nodes.Union(node1, node2);
        
        output.append(node1, node2, Z2[i]->dist, size_(node1)+size_(node2));
    }
}


void dbskfg_prune_fragments_process::cluster_data_average(
    unsigned int N,
    double* D,
    double* Z)
{
    cluster_result Z2(N-1);
    auto_array_ptr<t_index> members;
    members.init(N, 1);

    NN_chain_core<METHOD_METR_AVERAGE, t_index>(N, D, members, Z2);

    union_find nodes;
    std::stable_sort(Z2[0], Z2[N-1]);
    nodes.init(N);
   
    
    linkage_output output(Z);
    t_index node1, node2;

    for (t_index i=0; i<N-1; i++) 
    {
       
        // Find the cluster identifiers for these points.
        node1 = nodes.Find(Z2[i]->node1);
        node2 = nodes.Find(Z2[i]->node2);
        // Merge the nodes in the union-find data structure by making them
        // children of a new node.
        nodes.Union(node1, node2);
        
        output.append(node1, node2, Z2[i]->dist, size_(node1)+size_(node2));
    }
}

void dbskfg_prune_fragments_process::write_distance_matrix(
    double size,
    double* compressed_distance_matrix,
    vcl_string output_file_path)
{
    vcl_ofstream output_binary_file;
    output_binary_file.open(output_file_path.c_str(),
                            vcl_ios::out |
                            vcl_ios::app |
                            vcl_ios::binary);

    output_binary_file.write(reinterpret_cast<char *>(&size),
                             sizeof(double));
    
    output_binary_file.write(
        reinterpret_cast<char *>(compressed_distance_matrix),
        sizeof(double)*size);

    output_binary_file.close();


}

void dbskfg_prune_fragments_process::write_dendrogram(
    double size,
    double* dendrogram,
    vcl_string output_file_path)
{
    vcl_ofstream output_binary_file;
    output_binary_file.open(output_file_path.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app |
                            vcl_ios::binary);

    output_binary_file.write(reinterpret_cast<char *>(&size),
                             sizeof(double));
    
    output_binary_file.write(
        reinterpret_cast<char *>(dendrogram),
        sizeof(double)*size);

    output_binary_file.close();
}

void dbskfg_prune_fragments_process::write_out_data(
    vcl_string binary_file_output)
{

    vcl_cout<<"Writing out "<<prototype_list_.size()<<" to "
            <<binary_file_output<<vcl_endl;

    vcl_ofstream output_binary_file;
    output_binary_file.open(binary_file_output.c_str(),
                            vcl_ios::out | 
                            vcl_ios::app | 
                            vcl_ios::binary);

    double size_x=0;
    double size_y=0;
    output_binary_file.write(reinterpret_cast<char *>(&size_x),
                              sizeof(double));
    output_binary_file.write(reinterpret_cast<char *>(&size_y),
                              sizeof(double));

    for ( unsigned int i=0; i < prototype_list_.size() ; ++i)
    {
        vgl_polygon<double> poly=prototype_list_[i];

        double num_vertices= poly[0].size();
        output_binary_file.write(reinterpret_cast<char *>(&num_vertices),
                                 sizeof(double));

        for (unsigned int p = 0; p < poly[0].size(); ++p)
        {
            double xcoord = poly[0][p].x();
            double ycoord = poly[0][p].y();

            output_binary_file.write(reinterpret_cast<char *>(&xcoord),
                                     sizeof(double));
            output_binary_file.write(reinterpret_cast<char *>(&ycoord),
                                     sizeof(double));
        
    
        }

    }
    
    output_binary_file.close();


    // write out distance matrix
    vcl_string output_distance_matrix=vul_file::strip_extension(
        binary_file_output);
    output_distance_matrix=output_distance_matrix+"_distance_mat.txt";

    vcl_ofstream output_txt_file;
    output_txt_file.open(output_distance_matrix.c_str(),
                         vcl_ios::out | 
                         vcl_ios::app );

    output_txt_file<<prototype_list_.size()<<vcl_endl;
    vcl_map< vcl_pair<unsigned int,unsigned int>, double>::iterator it;
    for ( it = distance_matrix_.begin(); it != distance_matrix_.end() ; ++it)
    {
        output_txt_file<<(*it).first.first
                        <<","
                        <<(*it).first.second
                        <<","
                        <<(*it).second
                        <<vcl_endl;


    }
    output_txt_file.close();
}
