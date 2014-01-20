// This is brcv/shp/dbskfg/algo/dbskfg_match_bag_of_fragments.cxx

//:
// \file
#include <dbskfg/algo/dbskfg_match_bag_of_fragments.h>
#include <dbskfg/algo/dbskfg_cgraph_directed_tree.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/pro/dbskfg_match_composite_graph_process.h>
#include <dbskfg/pro/dbskfg_load_composite_graph_process.h>
#include <dbskfg/pro/dbskfg_load_binary_composite_graph_process.h>
#include <dbskfg/dbskfg_utilities.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vsol/vsol_box_2d.h>
#include <bxml/bxml_write.h>
#include <dbxml/dbxml_algos.h>
#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <vcl_algorithm.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <dbsk2d/dbsk2d_file_io.h>
#include <dbskr/dbskr_tree_edit.h>

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vl/imopv.h>
#include <vl/generic.h>
#include <vil/vil_transpose.h>
#include <vl/mathop.h>

#include <vgl/vgl_distance.h>
#include <string.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_vector.h>

#include <vnl/algo/vnl_symmetric_eigensystem.h>

dbskfg_match_bag_of_fragments::dbskfg_match_bag_of_fragments
( 
    vcl_string model_dir,
    vcl_string query_dir,
    vcl_string output_file,
    bool elastic_splice_cost, 
    float scurve_sample_ds, 
    float scurve_interpolate_ds, 
    bool localized_edit,
    double scurve_matching_R, 
    bool circular_ends, 
    bool combined_edit, 
    bool use_approx,
    bool scale_bbox,
    bool scale_root,
    bool app_sift,
    vil_image_resource_sptr model_image,
    vil_image_resource_sptr query_image
    ):elastic_splice_cost_(elastic_splice_cost),
      scurve_sample_ds_(scurve_sample_ds),
      scurve_interpolate_ds_(scurve_interpolate_ds),
      localized_edit_(localized_edit),
      scurve_matching_R_(scurve_matching_R),
      circular_ends_(circular_ends),
      combined_edit_(combined_edit),
      use_approx_(use_approx),
      output_match_file_(output_file),
      output_html_file_(output_file),
      output_binary_file_(output_file),
      output_removed_regions_(output_file),
      scale_bbox_(scale_bbox),
      scale_root_(scale_root),
      app_sift_(app_sift),
      model_image_(model_image),
      query_image_(query_image),
      model_grad_data_(0),
      model_grad_red_data_(0),
      model_grad_green_data_(0),
      model_grad_blue_data_(0),
      query_grad_data_(0),
      query_grad_red_data_(0),
      query_grad_green_data_(0),
      query_grad_blue_data_(0),
      model_sift_filter_(0),
      query_sift_filter_(0)
      
{

    output_match_file_ = output_match_file_ + "_similarity_matrix.xml";
    output_html_file_  = output_html_file_ +  "_similarity_matrix.html";
    output_binary_file_ = output_binary_file_ + "_binary_similarity_matrix.bin";
    output_removed_regions_ = output_removed_regions_ + "_removed_regions.txt";

    // Load model
    if ( vul_file_extension(model_dir) == ".bin")
    { 
        if ( scale_bbox_ )
        {
            dbskfg_utilities::read_binary_file(model_dir,
                                               model_contours_,
                                               model_con_ids_,
                                               model_image_size_);
        }
        else
        {
            load_binary_model(model_dir);
        }
    }
    else
    {
        load_model(model_dir);
    }

    // Load query
    if ( vul_file_extension(query_dir) == ".bin")
    { 
        if ( scale_bbox_ )
        {
            dbskfg_utilities::read_binary_file(query_dir,
                                               query_contours_,
                                               query_con_ids_,
                                               query_image_size_);
        }
        else
        {
            load_binary_query(query_dir);
        }
    }
    else
    {
        load_query(query_dir);
    }
    
    if ( scale_bbox_)
    {
        binary_sim_matrix_.set_size(model_contours_.size(),
                                    query_contours_.size());

        binary_sim_length_matrix_.set_size(model_contours_.size(),
                                           query_contours_.size());

        binary_app_sim_matrix_.set_size(model_contours_.size(),
                                        query_contours_.size());

        binary_app_norm_sim_matrix_.set_size(model_contours_.size(),
                                             query_contours_.size());

        binary_app_rgb_sim_matrix_.set_size(model_contours_.size(),
                                            query_contours_.size());

    }
    else
    {

        binary_sim_matrix_.set_size(model_fragments_.size(),
                                    query_fragments_.size());

        binary_sim_length_matrix_.set_size(model_fragments_.size(),
                                           query_fragments_.size());

        binary_app_sim_matrix_.set_size(model_fragments_.size(),
                                        query_fragments_.size());

        binary_app_norm_sim_matrix_.set_size(model_fragments_.size(),
                                             query_fragments_.size());

        binary_app_rgb_sim_matrix_.set_size(model_fragments_.size(),
                                            query_fragments_.size());


    }

    if ( app_sift_)
    {
        compute_grad_maps(model_image_,
                          &model_grad_data_,
                          &model_sift_filter_);

        compute_grad_color_maps(model_image_,
                                &model_grad_red_data_,
                                0);

        compute_grad_color_maps(model_image_,
                                &model_grad_green_data_,
                                1);

        compute_grad_color_maps(model_image_,
                                &model_grad_blue_data_,
                                2);

        compute_grad_maps(query_image_,
                          &query_grad_data_,
                          &query_sift_filter_);

        compute_grad_color_maps(query_image_,
                                &query_grad_red_data_,
                                0);

        compute_grad_color_maps(query_image_,
                                &query_grad_green_data_,
                                1);
        
        compute_grad_color_maps(query_image_,
                                &query_grad_blue_data_,
                                2);

 
        vl_sift_set_magnif(model_sift_filter_,1.0);
        vl_sift_set_magnif(query_sift_filter_,1.0);
    }
}

dbskfg_match_bag_of_fragments::~dbskfg_match_bag_of_fragments()
{

    if ( model_sift_filter_)
    {
        vl_sift_delete(model_sift_filter_);
        model_sift_filter_=0;
    }

    if ( query_sift_filter_ )
    {
        vl_sift_delete(query_sift_filter_);
        query_sift_filter_=0;
    }

    if ( model_grad_data_ )
    {
        vl_free(model_grad_data_);
        model_grad_data_=0;
    }

    if ( query_grad_data_ )
    {
        vl_free(query_grad_data_);
        query_grad_data_=0;
    }
    
}

void dbskfg_match_bag_of_fragments::load_model(vcl_string model_dir)
{
    // Find file with this glob pattern
    vcl_string glob=model_dir+"/*_fragment_list.txt";
    vcl_string fragment_list;

    for (vul_file_iterator fn=glob; fn; ++fn)
    { 
        fragment_list=fn();
 
    }
    
    if ( fragment_list.size() == 0 )
    {
        vcl_cerr<<"Error Loading Model fragments"<<vcl_endl;
        return;
    }

    vcl_cout<<"Loading Model Set of Fragments"<<vcl_endl;

    // Open the file
    vcl_ifstream file_opener;
    file_opener.open(fragment_list.c_str());

    // Read each exemplar name
    vcl_string temp;
    unsigned model_id(0);
    while(!file_opener.eof())
    {
        getline(file_opener,temp);
        if ( temp.size() )
        {
            vcl_cout<<"Loading Model File: "<<temp<<vcl_endl;
            model_fragments_[model_id]=vcl_make_pair(
                temp,
                load_composite_graph(temp));
            model_id++;
        }
    }
    
    vcl_cout<<vcl_endl;

}

void dbskfg_match_bag_of_fragments::load_binary_model(vcl_string model_dir)
{
    bpro1_filepath filepath(model_dir);

    dbskfg_load_binary_composite_graph_process load_pro;
    load_pro.clear_input();
    load_pro.clear_output();

    load_pro.parameters()->set_value( "-cginput" ,filepath);

    bool flag=load_pro.execute();
    load_pro.finish();

    vcl_map<unsigned int,dbskfg_composite_graph_sptr> cgraphs = 
        load_pro.get_cgraphs();

    vcl_map<unsigned int,dbskfg_composite_graph_sptr>::iterator it;
    for ( it = cgraphs.begin() ; it != cgraphs.end() ; ++it)
    {
        vcl_stringstream stream;
        stream<<"model_"<<(*it).first;
        model_fragments_[(*it).first]=vcl_make_pair(stream.str(),(*it).second);
    }


    vcl_vector<unsigned int> regions_removed=load_pro.get_frags_removed();
    if ( regions_removed.size() )
    {
         vcl_ofstream region_file;
         region_file.open(output_removed_regions_.c_str(),
                          vcl_ios::out | 
                          vcl_ios::app );
         for ( unsigned int i=0; i < regions_removed.size() ; ++i)
         {
             region_file<<regions_removed[i]<<" ";
             
         }
         region_file<<vcl_endl;
         region_file.close();

    }

}

void dbskfg_match_bag_of_fragments::load_query(vcl_string query_dir)
{
    // Find file with this glob pattern
    vcl_string glob=query_dir+"/*_fragment_list.txt";
    vcl_string fragment_list;

    for (vul_file_iterator fn=glob; fn; ++fn)
    { 
        fragment_list=fn();
 
    }

    if ( fragment_list.size() == 0 )
    {
        vcl_cerr<<"Error Loading Query fragments"<<vcl_endl;
        return;
    }

    vcl_cout<<"Opening Query Set of Fragments"<<vcl_endl;
    
    // Open the file
    vcl_ifstream file_opener;
    file_opener.open(fragment_list.c_str());

    // Read each exemplar name
    vcl_string temp;
    unsigned query_id(0);
    while(!file_opener.eof())
    {
        getline(file_opener,temp);
        if ( temp.size()) 
        {
            vcl_cout<<"Loading Query File: "<<temp<<vcl_endl;
            query_fragments_[query_id]=vcl_make_pair(
                temp,
                load_composite_graph(temp));
            query_id++;
        }
    }
    vcl_cout<<vcl_endl;

}

void dbskfg_match_bag_of_fragments::load_binary_query(vcl_string query_dir)
{
    bpro1_filepath filepath(query_dir);

    dbskfg_load_binary_composite_graph_process load_pro;
    load_pro.clear_input();
    load_pro.clear_output();

    load_pro.parameters()->set_value( "-cginput" ,filepath);

    bool flag=load_pro.execute();
    load_pro.finish();

    vcl_map<unsigned int,dbskfg_composite_graph_sptr> cgraphs = 
        load_pro.get_cgraphs();

    vcl_map<unsigned int,dbskfg_composite_graph_sptr>::iterator it;
    for ( it = cgraphs.begin() ; it != cgraphs.end() ; ++it)
    {
        vcl_stringstream stream;
        stream<<"query_"<<(*it).first;
        query_fragments_[(*it).first]=vcl_make_pair(stream.str(),(*it).second);
    }

    vcl_vector<unsigned int> regions_removed=load_pro.get_frags_removed();
    if ( regions_removed.size() )
    {
        vcl_ofstream region_file;
        region_file.open(output_removed_regions_.c_str(),
                         vcl_ios::out | 
                         vcl_ios::app );
        for ( unsigned int i=0; i < regions_removed.size() ; ++i)
        {
            region_file<<regions_removed[i]<<" ";
             
        }
        region_file<<vcl_endl;
        region_file.close();

    }

}

dbskfg_composite_graph_sptr dbskfg_match_bag_of_fragments::
load_composite_graph(vcl_string filename)
{
    bpro1_filepath filepath(filename);

    dbskfg_load_composite_graph_process load_pro;
    load_pro.clear_input();
    load_pro.clear_output();

    load_pro.parameters()->set_value( "-cginput" ,filepath);

    bool flag=load_pro.execute();
    load_pro.finish();

    vcl_vector<bpro1_storage_sptr> cg_results;
    cg_results = load_pro.get_output();

    dbskfg_composite_graph_storage_sptr cg_storage;
    cg_storage.vertical_cast(cg_results[0]);
    
    return cg_storage->get_composite_graph();
    
}

bool dbskfg_match_bag_of_fragments::match()
{
    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    vcl_cout<<"Matching "
            <<model_fragments_.size()
            <<" model fragments to "
            <<query_fragments_.size()
            <<" query fragments"
            <<vcl_endl;
 
    // Loop over model and query
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator m_iterator;
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator q_iterator;

    for ( m_iterator = model_fragments_.begin() ; 
          m_iterator != model_fragments_.end() ; ++m_iterator)
    {
        vcl_map<double,vcl_pair<unsigned int,unsigned int> >
            model_map;

        for ( q_iterator = query_fragments_.begin() ; 
              q_iterator != query_fragments_.end() ; ++q_iterator)
        {
            vcl_cout<<"Matching "
                    <<(*m_iterator).second.first
                    <<" to "
                    <<(*q_iterator).second.first
                    <<vcl_endl;

            // Match model to query
            double cost1 = match_two_graphs((*m_iterator).second.second,
                                            (*q_iterator).second.second);
            double cost2 = match_two_graphs((*q_iterator).second.second,
                                            (*m_iterator).second.second);
            double cost  = vcl_min(cost1,cost2);

            unsigned int model_id= (*m_iterator).first;
            unsigned int query_id= (*q_iterator).first;
            model_map[cost]=vcl_make_pair(model_id,query_id);
        }
        sim_matrix_.push_back(model_map);
    }

    write_out_sim_matrix();

    create_html_match_file();

    return true;
}


bool dbskfg_match_bag_of_fragments::binary_match()
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    vcl_cout<<"Matching "
            <<model_fragments_.size()
            <<" model fragments to "
            <<query_fragments_.size()
            <<" query fragments"
            <<vcl_endl;
 
    // Loop over model and query
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator m_iterator;
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator q_iterator;

    for ( m_iterator = model_fragments_.begin() ; 
          m_iterator != model_fragments_.end() ; ++m_iterator)
    {
        //: prepare the trees also
        dbskfg_cgraph_directed_tree_sptr model_tree = new 
            dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                        scurve_interpolate_ds_, 
                                        scurve_matching_R_);

        bool f1=model_tree->acquire
            ((*m_iterator).second.second, elastic_splice_cost_, 
             circular_ends_, combined_edit_);

        vcl_map<double,vcl_pair<unsigned int,unsigned int> >
            model_map;

        for ( q_iterator = query_fragments_.begin() ; 
              q_iterator != query_fragments_.end() ; ++q_iterator)
        {
            //: prepare the trees also
            dbskfg_cgraph_directed_tree_sptr query_tree = new
                dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                            scurve_interpolate_ds_, 
                                            scurve_matching_R_);
            
            bool f1=query_tree->acquire
                ((*q_iterator).second.second, elastic_splice_cost_, 
             circular_ends_, combined_edit_);

            double norm_shape_cost(0.0);
            double app_diff(0.0);
            double norm_app_cost(0.0);
            double rgb_avg_cost(0.0);
            double norm_shape_cost_length(0.0);

            // Match model to query
            match_two_graphs(model_tree,
                             query_tree,
                             norm_shape_cost,
                             norm_shape_cost_length,
                             app_diff,
                             norm_app_cost,
                             rgb_avg_cost);

            unsigned int model_id= (*m_iterator).first;
            unsigned int query_id= (*q_iterator).first;
            binary_sim_matrix_[model_id][query_id]=norm_shape_cost;
            binary_sim_length_matrix_[model_id][query_id]=
                norm_shape_cost_length;
            binary_app_sim_matrix_[model_id][query_id]=app_diff;
            binary_app_norm_sim_matrix_[model_id][query_id]=norm_app_cost;
            binary_app_rgb_sim_matrix_[model_id][query_id]=rgb_avg_cost;

            query_tree=0;
        }
        vcl_cout<<"Finished "<<(*m_iterator).second.first<<" to all queires"
                <<vcl_endl;
        model_tree=0;
    }

    // write out data

    vcl_ofstream binary_sim_file;
    binary_sim_file.open(output_binary_file_.c_str(),
                         vcl_ios::out | 
                         vcl_ios::app | 
                         vcl_ios::binary);

  
    write_binary_fragments(binary_sim_file,model_fragments_);
    write_binary_fragments(binary_sim_file,query_fragments_);

    double matrix_size=binary_sim_matrix_.columns()*
        binary_sim_matrix_.rows()*2;
    binary_sim_file.write(reinterpret_cast<char *>(&matrix_size),
                          sizeof(double));

    for ( unsigned int c=0; c < binary_sim_matrix_.columns() ; ++c)
    {
        for ( unsigned int r=0; r < binary_sim_matrix_.rows() ; ++r)
        {
            double value=binary_sim_matrix_[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));

            value=binary_sim_length_matrix_[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));

            // value=binary_app_sim_matrix_[r][c];
            // binary_sim_file.write(reinterpret_cast<char *>(&value),
            //                       sizeof(double));

            // value=binary_app_norm_sim_matrix_[r][c];
            // binary_sim_file.write(reinterpret_cast<char *>(&value),
            //                       sizeof(double));

            // value=binary_app_rgb_sim_matrix_[r][c];
            // binary_sim_file.write(reinterpret_cast<char *>(&value),
            //                       sizeof(double));
            
        }
    } 
    
    binary_sim_file.close();

    if ( model_sift_filter_)
    {
        vl_sift_delete(model_sift_filter_);
        model_sift_filter_=0;
    }

    if ( query_sift_filter_ )
    {
        vl_sift_delete(query_sift_filter_);
        query_sift_filter_=0;
    }

    if ( model_grad_data_ )
    {
        vl_free(model_grad_data_);
        model_grad_data_=0;
    }

    if ( query_grad_data_ )
    {
        vl_free(query_grad_data_);
        query_grad_data_=0;
    }
   
    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"MatchTime "
            <<model_fragments_.size()
            <<" model fragments to "
            <<query_fragments_.size()
            <<" query fragments is "
            <<vox_time<<" sec"<<vcl_endl;

    return true;
}

bool dbskfg_match_bag_of_fragments::binary_app_match()
{
    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    vcl_cout<<"Matching "
            <<model_fragments_.size()
            <<" model fragments to "
            <<query_fragments_.size()
            <<" query fragments using edit distance app"
            <<vcl_endl;
 
    // Loop over model and query
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator m_iterator;
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator q_iterator;
    vcl_map<unsigned int,vnl_vector<double> > query_descriptors;

    for ( m_iterator = model_fragments_.begin() ; 
          m_iterator != model_fragments_.end() ; ++m_iterator)
    {
        //: prepare the trees also
        dbskfg_cgraph_directed_tree_sptr model_tree = new 
            dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                        scurve_interpolate_ds_, 
                                        scurve_matching_R_);

        bool f1=model_tree->acquire
            ((*m_iterator).second.second, elastic_splice_cost_, 
             circular_ends_, combined_edit_);

        vcl_map<int,vcl_vector<dbskfg_sift_data> > fragments;
        vsol_box_2d_sptr bbox(0);
        model_tree->compute_region_descriptor(fragments,bbox);
        vnl_vector<double>
            m_descr = compute_second_order_pooling(
                fragments,
                model_grad_data_,model_sift_filter_,bbox);

        vcl_map<double,vcl_pair<unsigned int,unsigned int> >
            model_map;
        
        for ( q_iterator = query_fragments_.begin() ; 
              q_iterator != query_fragments_.end() ; ++q_iterator)
        {

            double distance=0.0;

            //: prepare the trees also                                 
            if ( !query_descriptors.count((*q_iterator).first ))
            {

                //: prepare the trees also
                dbskfg_cgraph_directed_tree_sptr query_tree = new
                    dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                                scurve_interpolate_ds_, 
                                                scurve_matching_R_);
                
                bool f1=query_tree->acquire
                    ((*q_iterator).second.second, elastic_splice_cost_, 
                     circular_ends_, combined_edit_);
                
                vcl_map<int,vcl_vector<dbskfg_sift_data> > fragments;
                vsol_box_2d_sptr bbox(0);
                query_tree->compute_region_descriptor(fragments,bbox);
                vnl_vector<double>
                    q_descr = compute_second_order_pooling(
                        fragments,
                        query_grad_data_,query_sift_filter_,bbox);
                distance=vnl_vector_ssd(m_descr,q_descr);
                query_descriptors[(*q_iterator).first]=q_descr;

                query_tree=0;
            }
            else
            {
                distance=vnl_vector_ssd(
                    m_descr,query_descriptors[(*q_iterator).first]);
            }
                
            double norm_shape_cost(0.0);
            double app_diff(0.0);
            double norm_app_cost(0.0);
            double rgb_avg_cost(0.0);
            
            unsigned int model_id= (*m_iterator).first;
            unsigned int query_id= (*q_iterator).first;
            binary_sim_matrix_[model_id][query_id]=distance;
            binary_app_sim_matrix_[model_id][query_id]=app_diff;
            binary_app_norm_sim_matrix_[model_id][query_id]=norm_app_cost;
            binary_app_rgb_sim_matrix_[model_id][query_id]=rgb_avg_cost;

        }
        vcl_cout<<"Finished "<<(*m_iterator).second.first<<" to all queires"
                <<vcl_endl;
        model_tree=0;
    }

    // write out data

    vcl_ofstream binary_sim_file;
    binary_sim_file.open(output_binary_file_.c_str(),
                         vcl_ios::out | 
                         vcl_ios::app | 
                         vcl_ios::binary);

  
    write_binary_fragments(binary_sim_file,model_fragments_);
    write_binary_fragments(binary_sim_file,query_fragments_);

    double matrix_size=binary_sim_matrix_.columns()*
        binary_sim_matrix_.rows()*4;
    binary_sim_file.write(reinterpret_cast<char *>(&matrix_size),
                          sizeof(double));

    for ( unsigned int c=0; c < binary_sim_matrix_.columns() ; ++c)
    {
        for ( unsigned int r=0; r < binary_sim_matrix_.rows() ; ++r)
        {
            double value=binary_sim_matrix_[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));

            value=binary_app_sim_matrix_[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));

            value=binary_app_norm_sim_matrix_[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));

            value=binary_app_rgb_sim_matrix_[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));
            
        }
    } 
    
    binary_sim_file.close();

    if ( model_sift_filter_)
    {
        vl_sift_delete(model_sift_filter_);
        model_sift_filter_=0;
    }

    if ( query_sift_filter_ )
    {
        vl_sift_delete(query_sift_filter_);
        query_sift_filter_=0;
    }

    if ( model_grad_data_ )
    {
        vl_free(model_grad_data_);
        model_grad_data_=0;
    }

    if ( query_grad_data_ )
    {
        vl_free(query_grad_data_);
        query_grad_data_=0;
    }
   
    return true;
}

bool dbskfg_match_bag_of_fragments::binary_scale_match()
{
    vcl_cout<<"Matching shock graphs with scaling"<<vcl_endl;

    if ( model_contours_.size() == 0 || query_contours_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    vcl_map<unsigned int,vcl_vector< vsol_spatial_object_2d_sptr > >::
        iterator m_iterator;
    vcl_map<unsigned int,vcl_vector< vsol_spatial_object_2d_sptr > >::
        iterator q_iterator;

    for ( m_iterator = model_contours_.begin() ; 
          m_iterator != model_contours_.end() ; ++m_iterator)
    {
        // create model bbox
        vcl_vector<vsol_spatial_object_2d_sptr> model_contours=
            (*m_iterator).second;

        // create new bounding box
        vsol_box_2d_sptr model_bbox = new vsol_box_2d();

        for ( unsigned int i=0; i < model_contours.size() ; ++i)
        {
            vsol_line_2d_sptr line= model_contours[i]
                ->cast_to_curve()->cast_to_line();
            model_bbox->add_point(line->p0()->x(),line->p0()->y());
            model_bbox->add_point(line->p1()->x(),line->p1()->y());
        }
        

        for ( q_iterator = query_contours_.begin() ; 
              q_iterator != query_contours_.end() ; ++q_iterator)
        {
            // create query bbox
            vcl_vector<vsol_spatial_object_2d_sptr> query_contours=
                (*q_iterator).second;

            
            // create new bounding box
            vsol_box_2d_sptr query_bbox = new vsol_box_2d();

            for ( unsigned int i=0; i < query_contours.size() ; ++i)
            {
                vsol_line_2d_sptr line= query_contours[i]
                    ->cast_to_curve()->cast_to_line();
                query_bbox->add_point(line->p0()->x(),line->p0()->y());
                query_bbox->add_point(line->p1()->x(),line->p1()->y());
            }

            // Comparing bbox
            double x_ratio = (model_bbox->get_max_x()-model_bbox->get_min_x())
                /(query_bbox->get_max_x()-query_bbox->get_min_x());

            double y_ratio = (model_bbox->get_max_y()-model_bbox->get_min_y())
                /(query_bbox->get_max_y()-query_bbox->get_min_y());

            // Create two new sets of contours
            vcl_vector<vsol_spatial_object_2d_sptr> 
                model_scaled_to_query_contours;

            vcl_vector<vsol_spatial_object_2d_sptr> 
                query_scaled_to_model_contours;

            for ( unsigned int i=0; i < query_contours.size() ; ++i)
            {
                vsol_line_2d_sptr line= query_contours[i]
                    ->cast_to_curve()->cast_to_line();
                vgl_point_2d<double> start=line->p0()->get_p();
                vgl_point_2d<double> end  =line->p1()->get_p();
                
                start.set(start.x()*x_ratio,start.y()*y_ratio);
                end.set(end.x()*x_ratio,end.y()*y_ratio);
                
                vsol_spatial_object_2d_sptr 
                    scaled_line = new vsol_line_2d(start,end);
                scaled_line->set_id(line->get_id());
                query_scaled_to_model_contours.push_back(scaled_line);
            }

            for ( unsigned int i=0; i < model_contours.size() ; ++i)
            {
                vsol_line_2d_sptr line= model_contours[i]
                    ->cast_to_curve()->cast_to_line();
                vgl_point_2d<double> start=line->p0()->get_p();
                vgl_point_2d<double> end  =line->p1()->get_p();
                
                start.set(start.x()*(1/x_ratio),start.y()*(1/y_ratio));
                end.set(end.x()*(1/x_ratio),end.y()*(1/y_ratio));
                
                vsol_spatial_object_2d_sptr 
                    scaled_line = new vsol_line_2d(start,end);
                scaled_line->set_id(line->get_id());
                model_scaled_to_query_contours.push_back(scaled_line);
            }

            // vcl_stringstream stream1,stream2,stream3,stream4;

            // stream1<<"Model_"<<(*m_iterator).first<<"_contours.bnd";
            // vcl_string model_filename=stream1.str();
            // stream1.str().clear();

            // stream2<<"Query_"<<(*q_iterator).first<<"_contours.bnd";
            // vcl_string query_filename=stream2.str();
            // stream2.str().clear();

            // stream3<<"Model_"<<(*m_iterator).first
            //       <<"_scaled_Query_"
            //       <<(*q_iterator).first
            //       <<"_contours.bnd";
            // vcl_string model_scaled_to_query_filename=stream3.str();
            // stream3.str().clear();

            // stream4<<"Query_"<<(*q_iterator).first
            //       <<"_scaled_Model_"
            //       <<(*m_iterator).first
            //       <<"_contours.bnd";
            // vcl_string query_scaled_to_model_filename=stream4.str();
            // stream4.str().clear();
            
            // Check model cgraph can be computed
            bool model_status=true;
            dbskfg_composite_graph_sptr model_original_scale_cg(0);
            {
                dbskfg_load_binary_composite_graph_process load_pro;
                model_status=load_pro.compute_graph(
                    model_contours,
                    model_con_ids_[(*m_iterator).first],
                    model_image_size_);
                if ( model_status)
                {
                    load_pro.get_first_graph(model_original_scale_cg);
                }
            }
            
            if ( !model_status)
            {
                break;
            }
        
            // Check query_graph can be compute
            bool query_status=true;
            dbskfg_composite_graph_sptr query_original_scale_cg(0);
            {
                dbskfg_load_binary_composite_graph_process load_pro;
                query_status=load_pro.compute_graph(
                    query_contours,
                    query_con_ids_[(*q_iterator).first],
                    query_image_size_);
                if ( query_status )
                {
                    load_pro.get_first_graph(query_original_scale_cg);
                }
            }

            if ( !query_status)
            {
                continue;
            }

            // dbsk2d_file_io::save_bnd_v3_0(model_filename,model_contours);
            // dbsk2d_file_io::save_bnd_v3_0(query_filename,query_contours);

            // dbsk2d_file_io::save_bnd_v3_0(model_scaled_to_query_filename,
            //                               model_scaled_to_query_contours);

            // dbsk2d_file_io::save_bnd_v3_0(query_scaled_to_model_filename,
            //                               query_scaled_to_model_contours);

            vcl_pair<unsigned int,unsigned int> huge_image(1024,1024);
            dbskfg_composite_graph_sptr model_scaled_to_query_cg(0);
            {
                dbskfg_load_binary_composite_graph_process load_pro;
                bool flag=load_pro.compute_graph(
                    model_scaled_to_query_contours,
                    model_con_ids_[(*m_iterator).first],
                    huge_image,
                    false);
                load_pro.get_first_graph(model_scaled_to_query_cg);

            }

            dbskfg_composite_graph_sptr query_scaled_to_model_cg(0);
            {
                dbskfg_load_binary_composite_graph_process load_pro;
                bool flag=load_pro.compute_graph(
                    query_scaled_to_model_contours,
                    query_con_ids_[(*q_iterator).first],
                    huge_image,
                    false);
                load_pro.get_first_graph(query_scaled_to_model_cg);

            } 

            // Do at original scale of shock graphs
            double cost1_1 = match_two_graphs(model_original_scale_cg,
                                              query_original_scale_cg);
            double cost1_2 = match_two_graphs(query_original_scale_cg,
                                              model_original_scale_cg);

            // Do at new scaled versions of shock graphs
            double cost2_1 = match_two_graphs(model_original_scale_cg,
                                              query_scaled_to_model_cg);
            double cost2_2 = match_two_graphs(query_original_scale_cg,
                                              model_scaled_to_query_cg);

            double cost1      = vcl_min(cost1_1,cost1_2);
            double cost2      = vcl_min(cost2_1,cost2_2);
            double final_cost = vcl_min(cost1,cost2);

            unsigned int model_id= (*m_iterator).first;
            unsigned int query_id= (*q_iterator).first;
            vcl_cout<<model_id<<","<<query_id;
            binary_sim_matrix_[model_id][query_id]=final_cost;
        }
    }

    // write out data

    vcl_ofstream binary_sim_file;
    binary_sim_file.open(output_binary_file_.c_str(),
                         vcl_ios::out | 
                         vcl_ios::app | 
                         vcl_ios::binary);

    double matrix_size=binary_sim_matrix_.columns()*
        binary_sim_matrix_.rows();
    binary_sim_file.write(reinterpret_cast<char *>(&matrix_size),
                          sizeof(double));

    for ( unsigned int c=0; c < binary_sim_matrix_.columns() ; ++c)
    {
        for ( unsigned int r=0; r < binary_sim_matrix_.rows() ; ++r)
        {
            double value=binary_sim_matrix_[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));
            
        }
    } 
    
    binary_sim_file.close();
    return true;
}

void dbskfg_match_bag_of_fragments::write_binary_fragments(
vcl_ofstream& binary_sim_file,
vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >& 
local_fragments )
{


    vcl_map<unsigned int,
        vcl_vector<vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > > > 
        local_contours;
    vcl_map<unsigned int,
        vcl_vector<vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > > > 
        local_shocks;

    vcl_map<unsigned int,
        vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >::iterator m_iterator;

    for ( m_iterator = local_fragments.begin() ; 
          m_iterator != local_fragments.end() ; ++m_iterator)
    {
        dbskfg_composite_graph_sptr cgraph=(*m_iterator).second.second;
        for (dbskfg_composite_graph::edge_iterator eit = cgraph->edges_begin(); 
             eit != cgraph->edges_end(); ++eit)
        {
            
            if ( (*eit)->link_type() == dbskfg_composite_link::CONTOUR_LINK)
            {
                vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > pair
                    = vcl_make_pair((*eit)->source()->pt(),
                                    (*eit)->target()->pt());
                local_contours[(*m_iterator).first].push_back(pair);
                
            }
            else
            {
                dbskfg_shock_link* slink=dynamic_cast<dbskfg_shock_link*>
                    (&(*(*eit)));
                vcl_vector<vgl_point_2d<double> > points=slink->ex_pts();
                for ( unsigned int i=1; i < points.size() ; ++i)
                {
                    
                    vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > pair
                        = vcl_make_pair(points[i-1],
                                        points[i]);
                    local_shocks[(*m_iterator).first].push_back(pair);
                }
            }
        }

    }

    double local_size =local_fragments.size();
  
    // write out numb  fragments
    binary_sim_file.write(reinterpret_cast<char *>(&local_size),
                          sizeof(double));



    vcl_map<unsigned int,
        vcl_vector<vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > > >
        ::iterator it;

    for ( it = local_contours.begin() ; it != local_contours.end() ; ++it)
    {
        vcl_vector<vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > >
            contours=local_contours[(*it).first];
        vcl_vector<vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > >
            shocks=local_shocks[(*it).first];

        double local_num_contours= contours.size()*4.0;
        binary_sim_file.write(reinterpret_cast<char *>(&local_num_contours),
                          sizeof(double));

        for ( unsigned int c=0; c < contours.size() ; ++c )
        {
            vgl_point_2d<double> source=contours[c].first;
            vgl_point_2d<double> target=contours[c].second;
        
        
            double x1_coord = source.x();
            double y1_coord = source.y();

            double x2_coord = target.x();
            double y2_coord = target.y();

            binary_sim_file.write(reinterpret_cast<char *>(&x1_coord),
                                     sizeof(double));
            binary_sim_file.write(reinterpret_cast<char *>(&y1_coord),
                                     sizeof(double));

            binary_sim_file.write(reinterpret_cast<char *>(&x2_coord),
                                     sizeof(double));
            binary_sim_file.write(reinterpret_cast<char *>(&y2_coord),
                                     sizeof(double));

        }

        double local_num_shocks= shocks.size()*4.0;
        binary_sim_file.write(reinterpret_cast<char *>(&local_num_shocks),
                          sizeof(double));

        for ( unsigned int s=0; s < shocks.size() ; ++s )
        {
            vgl_point_2d<double> source=shocks[s].first;
            vgl_point_2d<double> target=shocks[s].second;
        
        
            double x1_coord = source.x();
            double y1_coord = source.y();

            double x2_coord = target.x();
            double y2_coord = target.y();

            binary_sim_file.write(reinterpret_cast<char *>(&x1_coord),
                                     sizeof(double));
            binary_sim_file.write(reinterpret_cast<char *>(&y1_coord),
                                     sizeof(double));

            binary_sim_file.write(reinterpret_cast<char *>(&x2_coord),
                                     sizeof(double));
            binary_sim_file.write(reinterpret_cast<char *>(&y2_coord),
                                     sizeof(double));

        }



    }
  

}

void dbskfg_match_bag_of_fragments::write_out_sim_matrix()
{
    // Create root element
    bxml_document  doc;
    bxml_data_sptr root     = new bxml_element("match_sim_matrix");
    bxml_element*  root_elm = dbxml_algos::
        cast_to_element(root,"match_sim_matrix");
    doc.set_root_element(root_elm); 
    root_elm->append_text("\n   ");

    // Create contour set
    bxml_data_sptr model     = new bxml_element("model");
    bxml_element*  model_elm = dbxml_algos::cast_to_element(model,
                                                            "model");
    root_elm->append_data(model);
    root_elm->append_text("\n   ");
    model_elm->append_text("\n      ");

    // Create query set
    bxml_data_sptr query     = new bxml_element("query");
    bxml_element*  query_elm = dbxml_algos::cast_to_element(query,
                                                            "query");
  
    root_elm->append_data(query);
    root_elm->append_text("\n   ");
    query_elm->append_text("\n      ");

   // Create bbox set
    bxml_data_sptr bbox     = new bxml_element("bbox");
    bxml_element*  bbox_elm = dbxml_algos::cast_to_element(bbox,
                                                            "bbox");
  
    root_elm->append_data(bbox);
    root_elm->append_text("\n   ");
    bbox_elm->append_text("\n      ");

    // Create costs set
    bxml_data_sptr costs     = new bxml_element("costs");
    bxml_element*  costs_elm = dbxml_algos::cast_to_element(costs,
                                                            "costs");  
    root_elm->append_data(costs);
    root_elm->append_text("\n");
    costs_elm->append_text("\n      ");

    // Loop over model and query
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator m_iterator;
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator q_iterator;

    unsigned int size=0;
    for ( m_iterator = model_fragments_.begin() ; 
          m_iterator != model_fragments_.end() ; ++m_iterator)
    {
        bxml_data_sptr model_fragment = new bxml_element("model_fragment");
        bxml_element *model_fragment_elm = dbxml_algos::cast_to_element(
            model_fragment,"model_fragment");

        unsigned int id       = (*m_iterator).first;
        vcl_string model_path = (*m_iterator).second.first;

        model_elm->append_data(model_fragment_elm);
        model_fragment_elm->set_attribute("id",id);
        model_fragment_elm->set_attribute("path",model_path);

        if ( size != model_fragments_.size()-1)
        {
            model_elm->append_text("\n      ");
        }
        else
        {
            model_elm->append_text("\n   ");
        }
        size++;
    }
    
    size=0;
    for ( q_iterator = query_fragments_.begin() ; 
          q_iterator != query_fragments_.end() ; ++q_iterator)
    {
        bxml_data_sptr query_fragment = new bxml_element("query_fragment");
        bxml_element *query_fragment_elm = dbxml_algos::cast_to_element(
            query_fragment,"query_fragment");

        unsigned int id       = (*q_iterator).first;
        vcl_string query_path = (*q_iterator).second.first;

        query_elm->append_data(query_fragment_elm);
        query_fragment_elm->set_attribute("id",id);
        query_fragment_elm->set_attribute("path",query_path);

        if ( size != query_fragments_.size()-1)
        {
            query_elm->append_text("\n      ");
        }
        else
        {
            query_elm->append_text("\n   ");
        }
        size++;

    }
    

    size=0;
    for ( q_iterator = query_fragments_.begin() ; 
          q_iterator != query_fragments_.end() ; ++q_iterator)
    {
        vsol_box_2d bbox;
        dbskfg_utilities::bbox((*q_iterator).second.second,bbox);
        bxml_data_sptr bbox_fragment = new bxml_element("bbox_fragment");
        bxml_element *bbox_fragment_elm = dbxml_algos::cast_to_element(
            bbox_fragment,"bbox_fragment");

        double xmin=bbox.get_min_x();
        double ymin=bbox.get_min_y();
        double xmax=bbox.get_max_x();
        double ymax=bbox.get_max_y();
        unsigned int id       = (*q_iterator).first;

        bbox_elm->append_data(bbox_fragment_elm);
        bbox_fragment_elm->set_attribute("id",id);
        bbox_fragment_elm->set_attribute("xmin",xmin);
        bbox_fragment_elm->set_attribute("ymin",ymin);
        bbox_fragment_elm->set_attribute("xmax",xmax);
        bbox_fragment_elm->set_attribute("ymax",ymax);

        if ( size != query_fragments_.size()-1)
        {
            bbox_elm->append_text("\n      ");
        }
        else
        {
            bbox_elm->append_text("\n   ");
        }
        size++;

    }
    
    // write out costs
    size=0;
    for ( unsigned int v=0; v < sim_matrix_.size() ; ++v)
    {
        vcl_map<double,vcl_pair<unsigned int,unsigned int> > model_to_query
            = sim_matrix_[v];
        vcl_map<double,vcl_pair<unsigned int,unsigned int> >::iterator it;

        
        // Create line set within countour set
        bxml_data_sptr one_to_many     = new bxml_element("one_to_many");
        bxml_element*  one_to_many_elm = dbxml_algos::cast_to_element(
            one_to_many,
            "one_to_many");

        costs_elm->append_data(one_to_many);
        one_to_many_elm->set_attribute("model_id",v);
        one_to_many_elm->append_text("\n          ");
 
        unsigned int length=0;
        for ( it = model_to_query.begin() ; it != model_to_query.end() ; ++it)
        {
            bxml_data_sptr query_match  = new bxml_element("query_match");
            bxml_element *query_match_elm = dbxml_algos::cast_to_element(
                query_match,"query_match");

            one_to_many_elm->append_data(query_match_elm);
           
            double cost=(*it).first;
            unsigned int id  =(*it).second.second;
            query_match_elm->set_attribute("cost",cost);
            query_match_elm->set_attribute("query_id",id);

            if ( length == model_to_query.size()-1)
            {
                one_to_many_elm->append_text("\n      ");
            }
            else
            {
                one_to_many_elm->append_text("\n          ");
            }
            length++;
        }
        
        if ( v != sim_matrix_.size() -1 )
        {
            costs_elm->append_text("\n      ");
        }
    }
    
    costs_elm->append_text("\n   ");

    bxml_write(output_match_file_,doc);


}

void dbskfg_match_bag_of_fragments::create_html_match_file()
{
    vcl_ofstream tf(output_html_file_.c_str());

    vcl_string table_caption="Similarity Matrix Model vs Query";

    //------------- Write out header info about table
    tf << "<TABLE BORDER=\"1\">\n";
    tf << "<caption align=\"top\">" << table_caption << "</caption>\n";
    tf << "  <TR>"<<vcl_endl;
    tf << "    <TH></TH>"<<vcl_endl;
    
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator q_iterator;

    // ------------- Write out first row
    unsigned int match=1;
    for ( q_iterator = query_fragments_.begin() ; 
          q_iterator != query_fragments_.end() ; ++q_iterator)
    {
        
        vcl_stringstream stream;
        stream<<match;
        tf << "    <TH> Match "<<stream.str()<<"</TH>"<<vcl_endl;
        match++;
    }
    tf <<"  </TR>"<<vcl_endl;

    // Write out each next row
    // write out costs
    for ( unsigned int v=0; v < sim_matrix_.size() ; ++v)
    {
        tf << "  <TR>"<<vcl_endl;

        vcl_map<double,vcl_pair<unsigned int,unsigned int> > model_to_query
            = sim_matrix_[v];
        vcl_map<double,vcl_pair<unsigned int,unsigned int> >::iterator it;
        
        // Grab image string
        vcl_string model_image = vul_file::strip_extension(
            vul_file::strip_directory(
                model_fragments_[v].first))+".png";
        
        vcl_string html_model_string = "<img src=\"images/"+
            model_image+"\" />"; 
        tf << "    <TD>"<<html_model_string<<"</TD>"<<vcl_endl;

        for ( it = model_to_query.begin() ; it != model_to_query.end() ; ++it)
        {
            // Grab Cost
            double cost = (*it).first;
            vcl_stringstream cost_stream;
            cost_stream<<cost;

            // Get query image
            vcl_string query_image = vul_file::strip_extension(
                vul_file::strip_directory(
                    query_fragments_[(*it).second.second].first))+".png";
        
            vcl_string html_query_string = "<img src=\"images/"+
                query_image+"\" />"+" sim: "+cost_stream.str(); 
            tf << "    <TD>"<<html_query_string<<" </TD>"<<vcl_endl;
    
        }
        tf <<"  </TR>"<<vcl_endl;
    }   
    tf<<"</TABLE>"<<vcl_endl;
    tf.close();

}

double dbskfg_match_bag_of_fragments::match_two_graphs(
    dbskfg_composite_graph_sptr& model, dbskfg_composite_graph_sptr& query,
    unsigned int root_scheme)
{
    // Create two composite graph storage
    dbskfg_composite_graph_storage_sptr model_storage =
        dbskfg_composite_graph_storage_new();
    dbskfg_composite_graph_storage_sptr query_storage =
        dbskfg_composite_graph_storage_new();
    
    model_storage->set_composite_graph(model);
    query_storage->set_composite_graph(query);

    dbskfg_match_composite_graph_process match_pro;

    match_pro.clear_input();
    match_pro.clear_output();

    match_pro.add_input(model_storage);
    match_pro.add_input(query_storage);

    match_pro.parameters()->set_value(
        "-elastic_splice_cost"  , elastic_splice_cost_); 
    match_pro.parameters()->set_value(
        "-scurve_sample_ds"     , scurve_sample_ds_);
    match_pro.parameters()->set_value(
        "-scurve_interpolate_ds", scurve_interpolate_ds_); 
    match_pro.parameters()->set_value(
        "-localized_edit"       , localized_edit_ ); 
    match_pro.parameters()->set_value(
        "-curve_matching_R"     , scurve_matching_R_ ); 
    match_pro.parameters()->set_value(
        "-circular_ends"        , circular_ends_); 
    match_pro.parameters()->set_value(
        "-combined_edit"        , combined_edit_); 
    match_pro.parameters()->set_value(
        "-use_approx"           , use_approx_);
    match_pro.parameters()->set_value(
        "-scale_root"           , scale_root_);
    match_pro.parameters()->set_value(
        "-app_sift"             , app_sift_);
    match_pro.parameters()->set_value(
        "-root_scheme"          , root_scheme);

    bool flag=match_pro.execute();
    double final_cost=-1.0;
    if ( flag == true )
    {
        final_cost = match_pro.final_cost();
    }
    match_pro.finish();

    return final_cost;
}


void dbskfg_match_bag_of_fragments::match_two_graphs(
    dbskfg_cgraph_directed_tree_sptr& model_tree, 
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    double& norm_shape_cost,
    double& norm_shape_cost_length,
    double& app_diff,
    double& norm_app_cost,
    double& rgb_avg_cost)
{

    vul_timer shape_timer;
    shape_timer.mark();

    // grab model roots
    vcl_set<int> model_roots;
    vcl_set<int> query_roots;
    
    // Insert largest radius root
    model_roots.insert(0);
    query_roots.insert(0);

    // Insert centroid root
    model_roots.insert(model_tree->centroid());
    query_roots.insert(query_tree->centroid());

    //: Curve list 1
    vcl_vector<dbskr_scurve_sptr> curve_list1;
    
    //: Curve list 2
    vcl_vector<dbskr_scurve_sptr> curve_list2;

    //: Map points from curve list 1 to curve list 2
    vcl_vector< vcl_vector < vcl_pair <int,int> > > map_list;

    //: Get path key
    vcl_vector< pathtable_key > path_map;

    // Keep track where tree1 and tree2 have switch
    bool flag=false;

    // Keep vector of costs
    double shape_cost_splice=1.0e6;
    double shape_cost_length=1.0e6;

    //instantiate the edit distance algorithms
    dbskr_tree_edit edit(model_tree.ptr(), 
                         query_tree.ptr(), circular_ends_, 
                         localized_edit_);

    dbskr_edit_distance_base::RootNodeSelection min_root_selection
        (dbskr_edit_distance_base::DEFAULT);

    unsigned int model_tree_branches=model_tree->size()/2;
    unsigned int query_tree_branches=query_tree->size()/2;

    unsigned int model_final_branches(0);
    unsigned int query_final_branches(0);

    vcl_set<int>::iterator it;
    for ( it = model_roots.begin() ; it != model_roots.end() ; ++it)
    {
        vcl_set<int>::iterator bit;
        for ( bit = query_roots.begin() ; bit != query_roots.end() ; ++bit)
        {
            if ( (*it)==0 )
            {

                edit.clear();
                edit.set_tree1(model_tree.ptr());
                edit.set_tree2(query_tree.ptr());
                edit.save_path(true);
                edit.set_curvematching_R(scurve_matching_R_);
                edit.set_use_approx(use_approx_);
                
                dbskr_edit_distance_base::RootNodeSelection value =  
                    dbskr_edit_distance_base::DEFAULT;

                if ( (*bit)==0 )
                {
                    value = dbskr_edit_distance_base::LARGEST_RADIUS_BOTH;
                }
                edit.set_root_node_selection(value);

                if (!edit.edit()) 
                {
                    vcl_cerr << "Problems in editing trees"<<vcl_endl;
                }

                double val = edit.final_cost();
                
                // Only doing by splice cost
                
                // Get splice costs
                // isnan found from math.h
                double model_tree_splice_cost = 
                    ( isnan(model_tree->total_splice_cost()) )
                    ? 0.0 : model_tree->total_splice_cost();
                double query_tree_splice_cost = 
                    ( isnan(query_tree->total_splice_cost()) )
                    ? 0.0 : query_tree->total_splice_cost();

                double model_tree_arc_length = 
                    ( isnan(
                        model_tree->
                        compute_total_reconstructed_boundary_length()) )
                    ? 0.0 : model_tree->
                    compute_total_reconstructed_boundary_length();

                double query_tree_arc_length = 
                    ( isnan(
                        query_tree->
                        compute_total_reconstructed_boundary_length()) )
                    ? 0.0 : query_tree->
                    compute_total_reconstructed_boundary_length();

                double norm_val = val/
                    (model_tree_splice_cost+query_tree_splice_cost );

                double norm_val_length = val/
                    (model_tree_arc_length+query_tree_arc_length );
                
                // vcl_cout << "final cost: " << val 
                //          << " final norm cost: " << norm_val 
                //          << "( tree1 tot splice: " << model_tree_splice_cost
                //          << ", tree2: " << query_tree_splice_cost
                //          << ")" << vcl_endl;

                // vcl_cout<<"Root1 "<<(*it)<<" Root2 "<<(*bit)<<" cost: "
                //         <<norm_val<<vcl_endl;

                if ( norm_val < shape_cost_splice )
                {
                    shape_cost_splice = norm_val;

                    curve_list1.clear();
                    curve_list2.clear();
                    map_list.clear();
                    path_map.clear();

                    min_root_selection=value;

                    // Get correspondece
                    edit.get_correspondence(curve_list1,
                                            curve_list2,
                                            map_list,
                                            path_map);
                    
                    model_final_branches=curve_list1.size();
                    query_final_branches=curve_list2.size();

                    flag=false;
                }
                
                if ( norm_val_length < shape_cost_length )
                {
                    shape_cost_length=norm_val_length;
                }
            }
            else if ( (*it) > 0 )
            {
                
                edit.clear();
                edit.set_tree1(query_tree.ptr());
                edit.set_tree2(model_tree.ptr());
                edit.save_path(true);
                edit.set_curvematching_R(scurve_matching_R_);
                edit.set_use_approx(use_approx_);
            
                dbskr_edit_distance_base::RootNodeSelection value =  
                    dbskr_edit_distance_base::DEFAULT;

                if ( (*bit) > 0 )
                {
                    value= dbskr_edit_distance_base::CENTROID_BOTH;
                }
                edit.set_root_node_selection(value);

                if (!edit.edit()) 
                {
                    vcl_cerr << "Problems in editing trees"<<vcl_endl;
                }

                double val = edit.final_cost();
                
                // Only doing by splice cost
                
                // Get splice costs
                // isnan found from math.h
                double model_tree_splice_cost = 
                    ( isnan(model_tree->total_splice_cost()) )
                    ? 0.0 : model_tree->total_splice_cost();
                double query_tree_splice_cost = 
                    ( isnan(query_tree->total_splice_cost()) )
                    ? 0.0 : query_tree->total_splice_cost();
                
                double model_tree_arc_length = 
                    ( isnan(
                        model_tree->
                        compute_total_reconstructed_boundary_length()) )
                    ? 0.0 : model_tree->
                    compute_total_reconstructed_boundary_length();

                double query_tree_arc_length = 
                    ( isnan(
                        query_tree->
                        compute_total_reconstructed_boundary_length()) )
                    ? 0.0 : query_tree->
                    compute_total_reconstructed_boundary_length();

                double norm_val = val/
                    (model_tree_splice_cost+query_tree_splice_cost );

                double norm_val_length = val/
                    (model_tree_arc_length+query_tree_arc_length );
  
                // vcl_cout << "final cost: " << val 
                //          << " final norm cost: " << norm_val 
                //          << "( tree1 tot splice: " << model_tree_splice_cost
                //          << ", tree2: " << query_tree_splice_cost
                //          << ")" << vcl_endl;
                
                
   
                // vcl_cout<<"Root1 "<<(*it)<<" Root2 "<<(*bit)<<" cost: "
                //         <<norm_val<<vcl_endl;

                if ( norm_val < shape_cost_splice )
                {
                    shape_cost_splice = norm_val;

                    curve_list1.clear();
                    curve_list2.clear();
                    map_list.clear();
                    path_map.clear();

                    min_root_selection=value;

                    // Get correspondece
                    edit.get_correspondence(curve_list1,
                                            curve_list2,
                                            map_list,
                                            path_map);
                    
                    model_final_branches=curve_list2.size();
                    query_final_branches=curve_list1.size();

                    flag=true;
                }
         
                if ( norm_val_length < shape_cost_length )
                {
                    shape_cost_length=norm_val_length;
                }
          
            }

            
        }
    }

    // if ( (model_tree_branches != model_final_branches) 
    //      &&
    //      (query_tree_branches != query_final_branches))
    // {
        
    //     dbskfg_cgraph_directed_tree_sptr model_app_tree = new 
    //         dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
    //                                     scurve_interpolate_ds_, 
    //                                     scurve_matching_R_);
    //     dbskfg_cgraph_directed_tree_sptr query_app_tree = new 
    //         dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
    //                                     scurve_interpolate_ds_, 
    //                                     scurve_matching_R_);
    //     bool f1=model_app_tree->acquire
    //         (model_tree->get_cgraph(), elastic_splice_cost_, 
    //          circular_ends_, combined_edit_,1.0e6);
    //     bool f2=query_app_tree->acquire
    //         (query_tree->get_cgraph(), elastic_splice_cost_, 
    //          circular_ends_, combined_edit_,1.0e6);


    //     //instantiate the edit distance algorithms
    //     dbskr_tree_edit edit_app(model_app_tree.ptr(), 
    //                              query_app_tree.ptr(), 
    //                              circular_ends_, 
    //                              localized_edit_);
        
    //     if ( flag ) 
    //     {
    //         edit_app.set_tree1(query_app_tree.ptr());
    //         edit_app.set_tree2(model_app_tree.ptr());
    //     }

    //     edit_app.save_path(true);
    //     edit_app.set_curvematching_R(scurve_matching_R_);
    //     edit_app.set_use_approx(use_approx_);
    //     edit_app.set_root_node_selection(min_root_selection);
    //     bool flag = edit_app.edit();

    //     curve_list1.clear();
    //     curve_list2.clear();
    //     map_list.clear();
    //     path_map.clear();

    //     edit_app.get_correspondence(curve_list1,
    //                                 curve_list2,
    //                                 map_list,
    //                                 path_map);

    //     model_app_tree=0;
    //     query_app_tree=0;
    // }

    double shape_time = shape_timer.real()/1000.0;
    shape_timer.mark();

    //vcl_cerr<<"************ Shape Time taken: "<<shape_time<<" sec"<<vcl_endl;
    
    if ( app_sift_ )
    {
        vul_timer app_timer;
        app_timer.mark();
        vcl_pair<double,double> app_cost=compute_sift_cost(curve_list1,
                                                           curve_list2,
                                                           map_list,
                                                           path_map,
                                                           flag);
        vcl_pair<double,double> sift_rgb_cost=compute_rgb_sift_cost(curve_list1,
                                                                    curve_list2,
                                                                    map_list,
                                                                    path_map,
                                                                    flag);
        
        double app_time = app_timer.real()/1000.0;
        app_timer.mark();
        
        //vcl_cerr<<"************ App   Time taken: "<<app_time<<" sec"<<vcl_endl;
        app_diff        = app_cost.first;
        norm_app_cost   = app_cost.second;
        rgb_avg_cost    = sift_rgb_cost.second;

    }
   
    norm_shape_cost = shape_cost_splice;
    norm_shape_cost_length=shape_cost_length;

    // vcl_cout<<" Norm Shape Cost: "<<norm_shape_cost<<vcl_endl;
    // vcl_cout<<" Norm App   Cost: "<<norm_app_cost<<vcl_endl;
    // vcl_cout<<" App Cost       : "<<app_diff<<vcl_endl;
    // vcl_cout<<" Norm Rgb Sift Cost    : "<< sift_rgb_cost.second<<vcl_endl;
    // vcl_cout<<" Unnorm Rgb Sift Cost  : "<< sift_rgb_cost.first<<vcl_endl;
}



void dbskfg_match_bag_of_fragments::compute_grad_maps(
    vil_image_resource_sptr& input_image,
    vl_sift_pix** grad_data,
    VlSiftFilt** filter)
{
    vil_image_view<vxl_byte> temp = 
        vil_convert_to_grey_using_rgb_weighting(
            0.2989,
            0.5870,
            0.1140,
            input_image->get_view());

    vil_image_view<vxl_byte> temp2=
        vil_transpose(temp);
    
    vil_image_view<double> image;
    vil_convert_cast(temp2,image);

    unsigned int width  = image.ni();
    unsigned int height = image.nj();

    double* gradient_magnitude = (double*) 
        vl_malloc(width*height*sizeof(double));
    double* gradient_angle     = (double*) 
        vl_malloc(width*height*sizeof(double));

    double* image_data=image.top_left_ptr();

    vl_imgradient_polar_d(
        gradient_magnitude, // gradient magnitude 
        gradient_angle,     // gradient angle
        1,                  // output width
        width,              // output height
        image_data,         // input image
        width,              // input image width
        height,             // input image height
        width);             // input image stride

    *filter = vl_sift_new(width,height,3,3,0);
    *grad_data=(vl_sift_pix*) vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    unsigned int index=0;
    for ( unsigned int i=0; i < width*height; ++i)
    {
        double value=gradient_magnitude[i];
        (*grad_data)[index]=value;
        ++index;
    }

    for ( unsigned int i=0; i < width*height; ++i)
    {
        double value=gradient_angle[i];
        (*grad_data)[index]=value;
        ++index;
    }

    vl_free(gradient_magnitude);
    vl_free(gradient_angle);

}

void dbskfg_match_bag_of_fragments::compute_grad_color_maps(
    vil_image_resource_sptr& input_image,
    vl_sift_pix** grad_data,
    unsigned int channel)
{
    vil_image_view<vxl_byte> imview = input_image->get_view();
    vil_image_view<vxl_byte> temp   = vil_plane(imview,channel);
    vil_image_view<vxl_byte> temp2  = vil_transpose(temp);
    
    vil_image_view<double> image;
    vil_convert_cast(temp2,image);

    unsigned int width  = image.ni();
    unsigned int height = image.nj();

    double* gradient_magnitude = (double*) 
        vl_malloc(width*height*sizeof(double));
    double* gradient_angle     = (double*) 
        vl_malloc(width*height*sizeof(double));

    double* image_data=image.top_left_ptr();

    vl_imgradient_polar_d(
        gradient_magnitude, // gradient magnitude 
        gradient_angle,     // gradient angle
        1,                  // output width
        width,              // output height
        image_data,         // input image
        width,              // input image width
        height,             // input image height
        width);             // input image stride

    *grad_data=(vl_sift_pix*) vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    unsigned int index=0;
    for ( unsigned int i=0; i < width*height; ++i)
    {
        double value=gradient_magnitude[i];
        (*grad_data)[index]=value;
        ++index;
    }

    for ( unsigned int i=0; i < width*height; ++i)
    {
        double value=gradient_angle[i];
        (*grad_data)[index]=value;
        ++index;
    }

    vl_free(gradient_magnitude);
    vl_free(gradient_angle);

}

vcl_pair<double,double> dbskfg_match_bag_of_fragments::compute_sift_cost(
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_vector< pathtable_key >& path_map,
    bool flag)
{
 

    VlFloatVectorComparisonFunction L2_distance =    
      vl_get_vector_comparison_function_f (VlDistanceL2) ;
    double sift_diff= 0.0;
    
    double arclength_shock_curve1=0.0;
    double arclength_shock_curve2=0.0;
    
    // Get matching pairs
    for (unsigned i = 0; i < map_list.size(); i++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];

        double local_distance=0.0;

        vcl_vector< vcl_vector<vl_sift_pix> > model_sift;
        vcl_vector< vcl_vector<vl_sift_pix> > query_sift;
        
        vcl_vector<vgl_point_2d<double> > shock_curve1;
        vcl_vector<vgl_point_2d<double> > shock_curve2;

        double local_arclength_shock_curve1=0.0;
        double local_arclength_shock_curve2=0.0;

        for (unsigned j = 0; j < map_list[i].size(); ++j) 
        {
            vcl_pair<int, int> cor = map_list[i][j];

            // Compute sift for both images
           
            // Shock Point 1 from Model
            vgl_point_2d<double> ps1 = sc1->sh_pt(cor.first);
            double radius_ps1        = sc1->time(cor.first);
            double theta_ps1         = sc1->theta(cor.first);
            vl_sift_pix descr_ps1[128];
            memset(descr_ps1, 0, sizeof(vl_sift_pix)*128);
            shock_curve1.push_back(ps1);

            // Shock Point 2 from Query
            vgl_point_2d<double> ps2 = sc2->sh_pt(cor.second);
            double radius_ps2        = sc2->time(cor.second);
            double theta_ps2         = sc2->theta(cor.second);
            vl_sift_pix descr_ps2[128];
            memset(descr_ps2, 0, sizeof(vl_sift_pix)*128);
            shock_curve2.push_back(ps2);

            if ( !flag )
            {
                vl_sift_calc_raw_descriptor(model_sift_filter_,
                                            model_grad_data_,
                                            descr_ps1,
                                            model_sift_filter_->width,
                                            model_sift_filter_->height,
                                            ps1.y(),
                                            ps1.x(),
                                            radius_ps1/2,
                                            theta_ps1);

                vl_sift_calc_raw_descriptor(query_sift_filter_,
                                            query_grad_data_,
                                            descr_ps2,
                                            query_sift_filter_->width,
                                            query_sift_filter_->height,
                                            ps2.y(),
                                            ps2.x(),
                                            radius_ps2/2,
                                            theta_ps2);
            }
            else
            {
                vl_sift_calc_raw_descriptor(model_sift_filter_,
                                            model_grad_data_,
                                            descr_ps2,
                                            model_sift_filter_->width,
                                            model_sift_filter_->height,
                                            ps2.y(),
                                            ps2.x(),
                                            radius_ps2/2,
                                            theta_ps2);

                vl_sift_calc_raw_descriptor(query_sift_filter_,
                                            query_grad_data_,
                                            descr_ps1,
                                            query_sift_filter_->width,
                                            query_sift_filter_->height,
                                            ps1.y(),
                                            ps1.x(),
                                            radius_ps1/2,
                                            theta_ps1);
          


            }

            vcl_vector<vl_sift_pix> descr_vec1;
            descr_vec1.assign(descr_ps1,descr_ps1+128);
            descr_vec1.push_back(ps1.x());
            descr_vec1.push_back(ps1.y());
            descr_vec1.push_back(radius_ps1);
            descr_vec1.push_back(theta_ps1);

            vcl_vector<vl_sift_pix> descr_vec2;    
            descr_vec2.assign(descr_ps2,descr_ps2+128);
            descr_vec2.push_back(ps2.x());
            descr_vec2.push_back(ps2.y());
            descr_vec2.push_back(radius_ps2);
            descr_vec2.push_back(theta_ps2);
        
            vl_sift_pix result[1];

            vl_eval_vector_comparison_on_all_pairs_f(result,
                                                     128,
                                                     descr_ps1,
                                                     1,
                                                     descr_ps2,
                                                     1,
                                                     L2_distance);

            sift_diff=sift_diff+result[0];
            local_distance=local_distance+result[0];

            model_sift.push_back(descr_vec1);
            query_sift.push_back(descr_vec2);

            if ( j > 0 )
            {
                local_arclength_shock_curve1=
                    vgl_distance(shock_curve1[j],shock_curve1[j-1])+
                    local_arclength_shock_curve1;

                local_arclength_shock_curve2=
                    vgl_distance(shock_curve2[j],shock_curve2[j-1])+
                    local_arclength_shock_curve2;

            }
        }
        
        arclength_shock_curve1=
            local_arclength_shock_curve1+arclength_shock_curve1;

        arclength_shock_curve2=
            local_arclength_shock_curve2+arclength_shock_curve2;

        // vcl_cout<<"Tree 1 dart ("
        //         <<path_map[i].first.first
        //         <<","
        //         <<path_map[i].first.second
        //         <<") Tree 2 dart ("
        //         <<path_map[i].second.first
        //         <<","
        //         <<path_map[i].second.second
        //         <<") L2 distance: "
        //         <<local_distance<<vcl_endl;
       
        // vcl_cout<<"Tree 1 dart ("
        //         <<path_map[i].first.first
        //         <<","
        //         <<path_map[i].first.second
        //         <<") length: "
        //         <<local_arclength_shock_curve1
        //         <<" Tree 2 dart ("
        //         <<path_map[i].second.first
        //         <<","
        //         <<path_map[i].second.second
        //         <<") length: "
        //         <<local_arclength_shock_curve2
        //         <<vcl_endl;

        // vcl_cout<<vcl_endl;

        // {
        //     vcl_stringstream model_stream;
        //     model_stream<<"Dart_model_"<<i<<"_app_correspondence.txt";
        //     vcl_stringstream query_stream;
        //     query_stream<<"Dart_query_"<<i<<"_app_correspondence.txt";

        //     vcl_ofstream model_file(model_stream.str().c_str());
        //     model_file<<model_sift.size()<<vcl_endl;
        //     for ( unsigned int b=0; b < model_sift.size() ; ++b)
        //     {
        //         vcl_vector<vl_sift_pix> vec=model_sift[b];
        //         for  ( unsigned int c=0; c < vec.size() ; ++c)
        //         {
        //             model_file<<vec[c]<<vcl_endl;
        //         }
        //     }
        //     model_file.close();

        //     vcl_ofstream query_file(query_stream.str().c_str());
        //     query_file<<query_sift.size()<<vcl_endl;
        //     for ( unsigned int b=0; b < query_sift.size() ; ++b)
        //     {
        //         vcl_vector<vl_sift_pix> vec=query_sift[b];
        //         for  ( unsigned int c=0; c < vec.size() ; ++c)
        //         {
        //             query_file<<vec[c]<<vcl_endl;
        //         }
        //     }
        //     query_file.close();
 
        //}
    }

    double norm_val=sift_diff/(arclength_shock_curve1+
                               arclength_shock_curve2);

    // vcl_cout << "final cost: " << sift_diff 
    //          << " final norm cost: " << norm_val 
    //          << "( tree1 total length: " << arclength_shock_curve1
    //          << ", tree2 total length: " << arclength_shock_curve2
    //          << ")" << vcl_endl;

    vcl_pair<double,double> app_diff(sift_diff,norm_val);
    return app_diff;
}

vcl_pair<double,double> dbskfg_match_bag_of_fragments::compute_rgb_sift_cost(
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_vector< pathtable_key >& path_map,
    bool flag)
{
 

    VlFloatVectorComparisonFunction L2_distance =    
      vl_get_vector_comparison_function_f (VlDistanceL2) ;
    double sift_diff= 0.0;
    
    double arclength_shock_curve1=0.0;
    double arclength_shock_curve2=0.0;
    unsigned int overall_index=0;
    // Get matching pairs
    for (unsigned i = 0; i < map_list.size(); i++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];

        double local_distance=0.0;

        vcl_vector< vcl_vector<vl_sift_pix> > model_sift;
        vcl_vector< vcl_vector<vl_sift_pix> > query_sift;
        
        vcl_vector<vgl_point_2d<double> > shock_curve1;
        vcl_vector<vgl_point_2d<double> > shock_curve2;

        double local_arclength_shock_curve1=0.0;
        double local_arclength_shock_curve2=0.0;

        for (unsigned j = 0; j < map_list[i].size(); ++j) 
        {
            vcl_pair<int, int> cor = map_list[i][j];
            
            // Compute sift for both images
           
            // Shock Point 1 from Model
            vgl_point_2d<double> ps1_red = sc1->sh_pt(cor.first);
            double radius_ps1_red        = sc1->time(cor.first);
            double theta_ps1_red         = sc1->theta(cor.first);
            vl_sift_pix descr_ps1_red[128];
            memset(descr_ps1_red, 0, sizeof(vl_sift_pix)*128);
            shock_curve1.push_back(ps1_red);
      
            vgl_point_2d<double> ps1_green = sc1->sh_pt(cor.first);
            double radius_ps1_green        = sc1->time(cor.first);
            double theta_ps1_green         = sc1->theta(cor.first);
            vl_sift_pix descr_ps1_green[128];
            memset(descr_ps1_green, 0, sizeof(vl_sift_pix)*128);
            shock_curve1.push_back(ps1_green);

            vgl_point_2d<double> ps1_blue = sc1->sh_pt(cor.first);
            double radius_ps1_blue        = sc1->time(cor.first);
            double theta_ps1_blue         = sc1->theta(cor.first);
            vl_sift_pix descr_ps1_blue[128];
            memset(descr_ps1_blue, 0, sizeof(vl_sift_pix)*128);
            shock_curve1.push_back(ps1_blue);

            // Shock Point 2 from Query
            vgl_point_2d<double> ps2_red = sc2->sh_pt(cor.second);
            double radius_ps2_red        = sc2->time(cor.second);
            double theta_ps2_red         = sc2->theta(cor.second);
            vl_sift_pix descr_ps2_red[128];
            memset(descr_ps2_red, 0, sizeof(vl_sift_pix)*128);
            shock_curve2.push_back(ps2_red);

            vgl_point_2d<double> ps2_green = sc2->sh_pt(cor.second);
            double radius_ps2_green        = sc2->time(cor.second);
            double theta_ps2_green         = sc2->theta(cor.second);
            vl_sift_pix descr_ps2_green[128];
            memset(descr_ps2_green, 0, sizeof(vl_sift_pix)*128);
            shock_curve2.push_back(ps2_green);

            vgl_point_2d<double> ps2_blue = sc2->sh_pt(cor.second);
            double radius_ps2_blue        = sc2->time(cor.second);
            double theta_ps2_blue         = sc2->theta(cor.second);
            vl_sift_pix descr_ps2_blue[128];
            memset(descr_ps2_blue, 0, sizeof(vl_sift_pix)*128);
            shock_curve2.push_back(ps2_blue);

            if ( !flag )
            {
                vl_sift_calc_raw_descriptor(model_sift_filter_,
                                            model_grad_red_data_,
                                            descr_ps1_red,
                                            model_sift_filter_->width,
                                            model_sift_filter_->height,
                                            ps1_red.y(),
                                            ps1_red.x(),
                                            radius_ps1_red/2,
                                            theta_ps1_red);

                vl_sift_calc_raw_descriptor(model_sift_filter_,
                                            model_grad_green_data_,
                                            descr_ps1_green,
                                            model_sift_filter_->width,
                                            model_sift_filter_->height,
                                            ps1_green.y(),
                                            ps1_green.x(),
                                            radius_ps1_green/2,
                                            theta_ps1_green);

                vl_sift_calc_raw_descriptor(model_sift_filter_,
                                            model_grad_blue_data_,
                                            descr_ps1_blue,
                                            model_sift_filter_->width,
                                            model_sift_filter_->height,
                                            ps1_blue.y(),
                                            ps1_blue.x(),
                                            radius_ps1_blue/2,
                                            theta_ps1_blue);

                vl_sift_calc_raw_descriptor(query_sift_filter_,
                                            query_grad_red_data_,
                                            descr_ps2_red,
                                            query_sift_filter_->width,
                                            query_sift_filter_->height,
                                            ps2_red.y(),
                                            ps2_red.x(),
                                            radius_ps2_red/2,
                                            theta_ps2_red);

                vl_sift_calc_raw_descriptor(query_sift_filter_,
                                            query_grad_green_data_,
                                            descr_ps2_green,
                                            query_sift_filter_->width,
                                            query_sift_filter_->height,
                                            ps2_green.y(),
                                            ps2_green.x(),
                                            radius_ps2_green/2,
                                            theta_ps2_green);

                vl_sift_calc_raw_descriptor(query_sift_filter_,
                                            query_grad_blue_data_,
                                            descr_ps2_blue,
                                            query_sift_filter_->width,
                                            query_sift_filter_->height,
                                            ps2_blue.y(),
                                            ps2_blue.x(),
                                            radius_ps2_blue/2,
                                            theta_ps2_blue);



            }
            else
            {
                vl_sift_calc_raw_descriptor(model_sift_filter_,
                                            model_grad_red_data_,
                                            descr_ps2_red,
                                            model_sift_filter_->width,
                                            model_sift_filter_->height,
                                            ps2_red.y(),
                                            ps2_red.x(),
                                            radius_ps2_red/2,
                                            theta_ps2_red);

                vl_sift_calc_raw_descriptor(model_sift_filter_,
                                            model_grad_green_data_,
                                            descr_ps2_green,
                                            model_sift_filter_->width,
                                            model_sift_filter_->height,
                                            ps2_green.y(),
                                            ps2_green.x(),
                                            radius_ps2_green/2,
                                            theta_ps2_green);

                vl_sift_calc_raw_descriptor(model_sift_filter_,
                                            model_grad_blue_data_,
                                            descr_ps2_blue,
                                            model_sift_filter_->width,
                                            model_sift_filter_->height,
                                            ps2_blue.y(),
                                            ps2_blue.x(),
                                            radius_ps2_blue/2,
                                            theta_ps2_blue);

                vl_sift_calc_raw_descriptor(query_sift_filter_,
                                            query_grad_red_data_,
                                            descr_ps1_red,
                                            query_sift_filter_->width,
                                            query_sift_filter_->height,
                                            ps1_red.y(),
                                            ps1_red.x(),
                                            radius_ps1_red/2,
                                            theta_ps1_red);

                vl_sift_calc_raw_descriptor(query_sift_filter_,
                                            query_grad_green_data_,
                                            descr_ps1_green,
                                            query_sift_filter_->width,
                                            query_sift_filter_->height,
                                            ps1_green.y(),
                                            ps1_green.x(),
                                            radius_ps1_green/2,
                                            theta_ps1_green);

                vl_sift_calc_raw_descriptor(query_sift_filter_,
                                            query_grad_blue_data_,
                                            descr_ps1_blue,
                                            query_sift_filter_->width,
                                            query_sift_filter_->height,
                                            ps1_blue.y(),
                                            ps1_blue.x(),
                                            radius_ps1_blue/2,
                                            theta_ps1_blue);

          


            }

            vcl_vector<vl_sift_pix> descr_vec1_red;
            descr_vec1_red.assign(descr_ps1_red,descr_ps1_red+128);
            descr_vec1_red.push_back(ps1_red.x());
            descr_vec1_red.push_back(ps1_red.y());
            descr_vec1_red.push_back(radius_ps1_red);
            descr_vec1_red.push_back(theta_ps1_red);

            vcl_vector<vl_sift_pix> descr_vec1_green;
            descr_vec1_green.assign(descr_ps1_green,descr_ps1_green+128);
            descr_vec1_green.push_back(ps1_green.x());
            descr_vec1_green.push_back(ps1_green.y());
            descr_vec1_green.push_back(radius_ps1_green);
            descr_vec1_green.push_back(theta_ps1_green);

            vcl_vector<vl_sift_pix> descr_vec1_blue;
            descr_vec1_blue.assign(descr_ps1_blue,descr_ps1_blue+128);
            descr_vec1_blue.push_back(ps1_blue.x());
            descr_vec1_blue.push_back(ps1_blue.y());
            descr_vec1_blue.push_back(radius_ps1_blue);
            descr_vec1_blue.push_back(theta_ps1_blue);

            vcl_vector<vl_sift_pix> descr_vec2_red;    
            descr_vec2_red.assign(descr_ps2_red,descr_ps2_red+128);
            descr_vec2_red.push_back(ps2_red.x());
            descr_vec2_red.push_back(ps2_red.y());
            descr_vec2_red.push_back(radius_ps2_red);
            descr_vec2_red.push_back(theta_ps2_red);
        
            vcl_vector<vl_sift_pix> descr_vec2_green;    
            descr_vec2_green.assign(descr_ps2_green,descr_ps2_green+128);
            descr_vec2_green.push_back(ps2_green.x());
            descr_vec2_green.push_back(ps2_green.y());
            descr_vec2_green.push_back(radius_ps2_green);
            descr_vec2_green.push_back(theta_ps2_green);
        
            vcl_vector<vl_sift_pix> descr_vec2_blue;    
            descr_vec2_blue.assign(descr_ps2_blue,descr_ps2_blue+128);
            descr_vec2_blue.push_back(ps2_blue.x());
            descr_vec2_blue.push_back(ps2_blue.y());
            descr_vec2_blue.push_back(radius_ps2_blue);
            descr_vec2_blue.push_back(theta_ps2_blue);
        
            vl_sift_pix result_red[1];
            vl_sift_pix result_green[1];
            vl_sift_pix result_blue[1];

            vl_eval_vector_comparison_on_all_pairs_f(result_red,
                                                     128,
                                                     descr_ps1_red,
                                                     1,
                                                     descr_ps2_red,
                                                     1,
                                                     L2_distance);

            vl_eval_vector_comparison_on_all_pairs_f(result_green,
                                                     128,
                                                     descr_ps1_green,
                                                     1,
                                                     descr_ps2_green,
                                                     1,
                                                     L2_distance);

            vl_eval_vector_comparison_on_all_pairs_f(result_blue,
                                                     128,
                                                     descr_ps1_blue,
                                                     1,
                                                     descr_ps2_blue,
                                                     1,
                                                     L2_distance);


            sift_diff=sift_diff+result_red[0]+result_green[0]+result_blue[0];
            local_distance=local_distance+
                result_red[0]+result_green[0]+result_blue[0];

            model_sift.push_back(descr_vec1_red);
            query_sift.push_back(descr_vec2_red);

            if ( j > 0 )
            {
                local_arclength_shock_curve1=
                    vgl_distance(shock_curve1[j],shock_curve1[j-1])+
                    local_arclength_shock_curve1;

                local_arclength_shock_curve2=
                    vgl_distance(shock_curve2[j],shock_curve2[j-1])+
                    local_arclength_shock_curve2;

            }
            overall_index++;
        }
        
        arclength_shock_curve1=
            local_arclength_shock_curve1+arclength_shock_curve1;

        arclength_shock_curve2=
            local_arclength_shock_curve2+arclength_shock_curve2;

        // vcl_cout<<"Tree 1 dart ("
        //         <<path_map[i].first.first
        //         <<","
        //         <<path_map[i].first.second
        //         <<") Tree 2 dart ("
        //         <<path_map[i].second.first
        //         <<","
        //         <<path_map[i].second.second
        //         <<") L2 distance: "
        //         <<local_distance<<vcl_endl;
       
        // vcl_cout<<"Tree 1 dart ("
        //         <<path_map[i].first.first
        //         <<","
        //         <<path_map[i].first.second
        //         <<") length: "
        //         <<local_arclength_shock_curve1
        //         <<" Tree 2 dart ("
        //         <<path_map[i].second.first
        //         <<","
        //         <<path_map[i].second.second
        //         <<") length: "
        //         <<local_arclength_shock_curve2
        //         <<vcl_endl;

        // vcl_cout<<vcl_endl;

        // {
        //     vcl_stringstream model_stream;
        //     model_stream<<"Dart_model_"<<i<<"_app_correspondence.txt";
        //     vcl_stringstream query_stream;
        //     query_stream<<"Dart_query_"<<i<<"_app_correspondence.txt";

        //     vcl_ofstream model_file(model_stream.str().c_str());
        //     model_file<<model_sift.size()<<vcl_endl;
        //     for ( unsigned int b=0; b < model_sift.size() ; ++b)
        //     {
        //         vcl_vector<vl_sift_pix> vec=model_sift[b];
        //         for  ( unsigned int c=0; c < vec.size() ; ++c)
        //         {
        //             model_file<<vec[c]<<vcl_endl;
        //         }
        //     }
        //     model_file.close();

        //     vcl_ofstream query_file(query_stream.str().c_str());
        //     query_file<<query_sift.size()<<vcl_endl;
        //     for ( unsigned int b=0; b < query_sift.size() ; ++b)
        //     {
        //         vcl_vector<vl_sift_pix> vec=query_sift[b];
        //         for  ( unsigned int c=0; c < vec.size() ; ++c)
        //         {
        //             query_file<<vec[c]<<vcl_endl;
        //         }
        //     }
        //     query_file.close();
 
        //}   
    }

    double norm_val=sift_diff/(overall_index);

    // vcl_cout << "final cost: " << sift_diff 
    //          << " final norm cost: " << norm_val 
    //          << "( tree1 total length: " << arclength_shock_curve1
    //          << ", tree2 total length: " << arclength_shock_curve2
    //          << ")" << vcl_endl;

    vcl_pair<double,double> app_diff(sift_diff,norm_val);
    return app_diff;
}

vnl_vector<double> dbskfg_match_bag_of_fragments::compute_second_order_pooling(
    vcl_map<int,vcl_vector<dbskfg_sift_data> >& fragments,
    vl_sift_pix* grad_data,
    VlSiftFilt* filter,
    vsol_box_2d_sptr& bbox
)
{


    unsigned int total_numb_descriptors=0;
    vcl_map<int,vcl_vector<dbskfg_sift_data> >::iterator kit;
    for ( kit = fragments.begin() ; kit != fragments.end() ; ++kit)
    {
        total_numb_descriptors=(*kit).second.size()+total_numb_descriptors;
    }

    vgl_point_2d<double> corner=vgl_point_2d<double>(bbox->get_min_x(),
                                                     bbox->get_min_y());
    double width=bbox->width();
    double height=bbox->height();

    vcl_map<unsigned int,vcl_vector<vl_sift_pix> > descriptors;
    vnl_matrix<vl_sift_pix> total_matrix(132,total_numb_descriptors,0.0);

    unsigned int index=0;
    vcl_map<int,vcl_vector<dbskfg_sift_data> >::iterator it;
    for ( it = fragments.begin() ; it != fragments.end() ; ++it)
    {
        vcl_vector<dbskfg_sift_data> data=(*it).second;
        for ( unsigned int i=0; i < data.size() ; ++i)
        {
            // Shock Point 2 from Query
            vgl_point_2d<double> pt = data[i].location_;
            double radius           = data[i].radius_;
            double theta            = data[i].phi_;

            double d1=(pt.x()-corner.x())/width;
            double d2=(pt.x()-corner.x())/height;
            double d3=(pt.y()-corner.y())/width;
            double d4=(pt.y()-corner.y())/height;

            //double min_x =
            vl_sift_pix descr[132];
            memset(descr, 0, sizeof(vl_sift_pix)*132);

            vl_sift_calc_raw_descriptor(filter,
                                        grad_data,
                                        descr,
                                        filter->width,
                                        filter->height,
                                        pt.y(),
                                        pt.x(),
                                        radius/2,
                                        theta);

            for ( unsigned int dv=0; dv < 128; ++dv)
            {
                descr[dv]=vcl_min(512.0F * descr[dv], 255.0F);
            }
         
            descr[128]=d1;
            descr[129]=d2;
            descr[130]=d3;
            descr[131]=d4;

            total_matrix.set_column(index,descr);
            vcl_vector<vl_sift_pix> descr_vec;
            descr_vec.assign(descr,descr+132);
            descriptors[index]=descr_vec;

            index++;
        } 
        
    }
    
    vnl_matrix<vl_sift_pix> total_matrix_transpose=total_matrix.transpose();

    vnl_matrix<vl_sift_pix> second_order_pool = 
        (total_matrix*total_matrix_transpose)/(total_numb_descriptors);

    // Numeric scaling
    vnl_diag_matrix<vl_sift_pix> small_scaling(132,0.001);
    second_order_pool=second_order_pool+small_scaling;

    vnl_symmetric_eigensystem<vl_sift_pix> eigensystem(second_order_pool);
    vnl_vector<vl_sift_pix> eigenvalues=(eigensystem.D).get_diagonal();

    for ( unsigned int e=0; e < eigenvalues.size() ; ++e)
    {
        vl_sift_pix value=eigenvalues.get(e);
        eigenvalues.put(e,vcl_log(value));
    } 
    
    (eigensystem.D).set(eigenvalues);
    vnl_matrix<vl_sift_pix> log_mapping=eigensystem.recompose();

    vnl_vector<double> upper_triangle(8778,0);

    unsigned int position=0;
    
    // Get upper triangle portion
    for ( unsigned int c=0; c < log_mapping.cols() ; ++c)
    {
        for ( unsigned int r=0; r < c+1 ; ++r)
        {
            double scaled_value=vnl_math_sgn(log_mapping[r][c])*
                vcl_pow(vcl_abs(log_mapping[r][c]),0.75);
            upper_triangle.put(position,scaled_value);
            position++;
        }
    }
    
    // {
    //     vcl_ofstream model_file("All_sift.txt");
    //     model_file<<descriptors.size()<<vcl_endl;
    //     vcl_map<unsigned int,vcl_vector<vl_sift_pix> >::iterator dit;
    //     for ( dit = descriptors.begin() ; dit != descriptors.end() ; ++dit)
    //     {
    //         vcl_vector<vl_sift_pix> vec=(*dit).second;
    //         for  ( unsigned int c=0; c < vec.size() ; ++c)
    //         {
    //             model_file<<vec[c]<<vcl_endl;
    //         }
    //     }
    //     model_file.close();
    
    // }


    // {
    //     vcl_ofstream model_file("sift2.txt");
        
    //     for ( unsigned int c=0; c < second_order_pool.columns() ; ++c)
    //     {
    //         for ( unsigned int r=0; r < second_order_pool.rows() ; ++r)
    //         {
    //             model_file<<second_order_pool[r][c]<<vcl_endl;
    //         }
    //     }

    //     model_file.close();
    
    // }

    // {
    //     vcl_ofstream model_file("final_desc.txt");
        
    //     for ( unsigned int c=0; c < upper_triangle.size() ; ++c)
    //     {
    //         model_file<<upper_triangle(c)<<vcl_endl;
    //     }

    //     model_file.close();
    
    // }

    return upper_triangle;
}
