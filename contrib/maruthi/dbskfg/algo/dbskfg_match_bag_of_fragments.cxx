// This is brcv/shp/dbskfg/algo/dbskfg_match_bag_of_fragments.cxx

//:
// \file
#include <dbskfg/algo/dbskfg_match_bag_of_fragments.h>
#include <dbskfg/algo/dbskfg_cgraph_directed_tree.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/pro/dbskfg_match_composite_graph_process.h>
#include <dbskfg/pro/dbskfg_load_composite_graph_process.h>
#include <dbskfg/pro/dbskfg_load_binary_composite_graph_process.h>
#include <dbskfg/algo/dbskfg_cg_tree_edit.h>
#include <dbskfg/dbskfg_utilities.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vsol/vsol_box_2d.h>
#include <bxml/bxml_write.h>
#include <dbxml/dbxml_algos.h>
#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/algo/dbskfg_composite_graph_fileio.h>
#include <vcl_algorithm.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <dbsk2d/dbsk2d_file_io.h>

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vil/vil_transpose.h>
extern "C" {
#include <vl/imopv.h>
#include <vl/generic.h>
#include <vl/mathop.h>
}

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_distance.h>
#include <string.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_vector.h>

#include <vnl/algo/vnl_symmetric_eigensystem.h>

#include <rgrl/rgrl_est_similarity2d.h>
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_trans_similarity.h>

#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include <bbas/bsol/bsol_intrinsic_curve_2d_sptr.h>
#include <bbas/bsol/bsol_intrinsic_curve_2d.h>

#include <dbcvr/dbcvr_clsd_cvmatch.h>
#include <dbcvr/dbcvr_clsd_cvmatch_sptr.h>

#include <bsol/bsol_algs.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>

#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>

#include <dbsol/dbsol_file_io.h>

#include <vidpro1/storage/vidpro1_image_storage.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

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
    bool scale_area,
    bool scale_length,
    bool app_sift,
    bool mirror,
    bool outside_shock,
    double area_weight,
    double ref_area,
    ShapeAlgorithmArea shape_alg,
    ColorSpace color_space,
    vil_image_resource_sptr model_image,
    vil_image_resource_sptr query_image,
    vcl_string model_image_path
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
      scale_area_(scale_area),
      scale_length_(scale_length),
      app_sift_(app_sift),
      mirror_(mirror),
      outside_shock_(outside_shock),
      area_weight_(area_weight),
      ref_area_(ref_area),
      shape_alg_(shape_alg),
      color_space_(color_space),
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
    
    // Load multiple images
    if ( model_image_path.size())
    {
        unsigned int index=0;
        vcl_ifstream myfile (model_image_path.c_str());
        if (myfile.is_open())
        {
            vcl_string line;
            while ( vcl_getline (myfile,line) )
            {
                vil_image_resource_sptr model_img_sptr = 
                    vil_load_image_resource(line.c_str());
                vcl_stringstream title_stream;
                title_stream<<"model_"<<index;
                
                vl_sift_pix* grad_data(0);
                vl_sift_pix* red_data(0);
                vl_sift_pix* blue_data(0);
                vl_sift_pix* green_data(0);
                VlSiftFilt* sift_filter(0);

                compute_grad_maps(model_img_sptr,
                                  &grad_data,
                                  &sift_filter);

                vil_image_view<double> o1,o2,o3;
                convert_to_color_space(model_img_sptr,o1,o2,o3,color_space_);

                compute_grad_color_maps(o1,
                                        &red_data);
                
                compute_grad_color_maps(o2,
                                        &green_data);
                
                compute_grad_color_maps(o3,
                                        &blue_data);

                vl_sift_set_magnif(sift_filter,1.0);

                model_images_grad_data_[title_stream.str()]=grad_data;
                model_images_sift_filter_[title_stream.str()]=sift_filter;
                model_images_grad_data_red_[title_stream.str()]=red_data;
                model_images_grad_data_green_[title_stream.str()]=green_data;
                model_images_grad_data_blue_[title_stream.str()]=blue_data;

                ++index;
            }
        }
    }

    if ( scale_bbox_)
    {
        binary_sim_matrix_.set_size(model_contours_.size(),
                                    query_contours_.size());
        binary_sim_matrix_.fill(0.0);

        binary_sim_length_matrix_.set_size(model_contours_.size(),
                                           query_contours_.size());
        binary_sim_length_matrix_.fill(0.0);

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
        binary_sim_matrix_.fill(0.0);

        binary_sim_length_matrix_.set_size(model_fragments_.size(),
                                           query_fragments_.size());
        binary_sim_length_matrix_.fill(0.0);

        binary_app_sim_matrix_.set_size(model_fragments_.size(),
                                        query_fragments_.size());

        binary_app_norm_sim_matrix_.set_size(model_fragments_.size(),
                                             query_fragments_.size());

        binary_app_rgb_sim_matrix_.set_size(model_fragments_.size(),
                                            query_fragments_.size());


    }

    if ( app_sift_)
    {
        if ( model_image_ )
        {
            compute_grad_maps(model_image_,
                              &model_grad_data_,
                              &model_sift_filter_);
            
            vil_image_view<double> o1,o2,o3;
            convert_to_color_space(model_image_,o1,o2,o3,color_space_);
            
            compute_grad_color_maps(o1,
                                    &model_grad_red_data_);
            
            compute_grad_color_maps(o2,
                                    &model_grad_green_data_);
            
            compute_grad_color_maps(o3,
                                    &model_grad_blue_data_);

            vl_sift_set_magnif(model_sift_filter_,1.0);

        }

        if ( query_image_ )
        {
            vcl_cout<<"Computing query image grad data"<<vcl_endl;

            compute_grad_maps(query_image_,
                              &query_grad_data_,
                              &query_sift_filter_);

            vil_image_view<double> o1,o2,o3;
            convert_to_color_space(query_image_,o1,o2,o3,color_space_);
            
            compute_grad_color_maps(o1,
                                    &query_grad_red_data_);
            
            compute_grad_color_maps(o2,
                                    &query_grad_green_data_);
            
            compute_grad_color_maps(o3,
                                    &query_grad_blue_data_);
            
            vl_sift_set_magnif(query_sift_filter_,1.0);

        }

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
    load_pro.parameters()->set_value( "-outside_shock" ,outside_shock_);

    bool flag=load_pro.execute();
    load_pro.finish();

    vcl_map<unsigned int,dbskfg_composite_graph_sptr> cgraphs = 
        load_pro.get_cgraphs();
    vcl_map<unsigned int,double> cgraph_area = load_pro.get_cgraph_area();
    vcl_map<unsigned int,double> cgraph_length = load_pro.get_cgraph_length();

    vcl_map<unsigned int,dbskfg_composite_graph_sptr>::iterator it;
    for ( it = cgraphs.begin() ; it != cgraphs.end() ; ++it)
    {
        vcl_stringstream stream;
        stream<<"model_"<<(*it).first;
        model_fragments_[(*it).first]=vcl_make_pair(stream.str(),(*it).second);
        model_fragments_area_[(*it).first]=vcl_make_pair(
            stream.str(),
            cgraph_area[(*it).first]);
        model_fragments_length_[(*it).first]=vcl_make_pair(
            stream.str(),
            cgraph_length[(*it).first]);

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
    load_pro.parameters()->set_value( "-outside_shock" ,outside_shock_);

    bool flag=load_pro.execute();
    load_pro.finish();

    vcl_map<unsigned int,dbskfg_composite_graph_sptr> cgraphs = 
        load_pro.get_cgraphs();
    vcl_map<unsigned int,double> cgraph_area = load_pro.get_cgraph_area();
    vcl_map<unsigned int,double> cgraph_length = load_pro.get_cgraph_length();

    vcl_map<unsigned int,dbskfg_composite_graph_sptr>::iterator it;
    for ( it = cgraphs.begin() ; it != cgraphs.end() ; ++it)
    {
        vcl_stringstream stream;
        stream<<"query_"<<(*it).first;
        query_fragments_[(*it).first]=vcl_make_pair(stream.str(),(*it).second);
        query_fragments_area_[(*it).first]=vcl_make_pair(
            stream.str(),
            cgraph_area[(*it).first]);
        query_fragments_length_[(*it).first]=vcl_make_pair(
            stream.str(),
            cgraph_length[(*it).first]);

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

    if ( !mirror_)
    {
        vcl_cout<<"Matching "
                <<model_fragments_.size()
                <<" model fragments to "
                <<query_fragments_.size()
                <<" query fragments"
                <<vcl_endl;
    }
    else
    {
        vcl_cout<<"Matching "
                <<model_fragments_.size()
                <<" model fragments to "
                <<query_fragments_.size()
                <<" query fragments with horizontal mirroring of query"
                <<vcl_endl;

    }

    // Loop over model and query
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator m_iterator;
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator q_iterator;

    for ( m_iterator = model_fragments_.begin() ; 
          m_iterator != model_fragments_.end() ; ++m_iterator)
    {
        vl_sift_pix* model_images_grad_data=
            model_images_grad_data_.count((*m_iterator).second.first)?
            model_images_grad_data_[(*m_iterator).second.first]:
            0;

        VlSiftFilt*model_images_sift_filter=
            model_images_sift_filter_.count((*m_iterator).second.first)?
            model_images_sift_filter_[(*m_iterator).second.first]:
            0;
            
        //: prepare the trees also
        dbskfg_cgraph_directed_tree_sptr model_tree = new 
            dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                        scurve_interpolate_ds_, 
                                        scurve_matching_R_,
                                        false,
                                        area_weight_,
                                        model_images_grad_data,
                                        model_images_sift_filter);


        bool f1=model_tree->acquire
            ((*m_iterator).second.second, elastic_splice_cost_, 
             circular_ends_, combined_edit_);

        vcl_map<double,vcl_pair<unsigned int,unsigned int> >
            model_map;

        for ( q_iterator = query_fragments_.begin() ; 
              q_iterator != query_fragments_.end() ; ++q_iterator)
        {

            vl_sift_pix* model_images_grad_data=
                model_images_grad_data_.count((*m_iterator).second.first)?
                model_images_grad_data_[(*m_iterator).second.first]:
                0;
            
            VlSiftFilt*model_images_sift_filter=
                model_images_sift_filter_.count((*m_iterator).second.first)?
                model_images_sift_filter_[(*m_iterator).second.first]:
                0;

            //: prepare the trees also
            dbskfg_cgraph_directed_tree_sptr query_tree = new
                dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                            scurve_interpolate_ds_, 
                                            scurve_matching_R_,
                                            false,
                                            area_weight_,
                                            model_images_grad_data,
                                            model_images_sift_filter);
            
            bool f1=query_tree->acquire
                ((*q_iterator).second.second, elastic_splice_cost_, 
                 circular_ends_, combined_edit_);

            double norm_shape_cost(0.0);
            double app_diff(0.0);
            double norm_app_cost(0.0);
            double rgb_avg_cost(0.0);
            double norm_shape_cost_length(0.0);

            // Match model to query
            match_two_graphs_root_node_orig(model_tree,
                                            query_tree,
                                            norm_shape_cost,
                                            norm_shape_cost_length,
                                            app_diff,
                                            norm_app_cost,
                                            rgb_avg_cost);
            
            if ( mirror_)
            {
                //: prepare the trees also
                dbskfg_cgraph_directed_tree_sptr query_mirror_tree = new
                    dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                                scurve_interpolate_ds_, 
                                                scurve_matching_R_,
                                                mirror_,
                                                area_weight_,
                                                model_images_grad_data,
                                                model_images_sift_filter);
                
                f1=query_mirror_tree->acquire
                    ((*q_iterator).second.second, elastic_splice_cost_, 
                     circular_ends_, combined_edit_);
                
                double norm_shape_mirror_cost(0.0);
                double app_mirror_diff(0.0);
                double norm_app_mirror_cost(0.0);
                double rgb_avg_mirror_cost(0.0);
                double norm_shape_mirror_cost_length(0.0);
                
                // Match model to query
                match_two_graphs_root_node_orig(model_tree,
                                                query_mirror_tree,
                                                norm_shape_mirror_cost,
                                                norm_shape_mirror_cost_length,
                                                app_mirror_diff,
                                                norm_app_mirror_cost,
                                                rgb_avg_mirror_cost,
                                                "",
                                                true);

                norm_shape_cost = ( norm_shape_cost < norm_shape_mirror_cost)
                    ? norm_shape_cost : norm_shape_mirror_cost;
                norm_shape_cost_length = ( norm_shape_cost_length 
                                           < norm_shape_mirror_cost_length)
                    ? norm_shape_cost_length : norm_shape_mirror_cost_length;
                app_diff = ( app_diff < app_mirror_diff )
                    ? app_diff: app_mirror_diff;
                norm_app_cost = ( norm_app_cost < norm_app_mirror_cost )
                    ? norm_app_cost: norm_app_mirror_cost;
            }

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

            value=binary_sim_length_matrix_[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));

            value=binary_app_sim_matrix_[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));

            value=binary_app_norm_sim_matrix_[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));

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

bool dbskfg_match_bag_of_fragments::binary_debug_match()
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    if ( !mirror_)
    {
        vcl_cout<<"Matching "
                <<model_fragments_.size()
                <<" model fragments to "
                <<query_fragments_.size()
                <<" query fragments"
                <<vcl_endl;
    }
    else
    {
        vcl_cout<<"Matching "
                <<model_fragments_.size()
                <<" model fragments to "
                <<query_fragments_.size()
                <<" query fragments with horizontal mirroring of query"
                <<vcl_endl;

    }
 
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
                                        scurve_matching_R_,
                                        false,
                                        area_weight_);

        bool f1=model_tree->acquire
            ((*m_iterator).second.second, elastic_splice_cost_, 
             circular_ends_, combined_edit_);

        vcl_string model_filename=(*m_iterator).second.first+"_tree.shg";
        model_tree->create_shg(model_filename.c_str());

        vcl_map<double,vcl_pair<unsigned int,unsigned int> >
            model_map;

        for ( q_iterator = query_fragments_.begin() ; 
              q_iterator != query_fragments_.end() ; ++q_iterator)
        {

            //: prepare the trees also
            dbskfg_cgraph_directed_tree_sptr query_tree = new
                dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                            scurve_interpolate_ds_, 
                                            scurve_matching_R_,
                                            false,
                                            area_weight_);
            
            bool f1=query_tree->acquire
                ((*q_iterator).second.second, elastic_splice_cost_, 
                 circular_ends_, combined_edit_);

            vcl_string query_filename=(*q_iterator).second.first+"_tree.shg";
            query_tree->create_shg(query_filename.c_str());

            double norm_shape_cost(0.0);
            double app_diff(0.0);
            double norm_app_cost(0.0);
            double rgb_avg_cost(0.0);
            double norm_shape_cost_length(0.0);

            vcl_string match_prefix =(*m_iterator).second.first + "_vs_" +
                (*q_iterator).second.first;

            // Match model to query
            match_two_graphs_root_node_orig(model_tree,
                                            query_tree,
                                            norm_shape_cost,
                                            norm_shape_cost_length,
                                            app_diff,
                                            norm_app_cost,
                                            rgb_avg_cost,
                                            match_prefix);

            if ( mirror_)
            {
                vcl_cout<<"Computing Mirror"<<vcl_endl;

                //: prepare the trees also
                dbskfg_cgraph_directed_tree_sptr query_mirror_tree = new
                    dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                                scurve_interpolate_ds_, 
                                                scurve_matching_R_,
                                                mirror_,
                                                area_weight_);
                
                f1=query_mirror_tree->acquire
                    ((*q_iterator).second.second, elastic_splice_cost_, 
                     circular_ends_, combined_edit_);
                
                vcl_string query_mirror_filename=
                    (*q_iterator).second.first+"_mirror_tree.shg";
                query_mirror_tree->create_shg(query_mirror_filename.c_str());

                double norm_shape_mirror_cost(0.0);
                double app_mirror_diff(0.0);
                double norm_app_mirror_cost(0.0);
                double rgb_avg_mirror_cost(0.0);
                double norm_shape_mirror_cost_length(0.0);
                
                vcl_string match_mirror_prefix =
                    (*m_iterator).second.first + "_vs_mirror_" +
                    (*q_iterator).second.first;

                // Match model to query
                match_two_graphs_root_node_orig(model_tree,
                                                query_mirror_tree,
                                                norm_shape_mirror_cost,
                                                norm_shape_mirror_cost_length,
                                                app_mirror_diff,
                                                norm_app_mirror_cost,
                                                rgb_avg_mirror_cost,
                                                match_mirror_prefix,
                                                true);

                if ( norm_shape_cost < norm_shape_mirror_cost )
                {
                    vcl_string delete_prefix=match_mirror_prefix+"*";

                    vul_file::delete_file_glob(delete_prefix);
                    vul_file::delete_file_glob(query_mirror_filename);
                }
                else
                {
                    vcl_string delete_prefix=match_prefix+"*";

                    vul_file::delete_file_glob(delete_prefix);
                    vul_file::delete_file_glob(query_filename);
                }

                norm_shape_cost = ( norm_shape_cost < norm_shape_mirror_cost)
                    ? norm_shape_cost : norm_shape_mirror_cost;
                norm_shape_cost_length = ( norm_shape_cost_length 
                                           < norm_shape_mirror_cost_length)
                    ? norm_shape_cost_length : norm_shape_mirror_cost_length;

            }

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

bool dbskfg_match_bag_of_fragments::binary_scale_root_match()
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    if ( !mirror_)
    {
        vcl_cout<<"Matching "
                <<model_fragments_.size()
                <<" model fragments to "
                <<query_fragments_.size()
                <<" query fragments using root node scaling";

        if ( scale_area_ )
        {
            vcl_cout<<" by area"<<vcl_endl;
        }
        else if ( scale_root_ )
        {
            vcl_cout<<" by radii"<<vcl_endl;
        }
        else
        {
            vcl_cout<<" by length"<<vcl_endl;
        }

    }
    else
    {
        vcl_cout<<"Matching "
                <<model_fragments_.size()
                <<" model fragments to "
                <<query_fragments_.size()
                <<" query fragments with horizontal mirroring of query"
                <<" and root node scaling";
        
        if ( scale_area_ )
        {
            vcl_cout<<" by area";
        }
        else if ( scale_root_ )
        {
            vcl_cout<<" by radii";
        }
        else
        {
            vcl_cout<<" by length";
        }

        vcl_cout<<" with shape alg: "<<shape_alg_<<vcl_endl;

    }
 
    double backup=scurve_matching_R_;

    // Loop over model and query
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator m_iterator;
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator q_iterator;

    for ( m_iterator = model_fragments_.begin() ; 
          m_iterator != model_fragments_.end() ; ++m_iterator)
    {
        vl_sift_pix* model_images_grad_data=
            model_images_grad_data_.count((*m_iterator).second.first)?
            model_images_grad_data_[(*m_iterator).second.first]:
            0;
        
        VlSiftFilt*model_images_sift_filter=
            model_images_sift_filter_.count((*m_iterator).second.first)?
            model_images_sift_filter_[(*m_iterator).second.first]:
            0;

        vl_sift_pix* model_images_grad_data_red=
            model_images_grad_data_red_.count((*m_iterator).second.first)?
            model_images_grad_data_red_[(*m_iterator).second.first]:
            0;

        vl_sift_pix* model_images_grad_data_green=
            model_images_grad_data_green_.count((*m_iterator).second.first)?
            model_images_grad_data_green_[(*m_iterator).second.first]:
            0;

        vl_sift_pix* model_images_grad_data_blue=
            model_images_grad_data_blue_.count((*m_iterator).second.first)?
            model_images_grad_data_blue_[(*m_iterator).second.first]:
            0;

        double model_area=model_fragments_area_[(*m_iterator).first]
            .second;
        double model_length=model_fragments_length_[(*m_iterator).first]
            .second;

        vcl_map<double,vcl_pair<unsigned int,unsigned int> >
            model_map;

        for ( q_iterator = query_fragments_.begin() ; 
              q_iterator != query_fragments_.end() ; ++q_iterator)
        {

            scurve_matching_R_=backup;
            double query_area=query_fragments_area_[(*q_iterator).first]
                .second;
            double query_length=query_fragments_length_[(*q_iterator).first]
                .second;
           
            double mean_area=(model_area+query_area)/2.0;
            double mean_length=(model_length+query_length)/2.0;

            double max_area=(model_area > query_area )?model_area: query_area;
            double min_area=(model_area < query_area )?model_area: query_area;

            double max_length=(model_length > query_length )?
                model_length: query_length;
            double min_length=(model_length < query_length )?
                model_length: query_length;


            double model_scale_ratio=1.0;
            double query_scale_ratio=1.0;
            
            double model_sample_ds=scurve_sample_ds_;
            double query_sample_ds=scurve_sample_ds_;
            
            model_sample_ds=scurve_sample_ds_*vcl_sqrt(model_area
                                                       /ref_area_);
            query_sample_ds=scurve_sample_ds_*vcl_sqrt(query_area
                                                       /ref_area_);

            if ( shape_alg_ == SCALE_TO_REF)
            {
                
                if ( scale_area_ )
                {
                    model_scale_ratio = vcl_sqrt(ref_area_/model_area);
                    query_scale_ratio = vcl_sqrt(ref_area_/query_area);
                }
                else if ( scale_length_ )
                {
                    model_scale_ratio = ref_area_/model_length;
                    query_scale_ratio = ref_area_/query_length;
                }
                
            }
            else if ( shape_alg_ == SCALE_TO_MEAN )
            {
                if ( scale_area_ )
                {
                    
                    model_scale_ratio = vcl_sqrt(mean_area/model_area);
                    query_scale_ratio = vcl_sqrt(mean_area/query_area);
                }
                else if ( scale_length_ )
                {
                    
                    model_scale_ratio = mean_length/model_length;
                    query_scale_ratio = mean_length/query_length;
                }

                scurve_matching_R_=scurve_matching_R_*
                    vcl_sqrt(mean_area/ref_area_);

            }
            else if ( shape_alg_ == SCALE_TO_MAX )
            {
                
                if ( scale_area_ )
                {
                    

                    model_scale_ratio = vcl_sqrt(max_area/model_area);
                    query_scale_ratio = vcl_sqrt(max_area/query_area);
                }
                else if ( scale_length_ )
                {
                    
                    model_scale_ratio = max_length/model_length;
                    query_scale_ratio = max_length/query_length;
                }

                scurve_matching_R_=scurve_matching_R_*
                    vcl_sqrt(max_area/ref_area_);

            }
            else if ( shape_alg_ == SCALE_TO_MIN )
            {

                if ( scale_area_ )
                {
                    
                    model_scale_ratio = vcl_sqrt(min_area/model_area);
                    query_scale_ratio = vcl_sqrt(min_area/query_area);
                }
                else if ( scale_length_ )
                {
                    
                    model_scale_ratio = min_length/model_length;
                    query_scale_ratio = min_length/query_length;
                }

                scurve_matching_R_=scurve_matching_R_*
                    vcl_sqrt(min_area/ref_area_);

            }


            vcl_string key = model_fragments_
                [(*q_iterator).first].first;

            vl_sift_pix* query_images_grad_data=
                model_images_grad_data_.count(key)?
                model_images_grad_data_[key]:
                0;
            
            VlSiftFilt* query_images_sift_filter=
                model_images_sift_filter_.count(key)?
                model_images_sift_filter_[key]:
                0;

            vl_sift_pix* query_images_grad_data_red=
                model_images_grad_data_red_.count(key)?
                model_images_grad_data_red_[key]:
                0;


            vl_sift_pix* query_images_grad_data_blue=
                model_images_grad_data_blue_.count(key)?
                model_images_grad_data_blue_[key]:
                0;

            vl_sift_pix* query_images_grad_data_green=
                model_images_grad_data_green_.count(key)?
                model_images_grad_data_green_[key]:
                0;

            if ( query_image_ )
            {

                query_images_grad_data = query_grad_data_;
                query_images_sift_filter = query_sift_filter_;
                query_images_grad_data_red = query_grad_red_data_;
                query_images_grad_data_green = query_grad_green_data_;
                query_images_grad_data_blue = query_grad_blue_data_;
            
            }

            //: prepare the trees also
            dbskfg_cgraph_directed_tree_sptr model_tree = new 
                dbskfg_cgraph_directed_tree(model_sample_ds, 
                                            scurve_interpolate_ds_, 
                                            scurve_matching_R_,
                                            false,
                                            area_weight_,
                                            model_images_grad_data,
                                            model_images_sift_filter,
                                            model_images_grad_data_red,
                                            model_images_grad_data_green,
                                            model_images_grad_data_blue,
                                            (*m_iterator).first);

            model_tree->acquire_tree_topology((*m_iterator).second.second);

            //: prepare the trees also
            dbskfg_cgraph_directed_tree_sptr query_tree = new
                dbskfg_cgraph_directed_tree(query_sample_ds, 
                                            scurve_interpolate_ds_, 
                                            scurve_matching_R_,
                                            false,
                                            area_weight_,
                                            query_images_grad_data,
                                            query_images_sift_filter,
                                            query_images_grad_data_red,
                                            query_images_grad_data_green,
                                            query_images_grad_data_blue);
            
            query_tree->acquire_tree_topology((*q_iterator).second.second);
            
            model_tree->set_scale_ratio(model_scale_ratio);

            model_tree->compute_delete_and_contract_costs(
                elastic_splice_cost_, 
                circular_ends_, 
                combined_edit_);

            query_tree->set_scale_ratio(query_scale_ratio);

            query_tree->compute_delete_and_contract_costs(
                elastic_splice_cost_, 
                circular_ends_, 
                combined_edit_);
            

            double norm_shape_cost(0.0);
            double app_diff(0.0);
            double norm_app_cost(0.0);
            double rgb_avg_cost(0.0);
            double norm_shape_cost_length(0.0);

            // Match model to query
            match_two_graphs_root_node_orig(model_tree,
                                            query_tree,
                                            norm_shape_cost,
                                            norm_shape_cost_length,
                                            app_diff,
                                            norm_app_cost,
                                            rgb_avg_cost);



            if ( mirror_)
            {
                //: prepare the trees also
                dbskfg_cgraph_directed_tree_sptr query_mirror_tree = new
                    dbskfg_cgraph_directed_tree(query_sample_ds, 
                                                scurve_interpolate_ds_, 
                                                scurve_matching_R_,
                                                mirror_,
                                                area_weight_,
                                                query_images_grad_data,
                                                query_images_sift_filter,
                                                query_images_grad_data_red,
                                                query_images_grad_data_green,
                                                query_images_grad_data_blue);

                query_mirror_tree->set_scale_ratio(query_scale_ratio);

                query_mirror_tree->acquire
                    ((*q_iterator).second.second, elastic_splice_cost_, 
                     circular_ends_, combined_edit_);
                
                double norm_shape_mirror_cost(0.0);
                double app_mirror_diff(0.0);
                double norm_app_mirror_cost(0.0);
                double rgb_avg_mirror_cost(0.0);
                double norm_shape_mirror_cost_length(0.0);
                
                // Match model to query
                match_two_graphs_root_node_orig(model_tree,
                                                query_mirror_tree,
                                                norm_shape_mirror_cost,
                                                norm_shape_mirror_cost_length,
                                                app_mirror_diff,
                                                norm_app_mirror_cost,
                                                rgb_avg_mirror_cost,
                                                "",
                                                true,
                                                norm_shape_cost);

                norm_shape_cost = ( norm_shape_cost < norm_shape_mirror_cost)
                    ? norm_shape_cost : norm_shape_mirror_cost;
                norm_shape_cost_length = ( norm_shape_cost_length 
                                           < norm_shape_mirror_cost_length)
                    ? norm_shape_cost_length : norm_shape_mirror_cost_length;
                app_diff = ( app_diff < app_mirror_diff )
                    ? app_diff: app_mirror_diff;
                norm_app_cost = ( norm_app_cost < norm_app_mirror_cost )
                    ? norm_app_cost: norm_app_mirror_cost;
                rgb_avg_cost = ( rgb_avg_cost < rgb_avg_mirror_cost )
                    ? rgb_avg_cost: rgb_avg_mirror_cost;

            }

            unsigned int model_id= (*m_iterator).first;
            unsigned int query_id= (*q_iterator).first;
            binary_sim_matrix_[model_id][query_id]=norm_shape_cost;
            binary_sim_length_matrix_[model_id][query_id]=
                norm_shape_cost_length;
            binary_app_sim_matrix_[model_id][query_id]=app_diff;
            binary_app_norm_sim_matrix_[model_id][query_id]=norm_app_cost;
            binary_app_rgb_sim_matrix_[model_id][query_id]=rgb_avg_cost;

            query_tree=0;
            model_tree=0;
        }
        vcl_cout<<"Finished "<<(*m_iterator).second.first<<" to all queires"
                <<vcl_endl;
    }

    // write out data
    write_out_dart_data();

    vcl_ofstream binary_sim_file;
    binary_sim_file.open(output_binary_file_.c_str(),
                         vcl_ios::out | 
                         vcl_ios::app | 
                         vcl_ios::binary);

  
    write_binary_fragments(binary_sim_file,model_fragments_);
    write_binary_fragments(binary_sim_file,query_fragments_);

    double matrix_size=binary_sim_matrix_.columns()*
        binary_sim_matrix_.rows()*5;
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

bool dbskfg_match_bag_of_fragments::binary_scale_mean_shape()
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    if ( !mirror_)
    {
        vcl_cout<<"Matching "
                <<model_fragments_.size()
                <<" model fragments to "
                <<query_fragments_.size()
                <<" query fragments by scaling to mean shape "<<vcl_endl;

    }
    else
    {
        vcl_cout<<"Matching "
                <<model_fragments_.size()
                <<" model fragments to "
                <<query_fragments_.size()
                <<" query fragments with horizontal mirroring of query"
                <<" and scaling to mean shape "<<vcl_endl;

    }
 
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
            if ( (*q_iterator).first <= (*m_iterator).first )
            {
                continue;
            }

            //: prepare the trees also
            dbskfg_cgraph_directed_tree_sptr model_tree = new 
                dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                            scurve_interpolate_ds_, 
                                            scurve_matching_R_,
                                            false,
                                            area_weight_);
            
            model_tree->acquire_tree_topology((*m_iterator).second.second);

            dbskfg_cgraph_directed_tree_sptr query_tree = new
                dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                            scurve_interpolate_ds_, 
                                            scurve_matching_R_,
                                            false,
                                            area_weight_);
            
            query_tree->acquire_tree_topology((*q_iterator).second.second);

            double model_area=model_fragments_area_[(*m_iterator).first]
                .second;
            double query_area=query_fragments_area_[(*q_iterator).first]
                .second;
            double mean_area=(model_area+query_area)/2.0;

            double model_scale_ratio=vcl_sqrt(mean_area/model_area);
            double query_scale_ratio=vcl_sqrt(mean_area/query_area);
            
            model_tree->set_scale_ratio(model_scale_ratio);
            query_tree->set_scale_ratio(query_scale_ratio);

            model_tree->compute_delete_and_contract_costs(
                elastic_splice_cost_, 
                circular_ends_, 
                combined_edit_);

            query_tree->compute_delete_and_contract_costs(
                elastic_splice_cost_, 
                circular_ends_, 
                combined_edit_);
            

            double norm_shape_cost(0.0);
            double app_diff(0.0);
            double norm_app_cost(0.0);
            double rgb_avg_cost(0.0);
            double norm_shape_cost_length(0.0);

            // Match model to query
            match_two_graphs_root_node_orig(model_tree,
                                            query_tree,
                                            norm_shape_cost,
                                            norm_shape_cost_length,
                                            app_diff,
                                            norm_app_cost,
                                            rgb_avg_cost);

            if ( mirror_)
            {
                //: prepare the trees also
                dbskfg_cgraph_directed_tree_sptr query_mirror_tree = new
                    dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                                scurve_interpolate_ds_, 
                                                scurve_matching_R_,
                                                mirror_,
                                                area_weight_);
                
                query_mirror_tree->set_scale_ratio(query_scale_ratio);

                query_mirror_tree->acquire
                    ((*q_iterator).second.second, elastic_splice_cost_, 
                     circular_ends_, combined_edit_);
                
                double norm_shape_mirror_cost(0.0);
                double app_mirror_diff(0.0);
                double norm_app_mirror_cost(0.0);
                double rgb_avg_mirror_cost(0.0);
                double norm_shape_mirror_cost_length(0.0);
                
                // Match model to query
                match_two_graphs_root_node_orig(model_tree,
                                                query_mirror_tree,
                                                norm_shape_mirror_cost,
                                                norm_shape_mirror_cost_length,
                                                app_mirror_diff,
                                                norm_app_mirror_cost,
                                                rgb_avg_mirror_cost);

                norm_shape_cost = ( norm_shape_cost < norm_shape_mirror_cost)
                    ? norm_shape_cost : norm_shape_mirror_cost;
                norm_shape_cost_length = ( norm_shape_cost_length 
                                           < norm_shape_mirror_cost_length)
                    ? norm_shape_cost_length : norm_shape_mirror_cost_length;

            }

            unsigned int model_id= (*m_iterator).first;
            unsigned int query_id= (*q_iterator).first;
            binary_sim_matrix_[model_id][query_id]=norm_shape_cost;
            binary_sim_length_matrix_[model_id][query_id]=
                norm_shape_cost_length;
            binary_app_sim_matrix_[model_id][query_id]=app_diff;
            binary_app_norm_sim_matrix_[model_id][query_id]=norm_app_cost;
            binary_app_rgb_sim_matrix_[model_id][query_id]=rgb_avg_cost;

            model_tree=0;
            query_tree=0;
        }
        vcl_cout<<"Finished "<<(*m_iterator).second.first<<" to all queires"
                <<vcl_endl;
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


bool dbskfg_match_bag_of_fragments::binary_scale_root_debug_match()
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    if ( !mirror_)
    {
        vcl_cout<<"Matching "
                <<model_fragments_.size()
                <<" model fragments to "
                <<query_fragments_.size()
                <<" query fragments in debug mode using root node scaling";

        if ( scale_area_ )
        {
            vcl_cout<<" by area"<<vcl_endl;
        }
        else
        {
            vcl_cout<<" by radii"<<vcl_endl;
        }
    }
    else
    {
        vcl_cout<<"Matching "
                <<model_fragments_.size()
                <<" model fragments to "
                <<query_fragments_.size()
                <<" query fragments in debug mode with "
                <<"horizontal mirroring of query"
                <<" and root node scaling";
        
        if ( scale_area_ )
        {
            vcl_cout<<" by area"<<vcl_endl;
        }
        else
        {
            vcl_cout<<" by radii"<<vcl_endl;
        }

    }
    vcl_cout<<"Ref area: "<<ref_area_<<vcl_endl;

    double backup=scurve_matching_R_;

    // Loop over model and query
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator m_iterator;
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator q_iterator;

    for ( m_iterator = model_fragments_.begin() ; 
          m_iterator != model_fragments_.end() ; ++m_iterator)
    {

        double model_area=model_fragments_area_[(*m_iterator).first]
            .second;
        double model_length=model_fragments_length_[(*m_iterator).first]
            .second;

        vcl_map<double,vcl_pair<unsigned int,unsigned int> >
            model_map;


        for ( q_iterator = query_fragments_.begin() ; 
              q_iterator != query_fragments_.end() ; ++q_iterator)
        {
            scurve_matching_R_=backup;
            double query_area=query_fragments_area_[(*q_iterator).first]
                .second;
            double query_length=query_fragments_length_[(*q_iterator).first]
                .second;
           
            double mean_area=(model_area+query_area)/2.0;
            double mean_length=(model_length+query_length)/2.0;

            double max_area=(model_area > query_area )?model_area: query_area;
            double min_area=(model_area < query_area )?model_area: query_area;

            double max_length=(model_length > query_length )?
                model_length: query_length;
            double min_length=(model_length < query_length )?
                model_length: query_length;


            double model_scale_ratio=1.0;
            double query_scale_ratio=1.0;
            
            double model_sample_ds=scurve_sample_ds_;
            double query_sample_ds=scurve_sample_ds_;
            
            model_sample_ds=scurve_sample_ds_*vcl_sqrt(model_area
                                                       /ref_area_);
            query_sample_ds=scurve_sample_ds_*vcl_sqrt(query_area
                                                       /ref_area_);

            if ( shape_alg_ == SCALE_TO_REF)
            {
                vcl_cout<<"Scaling to ref area "<<ref_area_<<vcl_endl;
                if ( scale_area_ )
                {
                    model_scale_ratio = vcl_sqrt(ref_area_/model_area);
                    query_scale_ratio = vcl_sqrt(ref_area_/query_area);
                }
                else if ( scale_length_ )
                {
                    model_scale_ratio = ref_area_/model_length;
                    query_scale_ratio = ref_area_/query_length;
                }
                
            }
            else if ( shape_alg_ == SCALE_TO_MEAN )
            {
                vcl_cout<<"Scaling to Mean using ";
                if ( scale_area_ )
                {
                    vcl_cout<<mean_area<<" area"<<vcl_endl;
                    model_scale_ratio = vcl_sqrt(mean_area/model_area);
                    query_scale_ratio = vcl_sqrt(mean_area/query_area);
                }
                else if ( scale_length_ )
                {
                    vcl_cout<<mean_length<<" length"<<vcl_endl;
                    model_scale_ratio = mean_length/model_length;
                    query_scale_ratio = mean_length/query_length;
                }

                scurve_matching_R_=scurve_matching_R_*
                    vcl_sqrt(mean_area/ref_area_);

            }
            else if ( shape_alg_ == SCALE_TO_MAX )
            {
                vcl_cout<<"Scaling to MAX using ";
                if ( scale_area_ )
                {
                    vcl_cout<<max_area<<" area"<<vcl_endl;

                    model_scale_ratio = vcl_sqrt(max_area/model_area);
                    query_scale_ratio = vcl_sqrt(max_area/query_area);
                }
                else if ( scale_length_ )
                {
                    vcl_cout<<max_length<<" length"<<vcl_endl;
                    model_scale_ratio = max_length/model_length;
                    query_scale_ratio = max_length/query_length;
                }

                scurve_matching_R_=scurve_matching_R_*
                    vcl_sqrt(max_area/ref_area_);

            }
            else if ( shape_alg_ == SCALE_TO_MIN )
            {
                vcl_cout<<"Scaling to MIN using ";
                if ( scale_area_ )
                {
                    vcl_cout<<min_area<<" area"<<vcl_endl;
                    model_scale_ratio = vcl_sqrt(min_area/model_area);
                    query_scale_ratio = vcl_sqrt(min_area/query_area);
                }
                else if ( scale_length_ )
                {
                    vcl_cout<<min_length<<" length"<<vcl_endl;
                    model_scale_ratio = min_length/model_length;
                    query_scale_ratio = min_length/query_length;
                }

                scurve_matching_R_=scurve_matching_R_*
                    vcl_sqrt(min_area/ref_area_);

            }
            vcl_cout<<"Scurve Matching R: "<<scurve_matching_R_<<vcl_endl;

            //: prepare the model tree
            dbskfg_cgraph_directed_tree_sptr model_tree = new 
                dbskfg_cgraph_directed_tree(model_sample_ds, 
                                            scurve_interpolate_ds_, 
                                            scurve_matching_R_,
                                            false,
                                            area_weight_);
            
            model_tree->acquire_tree_topology((*m_iterator).second.second);
                        

            //: prepare the query tree
            dbskfg_cgraph_directed_tree_sptr query_tree = new
                dbskfg_cgraph_directed_tree(query_sample_ds, 
                                            scurve_interpolate_ds_, 
                                            scurve_matching_R_,
                                            false,
                                            area_weight_);
            
            query_tree->acquire_tree_topology((*q_iterator).second.second);
            vcl_cout<<"Model Scale Ratio: "<<model_scale_ratio<<vcl_endl;
            vcl_cout<<"Query Scale Ratio: "<<query_scale_ratio<<vcl_endl;
            
            model_tree->set_scale_ratio(model_scale_ratio);

            model_tree->compute_delete_and_contract_costs(
                elastic_splice_cost_, 
                circular_ends_, 
                combined_edit_);

            query_tree->set_scale_ratio(query_scale_ratio);

            query_tree->compute_delete_and_contract_costs(
                elastic_splice_cost_, 
                circular_ends_, 
                combined_edit_);

            vgl_polygon<double> model_poly(1);
            vgl_polygon<double> query_poly(1);

            model_tree->compute_reconstructed_boundary_polygon(model_poly);
            query_tree->compute_reconstructed_boundary_polygon(query_poly);

            vcl_cout<<"Model area: "<<vgl_area(model_poly)<<vcl_endl;
            vcl_cout<<"Query area: "<<vgl_area(query_poly)<<vcl_endl;

            vcl_string model_filename=(*m_iterator).second.first+"_tree.shg";
            model_tree->create_shg(model_filename.c_str());
            
            vcl_string query_filename=(*q_iterator).second.first+"_tree.shg";
            query_tree->create_shg(query_filename.c_str());

            double norm_shape_cost(0.0);
            double app_diff(0.0);
            double norm_app_cost(0.0);
            double rgb_avg_cost(0.0);
            double norm_shape_cost_length(0.0);

            model_tree->compute_average_ds();
            query_tree->compute_average_ds();

            vcl_vector<double> model_ds=model_tree->get_average_ds();
            vcl_vector<double> query_ds=query_tree->get_average_ds();

            vcl_cout<<"Model bnd ds: "<<model_ds[0]<<vcl_endl;
            vcl_cout<<"Query bnd ds: "<<query_ds[0]<<vcl_endl;

            vcl_string match_prefix =(*m_iterator).second.first + "_vs_" +
                (*q_iterator).second.first;

            // Match model to query
            match_two_graphs_root_node_orig(model_tree,
                                            query_tree,
                                            norm_shape_cost,
                                            norm_shape_cost_length,
                                            app_diff,
                                            norm_app_cost,
                                            rgb_avg_cost,
                                            match_prefix);

            if ( mirror_)
            {
                vcl_cout<<"Computing Mirror"<<vcl_endl;

                //: prepare the trees also
                dbskfg_cgraph_directed_tree_sptr query_mirror_tree = new
                    dbskfg_cgraph_directed_tree(query_sample_ds, 
                                                scurve_interpolate_ds_, 
                                                scurve_matching_R_,
                                                mirror_,
                                                area_weight_);
                
                query_mirror_tree->set_scale_ratio(query_scale_ratio);

                query_mirror_tree->acquire
                    ((*q_iterator).second.second, elastic_splice_cost_, 
                     circular_ends_, combined_edit_);
                
                vcl_string query_mirror_filename=
                    (*q_iterator).second.first+"_mirror_tree.shg";
                query_mirror_tree->create_shg(query_mirror_filename.c_str());

                double norm_shape_mirror_cost(0.0);
                double app_mirror_diff(0.0);
                double norm_app_mirror_cost(0.0);
                double rgb_avg_mirror_cost(0.0);
                double norm_shape_mirror_cost_length(0.0);
                       
                vcl_string match_mirror_prefix =
                    (*m_iterator).second.first + "_vs_mirror_" +
                    (*q_iterator).second.first;

                // Match model to query
                match_two_graphs_root_node_orig(model_tree,
                                                query_mirror_tree,
                                                norm_shape_mirror_cost,
                                                norm_shape_mirror_cost_length,
                                                app_mirror_diff,
                                                norm_app_mirror_cost,
                                                rgb_avg_mirror_cost,
                                                match_mirror_prefix,
                                                true);

                if ( norm_shape_cost < norm_shape_mirror_cost )
                {
                    vcl_string delete_prefix=match_mirror_prefix+"*";

                    vul_file::delete_file_glob(delete_prefix);
                    vul_file::delete_file_glob(query_mirror_filename);
                }
                else
                {
                    vcl_string delete_prefix=match_prefix+"*";

                    vul_file::delete_file_glob(delete_prefix);
                    vul_file::delete_file_glob(query_filename);
                }

                norm_shape_cost = ( norm_shape_cost < norm_shape_mirror_cost)
                    ? norm_shape_cost : norm_shape_mirror_cost;
                norm_shape_cost_length = ( norm_shape_cost_length 
                                           < norm_shape_mirror_cost_length)
                    ? norm_shape_cost_length : norm_shape_mirror_cost_length;

            }

            unsigned int model_id= (*m_iterator).first;
            unsigned int query_id= (*q_iterator).first;
            binary_sim_matrix_[model_id][query_id]=norm_shape_cost;
            binary_sim_length_matrix_[model_id][query_id]=
                norm_shape_cost_length;
            binary_app_sim_matrix_[model_id][query_id]=app_diff;
            binary_app_norm_sim_matrix_[model_id][query_id]=norm_app_cost;
            binary_app_rgb_sim_matrix_[model_id][query_id]=rgb_avg_cost;

            query_tree=0;
            model_tree=0;

            vcl_cout<<vcl_endl;
        }
        vcl_cout<<"Finished "<<(*m_iterator).second.first<<" to all queires"
                <<vcl_endl;
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
    double unorm_shape_cost=1.0e6;

    //instantiate the edit distance algorithms
    dbskfg_cg_tree_edit edit(model_tree.ptr(), 
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
                    unorm_shape_cost = val;

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

                model_tree->reset_up_flags();
                query_tree->reset_up_flags();

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
                    unorm_shape_cost = val;

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

                model_tree->reset_up_flags();
                query_tree->reset_up_flags();

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
    //     dbskfg_cg_tree_edit edit_app(model_app_tree.ptr(), 
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
        vcl_vector<double> dart_distances;
        vcl_pair<double,double> sift_rgb_cost=compute_rgb_sift_cost(
            curve_list1,
            curve_list2,
            map_list,
            path_map,
            dart_distances,
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
    
    // unsigned int ds=scurve_sample_ds_;
    // vgl_h_matrix_2d<double> H;
    // compute_similarity(H,
    //                    curve_list1,
    //                    curve_list2,
    //                    map_list,
    //                    path_map,
    //                    ds,
    //                    flag);
    
    // vcl_pair<double,double> p1=
    //     compute_transformed_polygon(H,model_tree,query_tree);

}

void dbskfg_match_bag_of_fragments::match_two_graphs_root_node_orig(
    dbskfg_cgraph_directed_tree_sptr& model_tree, 
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    double& norm_shape_cost,
    double& norm_shape_cost_length,
    double& app_diff,
    double& norm_app_cost,
    double& rgb_avg_cost,
    vcl_string match_file_prefix,
    bool mirror,
    double orig_edit_distance)
{

    vul_timer shape_timer;
    shape_timer.mark();

    // grab model roots
    vcl_set<int> model_roots;
    vcl_set<int> query_roots;
    
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
    double unorm_shape_cost=1.0e6;

    //instantiate the edit distance algorithms
    dbskfg_cg_tree_edit edit(model_tree.ptr(), 
                         query_tree.ptr(), circular_ends_, 
                         localized_edit_);

    dbskr_edit_distance_base::RootNodeSelection min_root_selection
        (dbskr_edit_distance_base::DEFAULT);

    unsigned int model_tree_branches=model_tree->size()/2;
    unsigned int query_tree_branches=query_tree->size()/2;

    unsigned int model_final_branches(0);
    unsigned int query_final_branches(0);

    {
        edit.clear();
        edit.set_tree1(model_tree.ptr());
        edit.set_tree2(query_tree.ptr());
        edit.save_path(true);
        edit.set_curvematching_R(scurve_matching_R_);
        edit.set_use_approx(use_approx_);
                
        dbskr_edit_distance_base::RootNodeSelection value =  
            dbskr_edit_distance_base::DEFAULT;
        
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
        
        // vcl_cout<<"Root1 "<<0<<" Root2 "<<query_tree->centroid()<<" cost: "
        //         <<norm_val<<vcl_endl;
        
        if ( norm_val < shape_cost_splice )
        {
            shape_cost_splice = norm_val;
            unorm_shape_cost = val;
            
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

            if ( match_file_prefix.size() )
            {

                vcl_cout << "final cost: " << val 
                         << " final norm cost: " << norm_val 
                         << "( tree1 tot splice: " << model_tree_splice_cost
                         << ", tree2: " << query_tree_splice_cost
                         << ")" << vcl_endl;
        
                vcl_cout<<"Root1 "<<0<<" Root2 "<<query_tree->centroid()
                        <<" cost: "
                        <<norm_val<<vcl_endl;

                vcl_string match_cost_table=match_file_prefix 
                    + "_match_table.shgm";
                vcl_string match_file=match_file_prefix 
                    + "_match_file.shgm";
                
                edit.populate_table(match_cost_table.c_str());
                edit.write_shgm(match_file.c_str());
                
            }
                

        }
                
        if ( norm_val_length < shape_cost_length )
        {
            shape_cost_length=norm_val_length;
        }
        
        model_tree->reset_up_flags();
        query_tree->reset_up_flags();
        
    }
    
    if ( !edit.get_switched() )
    {
        
        edit.clear();
        edit.set_tree1(query_tree.ptr());
        edit.set_tree2(model_tree.ptr());
        edit.save_path(true);
        edit.set_curvematching_R(scurve_matching_R_);
        edit.set_use_approx(use_approx_);
        
        dbskr_edit_distance_base::RootNodeSelection value =  
            dbskr_edit_distance_base::DEFAULT;
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
        //          << "( tree1 tot splice: " << query_tree_splice_cost
        //          << ", tree2: " << model_tree_splice_cost
        //          << ")" << vcl_endl;
                
        // vcl_cout<<"Root1 "<<0<<" Root2 "<<model_tree->centroid()<<" cost: "
        //         <<norm_val<<vcl_endl;
        
        if ( norm_val < shape_cost_splice )
        {
            
            shape_cost_splice = norm_val;
            unorm_shape_cost = val;
            
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

            if ( match_file_prefix.size() )
            {

                vcl_cout << "final cost: " << val 
                         << " final norm cost: " << norm_val 
                         << "( tree1 tot splice: " << query_tree_splice_cost
                         << ", tree2: " << model_tree_splice_cost
                         << ")" << vcl_endl;
                
                vcl_cout<<"Root1 "<<0<<" Root2 "<<
                    model_tree->centroid()<<" cost: "
                        <<norm_val<<vcl_endl;

                vcl_string match_cost_table=match_file_prefix 
                    + "_match_table.shgm";
                vcl_string match_file=match_file_prefix 
                    + "_match_file.shgm";
                
                edit.populate_table(match_cost_table.c_str());
                edit.write_shgm(match_file.c_str());
                
            }
        }
        
        if ( norm_val_length < shape_cost_length )
        {
            shape_cost_length=norm_val_length;
        }
        
        model_tree->reset_up_flags();
        query_tree->reset_up_flags();
        
    }
    else
    {
        flag=true;
    }

    double shape_time = shape_timer.real()/1000.0;
    shape_timer.mark();

    //vcl_cerr<<"************ Shape Time taken: "<<shape_time<<" sec"<<vcl_endl;
 
    bool flag_mirror=true;
    if ( mirror )
    {
        if ( shape_cost_splice > orig_edit_distance)
        {
            app_diff        = 1.0e6;
            norm_app_cost   = 1.0e6;
            rgb_avg_cost    = 1.0e6;

            flag_mirror=false;
        }
    }

    if ( app_sift_ && flag_mirror )
    {

        double width=0.0;
        if ( mirror )
        {
            vcl_cout<<"Performing mirror app matching"<<vcl_endl;
            width=query_tree->bbox()->width();
        }

        vl_sift_pix* model_grad_data=model_tree->get_grad_data();
        vl_sift_pix* query_grad_data=query_tree->get_grad_data();

        vl_sift_pix* model_red_grad_data=model_tree->get_red_grad_data();
        vl_sift_pix* query_red_grad_data=query_tree->get_red_grad_data();

        vl_sift_pix* model_green_grad_data=model_tree->get_green_grad_data();
        vl_sift_pix* query_green_grad_data=query_tree->get_green_grad_data();

        vl_sift_pix* model_blue_grad_data=model_tree->get_blue_grad_data();
        vl_sift_pix* query_blue_grad_data=query_tree->get_blue_grad_data();

        VlSiftFilt* model_sift_filter=model_tree->get_sift_filter();
        VlSiftFilt* query_sift_filter=query_tree->get_sift_filter();

        vul_timer app_timer;
        app_timer.mark();
        vcl_pair<double,double> app_cost=compute_sift_cost(curve_list1,
                                                           curve_list2,
                                                           map_list,
                                                           path_map,
                                                           flag,
                                                           width,
                                                           model_grad_data,
                                                           model_sift_filter,
                                                           query_grad_data,
                                                           query_sift_filter,
                                                           model_tree
                                                           ->get_scale_ratio(),
                                                           query_tree
                                                           ->get_scale_ratio());

        vcl_vector<double> dart_distances;
        vcl_pair<double,double> sift_rgb_cost=compute_rgb_sift_cost(
            curve_list1,
            curve_list2,
            map_list,
            path_map,
            dart_distances,
            flag,
            width,
            model_red_grad_data,
            query_red_grad_data,
            model_green_grad_data,
            query_green_grad_data,
            model_blue_grad_data,
            query_blue_grad_data,
            model_sift_filter,
            query_sift_filter,
            model_tree->get_scale_ratio(),
            query_tree->get_scale_ratio());
        
        unsigned int model_tag=model_tree->get_id();

        if ( !flag )
        {
            if ( model_dart_distances_.count(model_tag))
            {
                model_dart_distances_.erase(model_tag);
                model_dart_curves_.erase(model_tag);
            }

            // Get matching pairs
            for (unsigned m = 0; m < map_list.size(); m++) 
            {
                dbskr_scurve_sptr mc = curve_list1[m];
                vcl_pair<unsigned int,unsigned int> query_key(
                    curve_list2[m]->get_curve_id().first,
                    curve_list2[m]->get_curve_id().second);
                if ( !query_dart_curves_.count(query_key))
                {
                    vcl_pair<unsigned int,unsigned int> temp
                        = query_key;
                    query_key.first=temp.second;
                    query_key.second=temp.first;                        
                }
                
                double cost=dart_distances[m];
                
                vcl_pair<vcl_pair<int,int>,double> key1;
                vcl_pair<vcl_pair<int,int>,dbskr_scurve_sptr> key2;
                    
                key1.first=query_key;
                key1.second=cost;
                
                key2.first=query_key;
                key2.second=mc;
                
                model_dart_distances_[model_tag].push_back(key1);
                model_dart_curves_[model_tag].push_back(key2);
            }
                
            
        }
        else
        {
            // Get matching pairs
            for (unsigned m = 0; m < map_list.size(); m++) 
            {
                dbskr_scurve_sptr mc = curve_list2[m];
                vcl_pair<unsigned int,unsigned int> query_key(
                    curve_list1[m]->get_curve_id().first,
                    curve_list1[m]->get_curve_id().second);
                if ( !query_dart_curves_.count(query_key))
                {
                    vcl_pair<unsigned int,unsigned int> temp
                        = query_key;
                    query_key.first=temp.second;
                    query_key.second=temp.first;
                    
                }
                double cost=dart_distances[m];
                
                vcl_pair<vcl_pair<int,int>,double> key1;
                vcl_pair<vcl_pair<int,int>,dbskr_scurve_sptr> key2;
                
                key1.first=query_key;
                key1.second=cost;
                
                key2.first=query_key;
                key2.second=mc;
                
                model_dart_distances_[model_tag].push_back(key1);
                model_dart_curves_[model_tag].push_back(key2);
                
            }
        }
    
            
        double app_time = app_timer.real()/1000.0;
        app_timer.mark();
        
        //vcl_cerr<<"************ App   Time taken: "<<app_time<<" sec"<<vcl_endl;
        app_diff        = app_cost.first;
        norm_app_cost   = app_cost.second;
        rgb_avg_cost    = sift_rgb_cost.second;

    }
   
    norm_shape_cost = shape_cost_splice;
    norm_shape_cost_length=shape_cost_length;

    if ( match_file_prefix.size() )
    {
        double width=0.0;
        if ( mirror )
        {
            width=query_tree->bbox()->width();
        }

        // Get matching pairs
        for (unsigned i = 0; i < map_list.size(); i++) 
        {
            dbskr_scurve_sptr sc1 = curve_list1[i];
            dbskr_scurve_sptr sc2 = curve_list2[i];

            vcl_stringstream stream;
            stream<<i;

            vcl_string temp=match_file_prefix+"_dart_"+stream.str()
                + "_curve_list.txt";
            
            vcl_ofstream model_file(temp.c_str());            
            for (unsigned j = 0; j < map_list[i].size(); ++j) 
            {
                vcl_pair<int, int> cor = map_list[i][j];

                vgl_point_2d<double> ps1 = sc1->sh_pt(cor.first);
                vgl_point_2d<double> ps2 = sc2->sh_pt(cor.second);

                if ( !flag )
                {
                    model_file<<ps1.x()
                              <<" "
                              <<ps1.y()
                              <<" "
                              <<vcl_fabs(width-(ps2.x()/query_tree
                                                ->get_scale_ratio()))
                              <<" "
                              <<ps2.y()/query_tree->get_scale_ratio()<<vcl_endl;
                }
                else
                {

                    model_file<<ps2.x()
                              <<" "
                              <<ps2.y()
                              <<" "
                              <<vcl_fabs(width-(ps1.x()/query_tree
                                                ->get_scale_ratio()))
                              <<" "
                              <<ps1.y()/query_tree->get_scale_ratio()<<vcl_endl;

                }
            }
            model_file.close();
        }
    }

    // vcl_cout<<" Norm Shape Cost: "<<norm_shape_cost<<vcl_endl;
    // vcl_cout<<" Norm App   Cost: "<<norm_app_cost<<vcl_endl;
    // vcl_cout<<" App Cost       : "<<app_diff<<vcl_endl;
    // vcl_cout<<" Norm Rgb Sift Cost    : "<< sift_rgb_cost.second<<vcl_endl;
    // vcl_cout<<" Unnorm Rgb Sift Cost  : "<< sift_rgb_cost.first<<vcl_endl;
    
    // unsigned int ds=scurve_sample_ds_;
    // vgl_h_matrix_2d<double> H;
    // compute_similarity(H,
    //                    curve_list1,
    //                    curve_list2,
    //                    map_list,
    //                    path_map,
    //                    ds,
    //                    flag);
    
    // vcl_pair<double,double> p1=
    //     compute_transformed_polygon(H,model_tree,query_tree);

}

void dbskfg_match_bag_of_fragments::match_two_debug_graphs(
    dbskfg_cgraph_directed_tree_sptr& model_tree, 
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    double& norm_shape_cost,
    double& norm_shape_cost_length,
    double& app_diff,
    double& norm_app_cost,
    double& rgb_avg_cost,
    vcl_string match_file_prefix)
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
    double unorm_shape_cost=1.0e6;

    //instantiate the edit distance algorithms
    dbskfg_cg_tree_edit edit(model_tree.ptr(), 
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
                
                vcl_cout << "final cost: " << val 
                         << " final norm cost: " << norm_val 
                         << "( tree1 tot splice: " << model_tree_splice_cost
                         << ", tree2: " << query_tree_splice_cost
                         << ")" << vcl_endl;

                vcl_cout<<"Root1 "<<(*it)<<" Root2 "<<(*bit)<<" cost: "
                       <<norm_val<<vcl_endl;

                if ( norm_val < shape_cost_splice )
                {
                    shape_cost_splice = norm_val;
                    unorm_shape_cost = val;

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

                    vcl_string match_cost_table=match_file_prefix 
                        + "_match_table.shgm";
                    vcl_string match_file=match_file_prefix 
                        + "_match_file.shgm";
                
                    edit.populate_table(match_cost_table.c_str());
                    edit.write_shgm(match_file.c_str());
                }
                
                if ( norm_val_length < shape_cost_length )
                {
                    shape_cost_length=norm_val_length;
                }

                model_tree->reset_up_flags();
                query_tree->reset_up_flags();
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
  
                vcl_cout << "final cost: " << val 
                         << " final norm cost: " << norm_val 
                         << "( tree1 tot splice: " << model_tree_splice_cost
                         << ", tree2: " << query_tree_splice_cost
                         << ")" << vcl_endl;
                
                
   
                vcl_cout<<"Root1 "<<(*it)<<" Root2 "<<(*bit)<<" cost: "
                        <<norm_val<<vcl_endl;

                if ( norm_val < shape_cost_splice )
                {

                    shape_cost_splice = norm_val;
                    unorm_shape_cost = val;

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

                    vcl_string match_cost_table=match_file_prefix 
                        + "_match_table.shgm";
                    vcl_string match_file=match_file_prefix 
                        + "_match_file.shgm";
                
                    edit.populate_table(match_cost_table.c_str());
                    edit.write_shgm(match_file.c_str());

                }
         
                if ( norm_val_length < shape_cost_length )
                {
                    shape_cost_length=norm_val_length;
                }

                model_tree->reset_up_flags();
                query_tree->reset_up_flags();

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
    //     dbskfg_cg_tree_edit edit_app(model_app_tree.ptr(), 
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
        vcl_vector<double> dart_distances;
        vcl_pair<double,double> sift_rgb_cost=compute_rgb_sift_cost(
            curve_list1,
            curve_list2,
            map_list,
            path_map,
            dart_distances,
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

    {
        // Get matching pairs
        for (unsigned i = 0; i < map_list.size(); i++) 
        {
            dbskr_scurve_sptr sc1 = curve_list1[i];
            dbskr_scurve_sptr sc2 = curve_list2[i];

            vcl_stringstream stream;
            stream<<i;

            vcl_string temp=match_file_prefix+"_dart_"+stream.str()
                        + "_curve_list.txt";
            
            vcl_ofstream model_file(temp.c_str());            
            for (unsigned j = 0; j < map_list[i].size(); ++j) 
            {
                vcl_pair<int, int> cor = map_list[i][j];

                vgl_point_2d<double> ps1 = sc1->sh_pt(cor.first);
                vgl_point_2d<double> ps2 = sc2->sh_pt(cor.second);

                if ( !flag )
                {
                    model_file<<ps1.x()
                              <<" "
                              <<ps1.y()
                              <<" "
                              <<ps2.x()
                              <<" "
                              <<ps2.y()<<vcl_endl;
                }
                else
                {
                    model_file<<ps2.x()
                              <<" "
                              <<ps2.y()
                              <<" "
                              <<ps1.x()
                              <<" "
                              <<ps1.y()<<vcl_endl;



                }
            }
            model_file.close();
        }
    }
    // if ( flag==true)
    // {
    //     match_file_prefix=match_file_prefix+"_switched";
    // }

    // vcl_cout<<" Norm Shape Cost: "<<norm_shape_cost<<vcl_endl;
    // vcl_cout<<" Norm App   Cost: "<<norm_app_cost<<vcl_endl;
    // vcl_cout<<" App Cost       : "<<app_diff<<vcl_endl;
    // vcl_cout<<" Norm Rgb Sift Cost    : "<< sift_rgb_cost.second<<vcl_endl;
    // vcl_cout<<" Unnorm Rgb Sift Cost  : "<< sift_rgb_cost.first<<vcl_endl;
    
    // unsigned int ds=scurve_sample_ds_;
    // vgl_h_matrix_2d<double> H;
    // compute_similarity(H,
    //                    curve_list1,
    //                    curve_list2,
    //                    map_list,
    //                    path_map,
    //                    ds,
    //                    flag);
    
    // vcl_pair<double,double> p1=
    //     compute_transformed_polygon(H,model_tree,query_tree);

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
    
    vil_image_view<double> image;
    vil_convert_cast(temp,image);

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
        double mag  = gradient_magnitude[i];
        double angle= gradient_angle[i];
        (*grad_data)[index]=mag;
        ++index;
        (*grad_data)[index]=angle;
        ++index;
    }

    vl_free(gradient_magnitude);
    vl_free(gradient_angle);

}

void dbskfg_match_bag_of_fragments::compute_edge_maps(
    vil_image_resource_sptr& input_image,
    vl_sift_pix** grad_data,
    VlSiftFilt** filter)
{

    // Create vid pro storage
    vidpro1_image_storage_sptr inp = new vidpro1_image_storage();
    inp->set_image(input_image);

    dbdet_third_order_color_edge_detector_process pro_color_edg;

    // Before we start the process lets clean input output
    pro_color_edg.clear_input();
    pro_color_edg.clear_output();

    pro_color_edg.parameters()->set_value( "-breduce" , true );
    pro_color_edg.parameters()->set_value( "-sigma" , 1.5 );

    pro_color_edg.add_input(inp);
    bool to_c_status = pro_color_edg.execute();
    pro_color_edg.finish();

    // Create output storage for edge detection
    vcl_vector<bpro1_storage_sptr> edge_det_results;

    // Grab output from color third order edge detection
    // if process did not fail
    if ( to_c_status )
    {
        edge_det_results = pro_color_edg.get_output();
    }

    //Clean up after ourselves
    pro_color_edg.clear_input();
    pro_color_edg.clear_output();

    dbdet_edgemap_storage_sptr input_edgemap;
    input_edgemap.vertical_cast(edge_det_results[0]);
    dbdet_edgemap_sptr edgemap = input_edgemap->get_edgemap();

    vil_image_view<vl_sift_pix> grad_map(input_image->ni(),input_image->nj());
    vil_image_view<vl_sift_pix> orient_map(input_image->ni(),input_image->nj());
    grad_map.fill(0.0);
    orient_map.fill(0.0);

    for (vcl_vector<dbdet_edgel *>::const_iterator 
             e_it = edgemap->edgels.begin(); 
         e_it != edgemap->edgels.end(); ++e_it)
    {
        dbdet_edgel* e = *e_it;
        
        const vgl_point_2d<double > &pt = (e->pt);
        int pt_x = dbdet_round(pt.x());
        int pt_y = dbdet_round(pt.y());
        
        if ( grad_map.in_range(pt_x,pt_y))
        {
            grad_map(pt_x,pt_y)=e->strength;
            orient_map(pt_x,pt_y)=e->tangent;
        }
    }

    unsigned int width  = input_image->ni();
    unsigned int height = input_image->nj();

    double* gradient_magnitude = (double*) 
        vl_malloc(width*height*sizeof(double));
    double* gradient_angle     = (double*) 
        vl_malloc(width*height*sizeof(double));

    *filter = vl_sift_new(width,height,3,3,0);
    *grad_data=(vl_sift_pix*) vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    unsigned int index=0;
    for (unsigned j=0;j<input_image->nj();++j)
    {
        for (unsigned i=0;i<input_image->ni();++i)
        {
            double mag  = grad_map(i,j);
            double angle= orient_map(i,j);
            (*grad_data)[index]=mag;
            ++index;
            (*grad_data)[index]=angle;
            ++index;
            
        }
    }

    // For Debugging purposes
    // vcl_cout<<index<<vcl_endl;
    // vcl_cout<<width*height*2<<vcl_endl;
    // vcl_ofstream grad_angle_data("grad_map_data.txt");
    // vcl_cout<<width<<" "<<height<<vcl_endl;
    // for (unsigned int w=0; w < width*height*2 ; w=w+2)
    // {
    //     grad_angle_data<<(*grad_data)[w]<<vcl_endl;
    //     grad_angle_data<<(*grad_data)[w+1]<<vcl_endl;

    // }
    // grad_angle_data.close();

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
    
    vil_image_view<double> image;
    vil_convert_cast(temp,image);

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
        double mag  = gradient_magnitude[i];
        double angle= gradient_angle[i];
        (*grad_data)[index]=mag;
        ++index;
        (*grad_data)[index]=angle;
        ++index;
    }

    vl_free(gradient_magnitude);
    vl_free(gradient_angle);

}

void dbskfg_match_bag_of_fragments::compute_grad_color_maps(
    vil_image_view<double>& image,
    vl_sift_pix** grad_data)
{

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
        double mag  = gradient_magnitude[i];
        double angle= gradient_angle[i];
        (*grad_data)[index]=mag;
        ++index;
        (*grad_data)[index]=angle;
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
    bool flag,
    double width,
    vl_sift_pix* model_grad_data,
    VlSiftFilt*  model_sift_filter,
    vl_sift_pix* query_grad_data,
    VlSiftFilt*  query_sift_filter,
    double model_scale_ratio,
    double query_scale_ratio)
{
 

    VlFloatVectorComparisonFunction Chi2_distance =    
      vl_get_vector_comparison_function_f (VlDistanceChi2) ;
    double sift_diff= 0.0;
    
    double arclength_shock_curve1=0.0;
    double arclength_shock_curve2=0.0;
    
    double splice_cost_shock_curve1=0.0;
    double splice_cost_shock_curve2=0.0;

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
        
        double sc1_splice = sc1->splice_cost(
            scurve_matching_R_,
            elastic_splice_cost_,
            false,
            combined_edit_,
            sc1->is_leaf_edge());

        double sc2_splice = sc2->splice_cost(
            scurve_matching_R_,
            elastic_splice_cost_,
            false,
            combined_edit_,
            sc2->is_leaf_edge());

        if ( !flag )
        {
            sc1_splice=sc1_splice/model_scale_ratio;
            sc2_splice=sc2_splice/query_scale_ratio;
        }
        else
        {
            sc1_splice=sc1_splice/query_scale_ratio;
            sc2_splice=sc2_splice/model_scale_ratio;

        }

        splice_cost_shock_curve1+=sc1_splice;
        splice_cost_shock_curve2+=sc2_splice;

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

            // Shock Point 2 from Query
            vgl_point_2d<double> ps2 = sc2->sh_pt(cor.second);
            double radius_ps2        = sc2->time(cor.second);
            double theta_ps2         = sc2->theta(cor.second);
            vl_sift_pix descr_ps2[128];
            memset(descr_ps2, 0, sizeof(vl_sift_pix)*128);

            if ( !flag )
            {
                ps1.set(ps1.x()/model_scale_ratio,
                        ps1.y()/model_scale_ratio);
                ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                        ps2.y()/query_scale_ratio);

                shock_curve1.push_back(ps1);
                shock_curve2.push_back(ps2);

                vl_sift_calc_raw_descriptor(model_sift_filter,
                                            model_grad_data,
                                            descr_ps1,
                                            model_sift_filter->width,
                                            model_sift_filter->height,
                                            ps1.x(),
                                            ps1.y(),
                                            (radius_ps1/model_scale_ratio)/2,
                                            theta_ps1);

                vl_sift_calc_raw_descriptor(query_sift_filter,
                                            query_grad_data,
                                            descr_ps2,
                                            query_sift_filter->width,
                                            query_sift_filter->height,
                                            ps2.x(),
                                            ps2.y(),
                                            (radius_ps2/query_scale_ratio)/2,
                                            theta_ps2);
                
                radius_ps1=(radius_ps1/model_scale_ratio);
                radius_ps2=(radius_ps2/query_scale_ratio);
            }
            else
            {
                ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                        ps1.y()/query_scale_ratio);
                ps2.set(ps2.x()/model_scale_ratio,
                        ps2.y()/model_scale_ratio);

                shock_curve1.push_back(ps1);
                shock_curve2.push_back(ps2);

                vl_sift_calc_raw_descriptor(model_sift_filter,
                                            model_grad_data,
                                            descr_ps2,
                                            model_sift_filter->width,
                                            model_sift_filter->height,
                                            ps2.x(),
                                            ps2.y(),
                                            (radius_ps2/model_scale_ratio)/2,
                                            theta_ps2);

                vl_sift_calc_raw_descriptor(query_sift_filter,
                                            query_grad_data,
                                            descr_ps1,
                                            query_sift_filter->width,
                                            query_sift_filter->height,
                                            ps1.x(),
                                            ps1.y(),
                                            (radius_ps1/query_scale_ratio)/2,
                                            theta_ps1);
          

                radius_ps1=(radius_ps1/query_scale_ratio);
                radius_ps2=(radius_ps2/model_scale_ratio);
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
                                                     Chi2_distance);

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

    double length_norm=sift_diff/(arclength_shock_curve1+
                                  arclength_shock_curve2);
    double splice_norm=sift_diff/(splice_cost_shock_curve1+
                                  splice_cost_shock_curve2);

    // vcl_cout << "final cost: " << sift_diff 
    //          << " final norm cost: " << splice_norm 
    //          << "( tree1 total splice: " << splice_cost_shock_curve1
    //          << ", tree2 total splice: " << splice_cost_shock_curve2
    //          << ")" << vcl_endl;

    vcl_pair<double,double> app_diff(splice_norm,length_norm);
    return app_diff;
}

vcl_pair<double,double> dbskfg_match_bag_of_fragments::compute_rgb_sift_cost(
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_vector< pathtable_key >& path_map,
    vcl_vector<double>& dart_distances,
    bool flag,
    double width,
    vl_sift_pix* model_red_grad_data,
    vl_sift_pix* query_red_grad_data,
    vl_sift_pix* model_green_grad_data,
    vl_sift_pix* query_green_grad_data,
    vl_sift_pix* model_blue_grad_data,
    vl_sift_pix* query_blue_grad_data,
    VlSiftFilt* model_sift_filter,
    VlSiftFilt* query_sift_filter,
    double model_scale_ratio,
    double query_scale_ratio)
{
 

    VlFloatVectorComparisonFunction Chi2_distance =    
      vl_get_vector_comparison_function_f (VlDistanceChi2) ;
    double sift_diff= 0.0;
    
    double arclength_shock_curve1=0.0;
    double arclength_shock_curve2=0.0;

    double splice_cost_shock_curve1=0.0;
    double splice_cost_shock_curve2=0.0;

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

        double sc1_splice = sc1->splice_cost(
            scurve_matching_R_,
            elastic_splice_cost_,
            false,
            combined_edit_,
            sc1->is_leaf_edge());

        double sc2_splice = sc2->splice_cost(
            scurve_matching_R_,
            elastic_splice_cost_,
            false,
            combined_edit_,
            sc2->is_leaf_edge());

        
        vcl_pair<unsigned int,unsigned int> query_key1(0,0);
        vcl_pair<unsigned int,unsigned int> query_key2(0,0);
        
        if ( !flag )
        {
            query_key1=sc2->get_curve_id();
            
            query_key2.first = query_key1.second;
            query_key2.second= query_key1.first;
        }
        else
        {
            query_key1=sc1->get_curve_id();
           
            query_key2.first = query_key1.second;
            query_key2.second= query_key1.first;

        }

        bool add_curve=true;

        if ( query_dart_curves_.count(query_key1) ||
             query_dart_curves_.count(query_key2) )
        {
            add_curve=false;
        }
        
        

        if ( !flag )
        {
            sc1_splice=sc1_splice/model_scale_ratio;
            sc2_splice=sc2_splice/query_scale_ratio;
        }
        else
        {
            sc1_splice=sc1_splice/query_scale_ratio;
            sc2_splice=sc2_splice/model_scale_ratio;

        }

        splice_cost_shock_curve1+=sc1_splice;
        splice_cost_shock_curve2+=sc2_splice;

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
      
            vgl_point_2d<double> ps1_green = sc1->sh_pt(cor.first);
            double radius_ps1_green        = sc1->time(cor.first);
            double theta_ps1_green         = sc1->theta(cor.first);
            vl_sift_pix descr_ps1_green[128];
            memset(descr_ps1_green, 0, sizeof(vl_sift_pix)*128);

            vgl_point_2d<double> ps1_blue = sc1->sh_pt(cor.first);
            double radius_ps1_blue        = sc1->time(cor.first);
            double theta_ps1_blue         = sc1->theta(cor.first);
            vl_sift_pix descr_ps1_blue[128];
            memset(descr_ps1_blue, 0, sizeof(vl_sift_pix)*128);

            // Shock Point 2 from Query
            vgl_point_2d<double> ps2_red = sc2->sh_pt(cor.second);
            double radius_ps2_red        = sc2->time(cor.second);
            double theta_ps2_red         = sc2->theta(cor.second);
            vl_sift_pix descr_ps2_red[128];
            memset(descr_ps2_red, 0, sizeof(vl_sift_pix)*128);

            vgl_point_2d<double> ps2_green = sc2->sh_pt(cor.second);
            double radius_ps2_green        = sc2->time(cor.second);
            double theta_ps2_green         = sc2->theta(cor.second);
            vl_sift_pix descr_ps2_green[128];
            memset(descr_ps2_green, 0, sizeof(vl_sift_pix)*128);

            vgl_point_2d<double> ps2_blue = sc2->sh_pt(cor.second);
            double radius_ps2_blue        = sc2->time(cor.second);
            double theta_ps2_blue         = sc2->theta(cor.second);
            vl_sift_pix descr_ps2_blue[128];
            memset(descr_ps2_blue, 0, sizeof(vl_sift_pix)*128);

            if ( !flag )
            {

                ps1_red.set(ps1_red.x()/model_scale_ratio,
                            ps1_red.y()/model_scale_ratio);
                ps1_green.set(ps1_green.x()/model_scale_ratio,
                              ps1_green.y()/model_scale_ratio);
                ps1_blue.set(ps1_blue.x()/model_scale_ratio,
                             ps1_blue.y()/model_scale_ratio);

                ps2_red.set(vcl_fabs(width-(ps2_red.x()/query_scale_ratio)),
                            ps2_red.y()/query_scale_ratio);
                ps2_green.set(vcl_fabs(width-(ps2_green.x()/query_scale_ratio)),
                            ps2_green.y()/query_scale_ratio);
                ps2_blue.set(vcl_fabs(width-(ps2_blue.x()/query_scale_ratio)),
                            ps2_blue.y()/query_scale_ratio);

                shock_curve1.push_back(ps1_red);
                shock_curve2.push_back(ps2_red);

                vl_sift_calc_raw_descriptor(model_sift_filter,
                                            model_red_grad_data,
                                            descr_ps1_red,
                                            model_sift_filter->width,
                                            model_sift_filter->height,
                                            ps1_red.x(),
                                            ps1_red.y(),
                                            (radius_ps1_red/model_scale_ratio)
                                            /2,
                                            theta_ps1_red);

                vl_sift_calc_raw_descriptor(model_sift_filter,
                                            model_green_grad_data,
                                            descr_ps1_green,
                                            model_sift_filter->width,
                                            model_sift_filter->height,
                                            ps1_green.x(),
                                            ps1_green.y(),
                                            (radius_ps1_green/model_scale_ratio)
                                            /2,
                                            theta_ps1_green);

                vl_sift_calc_raw_descriptor(model_sift_filter,
                                            model_blue_grad_data,
                                            descr_ps1_blue,
                                            model_sift_filter->width,
                                            model_sift_filter->height,
                                            ps1_blue.x(),
                                            ps1_blue.y(),
                                            (radius_ps1_blue/model_scale_ratio)
                                            /2,
                                            theta_ps1_blue);

                vl_sift_calc_raw_descriptor(query_sift_filter,
                                            query_red_grad_data,
                                            descr_ps2_red,
                                            query_sift_filter->width,
                                            query_sift_filter->height,
                                            ps2_red.x(),
                                            ps2_red.y(),
                                            (radius_ps2_red/
                                             query_scale_ratio)/2,
                                            theta_ps2_red);

                vl_sift_calc_raw_descriptor(query_sift_filter,
                                            query_green_grad_data,
                                            descr_ps2_green,
                                            query_sift_filter->width,
                                            query_sift_filter->height,
                                            ps2_green.x(),
                                            ps2_green.y(),
                                            (radius_ps2_green/
                                             query_scale_ratio)/2,
                                            theta_ps2_green);

                vl_sift_calc_raw_descriptor(query_sift_filter,
                                            query_blue_grad_data,
                                            descr_ps2_blue,
                                            query_sift_filter->width,
                                            query_sift_filter->height,
                                            ps2_blue.x(),
                                            ps2_blue.y(),
                                            (radius_ps2_blue/
                                             query_scale_ratio)/2,
                                            theta_ps2_blue);

                radius_ps1_red=(radius_ps1_red/model_scale_ratio);
                radius_ps1_green=(radius_ps1_green/model_scale_ratio);
                radius_ps1_blue=(radius_ps1_blue/model_scale_ratio);

                radius_ps2_red=(radius_ps2_red/query_scale_ratio);
                radius_ps2_green=(radius_ps2_green/query_scale_ratio);
                radius_ps2_blue=(radius_ps2_blue/query_scale_ratio);

                if ( add_curve )
                {
                    query_dart_curves_[query_key1].push_back(ps2_red);
                }

            }
            else
            {

                ps1_red.set(vcl_fabs(width-(ps1_red.x()/query_scale_ratio)),
                        ps1_red.y()/query_scale_ratio);
                ps1_green.set(vcl_fabs(width-(ps1_green.x()/query_scale_ratio)),
                        ps1_green.y()/query_scale_ratio);
                ps1_blue.set(vcl_fabs(width-(ps1_blue.x()/query_scale_ratio)),
                        ps1_blue.y()/query_scale_ratio);

                ps2_red.set(ps2_red.x()/model_scale_ratio,
                            ps2_red.y()/model_scale_ratio);
                ps2_green.set(ps2_green.x()/model_scale_ratio,
                              ps2_green.y()/model_scale_ratio);
                ps2_blue.set(ps2_blue.x()/model_scale_ratio,
                             ps2_blue.y()/model_scale_ratio);

                shock_curve1.push_back(ps1_red);
                shock_curve2.push_back(ps2_red);

                vl_sift_calc_raw_descriptor(model_sift_filter,
                                            model_red_grad_data,
                                            descr_ps2_red,
                                            model_sift_filter->width,
                                            model_sift_filter->height,
                                            ps2_red.x(),
                                            ps2_red.y(),
                                            (radius_ps2_red/model_scale_ratio)
                                            /2,
                                            theta_ps2_red);

                vl_sift_calc_raw_descriptor(model_sift_filter,
                                            model_green_grad_data,
                                            descr_ps2_green,
                                            model_sift_filter->width,
                                            model_sift_filter->height,
                                            ps2_green.x(),
                                            ps2_green.y(),
                                            (radius_ps2_green/model_scale_ratio)
                                            /2,
                                            theta_ps2_green);

                vl_sift_calc_raw_descriptor(model_sift_filter,
                                            model_blue_grad_data,
                                            descr_ps2_blue,
                                            model_sift_filter->width,
                                            model_sift_filter->height,
                                            ps2_blue.x(),
                                            ps2_blue.y(),
                                            (radius_ps2_blue/model_scale_ratio)
                                            /2,
                                            theta_ps2_blue);

                vl_sift_calc_raw_descriptor(query_sift_filter,
                                            query_red_grad_data,
                                            descr_ps1_red,
                                            query_sift_filter->width,
                                            query_sift_filter->height,
                                            ps1_red.x(),
                                            ps1_red.y(),
                                            (radius_ps1_red/
                                             query_scale_ratio)/2,
                                            theta_ps1_red);

                vl_sift_calc_raw_descriptor(query_sift_filter,
                                            query_green_grad_data,
                                            descr_ps1_green,
                                            query_sift_filter->width,
                                            query_sift_filter->height,
                                            ps1_green.x(),
                                            ps1_green.y(),
                                            (radius_ps1_green/
                                             query_scale_ratio)/2,
                                            theta_ps1_green);

                vl_sift_calc_raw_descriptor(query_sift_filter,
                                            query_blue_grad_data,
                                            descr_ps1_blue,
                                            query_sift_filter->width,
                                            query_sift_filter->height,
                                            ps1_blue.x(),
                                            ps1_blue.y(),
                                            (radius_ps1_blue/
                                             query_scale_ratio)/2,
                                            theta_ps1_blue);

          
                radius_ps1_red=(radius_ps1_red/query_scale_ratio);
                radius_ps1_green=(radius_ps1_green/query_scale_ratio);
                radius_ps1_blue=(radius_ps1_blue/query_scale_ratio);

                radius_ps2_red=(radius_ps2_red/model_scale_ratio);
                radius_ps2_green=(radius_ps2_green/model_scale_ratio);
                radius_ps2_blue=(radius_ps2_blue/model_scale_ratio);
     
                if ( add_curve )
                {
                    query_dart_curves_[query_key1].push_back(ps1_red);
                }
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
                                                     Chi2_distance);

            vl_eval_vector_comparison_on_all_pairs_f(result_green,
                                                     128,
                                                     descr_ps1_green,
                                                     1,
                                                     descr_ps2_green,
                                                     1,
                                                     Chi2_distance);

            vl_eval_vector_comparison_on_all_pairs_f(result_blue,
                                                     128,
                                                     descr_ps1_blue,
                                                     1,
                                                     descr_ps2_blue,
                                                     1,
                                                     Chi2_distance);


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

        dart_distances.push_back(local_distance);

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
        // }
    }

    double length_norm=sift_diff/(arclength_shock_curve1+
                                  arclength_shock_curve2);
    double splice_norm=sift_diff/(splice_cost_shock_curve1+
                                  splice_cost_shock_curve2);

    // vcl_cout << "final cost: " << sift_diff 
    //          << " final norm cost: " << norm_val 
    //          << "( tree1 total length: " << arclength_shock_curve1
    //          << ", tree2 total length: " << arclength_shock_curve2
    //          << ")" << vcl_endl;

    vcl_pair<double,double> app_diff(length_norm,splice_norm);
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

void dbskfg_match_bag_of_fragments::compute_transformation(
    vgl_h_matrix_2d<double>& H,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_vector< pathtable_key >& path_map,
    unsigned int sampling_interval,
    bool flag)
{

    vcl_vector<vgl_homg_point_2d<double> > model_points,query_points;

    // Get matching pairs
    for (unsigned i = 0; i < map_list.size(); i++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];

        for (unsigned j = 0; j < map_list[i].size(); j+=sampling_interval) 
        {
            vcl_pair<int, int> cor = map_list[i][j];

            vgl_homg_point_2d<double> pm1(sc1->bdry_minus_pt(cor.first));
            vgl_homg_point_2d<double> pm2(sc2->bdry_minus_pt(cor.second));
            
            vgl_homg_point_2d<double> pp1(sc1->bdry_plus_pt(cor.first));
            vgl_homg_point_2d<double> pp2(sc2->bdry_plus_pt(cor.second));
            
            if (! flag )
            {
                model_points.push_back(pm1);
                model_points.push_back(pp1);

                query_points.push_back(pm2);
                query_points.push_back(pp2);

            }
            else
            {
                query_points.push_back(pm1);
                query_points.push_back(pp1);

                model_points.push_back(pm2);
                model_points.push_back(pp2);


            }
        }
    }
}

void dbskfg_match_bag_of_fragments::compute_similarity(
    vgl_h_matrix_2d<double>& H,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_vector< pathtable_key >& path_map,
    unsigned int sampling_interval,
    bool flag)
{

    vcl_vector< rgrl_feature_sptr > pts1;
    vcl_vector< rgrl_feature_sptr > pts2;
    for (unsigned i = 0; i < map_list.size(); i++)
    {
        
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];
        
        for (unsigned j = 0; j < map_list[i].size(); j+=sampling_interval) 
        {
            vcl_pair<int, int> cor = map_list[i][j];
            
            vnl_vector<double> v(2);

            v[0] = sc1->bdry_minus_pt(cor.first).x();
            v[1] = sc1->bdry_minus_pt(cor.first).y();
            pts1.push_back(new rgrl_feature_point(v));
            
            v[0] = sc2->bdry_minus_pt(cor.second).x();
            v[1] = sc2->bdry_minus_pt(cor.second).y();
            pts2.push_back(new rgrl_feature_point(v));
        
            v[0] = sc1->bdry_plus_pt(cor.first).x();
            v[1] = sc1->bdry_plus_pt(cor.first).y();
            pts1.push_back(new rgrl_feature_point(v));
        
            v[0] = sc2->bdry_plus_pt(cor.second).x();
            v[1] = sc2->bdry_plus_pt(cor.second).y();
            pts2.push_back(new rgrl_feature_point(v));
        }
    }
 
    rgrl_match_set_sptr ms = new 
        rgrl_match_set( rgrl_feature_point::type_id() );

    if (!flag) 
    {
        for ( unsigned i=0; i < pts1.size(); ++i ) 
        {
            ms->add_feature_and_match( pts1[i], 0, pts2[i] );
        }

    } 
    else 
    {
        for ( unsigned i=0; i < pts2.size(); ++i ) 
        {
            ms->add_feature_and_match( pts2[i], 0, pts1[i] );
        }
    }
   
    rgrl_estimator_sptr est = new rgrl_est_similarity2d(2);
    rgrl_trans_similarity dummy_trans(2);
    rgrl_transformation_sptr trans = est->estimate( ms, dummy_trans );
    if (!trans) 
    {
        vcl_cout << "transformation could not be estimated by rgrl\n";
    }

    rgrl_trans_similarity* s_trans = 
        dynamic_cast<rgrl_trans_similarity*>(trans.as_pointer());
    if (!s_trans) 
    {
        vcl_cout << "transformation pointer could not be retrieved\n";
    }
  
    H.set_identity();
    vnl_matrix_fixed<double, 3, 3> t_matrix;
    t_matrix(0,0) = s_trans->A()(0,0);
    t_matrix(0,1) = s_trans->A()(0,1);
    t_matrix(1,0) = s_trans->A()(1,0);
    t_matrix(1,1) = s_trans->A()(1,1);
    t_matrix(0,2) = 0;
    t_matrix(1,2) = 0;
    t_matrix(2,0) = 0;
    t_matrix(2,1) = 0;
    t_matrix(2,2) = 1;
    H.set(t_matrix);
    H.set_translation(s_trans->t()[0], s_trans->t()[1]);

}


vcl_pair<double,double> 
dbskfg_match_bag_of_fragments::compute_transformed_polygon(
    vgl_h_matrix_2d<double>& H,dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree)
{
    vgl_polygon<double> model_poly(1);
    model_tree->compute_reconstructed_boundary_polygon(model_poly);

    vgl_polygon<double> query_poly(1);
    query_tree->compute_reconstructed_boundary_polygon(query_poly);

    vgl_point_2d<double> centroid=vgl_centroid(model_poly);
    
    vgl_h_matrix_2d<double> model_H(H);
    model_H.set_translation(0,0);

    vgl_polygon<double> model_model_poly(1);
    vgl_polygon<double> model_transformed_poly(1);
    for (unsigned int s = 0; s < model_poly.num_sheets(); ++s)
    {
        for (unsigned int p = 0; p < model_poly[s].size(); ++p)
        {
            // Compute model to model poly
            {
                // 1. Subtract off Centroid
                vgl_homg_point_2d<double> pt(model_poly[s][p].x()-centroid.x(),
                                             model_poly[s][p].y()-centroid.y());
                
                // 2. Transform Point
                vgl_homg_point_2d<double> transformed=model_H(pt);
                
                // 3. Move point back
                vgl_point_2d<double> temp(transformed);
                vgl_point_2d<double> pt_t(temp.x() + centroid.x(), 
                                          temp.y() + centroid.y());
                
                model_model_poly.push_back(pt_t);
            }

            // Compute model to query poly
            {
                // 1. Get Pt
                vgl_homg_point_2d<double> pt(model_poly[s][p].x(),
                                             model_poly[s][p].y());
                
                // 2. Transform Point
                vgl_homg_point_2d<double> transformed=H(pt);
                
                vgl_point_2d<double> pt_t(transformed);
                
                model_transformed_poly.push_back(pt_t);
            }
        } 
    
    }
    
    double m_m_area_of_intersection(0);
    double m_q_area_of_intersection(0);

    // Store result of model intersection with itself
    {
        int value;

        vgl_polygon<double> m_m_intersection;
        
        //Take union of two polygons
        m_m_intersection = vgl_clip(model_poly,              // p1
                                    model_model_poly,        // p2
                                    vgl_clip_type_intersect, // p1 |  p2
                                    &value);                 // test if success
        
        m_m_area_of_intersection
            =vgl_area(m_m_intersection)/vgl_area(model_poly);
    }
    
    // Store result of model intersection with query
    {
        int value;

        vgl_polygon<double> m_q_intersection;
        
        //Take union of two polygons
        m_q_intersection = vgl_clip(model_transformed_poly,   // p1
                                    query_poly,               // p2
                                    vgl_clip_type_intersect,  // p1 |  p2
                                    &value);                  // test if success
        m_q_area_of_intersection
            =vgl_area(m_q_intersection)/vgl_area(query_poly);

    }
   
    vcl_pair<double,double> result;
    result.first=m_m_area_of_intersection;
    result.second=m_q_area_of_intersection;


    // vcl_cout<<"Model to model: "<<result.first<<vcl_endl;
    // vcl_cout<<"Model to query: "<<result.second<<vcl_endl;

    // {
    //     vcl_ofstream model_file("model_poly.txt");
    //     for (unsigned int s = 0; s < model_poly.num_sheets(); ++s)
    //     {
    //         for (unsigned int p = 0; p < model_poly[s].size(); ++p)
    //         {
    //             model_file<<model_poly[s][p].x()
    //                       <<","<<model_poly[s][p].y()<<vcl_endl;
    //         }
    //     }
    //     model_file.close();
    // }

    // {
    //     vcl_ofstream model_file("model_transformed_poly.txt");
    //     for (unsigned int s = 0; s < model_transformed_poly.num_sheets(); ++s)
    //     {
    //         for (unsigned int p = 0; p < model_transformed_poly[s].size(); ++p)
    //         {
    //             model_file<<model_transformed_poly[s][p].x()<<","
    //                       <<model_transformed_poly[s][p].y()<<vcl_endl;
    //         }
    //     }
    //     model_file.close();
    // }

    // {
    //     vcl_ofstream model_file("model_model_poly.txt");
    //     for (unsigned int s = 0; s < model_model_poly.num_sheets(); ++s)
    //     {
    //         for (unsigned int p = 0; p < model_model_poly[s].size(); ++p)
    //         {
    //             model_file<<model_model_poly[s][p].x()<<","
    //                       <<model_model_poly[s][p].y()<<vcl_endl;
    //         }
    //     }
    //     model_file.close();
    // }

    // {
    //     vcl_ofstream query_file("query_poly.txt");
    //     for (unsigned int s = 0; s < query_poly.num_sheets(); ++s)
    //     {
    //         for (unsigned int p = 0; p < query_poly[s].size(); ++p)
    //         {
    //             query_file<<query_poly[s][p].x()
    //                       <<","<<query_poly[s][p].y()<<vcl_endl;
    //         }
    //     }
    //     query_file.close();
    // }

    return result;
}

double dbskfg_match_bag_of_fragments::compute_outer_shock_edit_distance(
    dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree)
{
    vgl_polygon<double> model_poly(1);
    model_tree->compute_reconstructed_boundary_polygon(model_poly);

    vgl_polygon<double> query_poly(1);
    query_tree->compute_reconstructed_boundary_polygon(query_poly);

    // create new bounding box
    vsol_box_2d_sptr bbox = new vsol_box_2d();
    
    for (unsigned int s = 0; s < model_poly.num_sheets(); ++s)
    {
        for (unsigned int p = 0; p < model_poly[s].size(); ++p)
        {
            bbox->add_point(model_poly[s][p].x(),
                            model_poly[s][p].y());
        }
    }


    for (unsigned int s = 0; s < query_poly.num_sheets(); ++s)
    {
        for (unsigned int p = 0; p < query_poly[s].size(); ++p)
        {
            bbox->add_point(query_poly[s][p].x(),
                            query_poly[s][p].y());
        }
    }

    // Enlarge bounding box from size
    // Calculate xcenter, ycenter
    double xcenter = bbox->width()/2.0;
    double ycenter = bbox->height()/2.0;
    
    // Translate to center and scale
    double xmin_scaled = ((0-xcenter)*2)+xcenter;
    double ymin_scaled = ((0-ycenter)*2)+ycenter;
    double xmax_scaled = ((bbox->width()-xcenter)*2)+xcenter;
    double ymax_scaled = ((bbox->height()-ycenter)*2)+ycenter;

    bbox->add_point(xmin_scaled,ymin_scaled);
    bbox->add_point(xmax_scaled,ymax_scaled);

    double start=((bbox->height()/2)+bbox->get_min_y())-0.5;
    double stop=((bbox->height()/2)+bbox->get_min_y())+0.5;

    vcl_vector<vsol_point_2d_sptr> points;
    points.push_back(new vsol_point_2d(bbox->get_min_x(),start));
    points.push_back(new vsol_point_2d(bbox->get_min_x(),bbox->get_min_y()));
    points.push_back(new vsol_point_2d(bbox->get_max_x(),bbox->get_min_y()));
    points.push_back(new vsol_point_2d(bbox->get_max_x(),bbox->get_max_y()));
    points.push_back(new vsol_point_2d(bbox->get_min_x(),bbox->get_max_y()));
    points.push_back(new vsol_point_2d(bbox->get_min_x(),stop));

    vsol_polyline_2d_sptr polyline=new vsol_polyline_2d(points);
    polyline->set_id(10e6);
    vsol_polygon_2d_sptr model_contours = bsol_algs::poly_from_vgl(model_poly);
    vsol_polygon_2d_sptr query_contours = bsol_algs::poly_from_vgl(query_poly);


    vidpro1_vsol2D_storage_sptr model_vsol = vidpro1_vsol2D_storage_new();
    model_vsol->add_object(polyline->cast_to_spatial_object());
    model_vsol->add_object(model_contours->cast_to_spatial_object());

    vidpro1_vsol2D_storage_sptr query_vsol = vidpro1_vsol2D_storage_new();
    query_vsol->add_object(polyline->cast_to_spatial_object());
    query_vsol->add_object(query_contours->cast_to_spatial_object());

    dbskfg_load_binary_composite_graph_process load_model_pro,load_query_pro;

    bool status=load_model_pro.compute_outer_shock(model_vsol);
    status=load_query_pro.compute_outer_shock(query_vsol);

    dbskfg_composite_graph_sptr model_cg(0),query_cg(0);
    load_model_pro.get_first_graph(model_cg);
    load_query_pro.get_first_graph(query_cg);

    //: prepare the trees also
    dbskfg_cgraph_directed_tree_sptr model_os_tree = new 
        dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                    scurve_interpolate_ds_, 
                                    scurve_matching_R_,
                                    false,
                                    area_weight_);
    
    bool f1=model_os_tree->acquire
        (model_cg, elastic_splice_cost_, 
         circular_ends_, combined_edit_);

    //: prepare the trees also
    dbskfg_cgraph_directed_tree_sptr query_os_tree = new 
        dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                    scurve_interpolate_ds_, 
                                    scurve_matching_R_,
                                    false,
                                    area_weight_);
    
    f1=query_os_tree->acquire
        (query_cg, elastic_splice_cost_, 
         circular_ends_, combined_edit_);
    
    double norm_shape_cost(0.0);
    double app_diff(0.0);
    double norm_app_cost(0.0);
    double rgb_avg_cost(0.0);
    double norm_shape_cost_length(0.0);
    
    // Match model to query
    match_two_graphs_root_node_orig(model_os_tree,
                                    query_os_tree,
                                    norm_shape_cost,
                                    norm_shape_cost_length,
                                    app_diff,
                                    norm_app_cost,
                                    rgb_avg_cost);

    // dbskfg_composite_graph_fileio fileio;
    // fileio.write_contour_composite_graph(model_cg,"model_cg_outer");
    // fileio.write_contour_composite_graph(query_cg,"query_cg_outer");

    // vcl_vector< vsol_spatial_object_2d_sptr > model_list = 
    //     model_vsol->all_data();

    // vcl_vector< vsol_spatial_object_2d_sptr > query_list = 
    //     query_vsol->all_data();

    // dbsol_save_cem(query_list,"query_list.cem");
    // dbsol_save_cem(model_list,"model_list.cem");
    // exit(0);

    model_os_tree=0;
    query_os_tree=0;
    model_vsol=0;
    query_vsol=0;
    return norm_shape_cost;
}


double
dbskfg_match_bag_of_fragments::compute_curve_matching_cost(
    dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree)
{
    vgl_polygon<double> model_poly(1);
    model_tree->compute_reconstructed_boundary_polygon(model_poly);

    vgl_polygon<double> query_poly(1);
    query_tree->compute_reconstructed_boundary_polygon(query_poly);

    bsol_intrinsic_curve_2d_sptr c1=new bsol_intrinsic_curve_2d();
    bsol_intrinsic_curve_2d_sptr c2=new bsol_intrinsic_curve_2d();

    for (unsigned int s = 0; s < model_poly.num_sheets(); ++s)
    {
        for (unsigned int p = 0; p < model_poly[s].size(); ++p)
        {
            c1->add_vertex(model_poly[s][p].x(),
                           model_poly[s][p].y());
        }
    }


    for (unsigned int s = 0; s < query_poly.num_sheets(); ++s)
    {
        for (unsigned int p = 0; p < query_poly[s].size(); ++p)
        {
            c2->add_vertex(query_poly[s][p].x(),
                           query_poly[s][p].y());
        }
    }

    c1->setOpen(false);        
    c2->setOpen(false);

    dbcvr_clsd_cvmatch_sptr clsd_cvmatch = new 
        dbcvr_clsd_cvmatch(c1,c2,(double)10.0f,3);
    clsd_cvmatch->setStretchCostFlag(false);
    clsd_cvmatch->Match();  
    
    double minCost=1E10;
    int minIndex;
    for (int count=0;count<c1->size();count++)
    {
        if (minCost>clsd_cvmatch->finalCost(count))
        {
            minCost=clsd_cvmatch->finalCost(count);
            minIndex=count;
        }
    }
    
    return minCost;
}


void dbskfg_match_bag_of_fragments::convert_to_color_space(
    vil_image_resource_sptr& input_image,
    vil_image_view<double>& o1,
    vil_image_view<double>& o2,
    vil_image_view<double>& o3,
    ColorSpace color_space)
{
    vil_image_view<vxl_byte> image = input_image->get_view();
    unsigned int w = image.ni(); 
    unsigned int h = image.nj();
    o1.set_size(w,h);
    o2.set_size(w,h);
    o3.set_size(w,h);

    if ( color_space == RGB )
    {

        vil_image_view<vxl_byte> red   = vil_plane(image,0);
        vil_image_view<vxl_byte> green = vil_plane(image,1);
        vil_image_view<vxl_byte> blue  = vil_plane(image,2);
        
        vil_convert_cast(red,o1);
        vil_convert_cast(green,o2);
        vil_convert_cast(blue,o3);

    }
    else
    {
        for (unsigned r = 0; r < h; r++)
        {
            for (unsigned c = 0; c < w; c++)
            {
                double red=image(c,r,0);
                double green=image(c,r,1);
                double blue=image(c,r,2);
                o1(c,r) = (red-green)/vcl_sqrt(2);
                o2(c,r) = (red+green-2*blue)/vcl_sqrt(6);
                o3(c,r) = (red+green+blue)/vcl_sqrt(3);
                
                if ( color_space == NOPP )
                {
                    o1(c,r)=o1(c,r)/o3(c,r);
                    o2(c,r)=o2(c,r)/o3(c,r);
                }
            }
        }
    }


}

void dbskfg_match_bag_of_fragments::write_out_dart_data()
{
    
    vcl_string dart_file="dart_data.txt";

    vcl_ofstream model_file(dart_file.c_str());
    model_file<<query_dart_curves_.size()<<vcl_endl;

    {
        vcl_map<vcl_pair<unsigned int,unsigned int>,
            vcl_vector<vgl_point_2d<double> > >
            ::iterator it;
        
        
        for ( it = query_dart_curves_.begin() ; it != query_dart_curves_.end();
              ++it)
        {
            vcl_pair<int,int> pair=(*it).first;
            vcl_vector<vgl_point_2d<double> > curve=(*it).second;
            model_file<<pair.first<<","<<pair.second<<vcl_endl;
            model_file<<curve.size()<<vcl_endl;
            for ( unsigned int c=0; c < curve.size() ; ++c)
            {
                model_file<<curve[c].x()<<","<<curve[c].y()<<vcl_endl;
            }
        }
    }

    // Write out model
    {

        model_file<<model_dart_distances_.size()<<vcl_endl;
        vcl_map<unsigned int,
            vcl_vector< vcl_pair<vcl_pair<unsigned int,unsigned int>,
            double> > >::
            iterator it;
        for ( it = model_dart_distances_.begin() ; it != model_dart_distances_
                  .end(); ++it)
        {
            vcl_vector< vcl_pair<vcl_pair<unsigned int,unsigned 
                int>,double> > vec=
                (*it).second;
            vcl_vector< vcl_pair<vcl_pair<unsigned int,
                unsigned int>, dbskr_scurve_sptr > > p2 =
                model_dart_curves_[(*it).first];

            model_file<<vec.size()<<vcl_endl;
            for ( unsigned int v=0; v < vec.size() ; ++v)
            {
                vcl_pair<vcl_pair<unsigned int,unsigned int>,
                    double> pair=vec[v];
                model_file<<pair.first.first<<","<<pair.first.second<<vcl_endl;
                model_file<<pair.second<<vcl_endl;

                dbskr_scurve_sptr curve=p2[v].second;
                model_file<<curve->num_points()<<vcl_endl;
                
                for ( unsigned int c=0; c < curve->num_points(); ++c)
                {
                    vgl_point_2d<double> pt=curve->sh_pt(c);
                    model_file<<pt.x()<<","<<pt.y()<<vcl_endl;
                }
                
            }
            
        }
        
        
    }
    model_file.close();








}
