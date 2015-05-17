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
#include <dbskfg/algo/dbskfg_app_curve_match.h>
#include <dbskfg/dbskfg_utilities.h>
#include <dbskfg/algo/dbskfg_compute_sift.h>
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

#include <vil/vil_bilin_interp.h>
#include <vil/vil_flip.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <bsta/bsta_joint_histogram_3d.h>
#include <bsta/bsta_spherical_histogram.h>
#include <bbas/bil/algo/bil_color_conversions.h>

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vil/vil_transpose.h>
extern "C" {
#include <vl/imopv.h>
#include <vl/generic.h>
#include <vl/kmeans.h>
#include <vl/dsift.h>
}

#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_distance.h>
#include <string.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix_ref.h>
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
#include <vil/algo/vil_orientations.h>

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
    bool mask_grad,
    double area_weight,
    double ref_area,
    ShapeAlgorithmArea shape_alg,
    GradColorSpace grad_color_space,
    RawColorSpace raw_color_space,
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
      output_binary_h_file_(output_file),
      output_removed_regions_(output_file),
      output_parts_file_(output_file),
      output_dist_file_(output_file),
      output_dist_to_category_file_(output_file),
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
      grad_color_space_(grad_color_space),
      raw_color_space_(raw_color_space),
      forest_(0),
      searcher_(0),
      means_cg_(0),
      covariances_cg_(0),
      priors_cg_(0),
      means_color_(0),
      covariances_color_(0),
      priors_color_(0),
      keywords_(0),
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
      model_grad_fliplr_data_(0),
      model_grad_fliplr_red_data_(0),
      model_grad_fliplr_green_data_(0),
      model_grad_fliplr_blue_data_(0),
      query_grad_fliplr_data_(0),
      query_grad_fliplr_red_data_(0),
      query_grad_fliplr_green_data_(0),
      query_grad_fliplr_blue_data_(0),
      model_sift_filter_(0),
      query_sift_filter_(0)
      
{

    if ( raw_color_space_ == dbskfg_match_bag_of_fragments::LAB )
    {
        vcl_cout<<"Computing color histograms over LAB space"<<vcl_endl;
    }
    else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::HSV )
    {
        vcl_cout<<"Computing color histograms over HSV space"<<vcl_endl;
    }
    else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::RGB_2 )
    {
        vcl_cout<<"Computing color histograms over RGB_2 space"<<vcl_endl;
    }
    else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::OPP_2 )
    {
        vcl_cout<<"Computing color histograms over OPP_2 space"<<vcl_endl;
    }
    else
    {
        vcl_cout<<"Computing color histograms over NOPP_2 space"<<vcl_endl;
    }

    if ( grad_color_space_ == dbskfg_match_bag_of_fragments::RGB )
    {
        vcl_cout<<"Computing color gradients over RGB space"<<vcl_endl;
    }
    else if ( grad_color_space_ == dbskfg_match_bag_of_fragments::OPP )
    {
        vcl_cout<<"Computing color gradients over OPP space"<<vcl_endl;
    }
    else if ( grad_color_space_ == dbskfg_match_bag_of_fragments::NOPP )
    {
        vcl_cout<<"Computing color gradients over NOPP space"<<vcl_endl;
    }
    else
    {
        vcl_cout<<"Computing color gradients over LAB space"<<vcl_endl;
    }

    output_match_file_ = output_match_file_ + "_similarity_matrix.xml";
    output_html_file_  = output_html_file_ +  "_similarity_matrix.html";
    output_binary_file_ = output_binary_file_ + "_binary_similarity_matrix.bin";
    output_binary_h_file_ = output_binary_h_file_ + "_binary_h_matrix.bin";
    output_parts_file_ = output_parts_file_ + "_parts.txt";
    output_removed_regions_ = output_removed_regions_ + "_removed_regions.txt";
    output_dist_to_category_file_ = output_dist_to_category_file_ + "_d2c.txt";
 
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

                vl_sift_pix* grad_fliplr_data(0);
                vl_sift_pix* red_fliplr_data(0);
                vl_sift_pix* blue_fliplr_data(0);
                vl_sift_pix* green_fliplr_data(0);

                VlSiftFilt* sift_filter(0);

                vgl_polygon<double> mask=model_fragments_polys_[index].second;

                compute_grad_maps(model_img_sptr,
                                  &grad_data,
                                  &sift_filter,
                                  mask,
                                  mask_grad);

                compute_grad_maps(model_img_sptr,
                                  &grad_fliplr_data,
                                  &sift_filter,
                                  mask,
                                  mask_grad,
                                  true);

                vil_image_view<double> o1,o2,o3;
                convert_to_color_space(model_img_sptr,o1,o2,o3,
                                       grad_color_space_);

                vil_image_view<double> chan_1,chan_2,chan_3;

                chan_1.set_size(model_img_sptr->ni(),model_img_sptr->nj());
                chan_2.set_size(model_img_sptr->ni(),model_img_sptr->nj());
                chan_3.set_size(model_img_sptr->ni(),model_img_sptr->nj());

                if ( raw_color_space_ == dbskfg_match_bag_of_fragments::LAB )
                {
                    convert_RGB_to_Lab(model_img_sptr->get_view(),
                                       chan_1,
                                       chan_2,
                                       chan_3);
                }
                else if ( raw_color_space_ == 
                          dbskfg_match_bag_of_fragments::HSV )
                {
                    // Todo later
                }
                else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::
                          RGB_2 )
                {
                    vil_image_view<vxl_byte> temp=model_img_sptr->get_view();

                    vil_image_view<vxl_byte> red   = vil_plane(temp,0);
                    vil_image_view<vxl_byte> green = vil_plane(temp,1);
                    vil_image_view<vxl_byte> blue  = vil_plane(temp,2);
        
                    vil_convert_cast(red,chan_1);
                    vil_convert_cast(green,chan_2);
                    vil_convert_cast(blue,chan_3);

                }
                else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::
                          OPP_2)
                {
                    convert_to_color_space(model_img_sptr,chan_1,chan_2,
                                           chan_3,
                                           dbskfg_match_bag_of_fragments::OPP);
                }
                else
                {
                    convert_to_color_space(model_img_sptr,chan_1,chan_2,
                                           chan_3,
                                           dbskfg_match_bag_of_fragments::NOPP);
                }

                
                compute_grad_color_maps(o1,
                                        &red_data,
                                        mask,
                                        mask_grad);
                
                compute_grad_color_maps(o2,
                                        &green_data,
                                        mask,
                                        mask_grad);
                
                compute_grad_color_maps(o3,
                                        &blue_data,
                                        mask,
                                        mask_grad);

                compute_grad_color_maps(o1,
                                        &red_fliplr_data,
                                        mask,
                                        mask_grad,
                                        true);
                
                compute_grad_color_maps(o2,
                                        &green_fliplr_data,
                                        mask,
                                        mask_grad,
                                        true);
                
                compute_grad_color_maps(o3,
                                        &blue_fliplr_data,
                                        mask,
                                        mask_grad,
                                        true);


                vl_sift_set_magnif(sift_filter,1.0);

                model_images_grad_data_[title_stream.str()]=grad_data;
                model_images_sift_filter_[title_stream.str()]=sift_filter;
                model_images_grad_data_red_[title_stream.str()]=red_data;
                model_images_grad_data_green_[title_stream.str()]=green_data;
                model_images_grad_data_blue_[title_stream.str()]=blue_data;

                model_fliplr_images_grad_data_
                    [title_stream.str()]=grad_fliplr_data;
                
                model_fliplr_images_grad_data_red_
                    [title_stream.str()]=red_fliplr_data;
                
                model_fliplr_images_grad_data_green_
                    [title_stream.str()]=green_fliplr_data;
               
                model_fliplr_images_grad_data_blue_
                    [title_stream.str()]=blue_fliplr_data;

                model_images_chan1_data_[title_stream.str()]=chan_1;
                model_images_chan2_data_[title_stream.str()]=chan_2;
                model_images_chan3_data_[title_stream.str()]=chan_3;

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

            vgl_polygon<double> mask=model_fragments_polys_[0].second;

            compute_grad_maps(model_image_,
                              &model_grad_data_,
                              &model_sift_filter_,
                              mask,
                              mask_grad);

            compute_grad_maps(model_image_,
                              &model_grad_fliplr_data_,
                              &model_sift_filter_,
                              mask,
                              mask_grad,
                              true);
            
            vil_image_view<double> o1,o2,o3;
            convert_to_color_space(model_image_,o1,o2,o3,grad_color_space_);
            
            model_chan1_data_.set_size(model_image_->ni(),model_image_->nj());
            model_chan2_data_.set_size(model_image_->ni(),model_image_->nj());
            model_chan3_data_.set_size(model_image_->ni(),model_image_->nj());

            if ( raw_color_space_ == dbskfg_match_bag_of_fragments::LAB )
            {
                convert_RGB_to_Lab(model_image_->get_view(),
                                   model_chan1_data_,
                                   model_chan2_data_,
                                   model_chan3_data_);
            }
            else if ( raw_color_space_ == 
                      dbskfg_match_bag_of_fragments::HSV )
            {
                // Todo later
            }
            else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::RGB_2 )
            {

                vil_image_view<vxl_byte> temp=model_image_->get_view();

                vil_image_view<vxl_byte> red   = vil_plane(temp,0);
                vil_image_view<vxl_byte> green = vil_plane(temp,1);
                vil_image_view<vxl_byte> blue  = vil_plane(temp,2);
        
                vil_convert_cast(red,model_chan1_data_);
                vil_convert_cast(green,model_chan2_data_);
                vil_convert_cast(blue,model_chan3_data_);

            }
            else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::OPP_2 )
            {
                convert_to_color_space(model_image_,
                                       model_chan1_data_,
                                       model_chan2_data_,
                                       model_chan3_data_,
                                       dbskfg_match_bag_of_fragments::OPP);
            }
            else
            {
                convert_to_color_space(model_image_,
                                       model_chan1_data_,
                                       model_chan2_data_,
                                       model_chan3_data_,
                                       dbskfg_match_bag_of_fragments::NOPP);
            }

            
            compute_grad_color_maps(o1,
                                    &model_grad_red_data_,
                                    mask,
                                    mask_grad);
            
            compute_grad_color_maps(o2,
                                    &model_grad_green_data_,
                                    mask,
                                    mask_grad);
            
            compute_grad_color_maps(o3,
                                    &model_grad_blue_data_,
                                    mask,
                                    mask_grad);

            compute_grad_color_maps(o1,
                                    &model_grad_fliplr_red_data_,
                                    mask,
                                    mask_grad,
                                    true);
            
            compute_grad_color_maps(o2,
                                    &model_grad_fliplr_green_data_,
                                    mask,
                                    mask_grad,
                                    true);
            
            compute_grad_color_maps(o3,
                                    &model_grad_fliplr_blue_data_,
                                    mask,
                                    mask_grad,
                                    true);
 
            vl_sift_set_magnif(model_sift_filter_,1.0);

        }

        if ( query_image_ )
        {
            vcl_cout<<"Computing query image grad data"<<vcl_endl;

            vgl_polygon<double> mask=query_fragments_polys_[0].second;

            compute_grad_maps(query_image_,
                              &query_grad_data_,
                              &query_sift_filter_,
                              mask,
                              mask_grad);

            compute_grad_maps(query_image_,
                              &query_grad_fliplr_data_,
                              &query_sift_filter_,
                              mask,
                              mask_grad,
                              true);

            vil_image_view<double> o1,o2,o3;
            convert_to_color_space(query_image_,o1,o2,o3,grad_color_space_);
            
            query_chan1_data_.set_size(query_image_->ni(),query_image_->nj());
            query_chan2_data_.set_size(query_image_->ni(),query_image_->nj());
            query_chan3_data_.set_size(query_image_->ni(),query_image_->nj());
           
            if ( raw_color_space_ == dbskfg_match_bag_of_fragments::LAB )
            {

                convert_RGB_to_Lab(query_image_->get_view(),
                                   query_chan1_data_,
                                   query_chan2_data_,
                                   query_chan3_data_);
            }
            else if ( raw_color_space_ == 
                      dbskfg_match_bag_of_fragments::HSV )
            {
                // Todo later
            }
            else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::RGB_2 )
            {
                vil_image_view<vxl_byte> temp=query_image_->get_view();

                vil_image_view<vxl_byte> red   = vil_plane(temp,0);
                vil_image_view<vxl_byte> green = vil_plane(temp,1);
                vil_image_view<vxl_byte> blue  = vil_plane(temp,2);
        
                vil_convert_cast(red,query_chan1_data_);
                vil_convert_cast(green,query_chan2_data_);
                vil_convert_cast(blue,query_chan3_data_);
            }
            else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::OPP_2 )
            {
                convert_to_color_space(query_image_,
                                       query_chan1_data_,
                                       query_chan2_data_,
                                       query_chan3_data_,
                                       dbskfg_match_bag_of_fragments::OPP);
            }
            else 
            {
                convert_to_color_space(query_image_,
                                       query_chan1_data_,
                                       query_chan2_data_,
                                       query_chan3_data_,
                                       dbskfg_match_bag_of_fragments::NOPP);
            }
            
            compute_grad_color_maps(o1,
                                    &query_grad_red_data_,
                                    mask,
                                    mask_grad);
            
            compute_grad_color_maps(o2,
                                    &query_grad_green_data_,
                                    mask,
                                    mask_grad);
            
            compute_grad_color_maps(o3,
                                    &query_grad_blue_data_,
                                    mask,
                                    mask_grad);

            compute_grad_color_maps(o1,
                                    &query_grad_fliplr_red_data_,
                                    mask,
                                    mask_grad,
                                    true);
            
            compute_grad_color_maps(o2,
                                    &query_grad_fliplr_green_data_,
                                    mask,
                                    mask_grad,
                                    true);
            
            compute_grad_color_maps(o3,
                                    &query_grad_fliplr_blue_data_,
                                    mask,
                                    mask_grad,
                                    true);
            
            vl_sift_set_magnif(query_sift_filter_,1.0);

            // Create query dist maps

            
            dist_maps_[0]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[30]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[60]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[90]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[120]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[150]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[179]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[209]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[239]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[267]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[297]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[327]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[357]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);
            dist_maps_[387]=
                vnl_matrix<double>(query_image_->ni(),query_image_->nj(),0.0);

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

    if ( forest_ )
    {
        vl_sift_pix* tree_data = (vl_sift_pix*) forest_->data;
        vl_free(tree_data);
        vl_kdforest_delete(forest_);
    }
    

    if ( means_cg_ )
    {
        vl_free(means_cg_);
        means_cg_=0;
    }

    if ( covariances_cg_ )
    {
        vl_free(covariances_cg_);
        covariances_cg_=0;
    }

    if ( priors_cg_ )
    {
        vl_free(priors_cg_);
        priors_cg_=0;
    }

    // if ( searcher_ )
    // {
    //     vl_kdforestsearcher_delete(searcher_);
    // }
    
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
    vcl_map<unsigned int,vgl_polygon<double> > cgraph_polys=
        load_pro.get_polygons();
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
        model_fragments_polys_[(*it).first]=vcl_make_pair(
            stream.str(),
            cgraph_polys[(*it).first]);


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
    vcl_map<unsigned int,vgl_polygon<double> > cgraph_polys=
        load_pro.get_polygons();

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
        query_fragments_polys_[(*it).first]=vcl_make_pair(
            stream.str(),
            cgraph_polys[(*it).first]);
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

        vil_image_view<double> model_channel1(model_images_chan1_data_
                                               [(*m_iterator).second.first]);
        
        vil_image_view<double> model_channel2(model_images_chan2_data_
                                               [(*m_iterator).second.first]);

        vil_image_view<double> model_channel3(model_images_chan3_data_
                                               [(*m_iterator).second.first]);
    
        //: prepare the trees also
        dbskfg_cgraph_directed_tree_sptr model_tree = new 
            dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                        scurve_interpolate_ds_, 
                                        scurve_matching_R_,
                                        false,
                                        area_weight_,
                                        model_images_grad_data,
                                        model_images_sift_filter,
                                        model_images_grad_data_red,
                                        model_images_grad_data_green,
                                        model_images_grad_data_blue,
                                        (*m_iterator).first,
                                        &model_channel1,
                                        &model_channel2,
                                        &model_channel3);
    

        bool f1=model_tree->acquire
            ((*m_iterator).second.second, elastic_splice_cost_, 
             circular_ends_, combined_edit_);

        vcl_map<double,vcl_pair<unsigned int,unsigned int> >
            model_map;

        for ( q_iterator = query_fragments_.begin() ; 
              q_iterator != query_fragments_.end() ; ++q_iterator)
        {

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

            vil_image_view<double> query_channel1(
                model_images_chan1_data_
                [(*m_iterator).second.first]);
            
            vil_image_view<double> query_channel2(
                model_images_chan2_data_
                [(*m_iterator).second.first]);

            vil_image_view<double> query_channel3(
                model_images_chan3_data_
                [(*m_iterator).second.first]);

            if ( query_image_ )
            {

                query_images_grad_data = query_grad_data_;
                query_images_sift_filter = query_sift_filter_;
                query_images_grad_data_red = query_grad_red_data_;
                query_images_grad_data_green = query_grad_green_data_;
                query_images_grad_data_blue = query_grad_blue_data_;
                query_channel1 = query_chan1_data_;
                query_channel2 = query_chan2_data_;
                query_channel3 = query_chan3_data_;
            }

            //: prepare the trees also
            dbskfg_cgraph_directed_tree_sptr query_tree = new
                dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                            scurve_interpolate_ds_, 
                                            scurve_matching_R_,
                                            false,
                                            area_weight_,
                                            query_images_grad_data,
                                            query_images_sift_filter,
                                            query_images_grad_data_red,
                                            query_images_grad_data_green,
                                            query_images_grad_data_blue,
                                            (*q_iterator).first,
                                            &query_channel1,
                                            &query_channel2,
                                            &query_channel3);
            
            bool f1=query_tree->acquire
                ((*q_iterator).second.second, elastic_splice_cost_, 
                 circular_ends_, combined_edit_);

            double norm_shape_cost(0.0);
            double app_diff(0.0);
            double norm_app_cost(0.0);
            double rgb_avg_cost(0.0);
            double norm_shape_cost_length(0.0);
            double frob_norm(0.0);

            // Match model to query
            match_two_graphs_root_node_orig(model_tree,
                                            query_tree,
                                            norm_shape_cost,
                                            norm_shape_cost_length,
                                            app_diff,
                                            norm_app_cost,
                                            rgb_avg_cost,
                                            frob_norm);
            
            if ( mirror_)
            {
                //: prepare the trees also
                dbskfg_cgraph_directed_tree_sptr query_mirror_tree = new
                    dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                                scurve_interpolate_ds_, 
                                                scurve_matching_R_,
                                                mirror_,
                                                area_weight_,
                                                query_images_grad_data,
                                                query_images_sift_filter,
                                                query_images_grad_data_red,
                                                query_images_grad_data_green,
                                                query_images_grad_data_blue,
                                                (*q_iterator).first,
                                                &query_channel1,
                                                &query_channel2,
                                                &query_channel3);

                
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
                                                frob_norm,
                                                "",
                                                true,
                                                norm_shape_cost);

                norm_shape_cost = ( norm_shape_cost < norm_shape_mirror_cost)
                    ? norm_shape_cost : norm_shape_mirror_cost;
                norm_shape_cost_length = ( norm_shape_cost_length 
                                           < norm_shape_mirror_cost_length)
                    ? norm_shape_cost_length : norm_shape_mirror_cost_length;
                // app_diff = ( app_diff < app_mirror_diff )
                //     ? app_diff: app_mirror_diff;
                // norm_app_cost = ( norm_app_cost < norm_app_mirror_cost )
                //     ? norm_app_cost: norm_app_mirror_cost;
                if ( rgb_avg_mirror_cost !=  1.0e6 )
                {
                    app_diff = app_mirror_diff;
                    norm_app_cost = norm_app_mirror_cost;
                    rgb_avg_cost = rgb_avg_mirror_cost;
                }

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
    // write_out_dart_data();

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
            double frob_norm(0.0);

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
                                            frob_norm,
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
                                                frob_norm,
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
    
    // Keep all h_matrices
    vcl_vector<double> h_matrices;
    
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

        vil_image_view<double> model_channel1(model_images_chan1_data_
                                              [(*m_iterator).second.first]);
        
        vil_image_view<double> model_channel2(model_images_chan2_data_
                                              [(*m_iterator).second.first]);

        vil_image_view<double> model_channel3(model_images_chan3_data_
                                              [(*m_iterator).second.first]);


        double model_area=model_fragments_area_[(*m_iterator).first]
            .second;
        double model_length=model_fragments_length_[(*m_iterator).first]
            .second;

        vcl_map<double,vcl_pair<unsigned int,unsigned int> >
            model_map;

        for ( q_iterator = query_fragments_.begin() ; 
              q_iterator != query_fragments_.end() ; ++q_iterator)
        {
            H_matrix_.fill(0);

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

            vil_image_view<double> query_channel1(
                model_images_chan1_data_
                [(*m_iterator).second.first]);
            
            vil_image_view<double> query_channel2(
                model_images_chan2_data_
                [(*m_iterator).second.first]);

            vil_image_view<double> query_channel3(
                model_images_chan3_data_
                [(*m_iterator).second.first]);

            if ( query_image_ )
            {

                query_images_grad_data = query_grad_data_;
                query_images_sift_filter = query_sift_filter_;
                query_images_grad_data_red = query_grad_red_data_;
                query_images_grad_data_green = query_grad_green_data_;
                query_images_grad_data_blue = query_grad_blue_data_;
                query_channel1 = query_chan1_data_;
                query_channel2 = query_chan2_data_;
                query_channel3 = query_chan3_data_;
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
                                            (*m_iterator).first,
                                            &model_channel1,
                                            &model_channel2,
                                            &model_channel3);

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
                                            query_images_grad_data_blue,
                                            (*q_iterator).first,
                                            &query_channel1,
                                            &query_channel2,
                                            &query_channel3);
            
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
            double frob_norm(0.0);

            // Match model to query
            match_two_graphs_root_node_orig(model_tree,
                                            query_tree,
                                            norm_shape_cost,
                                            norm_shape_cost_length,
                                            app_diff,
                                            norm_app_cost,
                                            rgb_avg_cost,
                                            frob_norm);



            if ( mirror_)
            {
                //: prepare the trees also
                dbskfg_cgraph_directed_tree_sptr query_mirror_tree = new
                    dbskfg_cgraph_directed_tree(
                        query_sample_ds, 
                        scurve_interpolate_ds_, 
                        scurve_matching_R_,
                        mirror_,
                        area_weight_,
                        query_grad_fliplr_data_,
                        query_images_sift_filter,
                        query_grad_fliplr_red_data_,
                        query_grad_fliplr_green_data_,
                        query_grad_fliplr_blue_data_,
                        (*q_iterator).first,
                        &query_channel1,
                        &query_channel2,
                        &query_channel3);

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
                                                frob_norm,
                                                "",
                                                true,
                                                norm_shape_cost);

                norm_shape_cost = ( norm_shape_cost < norm_shape_mirror_cost)
                    ? norm_shape_cost : norm_shape_mirror_cost;
                norm_shape_cost_length = ( norm_shape_cost_length 
                                           < norm_shape_mirror_cost_length)
                    ? norm_shape_cost_length : norm_shape_mirror_cost_length;
                // app_diff = ( app_diff < app_mirror_diff )
                //     ? app_diff: app_mirror_diff;
                // norm_app_cost = ( norm_app_cost < norm_app_mirror_cost )
                //     ? norm_app_cost: norm_app_mirror_cost;
                if ( rgb_avg_mirror_cost !=  1.0e6 )
                {
                    rgb_avg_cost = rgb_avg_mirror_cost;
                    norm_app_cost = norm_app_mirror_cost;
                    rgb_avg_cost = rgb_avg_mirror_cost;
                    app_diff = app_mirror_diff;
                }

                // rgb_avg_cost = ( rgb_avg_cost < rgb_avg_mirror_cost )
                //     ? rgb_avg_cost: rgb_avg_mirror_cost;

            }

            unsigned int model_id= (*m_iterator).first;
            unsigned int query_id= (*q_iterator).first;
            binary_sim_matrix_[model_id][query_id]=norm_shape_cost;
            binary_sim_length_matrix_[model_id][query_id]=
                norm_shape_cost_length;
            binary_app_sim_matrix_[model_id][query_id]=app_diff;
            binary_app_norm_sim_matrix_[model_id][query_id]=norm_app_cost;
            binary_app_rgb_sim_matrix_[model_id][query_id]=rgb_avg_cost;

            double* data=H_matrix_.data_block();
            
            h_matrices.push_back(data[0]);
            h_matrices.push_back(data[1]);
            h_matrices.push_back(data[2]);
            h_matrices.push_back(data[3]);
            h_matrices.push_back(data[4]);
            h_matrices.push_back(data[5]);
            h_matrices.push_back(data[6]);
            h_matrices.push_back(data[7]);
            h_matrices.push_back(data[8]);
            

            // Determine matrix mat
            vcl_map<unsigned int,vnl_matrix<double> >::iterator fit;
            fit=dist_maps_.lower_bound(model_tree->get_id());
            
            if ( !dist_maps_.count(model_tree->get_id()))
            {
                fit--;
            }

            vcl_map<vcl_pair<int,int>,double>::iterator lit;
            for ( lit=local_dist_map_.begin(); lit != local_dist_map_.end(); 
                  ++lit)
            {
                vcl_pair<int,int> key=(*lit).first;

                double sample_distance = (*lit).second;
                double mat_value       = ((*fit).second)(key.first,key.second);

                if ( mat_value == 0.0 )
                {
                    ((*fit).second).put(key.first,key.second,
                                        sample_distance);

                }
                else
                {
                    
                    if ( sample_distance < mat_value)
                    {
                        ((*fit).second).put(key.first,key.second,
                                            sample_distance);
                    }
                }
            }

            local_dist_map_.clear();

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


    // vcl_ofstream binary_h_file;
    // binary_h_file.open(output_binary_h_file_.c_str(),
    //                    vcl_ios::out | 
    //                    vcl_ios::app | 
    //                    vcl_ios::binary);

    // matrix_size=h_matrices.size();
    // binary_h_file.write(reinterpret_cast<char *>(&matrix_size),
    //                     sizeof(double));

    // for ( unsigned int i=0; i < h_matrices.size() ; ++i)
    // {
    //     double value=h_matrices[i];
    //     binary_h_file.write(reinterpret_cast<char *>(&value),
    //                         sizeof(double));
    // }

    // binary_h_file.close();


    vcl_ofstream parts_file;
    parts_file.open(output_parts_file_.c_str(),
                    vcl_ios::out | 
                    vcl_ios::app ); 
                    

    parts_file<<query_parts_.size()<<vcl_endl;

    vcl_map<unsigned int,vcl_vector<vgl_point_2d<double> > >::iterator it;

    for ( it = query_parts_.begin() ; it != query_parts_.end() ; ++it)
    {
        vcl_vector<vgl_point_2d<double> > pts=(*it).second;

        for ( unsigned int p=0; p < pts.size() ; ++p)
        {
            parts_file<<pts[p].x()<<" "<<pts[p].y()<<vcl_endl;
        }
    }

    parts_file.close();



    vcl_ofstream d2c_file;
    d2c_file.open(output_dist_to_category_file_.c_str(),
                  vcl_ios::out | 
                  vcl_ios::app ); 
    
    vcl_map<unsigned int,vnl_matrix<double> >::iterator mit;
    for ( mit = dist_maps_.begin() ; mit != dist_maps_.end() ; ++mit)
    {
        d2c_file<<(*mit).second.absolute_value_sum()<<vcl_endl;
    }

    d2c_file.close();

    dist_maps_.clear();
    
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


bool dbskfg_match_bag_of_fragments::train_bag_of_words(int keywords)
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    // keep track of sift features in vcl vector
    vcl_vector<vl_sift_pix> descriptors;

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

        vil_image_view<double> model_channel1(model_images_chan1_data_
                                               [(*m_iterator).second.first]);
        
        vil_image_view<double> model_channel2(model_images_chan2_data_
                                               [(*m_iterator).second.first]);

        vil_image_view<double> model_channel3(model_images_chan3_data_
                                               [(*m_iterator).second.first]);
    
        //: prepare the trees also
        dbskfg_cgraph_directed_tree_sptr model_tree = new 
            dbskfg_cgraph_directed_tree(scurve_sample_ds_, 
                                        scurve_interpolate_ds_, 
                                        scurve_matching_R_,
                                        false,
                                        area_weight_,
                                        model_images_grad_data,
                                        model_images_sift_filter,
                                        model_images_grad_data_red,
                                        model_images_grad_data_green,
                                        model_images_grad_data_blue,
                                        (*m_iterator).first,
                                        &model_channel1,
                                        &model_channel2,
                                        &model_channel3);


        bool f1=model_tree->acquire
            ((*m_iterator).second.second, elastic_splice_cost_, 
             circular_ends_, combined_edit_);

        model_tree->compute_sift_tree(descriptors);

    }

    vl_sift_pix* centers(0);
    vl_sift_pix* data=descriptors.data();
    int dimension  = 384;
    int numData    = descriptors.size()/384;
    int numCenters = keywords;

    vcl_cout<<"Clustering "<<numData<<" opp sift descriptors "<<vcl_endl;

    // Let time how long this takes
    // Start timer
    vul_timer t2;

    // Use float data and the L2 distance for clustering
    VlKMeans* kmeans = vl_kmeans_new (VL_TYPE_FLOAT,VlDistanceL2) ;
    
    // Use Lloyd algorithm
    vl_kmeans_set_algorithm (kmeans, VlKMeansElkan) ;

    // Initialize the cluster centers by randomly sampling the data
    vl_kmeans_init_centers_plus_plus(kmeans, data, dimension, 
                                     numData, numCenters) ;
    

    // Run at most 100 iterations of cluster refinement using Lloyd algorithm
    vl_kmeans_set_verbosity	(kmeans,1);
    vl_kmeans_set_max_num_iterations (kmeans, 100) ;
    vl_kmeans_refine_centers (kmeans, data, numData) ;

    // Obtain the cluster centers
    centers = (vl_sift_pix*) vl_kmeans_get_centers(kmeans) ;

    vl_kmeans_delete(kmeans);

    double vox_time2 = t2.real()/1000.0;
    t2.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"Clustering Time: "
            <<vox_time2<<" sec"<<vcl_endl;

    
    // Write out centers
    vcl_ofstream center_stream("bag_of_words_sift.txt");

    center_stream<<numCenters<<vcl_endl;
    center_stream<<dimension<<vcl_endl;

    for ( unsigned int c=0; c < numCenters*dimension ; ++c)
    {
        center_stream<<centers[c]<<vcl_endl;

    }

    center_stream.close();

    // Write out raw data 
    // vcl_ofstream stream("raw_data.txt");
    
    // for ( unsigned int s=0; s < descriptors.size() ; ++s)
    // {
    //     stream<<descriptors[s]<<vcl_endl;
    // }
    
    // stream.close() ;

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"TrainTime: "
            <<vox_time<<" sec"<<vcl_endl;
    return true;


}


bool dbskfg_match_bag_of_fragments::train_gmm(int keywords)
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    vcl_cout<<"Training gmm computing sift descriptors"<<vcl_endl;

    // keep track of sift features in vcl vector
    vcl_vector<double> descriptors;

    // Loop over model and query
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator m_iterator;

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

        vgl_polygon<double> poly=model_fragments_polys_
            [(*m_iterator).first].second;
        
        vgl_box_2d<double> bbox;

        vcl_set<vcl_pair<int,int> > in_bounds;

        // do not include boundary
        vgl_polygon_scan_iterator<double> psi(poly, false);  
        for (psi.reset(); psi.next(); ) 
        {
            int y = psi.scany();
            for (int x = psi.startx(); x <= psi.endx(); ++x) 
            {
                vgl_point_2d<double> query_pt(x,y);
                
                vcl_pair<int,int> ib(x,y);
                in_bounds.insert(ib);

                bbox.add(query_pt);
            }
        }

        int stride=8;
        double fixed_radius=16;
        double fixed_theta=0.0;

        for ( unsigned int y=bbox.min_y(); y <= bbox.max_y(); y=y+stride)
        {
            for ( unsigned int x=bbox.min_x(); x <= bbox.max_x() ; x=x+stride) 
            {
                vcl_pair<int,int> key(x,y);

                if ( !in_bounds.count(key) )
                {
                    continue;
                }

                vgl_point_2d<double> ps1(x,y);

                double scale_1_radius=16;
                double scale_2_radius=12;
                double scale_3_radius=8;
                double scale_4_radius=4;

                vnl_vector<vl_sift_pix> scale_1_descriptor(384,0.0);
                vnl_vector<vl_sift_pix> scale_2_descriptor(384,0.0);
                vnl_vector<vl_sift_pix> scale_3_descriptor(384,0.0);
                vnl_vector<vl_sift_pix> scale_4_descriptor(384,0.0);

                compute_descr(ps1,
                              scale_1_radius,
                              fixed_theta,
                              model_images_grad_data_red,
                              model_images_grad_data_green,
                              model_images_grad_data_blue,
                              model_images_sift_filter,
                              scale_1_descriptor);

                compute_descr(ps1,
                              scale_2_radius,
                              fixed_theta,
                              model_images_grad_data_red,
                              model_images_grad_data_green,
                              model_images_grad_data_blue,
                              model_images_sift_filter,
                              scale_2_descriptor);

                compute_descr(ps1,
                              scale_3_radius,
                              fixed_theta,
                              model_images_grad_data_red,
                              model_images_grad_data_green,
                              model_images_grad_data_blue,
                              model_images_sift_filter,
                              scale_3_descriptor);


                compute_descr(ps1,
                              scale_4_radius,
                              fixed_theta,
                              model_images_grad_data_red,
                              model_images_grad_data_green,
                              model_images_grad_data_blue,
                              model_images_sift_filter,
                              scale_4_descriptor);
                {
                    for ( unsigned int c=0; c < scale_1_descriptor.size() ; ++c)
                    {
                        descriptors.push_back(scale_1_descriptor[c]);
                    }
                }

                {
                    for ( unsigned int c=0; c < scale_2_descriptor.size() ; ++c)
                    {
                        descriptors.push_back(scale_2_descriptor[c]);
                    }
                }

                {
                    for ( unsigned int c=0; c < scale_3_descriptor.size() ; ++c)
                    {
                        descriptors.push_back(scale_3_descriptor[c]);
                    }
                }


                {
                    for ( unsigned int c=0; c < scale_4_descriptor.size() ; ++c)
                    {
                        descriptors.push_back(scale_4_descriptor[c]);
                    }
                }

            }

        }
    }
    
    // For debugging purposes
    // vcl_ofstream out_stream("sift_points.txt");

    // for ( unsigned int ss=0; ss < descriptors.size() ; ++ss)
    // {
    //     out_stream<<descriptors[ss]<<vcl_endl;
    // }

    // out_stream.close();

    double* data=descriptors.data();
    int dimension  = 384;
    int numData    = descriptors.size()/384;
    int numCenters = keywords;

    double * means ;
    double * covariances ;
    double * priors ;
    double * posteriors ;

    vcl_cout<<"GMM "<<numData<<" opp sift descriptors "<<vcl_endl;

    // Let time how long this takes
    // Start timer
    vul_timer t2;

    // create a new instance of a GMM object for double data
    VlGMM* gmm = vl_gmm_new (VL_TYPE_DOUBLE, dimension, numCenters) ;

    // set verbosity
    vl_gmm_set_verbosity (gmm, 1);

    // set the maximum number of EM iterations to 100
    vl_gmm_set_max_num_iterations (gmm, 100) ;

    // set the initialization to random selection
    vl_gmm_set_initialization (gmm,VlGMMKMeans);

    // cluster the data, i.e. learn the GMM
    vl_gmm_cluster (gmm, data, numData);

    // get the means, covariances, and priors of the GMM
    means = (double *)vl_gmm_get_means(gmm);
    covariances = (double *)vl_gmm_get_covariances(gmm);
    priors = (double *)vl_gmm_get_priors(gmm);

    double vox_time2 = t2.real()/1000.0;
    t2.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"Clustering Time: "
            <<vox_time2<<" sec"<<vcl_endl;

    
    // Write out centers
    vcl_ofstream center_stream("gmm_sift.txt");

    center_stream<<numCenters<<vcl_endl;
    center_stream<<dimension<<vcl_endl;

    for ( unsigned int c=0; c < numCenters*dimension ; ++c)
    {
        center_stream<<means[c]<<vcl_endl;

    }

    for ( unsigned int c=0; c < numCenters*dimension ; ++c)
    {
        center_stream<<covariances[c]<<vcl_endl;

    }

    for ( unsigned int c=0; c < numCenters ; ++c)
    {
        center_stream<<priors[c]<<vcl_endl;

    }

    center_stream.close();

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"TrainTime: "
            <<vox_time<<" sec"<<vcl_endl;

    return true;


}

bool dbskfg_match_bag_of_fragments::train_gmm_color(int keywords)
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    vcl_cout<<"Training gmm color descriptors"<<vcl_endl;

    unsigned int hist_size=0;

    // keep track of sift features in vcl vector
    vcl_vector<double> descriptors;

    // Loop over model and query
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator m_iterator;

    for ( m_iterator = model_fragments_.begin() ; 
          m_iterator != model_fragments_.end() ; ++m_iterator)
    {
        VlSiftFilt*model_images_sift_filter=
            model_images_sift_filter_.count((*m_iterator).second.first)?
            model_images_sift_filter_[(*m_iterator).second.first]:
            0;

        vil_image_view<double> model_channel1(model_images_chan1_data_
                                              [(*m_iterator).second.first]);
        
        vil_image_view<double> model_channel2(model_images_chan2_data_
                                              [(*m_iterator).second.first]);

        vil_image_view<double> model_channel3(model_images_chan3_data_
                                              [(*m_iterator).second.first]);

        vgl_polygon<double> poly=model_fragments_polys_
            [(*m_iterator).first].second;
        
        vgl_box_2d<double> bbox;

        vcl_set<vcl_pair<int,int> > in_bounds;

        // do not include boundary
        vgl_polygon_scan_iterator<double> psi(poly, false);  
        for (psi.reset(); psi.next(); ) 
        {
            int y = psi.scany();
            for (int x = psi.startx(); x <= psi.endx(); ++x) 
            {
                vgl_point_2d<double> query_pt(x,y);
                
                vcl_pair<int,int> ib(x,y);
                in_bounds.insert(ib);

                bbox.add(query_pt);
            }
        }

        int stride=8;
        double fixed_radius=2;
        double fixed_theta=0.0;

        for ( unsigned int y=bbox.min_y(); y <= bbox.max_y(); y=y+stride)
        {
            for ( unsigned int x=bbox.min_x(); x <= bbox.max_x() ; x=x+stride) 
            {
                vcl_pair<int,int> key(x,y);

                if ( !in_bounds.count(key) )
                {
                    continue;
                }

                vgl_point_2d<double> model_pt(x,y);

                vcl_set<vcl_pair<double,double> > model_sift_samples;
                        
                compute_color_over_sift(
                    model_images_sift_filter,
                    model_images_sift_filter->width,
                    model_images_sift_filter->height,
                    model_pt.x(),
                    model_pt.y(),
                    fixed_radius,
                    fixed_theta,
                    model_sift_samples);
 
                vcl_vector<double> model_descr;
                
                compute_color_region_hist(
                    model_sift_samples,
                    model_channel1,
                    model_channel2,
                    model_channel3,
                    model_descr,
                    dbskfg_match_bag_of_fragments::DEFAULT);
                
                hist_size = model_descr.size();

                vnl_vector<double> vec_model(model_descr.size(),0);             
                
                for ( unsigned int m=0; m < model_descr.size(); ++m)
                {
                    vec_model.put(m,model_descr[m]);               
                }

                vec_model *= 1/vec_model.sum();


                for ( unsigned int c=0; c < vec_model.size() ; ++c)
                {
                    descriptors.push_back(vec_model[c]);
                }
            }

        }
    }
    
    // For debugging purposes
    // vcl_ofstream out_stream("sift_points.txt");

    // for ( unsigned int ss=0; ss < descriptors.size() ; ++ss)
    // {
    //     out_stream<<descriptors[ss]<<vcl_endl;
    // }

    // out_stream.close();

    double* data=descriptors.data();
    int dimension  = hist_size;
    int numData    = descriptors.size()/hist_size;
    int numCenters = keywords;

    double * means ;
    double * covariances ;
    double * priors ;
    double * posteriors ;

    vcl_cout<<"GMM "<<numData<<" color 3d histograms of dimension "<<
        dimension<<vcl_endl;

    // Let time how long this takes
    // Start timer
    vul_timer t2;

    // create a new instance of a GMM object for double data
    VlGMM* gmm = vl_gmm_new (VL_TYPE_DOUBLE, dimension, numCenters) ;

    // set verbosity
    vl_gmm_set_verbosity (gmm, 1);

    // set the maximum number of EM iterations to 100
    vl_gmm_set_max_num_iterations (gmm, 100) ;

    // set the initialization to random selection
    vl_gmm_set_initialization (gmm,VlGMMKMeans);

    // cluster the data, i.e. learn the GMM
    vl_gmm_cluster (gmm, data, numData);

    // get the means, covariances, and priors of the GMM
    means = (double *)vl_gmm_get_means(gmm);
    covariances = (double *)vl_gmm_get_covariances(gmm);
    priors = (double *)vl_gmm_get_priors(gmm);

    double vox_time2 = t2.real()/1000.0;
    t2.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"Clustering Time: "
            <<vox_time2<<" sec"<<vcl_endl;

    
    // Write out centers
    vcl_ofstream center_stream("gmm_color_3d_hists.txt");

    center_stream<<numCenters<<vcl_endl;
    center_stream<<dimension<<vcl_endl;

    for ( unsigned int c=0; c < numCenters*dimension ; ++c)
    {
        center_stream<<means[c]<<vcl_endl;

    }

    for ( unsigned int c=0; c < numCenters*dimension ; ++c)
    {
        center_stream<<covariances[c]<<vcl_endl;

    }

    for ( unsigned int c=0; c < numCenters ; ++c)
    {
        center_stream<<priors[c]<<vcl_endl;

    }

    center_stream.close();

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"TrainTime: "
            <<vox_time<<" sec"<<vcl_endl;

    return true;


}

bool dbskfg_match_bag_of_fragments::train_gmm_raw_color(int keywords)
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    if ( model_fragments_.size() == 0 || query_fragments_.size() == 0 )
    {
        vcl_cerr<<"Matching fragments sets have one that is zero"<<vcl_endl;
        return false;
    }

    vcl_cout<<"Training gmm raw_color descriptors"<<vcl_endl;

    // keep track of sift features in vcl vector
    vcl_vector<double> descriptors;

    // Loop over model and query
    vcl_map<unsigned int,vcl_pair<vcl_string,dbskfg_composite_graph_sptr> >
        ::iterator m_iterator;

    int stride=4;

    for ( m_iterator = model_fragments_.begin() ; 
          m_iterator != model_fragments_.end() ; ++m_iterator)
    {
        VlSiftFilt*model_images_sift_filter=
            model_images_sift_filter_.count((*m_iterator).second.first)?
            model_images_sift_filter_[(*m_iterator).second.first]:
            0;

        vil_image_view<double> model_channel1(model_images_chan1_data_
                                              [(*m_iterator).second.first]);
        
        vil_image_view<double> model_channel2(model_images_chan2_data_
                                              [(*m_iterator).second.first]);

        vil_image_view<double> model_channel3(model_images_chan3_data_
                                              [(*m_iterator).second.first]);

        vgl_polygon<double> poly=model_fragments_polys_
            [(*m_iterator).first].second;
        
        vgl_box_2d<double> bbox;

        vcl_set<vcl_pair<int,int> > in_bounds;

        // do not include boundary
        vgl_polygon_scan_iterator<double> psi(poly, false);  
        for (psi.reset(); psi.next(); ) 
        {
            int y = psi.scany();
            for (int x = psi.startx(); x <= psi.endx(); ++x) 
            {
                vgl_point_2d<double> query_pt(x,y);
                
                vcl_pair<int,int> ib(x,y);
                in_bounds.insert(ib);

                bbox.add(query_pt);
            }
        }

        for ( unsigned int y=bbox.min_y(); y <= bbox.max_y(); y=y+stride)
        {
            for ( unsigned int x=bbox.min_x(); x <= bbox.max_x() ; x=x+stride) 
            {
                vcl_pair<int,int> key(x,y);

                if ( !in_bounds.count(key) )
                {
                    continue;
                }

                double red   = vil_bilin_interp_safe(model_channel1,x,y);
                double green = vil_bilin_interp_safe(model_channel2,x,y);
                double blue  = vil_bilin_interp_safe(model_channel3,x,y);

                descriptors.push_back(red);
                descriptors.push_back(green);
                descriptors.push_back(blue);
            }
        }
        
    }
    
    
    // For debugging purposes
    // vcl_ofstream out_stream("sift_points.txt");

    // for ( unsigned int ss=0; ss < descriptors.size() ; ++ss)
    // {
    //     out_stream<<descriptors[ss]<<vcl_endl;
    // }

    // out_stream.close();

    double* data=descriptors.data();
    int dimension  = 3;
    int numData    = descriptors.size()/dimension;
    int numCenters = keywords;

    double * means ;
    double * covariances ;
    double * priors ;
    double * posteriors ;

    vcl_cout<<"GMM "<<numData<<" color raw 3d values of dimension "<<
        dimension<<vcl_endl;

    // Let time how long this takes
    // Start timer
    vul_timer t2;

    // create a new instance of a GMM object for double data
    VlGMM* gmm = vl_gmm_new (VL_TYPE_DOUBLE, dimension, numCenters) ;

    // set verbosity
    vl_gmm_set_verbosity (gmm, 1);

    // set the maximum number of EM iterations to 100
    vl_gmm_set_max_num_iterations (gmm, 100) ;

    // set the initialization to random selection
    vl_gmm_set_initialization (gmm,VlGMMKMeans);

    // cluster the data, i.e. learn the GMM
    vl_gmm_cluster (gmm, data, numData);

    // get the means, covariances, and priors of the GMM
    means = (double *)vl_gmm_get_means(gmm);
    covariances = (double *)vl_gmm_get_covariances(gmm);
    priors = (double *)vl_gmm_get_priors(gmm);

    double vox_time2 = t2.real()/1000.0;
    t2.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"Clustering Time: "
            <<vox_time2<<" sec"<<vcl_endl;

    
    // Write out centers
    vcl_ofstream center_stream("gmm_color_raw_3d_values.txt");

    center_stream<<numCenters<<vcl_endl;
    center_stream<<dimension<<vcl_endl;

    for ( unsigned int c=0; c < numCenters*dimension ; ++c)
    {
        center_stream<<means[c]<<vcl_endl;

    }

    for ( unsigned int c=0; c < numCenters*dimension ; ++c)
    {
        center_stream<<covariances[c]<<vcl_endl;

    }

    for ( unsigned int c=0; c < numCenters ; ++c)
    {
        center_stream<<priors[c]<<vcl_endl;

    }

    center_stream.close();

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"TrainTime: "
            <<vox_time<<" sec"<<vcl_endl;

    return true;


}


void dbskfg_match_bag_of_fragments::set_gmm_train(vcl_string& file_path)
{

    vcl_cout<<"Loading bow training file: "<<file_path<<vcl_endl;

    int dimension  = 384;
    int numCenters = 0;

    vcl_ifstream myfile (file_path.c_str());
    if (myfile.is_open())
    {
        myfile>>numCenters;
        myfile>>dimension;
        
        keywords_=numCenters;

        vcl_cout<<"Num Centers: "<<keywords_<<vcl_endl;
        vcl_cout<<"Dimension:   "<<dimension<<vcl_endl;

        means_cg_ = (float*) vl_malloc(
            sizeof(float)*dimension*numCenters);

        covariances_cg_ = (float*) vl_malloc(
            sizeof(float)*dimension*numCenters);

        priors_cg_ = (float*) vl_malloc(
            sizeof(float)*numCenters);
        
        for ( unsigned int c=0; c < dimension*numCenters ; ++c)
        {
            myfile>>means_cg_[c];

        }

        for ( unsigned int c=0; c < dimension*numCenters ; ++c)
        {
            myfile>>covariances_cg_[c];

        }

        for ( unsigned int c=0; c < numCenters ; ++c)
        {
            myfile>>priors_cg_[c];

        }


    }

    myfile.close();



}

void dbskfg_match_bag_of_fragments::set_gmm_color_train(vcl_string& file_path)
{

    vcl_cout<<"Loading bow training file: "<<file_path<<vcl_endl;

    int dimension  = 384;
    int numCenters = 0;

    vcl_ifstream myfile (file_path.c_str());
    if (myfile.is_open())
    {
        myfile>>numCenters;
        myfile>>dimension;
        
        keywords_=numCenters;

        vcl_cout<<"Num Centers: "<<keywords_<<vcl_endl;
        vcl_cout<<"Dimension:   "<<dimension<<vcl_endl;

        means_color_ = (float*) vl_malloc(
            sizeof(float)*dimension*numCenters);

        covariances_color_ = (float*) vl_malloc(
            sizeof(float)*dimension*numCenters);

        priors_color_ = (float*) vl_malloc(
            sizeof(float)*numCenters);
        
        for ( unsigned int c=0; c < dimension*numCenters ; ++c)
        {
            myfile>>means_color_[c];

        }

        for ( unsigned int c=0; c < dimension*numCenters ; ++c)
        {
            myfile>>covariances_color_[c];

        }

        for ( unsigned int c=0; c < numCenters ; ++c)
        {
            myfile>>priors_color_[c];

        }


    }

    myfile.close();



}

void dbskfg_match_bag_of_fragments::set_bow_train(vcl_string& file_path)
{

    vcl_cout<<"Loading bow training file: "<<file_path<<vcl_endl;

    int dimension  = 384;
    int numCenters = 0;

    vl_sift_pix* data(0);


    vcl_ifstream myfile (file_path.c_str());
    if (myfile.is_open())
    {
        myfile>>numCenters;
        myfile>>dimension;
        
        keywords_=numCenters;

        vcl_cout<<"Num Centers: "<<keywords_<<vcl_endl;
        vcl_cout<<"Dimension:   "<<dimension<<vcl_endl;

        data = (vl_sift_pix*) vl_malloc(
            sizeof(vl_sift_pix)*dimension*numCenters);
        
        for ( unsigned int c=0; c < dimension*numCenters ; ++c)
        {
            myfile>>data[c];

        }
    }

    myfile.close();


    /* KDTree, L2 comparison metric, dimension 128, 1 tree, L1 metric */
    forest_ = vl_kdforest_new(VL_TYPE_FLOAT, 384, 4, VlDistanceL2);

    // Build tree
    vl_kdforest_build(forest_,
                      numCenters,
                      data);

    // Construct searcher
    searcher_  = vl_kdforest_new_searcher(forest_);

}


void dbskfg_match_bag_of_fragments::set_part_file(vcl_string& file_path)
{

    vcl_cout<<"Loading Parts file: "<<file_path<<vcl_endl;

    vcl_ifstream myfile(file_path.c_str());

    unsigned int index=0;

    bool flag=false;
    if (myfile.is_open())
    {
        while(1)
        {
            vcl_vector<vgl_point_2d<double> > parts;
            for ( unsigned int s=0; s < 15; ++s)
            {
                vcl_string line;
                
                if ( !vcl_getline(myfile,line) )
                {
                    flag=true;
                    break;
                }
                
                
                vcl_stringstream stream(line);
                
                unsigned int id(0),part_id(0);
                
                double x_location(0.0),y_location(0.0);
                
                unsigned int visible(0);
                
                stream>>id
                      >>part_id
                      >>x_location
                      >>y_location
                      >>visible;
                
                vgl_point_2d<double> part_location(x_location-1,y_location-1);
                
                model_parts_[index].push_back(part_location);
                
            }

            if ( flag )
            {
                break;
            }
            index++;
        }
        
        
    }

    myfile.close();


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
            double frob_norm(0.0);

            // Match model to query
            match_two_graphs_root_node_orig(model_tree,
                                            query_tree,
                                            norm_shape_cost,
                                            norm_shape_cost_length,
                                            app_diff,
                                            norm_app_cost,
                                            rgb_avg_cost,
                                            frob_norm);

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
                                                rgb_avg_mirror_cost,
                                                frob_norm);

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
            double frob_norm(0.0);

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
                                            frob_norm,
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
                                                frob_norm,
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
    //      compute_transformed_polygon(H,model_tree,query_tree);

}

void dbskfg_match_bag_of_fragments::match_two_graphs_root_node_orig(
    dbskfg_cgraph_directed_tree_sptr& model_tree, 
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    double& norm_shape_cost,
    double& norm_shape_cost_length,
    double& app_diff,
    double& norm_app_cost,
    double& rgb_avg_cost,
    double& frob_norm,
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

    vcl_stringstream title;
    title<<"Model_"<<model_tree->get_id()<<"_vs_Query_"<<query_tree->get_id();
    
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
    
    double norm=0.0;
    vnl_matrix_fixed<double, 3,3 > M;
    if ( curve_list1.size() && curve_list2.size())
    {
        double width=0.0;

        if ( mirror )
        {
            width=query_tree->bbox()->width();
        }

        unsigned int ds=scurve_sample_ds_;
        vgl_h_matrix_2d<double> H;
        compute_similarity(H,
                           curve_list1,
                           curve_list2,
                           map_list,
                           path_map,
                           ds,
                           flag);
                           // model_tree->get_scale_ratio(),
                           // query_tree->get_scale_ratio(),
                           // width);
        
        M=H.get_matrix();
        norm=M.frobenius_norm();
    }
    else
    {
        norm =1.0e6;
    }

    bool flag_mirror=true;
    if ( mirror )
    {
        if ( shape_cost_splice > orig_edit_distance)
  	{
            app_diff = 1.0e6;
            norm_app_cost = 1.0e6;
            rgb_avg_cost = 1.0e6;
 	 
            flag_mirror=false;
  	}
        else
        {
            if ( !(vcl_fabs(orig_edit_distance-shape_cost_splice) >= 0.05))
            {
                if ( norm > frob_norm)
                {
                    app_diff        = 1.0e6;
                    norm_app_cost   = 1.0e6;
                    rgb_avg_cost    = 1.0e6;

                    flag_mirror=false;
                }
                else
                {
                    H_matrix_=M;
                }
            }
            else
            {
                H_matrix_=M;
            }
        }
    }
    else
    {
        H_matrix_=M;
    }

    frob_norm=norm;

    // Doing find part correpondences
    // if ( flag_mirror )
    // {

    //     double width=0.0;
    //     if ( mirror )
    //     {
    //         vcl_cout<<"Performing mirror find part correpondences"<<vcl_endl;
    //         width=query_tree->bbox()->width();
    //     }

    //     // compute_app_alignment(
    //     //         curve_list1,
    //     //         curve_list2,
    //     //         map_list,
    //     //         *model_tree->get_channel1(),
    //     //         *query_tree->get_channel1(),
    //     //         flag,
    //     //         width,
    //     //         model_tree->get_grad_data(),
    //     //         model_tree->get_sift_filter(),
    //     //         query_tree->get_grad_data(),
    //     //         query_tree->get_sift_filter(),
    //     //         model_tree->get_scale_ratio(),
    //     //         query_tree->get_scale_ratio());

    //     unsigned int model_id = model_tree->get_id();
        
    //     if ( query_parts_.count(model_id) )
    //     {
    //         query_parts_[model_id].clear();
    //     }
        
    //     // warp_image(
    //     //     model_tree,
    //     //     query_tree,
    //     //     curve_list1,
    //     //     curve_list2,
    //     //     map_list,
    //     //     flag,
    //     //     width);
        
    //     vcl_vector<vgl_point_2d<double> > model_parts = model_parts_[model_id];

    //     for ( unsigned int id=0; id < model_parts.size() ; ++id)
    //     {
    //         vgl_point_2d<double> query_pt(model_parts[id]);
    //         vgl_point_2d<double> test(-1.0,-1.0);
    //         vgl_point_2d<double> mapping_pt(-1.0,-1.0);

    //         if ( query_pt != test )
    //         {
            
    //             mapping_pt=
    //                 find_part_correspondences(query_pt,
    //                                           curve_list1,
    //                                           curve_list2,
    //                                           map_list,
    //                                           flag,
    //                                           width,
    //                                           model_tree
    //                                           ->get_scale_ratio(),
    //                                           query_tree
    //                                           ->get_scale_ratio());
    //         }

    //         query_parts_[model_id].push_back(mapping_pt);
            
    //     }
    // }


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

        vil_image_view<double>* model_channel1=model_tree->get_channel1();
        vil_image_view<double>* query_channel1=query_tree->get_channel1();

        vil_image_view<double>* model_channel2=model_tree->get_channel2();
        vil_image_view<double>* query_channel2=query_tree->get_channel2();

        vil_image_view<double>* model_channel3=model_tree->get_channel3();
        vil_image_view<double>* query_channel3=query_tree->get_channel3();

        VlSiftFilt* model_sift_filter=model_tree->get_sift_filter();
        VlSiftFilt* query_sift_filter=query_tree->get_sift_filter();


        // vcl_stringstream title;
        // title<<"Model_"<<model_tree->get_id()<<"_vs_"
        //      <<"Query_"<<query_tree->get_id();

        // vcl_string pref=title.str();

        // draw_part_correspondence(
        //     curve_list1,
        //     curve_list2,
        //     map_list,
        //     flag,
        //     width,
        //     model_tree->get_scale_ratio(),
        //     query_tree->get_scale_ratio(),
        //     pref);



        vul_timer app_timer;
        app_timer.mark();
        // vcl_pair<double,double> app_cost=compute_sift_cost(curve_list1,
        //                                                    curve_list2,
        //                                                    map_list,
        //                                                    path_map,
        //                                                    flag,
        //                                                    width,
        //                                                    model_grad_data,
        //                                                    model_sift_filter,
        //                                                    query_grad_data,
        //                                                    query_sift_filter,
        //                                                    model_tree
        //                                                    ->get_scale_ratio(),
        //                                                    query_tree
        //                                                    ->get_scale_ratio());

        vcl_vector<double> dart_distances;
        vcl_pair<double,double> app_cost(0.0,0.0);
        // vcl_pair<double,double> sift_rgb_cost=compute_dense_rgb_sift_cost(
        //     curve_list1,
        //     curve_list2,
        //     map_list,
        //     path_map,
        //     dart_distances,
        //     *model_channel1,
        //     *model_channel2,
        //     *model_channel3,
        //     *query_channel1,
        //     *query_channel2,
        //     *query_channel3,
        //     flag,
        //     width,
        //     model_red_grad_data,
        //     query_red_grad_data,
        //     model_green_grad_data,
        //     query_green_grad_data,
        //     model_blue_grad_data,
        //     query_blue_grad_data,
        //     model_sift_filter,
        //     query_sift_filter,
        //     model_tree->get_scale_ratio(),
        //     query_tree->get_scale_ratio(),
        //     model_tree->get_root_node_radius(),
        //     query_tree->get_root_node_radius(),
        //     title.str());


        // vcl_pair<double,double> sift_rgb_cost =
        //     compute_common_frame_distance_qm(
        //         model_tree,
        //         query_tree,
        //         curve_list1,
        //         curve_list2,
        //         map_list,
        //         flag,
        //         width);

        double extra_cost(0.0);
        vcl_pair<double,double> sift_rgb_cost =
            compute_common_frame_distance_bbox_qm(
                model_tree,
                query_tree,
                curve_list1,
                curve_list2,
                map_list,
                extra_cost,
                flag,
                width);

        // vcl_pair<double,double> sift_rgb_cost =
        //     compute_implicit_distance_bbox_qm(
        //         model_tree,
        //         query_tree,
        //         curve_list1,
        //         curve_list2,
        //         map_list,
        //         flag,
        //         width);

        // vcl_pair<double,double> sift_rgb_cost =
        //     compute_common_frame_distance_bbox_mq(
        //         model_tree,
        //         query_tree,
        //         curve_list1,
        //         curve_list2,
        //         map_list,
        //         flag,
        //         width);

        // vcl_pair<double,double> sift_rgb_cost =
        //     compute_common_frame_distance_part_qm(
        //         model_tree,
        //         query_tree,
        //         curve_list1,
        //         curve_list2,
        //         map_list,
        //         flag,
        //         width);

        // vcl_pair<double,double> sift_rgb_cost =
        //     compute_common_frame_distance_dsift_qm(
        //         model_tree,
        //         query_tree,
        //         curve_list1,
        //         curve_list2,
        //         map_list,
        //         flag,
        //         width);

        // vcl_pair<double,double> sift_rgb_cost=compute_mi(
        //     curve_list1,
        //     curve_list2,
        //     map_list,
        //     path_map,
        //     dart_distances,
        //     *model_channel1,
        //     *model_channel2,
        //     *model_channel3,
        //     *query_channel1,
        //     *query_channel2,
        //     *query_channel3,
        //     flag,
        //     width,
        //     model_red_grad_data,
        //     query_red_grad_data,
        //     model_green_grad_data,
        //     query_green_grad_data,
        //     model_blue_grad_data,
        //     query_blue_grad_data,
        //     model_sift_filter,
        //     query_sift_filter,
        //     model_tree->get_scale_ratio(),
        //     query_tree->get_scale_ratio());

        // vcl_pair<double,double> sift_rgb_cost=compute_bow(
        //     curve_list1,
        //     curve_list2,
        //     map_list,
        //     path_map,
        //     dart_distances,
        //     *model_channel1,
        //     *model_channel2,
        //     *model_channel3,
        //     *query_channel1,
        //     *query_channel2,
        //     *query_channel3,
        //     flag,
        //     width,
        //     model_red_grad_data,
        //     query_red_grad_data,
        //     model_green_grad_data,
        //     query_green_grad_data,
        //     model_blue_grad_data,
        //     query_blue_grad_data,
        //     model_sift_filter,
        //     query_sift_filter,
        //     model_tree->get_scale_ratio(),
        //     query_tree->get_scale_ratio());
        
        // vcl_pair<double,double> app_cost=compute_body_centric_sift(
        //     curve_list1,
        //     curve_list2,
        //     map_list,
        //     path_map,
        //     dart_distances,
        //     model_red_grad_data,
        //     query_red_grad_data,
        //     model_green_grad_data,
        //     query_green_grad_data,
        //     model_blue_grad_data,
        //     query_blue_grad_data,
        //     model_sift_filter,
        //     query_sift_filter,
        //     model_tree->get_scale_ratio(),
        //     query_tree->get_scale_ratio(),
        //     flag,
        //     width);

        // vcl_pair<double,double> sift_rgb_cost=compute_3d_hist_color(
        //     curve_list1,
        //     curve_list2,
        //     map_list,
        //     path_map,
        //     dart_distances,
        //     *model_channel1,
        //     *model_channel2,
        //     *model_channel3,
        //     *query_channel1,
        //     *query_channel2,
        //     *query_channel3,
        //     model_tree->get_scale_ratio(),
        //     query_tree->get_scale_ratio(),
        //     flag,
        //     width);
         
        // unsigned int model_tag=model_tree->get_id();
        
        // if ( model_dart_distances_.count(model_tag))
        // {
        //     model_dart_distances_.erase(model_tag);
        //     model_dart_curves_.erase(model_tag);
        // }

        // if ( !flag )
        // {
        //     // Get matching pairs
        //     for (unsigned m = 0; m < map_list.size(); m++) 
        //     {
        //         dbskr_scurve_sptr mc = curve_list1[m];
        //         vcl_pair<unsigned int,unsigned int> query_key(
        //             curve_list2[m]->get_curve_id().first,
        //             curve_list2[m]->get_curve_id().second);
        //         if ( !query_dart_curves_.count(query_key))
        //         {
        //             vcl_pair<unsigned int,unsigned int> temp
        //                 = query_key;
        //             query_key.first=temp.second;
        //             query_key.second=temp.first;                        
        //         }
                
        //         double cost=dart_distances[m];
                
        //         vcl_pair<vcl_pair<int,int>,double> key1;
        //         vcl_pair<vcl_pair<int,int>,dbskr_scurve_sptr> key2;
                    
        //         key1.first=query_key;
        //         key1.second=cost;
                
        //         key2.first=query_key;
        //         key2.second=mc;
                
        //         model_dart_distances_[model_tag].push_back(key1);
        //         model_dart_curves_[model_tag].push_back(key2);
        //     }
                
            
        // }
        // else
        // {
        //     // Get matching pairs
        //     for (unsigned m = 0; m < map_list.size(); m++) 
        //     {
        //         dbskr_scurve_sptr mc = curve_list2[m];
        //         vcl_pair<unsigned int,unsigned int> query_key(
        //             curve_list1[m]->get_curve_id().first,
        //             curve_list1[m]->get_curve_id().second);
        //         if ( !query_dart_curves_.count(query_key))
        //         {
        //             vcl_pair<unsigned int,unsigned int> temp
        //                 = query_key;
        //             query_key.first=temp.second;
        //             query_key.second=temp.first;
                    
        //         }
        //         double cost=dart_distances[m];
                
        //         vcl_pair<vcl_pair<int,int>,double> key1;
        //         vcl_pair<vcl_pair<int,int>,dbskr_scurve_sptr> key2;
                
        //         key1.first=query_key;
        //         key1.second=cost;
                
        //         key2.first=query_key;
        //         key2.second=mc;
                
        //         model_dart_distances_[model_tag].push_back(key1);
        //         model_dart_curves_[model_tag].push_back(key2);
                
        //     }
        // }
    
            
        double app_time = app_timer.real()/1000.0;
        app_timer.mark();
        
        //vcl_cerr<<"************ App   Time taken: "<<app_time<<" sec"<<vcl_endl;
        app_diff        = extra_cost;
        norm_app_cost   = sift_rgb_cost.first;
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
    VlSiftFilt** filter,
    vgl_polygon<double>& poly,
    bool mask_grad,
    bool fliplr)
{
    vil_image_view<vxl_byte> temp = 
        vil_convert_to_grey_using_rgb_weighting(
            0.2989,
            0.5870,
            0.1140,
            input_image->get_view());
    
    vil_image_view<double> orig_image;
    vil_convert_cast(temp,orig_image);

    vil_image_view<double> flipped_image(orig_image.ni(),
                                         orig_image.nj());

    if ( fliplr )
    {
        for ( unsigned int cols=0; cols < flipped_image.nj() ; ++cols)
        {
            for ( unsigned int rows=0; rows < flipped_image.ni() ; ++rows)
            {
                flipped_image(rows,cols)=orig_image(flipped_image.ni()-1-rows,
                                                    cols);

            }
        }
    }
    else
    {
        flipped_image=orig_image;
    }

    unsigned int width  = flipped_image.ni();
    unsigned int height = flipped_image.nj();

    double* gradient_magnitude = (double*) 
        vl_malloc(width*height*sizeof(double));
    double* gradient_angle     = (double*) 
        vl_malloc(width*height*sizeof(double));

    double* flipped_image_data=flipped_image.top_left_ptr();

    vl_imgradient_polar_d(
        gradient_magnitude, // gradient magnitude 
        gradient_angle,     // gradient angle
        1,                  // output width
        width,              // output height
        flipped_image_data, // input image
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

        div_t divresult = div(i,width);
        
        int xcoord=divresult.rem;
        int ycoord=divresult.quot;
        
        if ( mask_grad )
        {
            if ( !poly.contains(xcoord,ycoord))
            {
                mag=0;
                angle=0;
            }
        }

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
    vil_image_view<double>& orig_image,
    vl_sift_pix** grad_data,
    vgl_polygon<double>& poly,
    bool mask_grad,
    bool fliplr)
{

    vil_image_view<double> flipped_image(orig_image.ni(),
                                         orig_image.nj());
    if ( mask_grad )
    {
        flipped_image.fill(0.0);

        vcl_set<vcl_pair<int,int> > in_bounds;

        // do not include boundary
        vgl_polygon_scan_iterator<double> psi(poly, false);  
        for (psi.reset(); psi.next(); ) 
        {
            int y = psi.scany();
            for (int x = psi.startx(); x <= psi.endx(); ++x) 
            {
                if ( fliplr )
                {
                    flipped_image(flipped_image.ni()-1-x,y)=
                        orig_image(
                            x,
                            y);
                }
                else
                {
                    flipped_image(x,y)=orig_image(x,y);
                }

                in_bounds.insert(vcl_make_pair(x,y));
                
            }
        }


        for ( int cols=0; cols < orig_image.nj() ; ++cols)
        {
            for ( int rows=0; rows < orig_image.ni() ; ++rows)
            {
                vcl_pair<int,int> key(rows,cols);

                if ( !in_bounds.count(key) )
                {
                    orig_image(rows,cols)=0;
                }
            }
        }

    }
    else
    {
        if ( fliplr )
        {
            for ( unsigned int cols=0; cols < flipped_image.nj() ; ++cols)
            {
                for ( unsigned int rows=0; rows < flipped_image.ni() ; ++rows)
                {
                    flipped_image(rows,cols)=orig_image(
                        flipped_image.ni()-1-rows,
                        cols);

                }
            }
        }
        else
        {
            flipped_image=orig_image;
        }
    }

    




    unsigned int width  = flipped_image.ni();
    unsigned int height = flipped_image.nj();


    vil_image_view<vl_sift_pix> grad_mag;
    vil_image_view<vl_sift_pix> grad_angle;
    
    vil_orientations_from_sobel(flipped_image,grad_angle,grad_mag);

    *grad_data=(vl_sift_pix*) vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    vl_sift_pix* gradient_magnitude=grad_mag.top_left_ptr();
    vl_sift_pix* gradient_angle=grad_angle.top_left_ptr();

    unsigned int index=0;
    for ( unsigned int row=0; row < grad_mag.nj() ; ++row)
    {
        for ( unsigned int col=0; col < grad_mag.ni() ; ++col)
        {
            vl_sift_pix mag  = grad_mag(col,row);
            vl_sift_pix angle= angle0To2Pi(grad_angle(col,row));
       
            (*grad_data)[index]=mag;
            ++index;
            (*grad_data)[index]=angle;
            ++index;
            
        }
    }

    // vil_image_view<double> flipped_image(orig_image.ni(),
    //                                      orig_image.nj());
    // if ( fliplr )
    // {
    //     for ( unsigned int cols=0; cols < flipped_image.nj() ; ++cols)
    //     {
    //         for ( unsigned int rows=0; rows < flipped_image.ni() ; ++rows)
    //         {
    //             flipped_image(rows,cols)=orig_image(flipped_image.ni()-1-rows,
    //                                                 cols);

    //         }
    //     }
    // }
    // else
    // {
    //     flipped_image=orig_image;
    // }

    // unsigned int width  = flipped_image.ni();
    // unsigned int height = flipped_image.nj();

    // double* gradient_magnitude = (double*) 
    //     vl_malloc(width*height*sizeof(double));
    // double* gradient_angle     = (double*) 
    //     vl_malloc(width*height*sizeof(double));

    // double* flipped_image_data=flipped_image.top_left_ptr();

    // vl_imgradient_polar_d(
    //     gradient_magnitude, // gradient magnitude 
    //     gradient_angle,     // gradient angle
    //     1,                  // output width
    //     width,              // output height
    //     flipped_image_data, // input image
    //     width,              // input image width
    //     height,             // input image height
    //     width);             // input image stride

    // *grad_data=(vl_sift_pix*) vl_malloc(sizeof(vl_sift_pix)*width*height*2);
    
    // unsigned int index=0;
    // for ( unsigned int i=0; i < width*height; ++i)
    // {
    //     double mag  = gradient_magnitude[i];
    //     double angle= gradient_angle[i];

    //     div_t divresult = div(i,width);
        
    //     int xcoord=divresult.rem;
    //     int ycoord=divresult.quot;
        
    //     if ( mask_grad )
    //     {
    //         if ( !poly.contains(xcoord,ycoord))
    //         {
    //             mag=0;
    //             angle=0;
    //         }
    //     }

    //     (*grad_data)[index]=mag;
    //     ++index;
    //     (*grad_data)[index]=angle;
    //     ++index;
    // }

    // vl_free(gradient_magnitude);
    // vl_free(gradient_angle);

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
    double query_scale_ratio,
    vcl_string prefix)
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
        
            // vl_sift_pix result_red[1];
            // vl_sift_pix result_green[1];
            // vl_sift_pix result_blue[1];
            vl_sift_pix result_final[1];

            vnl_vector<vl_sift_pix> descr1(384,0.0);
            vnl_vector<vl_sift_pix> descr2(384,0.0);

            for ( unsigned int d=0; d < 128 ; ++d)
            {
                descr1.put(d,descr_ps1_red[d]);
                descr1.put(d+128,descr_ps1_green[d]);
                descr1.put(d+256,descr_ps1_blue[d]);

                descr2.put(d,descr_ps2_red[d]);
                descr2.put(d+128,descr_ps2_green[d]);
                descr2.put(d+256,descr_ps2_blue[d]);

        
            }

            descr1.normalize();
            descr2.normalize();

            vl_eval_vector_comparison_on_all_pairs_f(result_final,
                                                     384,
                                                     descr1.data_block(),
                                                     1,
                                                     descr2.data_block(),
                                                     1,
                                                     Chi2_distance);

            // vl_eval_vector_comparison_on_all_pairs_f(result_red,
            //                                          128,
            //                                          descr_ps1_red,
            //                                          1,
            //                                          descr_ps2_red,
            //                                          1,
            //                                          Chi2_distance);

            // vl_eval_vector_comparison_on_all_pairs_f(result_green,
            //                                          128,
            //                                          descr_ps1_green,
            //                                          1,
            //                                          descr_ps2_green,
            //                                          1,
            //                                          Chi2_distance);

            // vl_eval_vector_comparison_on_all_pairs_f(result_blue,
            //                                          128,
            //                                          descr_ps1_blue,
            //                                          1,
            //                                          descr_ps2_blue,
            //                                          1,
            //                                          Chi2_distance);


            sift_diff=sift_diff+0.5*result_final[0];
            local_distance=local_distance+
                0.5*result_final[0];

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
        //     model_stream<<prefix<<"_dart_model_"<<i<<"_app_correspondence.txt";
        //     vcl_stringstream query_stream;
        //     query_stream<<prefix<<"_dart_query_"<<i<<"_app_correspondence.txt";

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

        //     vcl_stringstream model_poly_stream;
        //     model_poly_stream<<prefix<<"_dart_model_poly_"
        //                      <<i<<"_app_correspondence.txt";
        //     vcl_stringstream query_poly_stream;
        //     query_poly_stream<<prefix<<"_dart_query_poly_"
        //                      <<i<<"_app_correspondence.txt";
            
        //     if ( !flag )
        //     {
        //         sc1->write_polygon(model_poly_stream.str());
        //         sc2->write_polygon(query_poly_stream.str(),width);
        //     }
        //     else
        //     {
        //         sc1->write_polygon(query_poly_stream.str(),width);
        //         sc2->write_polygon(model_poly_stream.str());
        //     }

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

    vcl_pair<double,double> app_diff(length_norm,sift_diff/overall_index);
    return app_diff;
}

vcl_pair<double,double> 
dbskfg_match_bag_of_fragments::compute_app_alignment_cost(
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
    double query_scale_ratio,
    double model_sift_scale,
    double query_sift_scale,
    vcl_string prefix)
{
    

    double total_alignment = 0.0;

    // Get matching pairs
    for (unsigned i = 0; i < map_list.size(); i++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];

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

        vgl_polygon<double> model_polygon(1);
        vgl_polygon<double> query_polygon(1);

        if ( !flag )
        {
            sc1->get_polygon(model_polygon);
            sc2->get_polygon(query_polygon,width);
        }
        else
        {
            sc1->get_polygon(query_polygon,width);
            sc2->get_polygon(model_polygon);
        }

        bool add_curve=true;

        if ( query_dart_curves_.count(query_key1) ||
             query_dart_curves_.count(query_key2) )
        {
            add_curve=false;
        }

        vnl_matrix<vl_sift_pix> model_matrix;
        vnl_matrix<vl_sift_pix> query_matrix;

        unsigned int sc1_points=sc1->num_points();

        unsigned int sc2_points=sc2->num_points();

        if ( !flag )
        {
            model_matrix.set_size(384,sc1_points);
            model_matrix.fill(0.0);

            query_matrix.set_size(384,sc2_points);
            query_matrix.fill(0.0);

            compute_sift_along_curve(sc1,
                                     model_matrix,
                                     model_red_grad_data,
                                     model_green_grad_data,
                                     model_blue_grad_data,
                                     model_sift_filter,
                                     model_sift_scale,
                                     model_scale_ratio);

            compute_sift_along_curve(sc2,
                                     query_matrix,
                                     query_red_grad_data,
                                     query_green_grad_data,
                                     query_blue_grad_data,
                                     query_sift_filter,
                                     query_sift_scale,
                                     query_scale_ratio,
                                     width);


            if ( add_curve )
            {
                for (unsigned int s = 0; s < query_polygon.num_sheets(); ++s)
                {
                    for (unsigned int p = 0; p < query_polygon[s].size(); ++p)
                    {
                        query_dart_curves_[query_key1].push_back(
                            query_polygon[s][p]);
                    }
                }
                
                query_dart_curves_[query_key1].push_back(
                    query_polygon[0][0]);
             
            }
        }
        else
        {

            model_matrix.set_size(384,sc2_points);
            model_matrix.fill(0.0);

            query_matrix.set_size(384,sc1_points);
            query_matrix.fill(0.0);

            compute_sift_along_curve(sc2,
                                     model_matrix,
                                     model_red_grad_data,
                                     model_green_grad_data,
                                     model_blue_grad_data,
                                     model_sift_filter,
                                     model_sift_scale,
                                     model_scale_ratio);

            compute_sift_along_curve(sc1,
                                     query_matrix,
                                     query_red_grad_data,
                                     query_green_grad_data,
                                     query_blue_grad_data,
                                     query_sift_filter,
                                     query_sift_scale,
                                     query_scale_ratio,
                                     width);


            if ( add_curve )
            {

                for (unsigned int s = 0; s < query_polygon.num_sheets(); ++s)
                {
                    for (unsigned int p = 0; p < query_polygon[s].size(); ++p)
                    {
                        query_dart_curves_[query_key1].push_back(
                            query_polygon[s][p]);
                    }
                }
                
                query_dart_curves_[query_key1].push_back(
                    query_polygon[0][0]);
             
            }


        }

        //model_matrix.normalize_columns();
        //query_matrix.normalize_columns();

        double dart_cost1(0.0),dart_cost2(0.0);
        {
            dbskfg_app_curve_match dpMatch(model_matrix,query_matrix);
            dpMatch.Match();
            dart_cost1=dpMatch.finalCost();
        }

        {
            dbskfg_app_curve_match dpMatch(query_matrix,model_matrix);
            dpMatch.Match();
            dart_cost2=dpMatch.finalCost();
        }


        total_alignment+=vcl_min(dart_cost1,dart_cost2);
        
        dart_distances.push_back(vcl_min(dart_cost1,dart_cost2));


        //************ Debug Section ****************************//
        // vcl_cout<<"Dart: "<<i<<" cost: "<<dart_cost<<vcl_endl;
        
        // vcl_stringstream text_stream;
        // text_stream<<"Dart_"<<i<<"_app_correspondence.txt";
        // vcl_stringstream text2_stream;
        // text2_stream<<"Sift_sc1_"<<i<<"_app_correspondence.txt";
        // vcl_stringstream text3_stream;
        // text3_stream<<"Sift_sc2_"<<i<<"_app_correspondence.txt";

        // vcl_ofstream sift_sc1(text2_stream.str().c_str());
        // vcl_ofstream sift_sc2(text3_stream.str().c_str());        
        // vcl_ofstream file_stream(text_stream.str().c_str());

        // vcl_vector<vcl_pair<int,int> > fmap=*(dpMatch.finalMap());        
        // vcl_vector<vcl_pair<int,int> >::iterator it;
        // for ( it = fmap.begin() ; it != fmap.end() ; ++it)
        // {
        //     vgl_point_2d<double> p1,p2;

        //     double theta1(0.0),theta2(0.0);
        //     double scale1(0.0),scale2(0.0);

        //     vnl_vector<vl_sift_pix> model_sift=model_matrix.get_column(
        //         (*it).first);
        //     vnl_vector<vl_sift_pix> query_sift=query_matrix.get_column(
        //         (*it).second);

        //     if ( !flag )
        //     {
        //         p1=sc1->sh_pt((*it).first);
        //         p2=sc2->sh_pt((*it).second);
                

        //         p1.set(p1.x()/model_scale_ratio,
        //                p1.y()/model_scale_ratio);
                
        //         p2.set(vcl_fabs(width-(p2.x()/query_scale_ratio)),
        //                p2.y()/query_scale_ratio);

        //         theta1=sc1->theta((*it).first);
        //         theta2=sc2->theta((*it).second);

        //         scale1=model_sift_scale;
        //         scale2=query_sift_scale;

        //     }
        //     else
        //     {
        //         p1=sc2->sh_pt((*it).first);
        //         p2=sc1->sh_pt((*it).second);

        //         p1.set(p1.x()/model_scale_ratio,
        //                p1.y()/model_scale_ratio);
                
        //         p2.set(vcl_fabs(width-(p2.x()/query_scale_ratio)),
        //                p2.y()/query_scale_ratio);


        //         theta1=sc2->theta((*it).first);
        //         theta2=sc1->theta((*it).second);

        //         scale1=model_sift_scale;
        //         scale2=query_sift_scale;

        //     }
            
        //     file_stream<<p1.x()<<" "
        //                <<p1.y()<<" "
        //                <<scale1<<" "
        //                <<theta1<<" "
        //                <<p2.x()<<" "
        //                <<p2.y()<<" "
        //                <<scale2<<" "
        //                <<theta2<<vcl_endl;

        //     for ( unsigned int n=0; n < model_sift.size() ; ++n)
        //     {

        //         if ( n == (model_sift.size() - 1) )
        //         {
        //             sift_sc1<<model_sift[n]<<vcl_endl;
        //         }
        //         else
        //         {
        //             sift_sc1<<model_sift[n]<<" ";
        //         }
        //     }


        //     for ( unsigned int n=0; n < query_sift.size() ; ++n)
        //     {

        //         if ( n == (query_sift.size() - 1) )
        //         {
        //             sift_sc2<<query_sift[n]<<vcl_endl;
        //         }
        //         else
        //         {
        //             sift_sc2<<query_sift[n]<<" ";
        //         }
        //     }

        // }

        // file_stream.close();
        // sift_sc1.close();
        // sift_sc2.close();

    }

    vcl_pair<double,double> final_cost(total_alignment,
                                       total_alignment/map_list.size());
    
    if ( map_list.size() == 0 )
    {
        final_cost.first=1.0e6;
        final_cost.second=1.0e6;
    }

    return final_cost;
}

void dbskfg_match_bag_of_fragments::draw_part_correspondence(
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool flag,
    double width,
    double model_scale_ratio,
    double query_scale_ratio,
    vcl_string prefix)
{
    
    vcl_vector<vgl_polygon<double> > model_polys;
    vcl_vector<vgl_polygon<double> > query_polys;
    
    // Get matching pairs
    for (unsigned i = 0; i < map_list.size(); i++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];

        for (unsigned j = 1; j < map_list[i].size(); ++j) 
        {
            vcl_pair<int, int> start_pair = map_list[i][j-1];
            vcl_pair<int, int> stop_pair = map_list[i][j];
            

            unsigned int sc1_start=vcl_min(start_pair.first,stop_pair.first);
            unsigned int sc1_stop=vcl_max(start_pair.first,stop_pair.first);
            
            unsigned int sc2_start=vcl_min(start_pair.second,stop_pair.second);
            unsigned int sc2_stop=vcl_max(start_pair.second,stop_pair.second);
            
            vgl_polygon<double> sc1_poly(1);
            vgl_polygon<double> sc2_poly(1);

            if ( !flag )
            {
                sc1->get_polygon(sc1_start,sc1_stop,sc1_poly);
                sc2->get_polygon(sc2_start,sc2_stop,sc2_poly,width);
                
                model_polys.push_back(sc1_poly);
                query_polys.push_back(sc2_poly);
            }
            else
            {
                sc1->get_polygon(sc1_start,sc1_stop,sc1_poly,width);
                sc2->get_polygon(sc2_start,sc2_stop,sc2_poly);
             
                model_polys.push_back(sc2_poly);
                query_polys.push_back(sc1_poly);
            }

        }

    }

    vcl_cout<<"Model poly size: "<<model_polys.size()<<" vs "
            <<query_polys.size()<<vcl_endl;

    vcl_string title=prefix+"_correspondence.txt";

    vcl_ofstream stream(title.c_str());
    
    {
    
        for ( unsigned int d=0; d < model_polys.size() ; ++d )
        {
            
            vgl_polygon<double> sc1=model_polys[d];
            vgl_polygon<double> sc2=query_polys[d];
            
            for (unsigned int p = 0; p < sc1[0].size(); ++p)
            {
                stream<<sc1[0][p].x()<<" "<<sc1[0][p].y();

                if ( p == sc1[0].size()-1)
                {
                    stream <<vcl_endl;;

                }
                else
                {
                    stream<<" ";
                }
            }

            for (unsigned int p = 0; p < sc2[0].size(); ++p)
            {
                stream<<sc2[0][p].x()<<" "<<sc2[0][p].y();

                if ( p == sc2[0].size()-1)
                {
                    stream <<vcl_endl;;

                }
                else
                {
                    stream<<" ";
                }
            }

            
            
        }
    }
       
    stream.close();

}

void dbskfg_match_bag_of_fragments::warp_image(
    dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool flag,
    double width)
{

    
    vil_image_view<double>* query_channel1=query_tree->get_channel1();
    vil_image_view<double>* query_channel2=query_tree->get_channel2();
    vil_image_view<double>* query_channel3=query_tree->get_channel3();


    vil_image_view<vil_rgb<vxl_byte> > temp(model_tree->get_channel1()->ni(),
                                            model_tree->get_channel1()->nj());
    vil_rgb<vxl_byte> bg_col(255, 255, 255);
    temp.fill(bg_col);

    vgl_polygon<double> poly=model_fragments_polys_
        [model_tree->get_id()].second;
    
    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            vgl_point_2d<double> query_pt(x,y);

            vgl_point_2d<double> rt_model(0,0),rt_query(0,0);

            int curve_list_id;

            vgl_point_2d<double> mapping_pt=
                find_part_correspondences(query_pt,
                                          curve_list1,
                                          curve_list2,
                                          map_list,
                                          rt_model,
                                          rt_query,
                                          curve_list_id,
                                          flag,
                                          width,
                                          model_tree
                                          ->get_scale_ratio(),
                                          query_tree
                                          ->get_scale_ratio());
            
            if ( mapping_pt.x() != -1 )
            {
                double xx=mapping_pt.x();
                double yy=mapping_pt.y();
                
                double red   = vil_bilin_interp_safe(*query_channel1,xx,yy);
                double green = vil_bilin_interp_safe(*query_channel2,xx,yy);
                double blue  = vil_bilin_interp_safe(*query_channel3,xx,yy);
                
                temp(x,y)=vil_rgb<vxl_byte>(red,green,blue);

            }
        }
    }
    

    vcl_stringstream name;
    name<<"Model_"<<model_tree->get_id()<<"_vs_Query_"<<query_tree->get_id()
        <<"_warp.png";

    vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);
    vil_save_image_resource(out_img, 
                            name.str().c_str()); 

}   


vcl_pair<double,double> 
dbskfg_match_bag_of_fragments::compute_common_frame_distance(
    dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool flag,
    double width)
{
    
    
    vcl_pair<double,double> app_distance(0.0,0.0);

    vl_sift_pix* model_red_grad_data=model_tree->get_red_grad_data();    
    vl_sift_pix* model_green_grad_data=model_tree->get_green_grad_data();      
    vl_sift_pix* model_blue_grad_data=model_tree->get_blue_grad_data();

    VlSiftFilt* model_sift_filter=model_tree->get_sift_filter();

    vil_image_view<double>* query_channel1=query_tree->get_channel1();
    vil_image_view<double>* query_channel2=query_tree->get_channel2();
    vil_image_view<double>* query_channel3=query_tree->get_channel3();


    vil_image_view<vil_rgb<vxl_byte> > temp(model_tree->get_channel1()->ni(),
                                            model_tree->get_channel1()->nj());
    vil_rgb<vxl_byte> bg_col(0, 0, 0);
    temp.fill(bg_col);

    vgl_polygon<double> poly=model_fragments_polys_
        [model_tree->get_id()].second;
    
    vcl_vector<vgl_point_2d<double> > bc_coords;

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            vgl_point_2d<double> query_pt(x,y);

            vgl_point_2d<double> model_rt(0,0),query_rt(0,0);

            int curve_list_id;

            vgl_point_2d<double> mapping_pt=
                find_part_correspondences(query_pt,
                                          curve_list1,
                                          curve_list2,
                                          map_list,
                                          model_rt,
                                          query_rt,
                                          curve_list_id,
                                          flag,
                                          width,
                                          model_tree
                                          ->get_scale_ratio(),
                                          query_tree
                                          ->get_scale_ratio());
            
            if ( mapping_pt.x() != -1 )
            {
                double xx=mapping_pt.x();
                double yy=mapping_pt.y();
                
                double red   = vil_bilin_interp_safe(*query_channel1,xx,yy);
                double green = vil_bilin_interp_safe(*query_channel2,xx,yy);
                double blue  = vil_bilin_interp_safe(*query_channel3,xx,yy);
                
                temp(x,y)=vil_rgb<vxl_byte>(red,green,blue);

            }

            bc_coords.push_back(model_rt);
        }
    }

    vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);

    vl_sift_pix* query_red_grad_data(0);
    vl_sift_pix* query_green_grad_data(0);
    vl_sift_pix* query_blue_grad_data(0);
    
    vil_image_view<double> o1,o2,o3;
    convert_to_color_space(out_img,o1,o2,o3,
                           grad_color_space_);

    
    compute_grad_color_maps(o1,
                            &query_red_grad_data,
                            poly,
                            false);
    
    compute_grad_color_maps(o2,
                            &query_green_grad_data,
                            poly,
                            false);
    
    compute_grad_color_maps(o3,
                            &query_blue_grad_data,
                            poly,
                            false);

    double fixed_radius=8;
    double fixed_theta=0.0;

    double trad_sift_distance=0.0;
    double bc_sift_distance=0.0;

    unsigned int index=0;

    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); x=x+3) 
        {
            vgl_point_2d<double> model_pt(x,y);
            
            trad_sift_distance += descr_cost(
                model_pt,
                fixed_radius,
                fixed_theta,
                model_pt,
                fixed_radius,
                fixed_theta,
                model_red_grad_data,
                query_red_grad_data,
                model_green_grad_data,
                query_green_grad_data,
                model_blue_grad_data,
                query_blue_grad_data,
                model_sift_filter,
                model_sift_filter);

            index=index+1;
        }
    }
    
    app_distance.first  = trad_sift_distance/index;
    app_distance.second = bc_sift_distance/index;

    vl_free(query_red_grad_data);
    vl_free(query_green_grad_data);
    vl_free(query_blue_grad_data);

    query_red_grad_data=0;
    query_green_grad_data=0;
    query_blue_grad_data=0;

    // vcl_cout<<app_distance.first<<" "<<app_distance.second<<vcl_endl;
    return app_distance;
}   

vcl_pair<double,double> 
dbskfg_match_bag_of_fragments::compute_common_frame_distance_qm(
    dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool flag,
    double width,
    bool debug)
{
    
    vcl_pair<double,double> app_distance(0.0,0.0);

    vl_sift_pix* query_red_grad_data  =query_tree->get_red_grad_data();    
    vl_sift_pix* query_green_grad_data=query_tree->get_green_grad_data();      
    vl_sift_pix* query_blue_grad_data =query_tree->get_blue_grad_data();

    VlSiftFilt* query_sift_filter=query_tree->get_sift_filter();

    vil_image_view<double>* model_channel1=model_tree->get_channel1();
    vil_image_view<double>* model_channel2=model_tree->get_channel2();
    vil_image_view<double>* model_channel3=model_tree->get_channel3();

    vil_image_view<double>* query_channel1=query_tree->get_channel1();
    vil_image_view<double>* query_channel2=query_tree->get_channel2();
    vil_image_view<double>* query_channel3=query_tree->get_channel3();

    vil_image_view<vil_rgb<vxl_byte> > temp;

    if ( debug )
    {
        temp.set_size(query_channel1->ni(),
                      query_channel1->nj());
        vil_rgb<vxl_byte> bg_col(255,255,255);
        temp.fill(bg_col);
    }

    int ni=query_channel1->ni();

    vgl_polygon<double> poly=query_fragments_polys_
        [query_tree->get_id()].second;
    
    vcl_map<vcl_pair<int,int>, vgl_point_2d<double> > bc_coords;

    vcl_set<vcl_pair<int,int> > out_of_bounds;
    
    vcl_map<vcl_pair<int,int>, int > point_to_curve_mapping;

    vil_image_view<double> o1(query_channel1->ni(),
                              query_channel2->nj());
    o1.fill(0);
    

    vil_image_view<double> o2(query_channel1->ni(),
                              query_channel2->nj());
    o2.fill(0);

    vil_image_view<double> o3(query_channel1->ni(),
                              query_channel2->nj());
    o3.fill(0);

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            vgl_point_2d<double> query_pt(x,y);

            vgl_point_2d<double> model_rt(0,0),query_rt(0,0);

            int curve_list_id(0);

            vgl_point_2d<double> mapping_pt=
                find_part_correspondences_qm(query_pt,
                                             curve_list1,
                                             curve_list2,
                                             map_list,
                                             model_rt,
                                             query_rt,
                                             curve_list_id,
                                             flag,
                                             width,
                                             model_tree
                                             ->get_scale_ratio(),
                                             query_tree
                                             ->get_scale_ratio());

            if ( mapping_pt.x() != -1 )
            {
                double xx=mapping_pt.x();
                double yy=mapping_pt.y();
                
                double red   = vil_bilin_interp_safe(*model_channel1,xx,yy);
                double green = vil_bilin_interp_safe(*model_channel2,xx,yy);
                double blue  = vil_bilin_interp_safe(*model_channel3,xx,yy);
                

                if ( width )
                {
                    if ( debug )
                    {
                        temp(ni-1-x,y)=vil_rgb<vxl_byte>(red,green,blue);
                    }

                    o1(ni-1-x,y)=red;
                    o2(ni-1-x,y)=green;
                    o3(ni-1-x,y)=blue;
                }
                else
                {
                    if ( debug )
                    {
                        temp(x,y)=vil_rgb<vxl_byte>(red,green,blue);
                    }

                    o1(x,y)=red;
                    o2(x,y)=green;
                    o3(x,y)=blue;

                }

                vcl_pair<int,int> key(x,y);
                bc_coords[key]=model_rt;

                point_to_curve_mapping[key]=curve_list_id;
            }
            else
            {
                out_of_bounds.insert(vcl_make_pair(x,y));
            }

        }
    }

    

    if ( debug )
    {
        vcl_stringstream name;
        name<<"Model_"<<model_tree->get_id()<<"_vs_Query_"<<query_tree->get_id()
            <<"_warp.png";

        vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);

        vil_save_image_resource(out_img, 
                                name.str().c_str()); 
    }

    vl_sift_pix* model_red_grad_data(0);
    vl_sift_pix* model_green_grad_data(0);
    vl_sift_pix* model_blue_grad_data(0);
    
    compute_grad_color_maps(o1,
                            &model_red_grad_data,
                            poly,
                            false);
    
    compute_grad_color_maps(o2,
                            &model_green_grad_data,
                            poly,
                            false);
    
    compute_grad_color_maps(o3,
                            &model_blue_grad_data,
                            poly,
                            false);

    double fixed_radius=16;
    double fixed_theta=0.0;

    double trad_sift_distance=0.0;
    double local_color_distance=0.0;

    unsigned int index=0;
    unsigned int stride=8;

    vnl_vector<double> part_distances(curve_list1.size(),0.0);

    vcl_vector<double> norm_factors(curve_list1.size(),0.0);

    vnl_matrix<double> dist_map;

    if ( debug )
    {
        dist_map.set_size(temp.ni(),temp.nj());
        dist_map.fill(0.0);
        stride=1;
    }
    
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); x=x+stride) 
        {
            if ( out_of_bounds.count(vcl_make_pair(x,y)))
            {
                continue;
            }

            vcl_pair<int,int> key(x,y);

            int curve_list_id=point_to_curve_mapping[key];

            vgl_point_2d<double> model_pt(x,y);
            
            if ( width )
            {
                model_pt.set(ni-1-x,y);
            }

            double sample_distance = descr_cost(
                model_pt,
                fixed_radius,
                fixed_theta,
                model_pt,
                fixed_radius,
                fixed_theta,
                model_red_grad_data,
                query_red_grad_data,
                model_green_grad_data,
                query_green_grad_data,
                model_blue_grad_data,
                query_blue_grad_data,
                query_sift_filter,
                query_sift_filter);

            trad_sift_distance += sample_distance;

            part_distances[curve_list_id]=part_distances[curve_list_id]+
                sample_distance;

            norm_factors[curve_list_id]=norm_factors[curve_list_id]+
                1.0;

            vcl_set<vcl_pair<double,double> > sift_samples;
                        
            compute_color_over_sift(
                query_sift_filter,
                query_sift_filter->width,
                query_sift_filter->height,
                model_pt.x(),
                model_pt.y(),
                fixed_radius,
                fixed_theta,
                sift_samples);

            vcl_vector<double> model_descr;
            vcl_vector<double> query_descr;
                        
            compute_color_region_hist(
                sift_samples,
                o1,
                o2,
                o3,
                model_descr,
                dbskfg_match_bag_of_fragments::DEFAULT);

            compute_color_region_hist(
                sift_samples,
                *query_channel1,
                *query_channel2,
                *query_channel3,
                query_descr,
                dbskfg_match_bag_of_fragments::DEFAULT);
                        
            vnl_vector<double> vec_model(model_descr.size(),0);
            vnl_vector<double> vec_query(query_descr.size(),0);

            for ( unsigned int m=0; m < model_descr.size(); ++m)
            {
                vec_model.put(m,model_descr[m]);
                vec_query.put(m,query_descr[m]);
            }

            vec_model.normalize(); vec_query.normalize();

            local_color_distance +=
                chi_squared_distance(vec_model,vec_query);

            if ( debug )
            {
                dist_map(x,y)=sample_distance;
            }
            
            index=index+1;
        }
        
        for ( unsigned int k=0; k < stride-1 ; ++k)
        {
            psi.next();
        }
    }

    // Normalize part distances
    for ( unsigned int i=0; i < part_distances.size() ; ++i)
    {
        part_distances[i]=part_distances[i]/norm_factors[i];

        if ( debug )
        {
            vcl_cout<<"Part "<<i<<" : "<<part_distances[i]<<vcl_endl;
        }
    }

    double part_norm_distance=part_distances.mean();

    if ( debug )
    {
        unsigned int g=model_tree->get_id();

        vcl_stringstream name;
        name<<output_dist_file_;
        if ( g < 10 )
        {
            name<<"_Model_000"<<g<<"_dist_map.txt";
        }
        else if ( g >= 10 && g < 100)
        {

            name<<"_Model_00"<<g<<"_dist_map.txt";
        }
        else
        {
            name<<"_Model_0"<<g<<"_dist_map.txt";
        }

        vcl_ofstream streamer(name.str().c_str());
        dist_map.print(streamer);
        streamer.close();
        
        dist_map.clear();
    }

    app_distance.first  = trad_sift_distance/index;
    app_distance.second = part_norm_distance;
    //app_distance.second = local_color_distance/index;

    vl_free(model_red_grad_data);
    vl_free(model_green_grad_data);
    vl_free(model_blue_grad_data);

    model_red_grad_data=0;
    model_green_grad_data=0;
    model_blue_grad_data=0;

    if ( debug )
    {
        vcl_cout<<"Color grad: "<<app_distance.first<<vcl_endl;
        vcl_cout<<"Color: "<<app_distance.second<<vcl_endl;
    }

    return app_distance;
}   

vcl_pair<double,double> 
dbskfg_match_bag_of_fragments::compute_common_frame_distance_bbox_qm(
    dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    double& extra_cost,
    bool flag,
    double width,
    bool debug)
{
    
    vcl_pair<double,double> app_distance(0.0,0.0);

    vl_sift_pix* query_red_grad_data  =query_tree->get_red_grad_data();    
    vl_sift_pix* query_green_grad_data=query_tree->get_green_grad_data();      
    vl_sift_pix* query_blue_grad_data =query_tree->get_blue_grad_data();

    VlSiftFilt* query_sift_filter=query_tree->get_sift_filter();

    vil_image_view<double>* model_channel1=model_tree->get_channel1();
    vil_image_view<double>* model_channel2=model_tree->get_channel2();
    vil_image_view<double>* model_channel3=model_tree->get_channel3();

    vil_image_view<double>* query_channel1=query_tree->get_channel1();
    vil_image_view<double>* query_channel2=query_tree->get_channel2();
    vil_image_view<double>* query_channel3=query_tree->get_channel3();

    vil_image_view<vil_rgb<vxl_byte> > temp;

    if ( debug )
    {
        temp.set_size(query_channel1->ni(),
                      query_channel1->nj());
        vil_rgb<vxl_byte> bg_col(255,255,255);
        temp.fill(bg_col);
    }

    int ni=query_channel1->ni();

    vgl_polygon<double> poly=query_fragments_polys_
        [query_tree->get_id()].second;
    
    vcl_map<vcl_pair<int,int>, vgl_point_2d<double> > bc_coords;

    vcl_set<vcl_pair<int,int> > out_of_bounds;
    vcl_set<vcl_pair<int,int> > in_bounds;
    
    vcl_map<vcl_pair<int,int>, int > point_to_curve_mapping;

    vil_image_view<double> o1(query_channel1->ni(),
                              query_channel2->nj());
    o1.fill(0);
    

    vil_image_view<double> o2(query_channel1->ni(),
                              query_channel2->nj());
    o2.fill(0);

    vil_image_view<double> o3(query_channel1->ni(),
                              query_channel2->nj());
    o3.fill(0);

    vgl_box_2d<double> bbox;

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            vgl_point_2d<double> query_pt(x,y);

            bbox.add(query_pt);
            vcl_pair<int,int> ib(x,y);
            in_bounds.insert(ib);

            vgl_point_2d<double> model_rt(0,0),query_rt(0,0);

            int curve_list_id(0);

            vgl_point_2d<double> mapping_pt=
                find_part_correspondences_qm(query_pt,
                                             curve_list1,
                                             curve_list2,
                                             map_list,
                                             model_rt,
                                             query_rt,
                                             curve_list_id,
                                             flag,
                                             width,
                                             model_tree
                                             ->get_scale_ratio(),
                                             query_tree
                                             ->get_scale_ratio());

            if ( mapping_pt.x() != -1 )
            {
                double xx=mapping_pt.x();
                double yy=mapping_pt.y();
                
                double red   = vil_bilin_interp_safe(*model_channel1,xx,yy);
                double green = vil_bilin_interp_safe(*model_channel2,xx,yy);
                double blue  = vil_bilin_interp_safe(*model_channel3,xx,yy);
                

                if ( width )
                {
                    if ( debug )
                    {
                        temp(ni-1-x,y)=vil_rgb<vxl_byte>(red,green,blue);
                    }

                    o1(ni-1-x,y)=red;
                    o2(ni-1-x,y)=green;
                    o3(ni-1-x,y)=blue;
                }
                else
                {
                    if ( debug )
                    {
                        temp(x,y)=vil_rgb<vxl_byte>(red,green,blue);
                    }

                    o1(x,y)=red;
                    o2(x,y)=green;
                    o3(x,y)=blue;

                }

                vcl_pair<int,int> key(x,y);
                bc_coords[key]=model_rt;

                point_to_curve_mapping[key]=curve_list_id;
            }
            else
            {
                out_of_bounds.insert(vcl_make_pair(x,y));
            }

        }
    }

    

    if ( debug )
    {
        vcl_stringstream name;
        name<<"Model_"<<model_tree->get_id()<<"_vs_Query_"<<query_tree->get_id()
            <<"_warp.png";

        vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);

        vil_save_image_resource(out_img, 
                                name.str().c_str()); 
    }

    vl_sift_pix* model_red_grad_data(0);
    vl_sift_pix* model_green_grad_data(0);
    vl_sift_pix* model_blue_grad_data(0);
    
    compute_grad_color_maps(o1,
                            &model_red_grad_data,
                            poly,
                            false);
    
    compute_grad_color_maps(o2,
                            &model_green_grad_data,
                            poly,
                            false);
    
    compute_grad_color_maps(o3,
                            &model_blue_grad_data,
                            poly,
                            false);

    double fixed_radius=16;
    double fixed_theta=0.0;
    double color_radius=2.0;

    double trad_sift_distance=0.0;
    double local_color_distance=0.0;
    double combined_distance=0.0;

    unsigned int index=0;
    unsigned int stride=8;

    VlFloatVectorComparisonFunction FV_distance =    
      vl_get_vector_comparison_function_f (VlDistanceL2) ;

    VlDoubleVectorComparisonFunction FV_distance_double =    
      vl_get_vector_comparison_function_d (VlDistanceL2) ;

    vnl_vector<double> part_distances(curve_list1.size(),0.0);

    vcl_vector<double> norm_factors(curve_list1.size(),0.0);

    vnl_matrix<double> dist_map;

    if ( debug )
    {
        dist_map.set_size(query_channel1->ni(),query_channel1->nj());
        dist_map.fill(0.0);
        stride=1;
    }
    
    for ( unsigned int y=bbox.min_y(); y <= bbox.max_y(); y=y+stride)
    {
        for ( unsigned int x=bbox.min_x(); x <= bbox.max_x() ; x=x+stride) 
        {
            if ( !in_bounds.count(vcl_make_pair(x,y)))
            {
                continue;
            }

            // if ( out_of_bounds.count(vcl_make_pair(x,y)))
            // {
            //     continue;
            // }

            vcl_pair<int,int> key(x,y);

            int curve_list_id=point_to_curve_mapping[key];

            vgl_point_2d<double> model_pt(x,y);
            vgl_point_2d<double> q_pt(x,y);

            if ( width )
            {
                model_pt.set(ni-1-x,y);
            }

            vnl_vector<vl_sift_pix> model_fv;
            vnl_vector<vl_sift_pix> query_fv;
            
            compute_descr_fv(model_pt,
                             fixed_radius,
                             fixed_theta,
                             model_red_grad_data,
                             model_green_grad_data,
                             model_blue_grad_data,
                             query_sift_filter,
                             model_fv);

            compute_descr_fv(model_pt,
                             fixed_radius,
                             fixed_theta,
                             query_red_grad_data,
                             query_green_grad_data,
                             query_blue_grad_data,
                             query_sift_filter,
                             query_fv);

            vl_sift_pix result_final[1];

            vl_eval_vector_comparison_on_all_pairs_f(result_final,
                                                     model_fv.size(),
                                                     model_fv.data_block(),
                                                     1,
                                                     query_fv.data_block(),
                                                     1,
                                                     FV_distance);
            
            double sample_distance = result_final[0];

            trad_sift_distance += sample_distance;

            part_distances[curve_list_id]=part_distances[curve_list_id]+
                sample_distance;

            norm_factors[curve_list_id]=norm_factors[curve_list_id]+
                1.0;


            vcl_vector<double> model_color_fv,query_color_fv;

            double color_scale=64.0;
            compute_mean_std_color_descr
                (q_pt,
                 color_scale,
                 *query_channel1,
                 *query_channel2,
                 *query_channel3,
                 query_color_fv);
            
            compute_mean_std_color_descr
                (model_pt,
                 color_scale,
                 *model_channel1,
                 *model_channel2,
                 *model_channel3,
                 model_color_fv);

            double color_final[1];
            vl_eval_vector_comparison_on_all_pairs_d(color_final,
                                                     model_color_fv.size(),
                                                     model_color_fv.data(),
                                                     1,
                                                     query_color_fv.data(),
                                                     1,
                                                     FV_distance_double);
            
            double color_distance = color_final[0];
                        
            local_color_distance += color_distance;

            vcl_vector<vl_sift_pix> combined_model_descriptor;
            vcl_vector<vl_sift_pix> combined_query_descriptor;

            for ( unsigned int cgfv = 0; cgfv < model_fv.size() ; ++cgfv)
            {
                combined_model_descriptor.push_back(model_fv[cgfv]);
                combined_query_descriptor.push_back(query_fv[cgfv]);
            }

            for ( unsigned int cfv = 0; cfv < model_color_fv.size() ; ++cfv)
            {
                combined_model_descriptor.push_back(model_color_fv[cfv]);
                combined_query_descriptor.push_back(query_color_fv[cfv]);
            }

            vl_sift_pix combined_final_result[1];
            vl_eval_vector_comparison_on_all_pairs_f(
                combined_final_result,
                combined_model_descriptor.size(),
                combined_model_descriptor.data(),
                1,
                combined_query_descriptor.data(),
                1,
                FV_distance);

            double combined_sampled_distance = combined_final_result[0]; 
           
            combined_distance += combined_sampled_distance;
            
            // vcl_set<vcl_pair<double,double> > query_sift_samples;
            // vcl_set<vcl_pair<double,double> > model_sift_samples;
                        
            // compute_color_over_sift(
            //     query_sift_filter,
            //     query_sift_filter->width,
            //     query_sift_filter->height,
            //     q_pt.x(),
            //     q_pt.y(),
            //     color_radius,
            //     fixed_theta,
            //     query_sift_samples);

            // model_sift_samples=query_sift_samples;

            // if ( width )
            // {
                
            //     model_sift_samples.clear();
            //     vcl_set<vcl_pair<double,double> >::iterator it;
            //     for ( it = query_sift_samples.begin() ; it != 
            //               query_sift_samples.end() ; ++it)
            //     {
                    
            //         vcl_pair<double,double> point=*it;
            //         vcl_pair<double,double> flipped(ni-1-point.first,
            //                                         point.second);
            //         model_sift_samples.insert(flipped);
            //     }
                
            // }

            // vcl_vector<vl_sift_pix> model_color_fv;
            // vcl_vector<vl_sift_pix> query_color_fv;
            
            // vcl_vector<vl_sift_pix> model_color_hist;
            // vcl_set<vcl_pair<double,double> >::iterator mit;
            // for ( mit=model_sift_samples.begin() ; 
            //       mit != model_sift_samples.end(); ++mit)
            // {

            //     double xx=(*mit).first;
            //     double yy=(*mit).second;

            //     double red   = vil_bilin_interp_safe(o1,xx,yy);
            //     double green = vil_bilin_interp_safe(o2,xx,yy);
            //     double blue  = vil_bilin_interp_safe(o3,xx,yy);
 
            //     model_color_hist.push_back(red);
            //     model_color_hist.push_back(green);
            //     model_color_hist.push_back(blue);
            // }

            // vcl_vector<vl_sift_pix> query_color_hist;
            // vcl_set<vcl_pair<double,double> >::iterator qit;
            // for ( qit=query_sift_samples.begin() ; 
            //       qit != query_sift_samples.end(); ++qit)
            // {
            //     double xx=(*qit).first;
            //     double yy=(*qit).second;

            //     double red   = vil_bilin_interp_safe(*query_channel1,xx,yy);
            //     double green = vil_bilin_interp_safe(*query_channel2,xx,yy);
            //     double blue  = vil_bilin_interp_safe(*query_channel3,xx,yy);
 
            //     query_color_hist.push_back(red);
            //     query_color_hist.push_back(green);
            //     query_color_hist.push_back(blue);

            // }

            // encode_color_triplet(model_color_hist,model_color_fv);
            // encode_color_triplet(query_color_hist,query_color_fv);
 
            // compute_color_region_hist(
            //     model_sift_samples,
            //     o1,
            //     o2,
            //     o3,
            //     model_descr,
            //     dbskfg_match_bag_of_fragments::DEFAULT);

            // compute_color_region_hist(
            //     query_sift_samples,
            //     *query_channel1,
            //     *query_channel2,
            //     *query_channel3,
            //     query_descr,
            //     dbskfg_match_bag_of_fragments::DEFAULT);

            // compute_color_region_hist_fv(
            //     model_sift_samples,
            //     o1,
            //     o2,
            //     o3,
            //     model_color_fv,
            //     dbskfg_match_bag_of_fragments::DEFAULT);

            // compute_color_region_hist_fv(
            //     query_sift_samples,
            //     *query_channel1,
            //     *query_channel2,
            //     *query_channel3,
            //     query_color_fv,
            //     dbskfg_match_bag_of_fragments::DEFAULT);

            // vnl_vector<double> vec_model(model_descr.size(),0);
            // vnl_vector<double> vec_query(query_descr.size(),0);

            // for ( unsigned int m=0; m < model_descr.size(); ++m)
            // {
            //     vec_model.put(m,model_descr[m]);
            //     vec_query.put(m,query_descr[m]);
            // }

            // vec_model *= 1/vec_model.sum();
            // vec_query *= 1/vec_query.sum();
            // double color_distance = 
            // chi_squared_distance(vec_model,vec_query); 
            
            local_dist_map_[key]=sample_distance;

            if ( debug )
            {
                dist_map(x,y)=sample_distance;
                //dist_map(x,y)=combined_sampled_distance;
            }
            
            index=index+1;
        }
    }

    // Normalize part distances
    for ( unsigned int i=0; i < part_distances.size() ; ++i)
    {
        part_distances[i]=part_distances[i]/norm_factors[i];

        if ( debug )
        {
            vcl_cout<<"Part "<<i<<" : "<<part_distances[i]<<vcl_endl;
        }
    }

    double part_norm_distance=part_distances.mean();

    if ( debug )
    {
        unsigned int g=model_tree->get_id();

        vcl_stringstream name;
        name<<output_dist_file_;
        if ( g < 10 )
        {
            name<<"_Model_000"<<g<<"_dist_map.txt";
        }
        else if ( g >= 10 && g < 100)
        {

            name<<"_Model_00"<<g<<"_dist_map.txt";
        }
        else
        {
            name<<"_Model_0"<<g<<"_dist_map.txt";
        }

        vcl_ofstream streamer(name.str().c_str());
        dist_map.print(streamer);
        streamer.close();
        
        dist_map.clear();
    }

    app_distance.first  = combined_distance/index;
    //app_distance.first  = trad_sift_distance/index;
    //app_distance.second = part_norm_distance;
    app_distance.second = local_color_distance/index;

    extra_cost= trad_sift_distance/index;
    vl_free(model_red_grad_data);
    vl_free(model_green_grad_data);
    vl_free(model_blue_grad_data);

    model_red_grad_data=0;
    model_green_grad_data=0;
    model_blue_grad_data=0;

    if ( debug )
    {
        vcl_cout<<"Color grad: "<<app_distance.first<<vcl_endl;
        vcl_cout<<"Color: "<<app_distance.second<<vcl_endl;
    }

    return app_distance;
}   


vcl_pair<double,double> 
dbskfg_match_bag_of_fragments::compute_implicit_distance_bbox_qm(
    dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool flag,
    double width,
    bool debug)
{
    
    vcl_pair<double,double> app_distance(0.0,0.0);

    vl_sift_pix* query_red_grad_data  =query_tree->get_red_grad_data();    
    vl_sift_pix* query_green_grad_data=query_tree->get_green_grad_data();      
    vl_sift_pix* query_blue_grad_data =query_tree->get_blue_grad_data();

    vl_sift_pix* model_red_grad_data  =model_tree->get_red_grad_data();    
    vl_sift_pix* model_green_grad_data=model_tree->get_green_grad_data();      
    vl_sift_pix* model_blue_grad_data =model_tree->get_blue_grad_data();

    VlSiftFilt* query_sift_filter=query_tree->get_sift_filter();
    VlSiftFilt* model_sift_filter=model_tree->get_sift_filter();

    vil_image_view<double>* model_channel1=model_tree->get_channel1();
    vil_image_view<double>* model_channel2=model_tree->get_channel2();
    vil_image_view<double>* model_channel3=model_tree->get_channel3();

    vil_image_view<double>* query_channel1=query_tree->get_channel1();
    vil_image_view<double>* query_channel2=query_tree->get_channel2();
    vil_image_view<double>* query_channel3=query_tree->get_channel3();

    vil_image_view<vil_rgb<vxl_byte> > temp;

    vgl_polygon<double> poly=query_fragments_polys_
        [query_tree->get_id()].second;
    
    vcl_map<vcl_pair<int,int>, vgl_point_2d<double> > bc_coords;

    vcl_set<vcl_pair<int,int> > out_of_bounds;
    vcl_set<vcl_pair<int,int> > in_bounds;
    
    vcl_map<vcl_pair<int,int>, int > point_to_curve_mapping;
    vcl_map<vcl_pair<int,int>, vgl_point_2d<double> > q_to_m_mapping;

    vgl_box_2d<double> bbox;

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            vgl_point_2d<double> query_pt(x,y);

            bbox.add(query_pt);
            vcl_pair<int,int> ib(x,y);
            in_bounds.insert(ib);

            vgl_point_2d<double> model_rt(0,0),query_rt(0,0);

            int curve_list_id(0);

            vgl_point_2d<double> mapping_pt=
                find_part_correspondences_qm(query_pt,
                                             curve_list1,
                                             curve_list2,
                                             map_list,
                                             model_rt,
                                             query_rt,
                                             curve_list_id,
                                             flag,
                                             width,
                                             model_tree
                                             ->get_scale_ratio(),
                                             query_tree
                                             ->get_scale_ratio());

            if ( mapping_pt.x() != -1 )
            {
                vcl_pair<int,int> key(x,y);
                bc_coords[key]=model_rt;

                point_to_curve_mapping[key]=curve_list_id;

                q_to_m_mapping[key]=mapping_pt;
            }
            else
            {
                out_of_bounds.insert(vcl_make_pair(x,y));
            }

        }
    }

    double fixed_radius=16;
    double fixed_theta=0.0;

    double trad_sift_distance=0.0;
    double local_color_distance=0.0;

    unsigned int index=0;
    unsigned int stride=8;

    vnl_vector<double> part_distances(curve_list1.size(),0.0);

    vcl_vector<double> norm_factors(curve_list1.size(),0.0);

    vnl_matrix<double> dist_map;

    if ( debug )
    {
        dist_map.set_size(query_channel1->ni(),query_channel1->nj());
        dist_map.fill(0.0);
        stride=1;
    }
    
    for ( unsigned int y=bbox.min_y(); y <= bbox.max_y(); y=y+stride)
    {
        for ( unsigned int x=bbox.min_x(); x <= bbox.max_x() ; x=x+stride) 
        {
            if ( !in_bounds.count(vcl_make_pair(x,y)))
            {
                continue;
            }

            if ( out_of_bounds.count(vcl_make_pair(x,y)))
            {
                continue;
            }

            vcl_pair<int,int> key(x,y);

            int curve_list_id=point_to_curve_mapping[key];

            vgl_point_2d<double> query_pt(x,y);
            vgl_point_2d<double> c_pt(x,y);
            vgl_point_2d<double> model_pt = q_to_m_mapping[key];
            
            if ( width )
            {
                query_pt.set(query_channel1->ni()-1-x,y);
            }

            double sample_distance = descr_cost(
                model_pt,
                fixed_radius,
                fixed_theta,
                query_pt,
                fixed_radius,
                fixed_theta,
                model_red_grad_data,
                query_red_grad_data,
                model_green_grad_data,
                query_green_grad_data,
                model_blue_grad_data,
                query_blue_grad_data,
                model_sift_filter,
                query_sift_filter);

            trad_sift_distance += sample_distance;

            part_distances[curve_list_id]=part_distances[curve_list_id]+
                sample_distance;

            norm_factors[curve_list_id]=norm_factors[curve_list_id]+
                1.0;

            vcl_set<vcl_pair<double,double> > query_sift_samples;
            vcl_set<vcl_pair<double,double> > model_sift_samples;
                        
            compute_color_over_sift(
                query_sift_filter,
                query_sift_filter->width,
                query_sift_filter->height,
                c_pt.x(),
                c_pt.y(),
                fixed_radius,
                fixed_theta,
                query_sift_samples);

            compute_color_over_sift(
                model_sift_filter,
                model_sift_filter->width,
                model_sift_filter->height,
                model_pt.x(),
                model_pt.y(),
                fixed_radius,
                fixed_theta,
                model_sift_samples);

            vcl_vector<double> model_descr;
            vcl_vector<double> query_descr;
                        
            compute_color_region_hist(
                model_sift_samples,
                *model_channel1,
                *model_channel2,
                *model_channel3,
                model_descr,
                dbskfg_match_bag_of_fragments::DEFAULT);

            compute_color_region_hist(
                query_sift_samples,
                *query_channel1,
                *query_channel2,
                *query_channel3,
                query_descr,
                dbskfg_match_bag_of_fragments::DEFAULT);
                        
            vnl_vector<double> vec_model(model_descr.size(),0);
            vnl_vector<double> vec_query(query_descr.size(),0);

            for ( unsigned int m=0; m < model_descr.size(); ++m)
            {
                vec_model.put(m,model_descr[m]);
                vec_query.put(m,query_descr[m]);
            }

            vec_model *= 1/vec_model.sum();
            vec_query *= 1/vec_query.sum();


            local_color_distance +=
                chi_squared_distance(vec_model,vec_query);

            if ( debug )
            {
                dist_map(x,y)=sample_distance;
            }
            
            index=index+1;
        }
    }

    // Normalize part distances
    for ( unsigned int i=0; i < part_distances.size() ; ++i)
    {
        part_distances[i]=part_distances[i]/norm_factors[i];

        if ( debug )
        {
            vcl_cout<<"Part "<<i<<" : "<<part_distances[i]<<vcl_endl;
        }
    }

    double part_norm_distance=part_distances.mean();

    if ( debug )
    {
        unsigned int g=model_tree->get_id();

        vcl_stringstream name;
        name<<output_dist_file_;
        if ( g < 10 )
        {
            name<<"_Model_000"<<g<<"_dist_map.txt";
        }
        else if ( g >= 10 && g < 100)
        {

            name<<"_Model_00"<<g<<"_dist_map.txt";
        }
        else
        {
            name<<"_Model_0"<<g<<"_dist_map.txt";
        }

        vcl_ofstream streamer(name.str().c_str());
        dist_map.print(streamer);
        streamer.close();
        
        dist_map.clear();
    }

    app_distance.first  = trad_sift_distance/index;
    //app_distance.second = part_norm_distance;
    app_distance.second = local_color_distance/index;

    if ( debug )
    {
        vcl_cout<<"Color grad: "<<app_distance.first<<vcl_endl;
        vcl_cout<<"Color: "<<app_distance.second<<vcl_endl;
    }

    return app_distance;
}   

vcl_pair<double,double> 
dbskfg_match_bag_of_fragments::compute_common_frame_distance_bbox_mq(
    dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool flag,
    double width,
    bool debug)
{
    
    vcl_pair<double,double> app_distance(0.0,0.0);

    vl_sift_pix* model_red_grad_data  =model_tree->get_red_grad_data();    
    vl_sift_pix* model_green_grad_data=model_tree->get_green_grad_data();      
    vl_sift_pix* model_blue_grad_data =model_tree->get_blue_grad_data();

    VlSiftFilt* model_sift_filter=model_tree->get_sift_filter();

    vil_image_view<double>* model_channel1=model_tree->get_channel1();
    vil_image_view<double>* model_channel2=model_tree->get_channel2();
    vil_image_view<double>* model_channel3=model_tree->get_channel3();

    vil_image_view<double>* query_channel1=query_tree->get_channel1();
    vil_image_view<double>* query_channel2=query_tree->get_channel2();
    vil_image_view<double>* query_channel3=query_tree->get_channel3();

    vil_image_view<vil_rgb<vxl_byte> > temp;

    if ( debug )
    {
        temp.set_size(model_channel1->ni(),
                      model_channel1->nj());
        vil_rgb<vxl_byte> bg_col(255,255,255);
        temp.fill(bg_col);
    }

    int ni=model_channel1->ni();

    vgl_polygon<double> poly=model_fragments_polys_
        [model_tree->get_id()].second;
    
    vcl_map<vcl_pair<int,int>, vgl_point_2d<double> > bc_coords;

    vcl_set<vcl_pair<int,int> > out_of_bounds;
    vcl_set<vcl_pair<int,int> > in_bounds;
    
    vcl_map<vcl_pair<int,int>, int > point_to_curve_mapping;

    vil_image_view<double> o1(model_channel1->ni(),
                              model_channel2->nj());
    o1.fill(0);
    

    vil_image_view<double> o2(model_channel1->ni(),
                              model_channel2->nj());
    o2.fill(0);

    vil_image_view<double> o3(model_channel1->ni(),
                              model_channel2->nj());
    o3.fill(0);

    vgl_box_2d<double> bbox;

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            vgl_point_2d<double> query_pt(x,y);

            bbox.add(query_pt);
            vcl_pair<int,int> ib(x,y);
            in_bounds.insert(ib);

            vgl_point_2d<double> model_rt(0,0),query_rt(0,0);

            int curve_list_id(0);

            vgl_point_2d<double> mapping_pt=
                find_part_correspondences(query_pt,
                                          curve_list1,
                                          curve_list2,
                                          map_list,
                                          model_rt,
                                          query_rt,
                                          curve_list_id,
                                          flag,
                                          width,
                                          model_tree
                                          ->get_scale_ratio(),
                                          query_tree
                                          ->get_scale_ratio());
            
            if ( mapping_pt.x() != -1 )
            {
                double xx=mapping_pt.x();
                double yy=mapping_pt.y();
                
                double red   = vil_bilin_interp_safe(*query_channel1,xx,yy);
                double green = vil_bilin_interp_safe(*query_channel2,xx,yy);
                double blue  = vil_bilin_interp_safe(*query_channel3,xx,yy);
                
                if ( debug )
                {
                    temp(x,y)=vil_rgb<vxl_byte>(red,green,blue);
                }
                
                o1(x,y)=red;
                o2(x,y)=green;
                o3(x,y)=blue;
                
                vcl_pair<int,int> key(x,y);
                bc_coords[key]=model_rt;

                point_to_curve_mapping[key]=curve_list_id;
            }
            else
            {
                out_of_bounds.insert(vcl_make_pair(x,y));
            }

        }
    }

    

    if ( debug )
    {
        vcl_stringstream name;
        name<<"Model_"<<model_tree->get_id()<<"_vs_Query_"<<query_tree->get_id()
            <<"_warp.png";

        vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);

        vil_save_image_resource(out_img, 
                                name.str().c_str()); 
    }

    vl_sift_pix* query_red_grad_data(0);
    vl_sift_pix* query_green_grad_data(0);
    vl_sift_pix* query_blue_grad_data(0);
    
    compute_grad_color_maps(o1,
                            &query_red_grad_data,
                            poly,
                            false);
    
    compute_grad_color_maps(o2,
                            &query_green_grad_data,
                            poly,
                            false);
    
    compute_grad_color_maps(o3,
                            &query_blue_grad_data,
                            poly,
                            false);

    double fixed_radius=16;
    double fixed_theta=0.0;

    double trad_sift_distance=0.0;
    double local_color_distance=0.0;

    unsigned int index=0;
    unsigned int stride=8;

    vnl_vector<double> part_distances(curve_list1.size(),0.0);

    vcl_vector<double> norm_factors(curve_list1.size(),0.0);

    vnl_matrix<double> dist_map;

    if ( debug )
    {
        dist_map.set_size(temp.ni(),temp.nj());
        dist_map.fill(0.0);
        stride=1;
    }
    
    for ( unsigned int y=bbox.min_y(); y <= bbox.max_y(); y=y+stride)
    {
        for ( unsigned int x=bbox.min_x(); x <= bbox.max_x() ; x=x+stride) 
        {
            if ( !in_bounds.count(vcl_make_pair(x,y)))
            {
                continue;
            }

            if ( out_of_bounds.count(vcl_make_pair(x,y)))
            {
                continue;
            }

            vcl_pair<int,int> key(x,y);

            int curve_list_id=point_to_curve_mapping[key];

            vgl_point_2d<double> model_pt(x,y);
            
            double sample_distance = descr_cost(
                model_pt,
                fixed_radius,
                fixed_theta,
                model_pt,
                fixed_radius,
                fixed_theta,
                model_red_grad_data,
                query_red_grad_data,
                model_green_grad_data,
                query_green_grad_data,
                model_blue_grad_data,
                query_blue_grad_data,
                model_sift_filter,
                model_sift_filter);

            trad_sift_distance += sample_distance;

            part_distances[curve_list_id]=part_distances[curve_list_id]+
                sample_distance;

            norm_factors[curve_list_id]=norm_factors[curve_list_id]+
                1.0;

            vcl_set<vcl_pair<double,double> > sift_samples;
                        
            compute_color_over_sift(
                model_sift_filter,
                model_sift_filter->width,
                model_sift_filter->height,
                model_pt.x(),
                model_pt.y(),
                fixed_radius,
                fixed_theta,
                sift_samples);

            vcl_vector<double> model_descr;
            vcl_vector<double> query_descr;
                        
            compute_color_region_hist(
                sift_samples,
                o1,
                o2,
                o3,
                query_descr,
                dbskfg_match_bag_of_fragments::DEFAULT);

            compute_color_region_hist(
                sift_samples,
                *model_channel1,
                *model_channel2,
                *model_channel3,
                model_descr,
                dbskfg_match_bag_of_fragments::DEFAULT);
                        
            vnl_vector<double> vec_model(model_descr.size(),0);
            vnl_vector<double> vec_query(query_descr.size(),0);

            for ( unsigned int m=0; m < model_descr.size(); ++m)
            {
                vec_model.put(m,model_descr[m]);
                vec_query.put(m,query_descr[m]);
            }

            vec_model.normalize(); vec_query.normalize();

            local_color_distance +=
                chi_squared_distance(vec_model,vec_query);

            if ( debug )
            {
                dist_map(x,y)=sample_distance;
            }
            
            index=index+1;
        }
    }

    // Normalize part distances
    for ( unsigned int i=0; i < part_distances.size() ; ++i)
    {
        part_distances[i]=part_distances[i]/norm_factors[i];

        if ( debug )
        {
            vcl_cout<<"Part "<<i<<" : "<<part_distances[i]<<vcl_endl;
        }
    }

    double part_norm_distance=part_distances.mean();

    if ( debug )
    {
        unsigned int g=model_tree->get_id();

        vcl_stringstream name;
        name<<output_dist_file_;
        if ( g < 10 )
        {
            name<<"_Model_000"<<g<<"_dist_map.txt";
        }
        else if ( g >= 10 && g < 100)
        {

            name<<"_Model_00"<<g<<"_dist_map.txt";
        }
        else
        {
            name<<"_Model_0"<<g<<"_dist_map.txt";
        }

        vcl_ofstream streamer(name.str().c_str());
        dist_map.print(streamer);
        streamer.close();
        
        dist_map.clear();
    }

    app_distance.first  = trad_sift_distance/index;
    app_distance.second = part_norm_distance;
    //app_distance.second = local_color_distance/index;

    vl_free(query_red_grad_data);
    vl_free(query_green_grad_data);
    vl_free(query_blue_grad_data);

    query_red_grad_data=0;
    query_green_grad_data=0;
    query_blue_grad_data=0;

    if ( debug )
    {
        vcl_cout<<"Color grad: "<<app_distance.first<<vcl_endl;
        vcl_cout<<"Color: "<<app_distance.second<<vcl_endl;
    }

    return app_distance;
}   

vcl_pair<double,double> 
dbskfg_match_bag_of_fragments::compute_common_frame_distance_part_qm(
    dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool flag,
    double width,
    bool debug)
{
    
    if ( part_distances_.count(model_tree->get_id()))
    {
        part_distances_[model_tree->get_id()].clear();
    }

    vcl_vector<vgl_polygon<double> > scurve_polys;
    query_tree->get_polygon_scurves(scurve_polys);

    vcl_map<double,unsigned int> area_mapping;

    for ( unsigned int d=0; d < scurve_polys.size() ; ++d)
    {
        vgl_polygon<double> query_poly=scurve_polys[d];
        
        area_mapping[vgl_area(query_poly)]=d;
    }


    if ( debug )
    {
        for ( unsigned int d=0; d < scurve_polys.size() ; ++d)
        {
            vcl_stringstream streamer;
            streamer<<"Query_poly_000"<<d<<".txt";

            vgl_polygon<double> query_poly=scurve_polys[d];

            {
            
                vcl_ofstream query_file(streamer.str().c_str());
                for (unsigned int s = 0; s < query_poly.num_sheets(); ++s)
                {
                    for (unsigned int p = 0; p < query_poly[s].size(); ++p)
                    {
                        query_file<<query_poly[s][p].x()
                                  <<","<<query_poly[s][p].y()<<vcl_endl;
                    }
                }
                query_file.close();
            }
        
        }
    }


    vcl_pair<double,double> app_distance(0.0,0.0);

    vl_sift_pix* query_red_grad_data  =query_tree->get_red_grad_data();    
    vl_sift_pix* query_green_grad_data=query_tree->get_green_grad_data();      
    vl_sift_pix* query_blue_grad_data =query_tree->get_blue_grad_data();

    VlSiftFilt* query_sift_filter=query_tree->get_sift_filter();

    vil_image_view<double>* model_channel1=model_tree->get_channel1();
    vil_image_view<double>* model_channel2=model_tree->get_channel2();
    vil_image_view<double>* model_channel3=model_tree->get_channel3();

    vil_image_view<double>* query_channel1=query_tree->get_channel1();
    vil_image_view<double>* query_channel2=query_tree->get_channel2();
    vil_image_view<double>* query_channel3=query_tree->get_channel3();

    vil_image_view<vil_rgb<vxl_byte> > temp;

    if ( debug )
    {
        temp.set_size(query_channel1->ni(),
                      query_channel1->nj());
        vil_rgb<vxl_byte> bg_col(255,255,255);
        temp.fill(bg_col);
    }

    int ni=query_channel1->ni();

    vgl_polygon<double> poly=query_fragments_polys_
        [query_tree->get_id()].second;
    
    vcl_map<vcl_pair<int,int>, vgl_point_2d<double> > bc_coords;
    
    vcl_map<vcl_pair<int,int>, int > point_to_curve_mapping;

    vil_image_view<double> o1(query_channel1->ni(),
                              query_channel2->nj());
    o1.fill(0);
    

    vil_image_view<double> o2(query_channel1->ni(),
                              query_channel2->nj());
    o2.fill(0);

    vil_image_view<double> o3(query_channel1->ni(),
                              query_channel2->nj());
    o3.fill(0);

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            vgl_point_2d<double> query_pt(x,y);

            vgl_point_2d<double> model_rt(0,0),query_rt(0,0);

            int curve_list_id(0);

            vgl_point_2d<double> mapping_pt=
                find_part_correspondences_qm(query_pt,
                                             curve_list1,
                                             curve_list2,
                                             map_list,
                                             model_rt,
                                             query_rt,
                                             curve_list_id,
                                             flag,
                                             width,
                                             model_tree
                                             ->get_scale_ratio(),
                                             query_tree
                                             ->get_scale_ratio());

            if ( mapping_pt.x() != -1 )
            {
                double xx=mapping_pt.x();
                double yy=mapping_pt.y();
                
                double red   = vil_bilin_interp_safe(*model_channel1,xx,yy);
                double green = vil_bilin_interp_safe(*model_channel2,xx,yy);
                double blue  = vil_bilin_interp_safe(*model_channel3,xx,yy);
                

                if ( width )
                {
                    if ( debug )
                    {
                        temp(ni-1-x,y)=vil_rgb<vxl_byte>(red,green,blue);
                    }

                    o1(ni-1-x,y)=red;
                    o2(ni-1-x,y)=green;
                    o3(ni-1-x,y)=blue;
                }
                else
                {
                    if ( debug )
                    {
                        temp(x,y)=vil_rgb<vxl_byte>(red,green,blue);
                    }

                    o1(x,y)=red;
                    o2(x,y)=green;
                    o3(x,y)=blue;

                }

                vcl_pair<int,int> key(x,y);
                bc_coords[key]=model_rt;

                point_to_curve_mapping[key]=curve_list_id;
            }

        }
    }

    

    if ( debug )
    {
        vcl_stringstream name;
        name<<"Model_"<<model_tree->get_id()<<"_vs_Query_"<<query_tree->get_id()
            <<"_warp.png";

        vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);

        vil_save_image_resource(out_img, 
                                name.str().c_str()); 
    }

    vl_sift_pix* model_red_grad_data(0);
    vl_sift_pix* model_green_grad_data(0);
    vl_sift_pix* model_blue_grad_data(0);
    
    compute_grad_color_maps(o1,
                            &model_red_grad_data,
                            poly,
                            false);
    
    compute_grad_color_maps(o2,
                            &model_green_grad_data,
                            poly,
                            false);
    
    compute_grad_color_maps(o3,
                            &model_blue_grad_data,
                            poly,
                            false);

    double fixed_radius=16;
    double fixed_theta=0.0;

    unsigned int stride=3;

    vnl_vector<double> middle_part_distances(curve_list1.size(),0.0);
    vcl_vector<double> middle_norm_factors(curve_list1.size(),0.0);
    vnl_vector<double> sg_part_distances(scurve_polys.size(),0.0);
    vnl_vector<double> sg_part_color_distances(scurve_polys.size(),0.0);

    vnl_matrix<double> dist_map;

    if ( debug )
    {
        dist_map.set_size(temp.ni(),temp.nj());
        dist_map.fill(0.0);
        stride=1;
    }

    vcl_map<double,unsigned int>::iterator ait;

    for ( ait=area_mapping.begin() ; ait != area_mapping.end() ; ++ait)
    {
        unsigned int d = (*ait).second;

        vgl_polygon<double> query_poly=scurve_polys[d];

        vgl_polygon_scan_iterator<double> q_psi(query_poly, false);  

        int norm_index=0;
        double trad_sift_distance=0.0;

        vcl_set< vcl_pair<double,double> > part_samples;

        for (q_psi.reset(); q_psi.next(); ) 
        {
            int y = q_psi.scany();
            for (int x = q_psi.startx(); x <= q_psi.endx(); x=x+stride) 
            {

                vgl_point_2d<double> model_pt(x,y);
            
                if ( width )
                {
                    model_pt.set(ni-1-x,y);
                }

                part_samples.insert(vcl_make_pair( (double) x,
                                                   (double) y));

                double sample_distance = descr_cost(
                    model_pt,
                    fixed_radius,
                    fixed_theta,
                    model_pt,
                    fixed_radius,
                    fixed_theta,
                    model_red_grad_data,
                    query_red_grad_data,
                    model_green_grad_data,
                    query_green_grad_data,
                    model_blue_grad_data,
                    query_blue_grad_data,
                    query_sift_filter,
                    query_sift_filter);

                trad_sift_distance += sample_distance;

                if ( point_to_curve_mapping.count(vcl_make_pair(x,y)))
                {
                    vcl_pair<int,int> key(x,y);
                    int curve_list_id=point_to_curve_mapping[key];

                    middle_part_distances[curve_list_id]=
                        middle_part_distances[curve_list_id]+
                        sample_distance;
                    
                    middle_norm_factors[curve_list_id]=
                        middle_norm_factors[curve_list_id]+
                        1.0;

                }

                norm_index=norm_index+1;

                if ( debug )
                {
                    dist_map(x,y)=sample_distance;
                }
            }
        
            for ( unsigned int k=0; k < stride-1 ; ++k)
            {
                q_psi.next();
            }
        }
        
        
        double temp_distance=trad_sift_distance;
        if ( norm_index > 1)
        {
            temp_distance=temp_distance/norm_index;
        }
        else
        {
            temp_distance=1000;
        }

        sg_part_distances[d]=temp_distance;

        part_distances_[model_tree->get_id()].push_back(temp_distance);

        
        vcl_vector<double> model_descr;
        vcl_vector<double> query_descr;
        
        compute_color_region_hist(
            part_samples,
            o1,
            o2,
            o3,
            model_descr,
            dbskfg_match_bag_of_fragments::DEFAULT);

        compute_color_region_hist(
            part_samples,
            *query_channel1,
            *query_channel2,
            *query_channel3,
            query_descr,
            dbskfg_match_bag_of_fragments::DEFAULT);
                        
        vnl_vector<double> vec_model(model_descr.size(),0);
        vnl_vector<double> vec_query(query_descr.size(),0);
            
        for ( unsigned int m=0; m < model_descr.size(); ++m)
        {
            vec_model.put(m,model_descr[m]);
            vec_query.put(m,query_descr[m]);
        }

        vec_model.normalize(); vec_query.normalize();

        double local_color_distance =
            chi_squared_distance(vec_model,vec_query);
        
        sg_part_color_distances[d]=temp_distance;

        if ( debug )
        {
            vcl_set< vcl_pair<double,double> >::iterator it;
            for ( it = part_samples.begin() ; it != part_samples.end();
                  ++it)
            {
                dist_map((*it).first,(*it).second)=sg_part_distances[d];

            }

            vcl_cout<<"SG Part color grad "<<d
                    <<" : "<<sg_part_distances[d]<<" samples: "
                    <<norm_index<<vcl_endl;
            vcl_cout<<"SG Part color "<<d
                    <<" : "<<sg_part_color_distances[d]<<" samples: "
                    <<norm_index<<vcl_endl;

        }
    }

    // Normalize part distances
    for ( unsigned int i=0; i < middle_part_distances.size() ; ++i)
    {
        middle_part_distances[i]=middle_part_distances[i]/
            middle_norm_factors[i];

        if ( debug )
        {
            vcl_cout<<"MS Part "<<i<<" : "<<middle_part_distances[i]<<vcl_endl;
        }
    }

    double middle_part_norm_distance=middle_part_distances.mean();

    if ( debug )
    {
        vcl_stringstream name;
        name<<"Model_"<<model_tree->get_id()<<"_vs_Query_"<<query_tree->get_id()
            <<"_dist_map.txt";
        
        vcl_ofstream streamer(name.str().c_str());
        dist_map.print(streamer);
        streamer.close();
        
        dist_map.clear();
    }

    app_distance.first  = middle_part_norm_distance;
    app_distance.second = sg_part_distances.mean();

    vl_free(model_red_grad_data);
    vl_free(model_green_grad_data);
    vl_free(model_blue_grad_data);

    model_red_grad_data=0;
    model_green_grad_data=0;
    model_blue_grad_data=0;

    if ( debug )
    {
        vcl_cout<<"Distance MS Parts: "<<app_distance.first<<vcl_endl;
        vcl_cout<<"Distance SG Parts: "<<app_distance.second<<vcl_endl;
    }

    return app_distance;
}   


vcl_pair<double,double> 
dbskfg_match_bag_of_fragments::compute_common_frame_distance_dsift_qm(
    dbskfg_cgraph_directed_tree_sptr& model_tree,
    dbskfg_cgraph_directed_tree_sptr& query_tree,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    bool flag,
    double width,
    bool debug)
{
    
    vcl_pair<double,double> app_distance(0.0,0.0);


    vil_image_view<double>* query_channel1=query_tree->get_channel1();
    vil_image_view<double>* query_channel2=query_tree->get_channel2();
    vil_image_view<double>* query_channel3=query_tree->get_channel3();

    vil_image_view<double>* model_channel1=model_tree->get_channel1();
    vil_image_view<double>* model_channel2=model_tree->get_channel2();
    vil_image_view<double>* model_channel3=model_tree->get_channel3();

    vil_image_view<vil_rgb<float> > temp(query_tree->get_channel1()->ni(),
                                         query_tree->get_channel1()->nj());
    vil_rgb<float> bg_col(0,0,0);
    temp.fill(bg_col);

    int ni=query_tree->get_channel1()->ni();

    vgl_polygon<double> poly=query_fragments_polys_
        [query_tree->get_id()].second;
    
    vcl_map<vcl_pair<int,int>, vgl_point_2d<double> > bc_coords;

    vcl_set<vcl_pair<int,int> > in_bounds;

    vgl_box_2d<double> box;

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            vgl_point_2d<double> query_pt(x,y);

            vgl_point_2d<double> model_rt(0,0),query_rt(0,0);

            int curve_list_id(0);

            vgl_point_2d<double> mapping_pt=
                find_part_correspondences_qm(query_pt,
                                             curve_list1,
                                             curve_list2,
                                             map_list,
                                             model_rt,
                                             query_rt,
                                             curve_list_id,
                                             flag,
                                             width,
                                             model_tree
                                             ->get_scale_ratio(),
                                             query_tree
                                             ->get_scale_ratio());

            if ( mapping_pt.x() != -1 )
            {
                double xx=mapping_pt.x();
                double yy=mapping_pt.y();
                
                float red   = vil_bilin_interp_safe(*model_channel1,xx,yy);
                float green = vil_bilin_interp_safe(*model_channel2,xx,yy);
                float blue  = vil_bilin_interp_safe(*model_channel3,xx,yy);
                

                if ( width )
                {
                    temp(ni-1-x,y)=vil_rgb<float>(red,green,blue);
                }
                else
                {
                    temp(x,y)=vil_rgb<float>(red,green,blue);
                }

                in_bounds.insert(vcl_make_pair(x,y));

                vcl_pair<int,int> key(x,y);
                bc_coords[key]=model_rt;

                box.add(vgl_point_2d<double>(x,y));
            }

        }
    }

    vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);


    int binSize =16;
    int stride  =8;

    vnl_matrix<double> dist_map;

    if ( debug )
    {
        dist_map.set_size(temp.ni(),temp.nj());
        dist_map.fill(0.0);
        stride=1;
    }

    vcl_vector< vnl_vector<vl_sift_pix> > model_descrs;
    vcl_vector< vcl_pair<int,int> > model_keypoints;
   
    compute_dsift_image(
        out_img,
        box,
        model_descrs,
        model_keypoints,
        stride,
        binSize);


    vcl_vector< vnl_vector<vl_sift_pix> > query_descrs;
    vcl_vector< vcl_pair<int,int> > query_keypoints;
   
    compute_dsift_image(
        *query_channel1,
        *query_channel2,
        *query_channel3,
        box,
        query_descrs,
        query_keypoints,
        stride,
        binSize);

    VlFloatVectorComparisonFunction Chi2_distance =    
      vl_get_vector_comparison_function_f (VlDistanceChi2) ;

    double trad_sift_distance(0.0);
    for ( unsigned int k=0; k < model_keypoints.size() ; ++k)
    {

        vcl_pair<int,int> keypoint=model_keypoints[k];

        vnl_vector<vl_sift_pix> descr1=model_descrs[k];
        vnl_vector<vl_sift_pix> descr2=query_descrs[k];
        
        
        vl_sift_pix result_final[1];
        
        vl_eval_vector_comparison_on_all_pairs_f(result_final,
                                                 descr1.size(),
                                                 descr1.data_block(),
                                                 1,
                                                 descr2.data_block(),
                                                 1,
                                                 Chi2_distance);
        double dist=(0.5)*result_final[0];
        
        trad_sift_distance += dist;
        
        if ( debug )
        {
            if ( in_bounds.count(keypoint))
            {
                dist_map(keypoint.first,keypoint.second)=dist;
            }
        }
        
    
    }

    if ( debug )
    {
        vcl_stringstream name;
        name<<"Model_"<<model_tree->get_id()<<"_vs_Query_"<<query_tree->get_id()
            <<"_dist_map.txt";
        
        vcl_ofstream streamer(name.str().c_str());
        dist_map.print(streamer);
        streamer.close();
        
        dist_map.clear();
    }

    app_distance.first  = trad_sift_distance/model_keypoints.size();
    app_distance.second = trad_sift_distance;

    if ( debug )
    {
        vcl_cout<<app_distance.first<<" "<<app_distance.second<<vcl_endl;
    }
    return app_distance;
}   



void dbskfg_match_bag_of_fragments::compute_dsift_image(
    vil_image_resource_sptr& input_image,
    vgl_box_2d<double>& box,
    vcl_vector<vnl_vector<vl_sift_pix> >& descrs,
    vcl_vector<vcl_pair<int,int> >& keypoints,
    int step,
    int binSize)
{

    vil_image_view<float> image = input_image->get_view();

    vil_image_view<float> red   = vil_plane(image,0);
    vil_image_view<float> green = vil_plane(image,1);
    vil_image_view<float> blue  = vil_plane(image,2);
    
    vil_image_view<float> chan_1,chan_2,chan_3;

    vil_convert_cast(red,chan_1);
    vil_convert_cast(green,chan_2);
    vil_convert_cast(blue,chan_3);

    VlDsiftFilter* filter= vl_dsift_new_basic(image.ni(),
                                              image.nj(),
                                              step,
                                              binSize);

    vl_dsift_set_bounds(filter,
                        box.min_x(),
                        box.min_y(),
                        box.max_x(),
                        box.max_y());

  
    // Process channel 1 
    {
        vl_dsift_process(filter,
                         chan_1.top_left_ptr());
        int numb_descriptors = vl_dsift_get_keypoint_num(filter);        
        float const* sift = vl_dsift_get_descriptors(filter);

        int offset = 0 ;
        int ptr_index=0; 

        for ( unsigned int i=0; i < numb_descriptors ; ++i)
        {
            vnl_vector<vl_sift_pix> vec(384,0);

            int stop=128*i+128;
            unsigned int v=0;
            for ( ; ptr_index < stop ; ++ptr_index)
            {
                vec.put(v+offset,sift[ptr_index]);
                ++v;
            }
            descrs.push_back(vec);
            
        }


        VlDsiftKeypoint const* kps = vl_dsift_get_keypoints(filter);
        
        for ( unsigned int i=0; i < numb_descriptors ; ++i)
        {
            vcl_pair<int,int> key(kps[i].x,kps[i].y);
            keypoints.push_back(key);
        }

    }

    // Process channel 2
    {
        // Process channel 2 and get descriptors
        vl_dsift_process(filter,
                         chan_2.top_left_ptr());        
        int numb_descriptors = vl_dsift_get_keypoint_num(filter);
        float const* sift = vl_dsift_get_descriptors(filter);

        int offset   = 128;
        int ptr_index= 0; 

        for ( unsigned int i=0; i < numb_descriptors ; ++i)
        {
            vnl_vector<vl_sift_pix> vec=descrs[i];

            int stop=128*i+128;
            unsigned int v=0;
            for ( ; ptr_index < stop ; ++ptr_index)
            {
                vec.put(v+offset,sift[ptr_index]);
                ++v;
            }
            
        }

    }


    // Process channel 3 
    {
        // Process and get sift descriptors
        vl_dsift_process(filter,
                         chan_3.top_left_ptr());
        int numb_descriptors = vl_dsift_get_keypoint_num(filter);
        float const* sift = vl_dsift_get_descriptors(filter);

        int offset    = 256;
        int ptr_index = 0; 

        for ( unsigned int i=0; i < numb_descriptors ; ++i)
        {
            vnl_vector<vl_sift_pix> vec=descrs[i];
            
            int stop=128*i+128;
            unsigned int v=0;
            for ( ; ptr_index < stop ; ++ptr_index)
            {
                vec.put(v+offset,sift[ptr_index]);
                ++v;
            }
            
        }

    }

    vl_dsift_delete(filter);

}

void dbskfg_match_bag_of_fragments::compute_dsift_image(
    vil_image_view<double>& channel_1,
    vil_image_view<double>& channel_2,
    vil_image_view<double>& channel_3,
    vgl_box_2d<double>& box,
    vcl_vector<vnl_vector<vl_sift_pix> >& descrs,
    vcl_vector<vcl_pair<int,int> >& keypoints,
    int step,
    int binSize)
{
    
    vil_image_view<float> chan_1,chan_2,chan_3;

    vil_convert_cast(channel_1,chan_1);
    vil_convert_cast(channel_2,chan_2);
    vil_convert_cast(channel_3,chan_3);

    VlDsiftFilter* filter= vl_dsift_new_basic(chan_1.ni(),
                                              chan_2.nj(),
                                              step,
                                              binSize);

    vl_dsift_set_bounds(filter,
                        box.min_x(),
                        box.min_y(),
                        box.max_x(),
                        box.max_y());

  
    // Process channel 1 
    {
        vl_dsift_process(filter,
                         chan_1.top_left_ptr());
        int numb_descriptors = vl_dsift_get_keypoint_num(filter);        
        float const* sift = vl_dsift_get_descriptors(filter);

        int offset = 0 ;
        int ptr_index=0; 

        for ( unsigned int i=0; i < numb_descriptors ; ++i)
        {
            vnl_vector<vl_sift_pix> vec(384,0);

            int stop=128*i+128;
            unsigned int v=0;
            for ( ; ptr_index < stop ; ++ptr_index)
            {
                vec.put(v+offset,sift[ptr_index]);
                ++v;
            }
            descrs.push_back(vec);
            
        }


        VlDsiftKeypoint const* kps = vl_dsift_get_keypoints(filter);
        
        for ( unsigned int i=0; i < numb_descriptors ; ++i)
        {
            vcl_pair<int,int> key(kps[i].x,kps[i].y);
            keypoints.push_back(key);
        }

    }

    // Process channel 2
    {
        // Process channel 2 and get descriptors
        vl_dsift_process(filter,
                         chan_2.top_left_ptr());        
        int numb_descriptors = vl_dsift_get_keypoint_num(filter);
        float const* sift = vl_dsift_get_descriptors(filter);

        int offset   = 128;
        int ptr_index= 0; 

        for ( unsigned int i=0; i < numb_descriptors ; ++i)
        {
            vnl_vector<vl_sift_pix> vec=descrs[i];

            int stop=128*i+128;
            unsigned int v=0;
            for ( ; ptr_index < stop ; ++ptr_index)
            {
                vec.put(v+offset,sift[ptr_index]);
                ++v;
            }
            
        }

    }


    // Process channel 3 
    {
        // Process and get sift descriptors
        vl_dsift_process(filter,
                         chan_3.top_left_ptr());
        int numb_descriptors = vl_dsift_get_keypoint_num(filter);
        float const* sift = vl_dsift_get_descriptors(filter);

        int offset    = 256;
        int ptr_index = 0; 

        for ( unsigned int i=0; i < numb_descriptors ; ++i)
        {
            vnl_vector<vl_sift_pix> vec=descrs[i];
            
            int stop=128*i+128;
            unsigned int v=0;
            for ( ; ptr_index < stop ; ++ptr_index)
            {
                vec.put(v+offset,sift[ptr_index]);
                ++v;
            }
            
        }

    }

    vl_dsift_delete(filter);

}

vgl_point_2d<double> dbskfg_match_bag_of_fragments::find_part_correspondences(
    vgl_point_2d<double> query_pt,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vgl_point_2d<double>& rt_model,
    vgl_point_2d<double>& rt_query,
    int& curve_list_id,
    bool flag,
    double width,
    double model_scale_ratio,
    double query_scale_ratio)
{

    vgl_point_2d<double> mapping_pt;

    vgl_point_2d<double> ps1(query_pt);
    
    ps1.set(ps1.x()*model_scale_ratio,
            ps1.y()*model_scale_ratio);
    
    if ( !flag )
    {
        bool in_part=false;
        unsigned int c=0;
        for ( ; c < curve_list1.size() ; ++c)
        {
            vgl_polygon<double> poly(1);
            dbskr_scurve_sptr sc1=curve_list1[c];
            sc1->get_polygon(poly);

            if ( poly.contains(query_pt.x(),query_pt.y()))
            {
                in_part=true;
                break;
            }
            
        }
        
        if ( !in_part)
        {
            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        dbskr_scurve_sptr model_curve=curve_list1[c];
        dbskr_scurve_sptr query_curve=curve_list2[c];

        curve_list_id=c;

        // Find point in model curve
        vgl_point_2d<double> int_pt;
        bool found=model_curve->intrinsinc_pt(ps1,int_pt);

        if ( !found )
        {
           
            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        vcl_vector<vcl_pair<int,int> > curve_map=map_list[c];

        double index=int_pt.x();

        double index_ratio=0.0;

        unsigned int v=0; 

        int model_int_diff(0);
        double int_diff(0.0);

        for ( ; v < curve_map.size()-1 ; ++v)
        {

            int lower=curve_map[v].first;
            int upper=curve_map[v+1].first;

            if ( upper < lower )
            {
                int temp=upper;
                upper=lower;
                lower=temp;
            }
            
            if ( index >= lower &&
                 index < upper )
            {
                model_int_diff=upper-lower;
                int_diff=index-lower;
                break;
                
            }

            
        }

        
        // Find mapping point
        int start_index=curve_map[v].second;
        int stop_index=curve_map[v+1].second;

        if ( stop_index < start_index)
        {
            int temp=stop_index;
            stop_index=start_index;
            start_index=temp;
        }

        int query_int_diff=stop_index-start_index;

        double s_map(0.0);

        if ( query_int_diff == 0 )
        {
            s_map=start_index;

        }
        else
        {
            double ratio=((double) query_int_diff)/((double) model_int_diff);
            s_map = start_index+int_diff*ratio;

        }

        double t_rad_model = model_curve->interp_radius(int_pt.x());
        double t_rad_query = query_curve->interp_radius(s_map);

        double t_map = int_pt.y()*(t_rad_query/t_rad_model);

        mapping_pt = query_curve->fragment_pt(s_map,
                                              t_map);

        rt_model.set(int_pt.y(),
                     model_curve->interp_theta(int_pt.x()));

        rt_query.set(t_map,
                     query_curve->interp_theta(s_map));

        
    }
    else
    {
        bool in_part=false;
        unsigned int c=0;
        for ( ; c < curve_list2.size() ; ++c)
        {
            vgl_polygon<double> poly(1);
            dbskr_scurve_sptr sc1=curve_list2[c];
            sc1->get_polygon(poly);

            if ( poly.contains(query_pt.x(),query_pt.y()))
            {
                in_part=true;
                break;
            }
            
        }

        if ( !in_part )
        {

            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        dbskr_scurve_sptr model_curve=curve_list2[c];
        dbskr_scurve_sptr query_curve=curve_list1[c];

        curve_list_id=c;

        // Find point in model curve
        vgl_point_2d<double> int_pt;
        bool found = model_curve->intrinsinc_pt(ps1,int_pt);

        if ( !found )
        {
            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        vcl_vector<vcl_pair<int,int> > curve_map=map_list[c];

        double index=int_pt.x();

        double index_ratio=0.0;

        unsigned int v=0; 

        int model_int_diff(0);
        double int_diff(0.0);

        for ( ; v < curve_map.size()-1 ; ++v)
        {

            int lower=curve_map[v].second;
            int upper=curve_map[v+1].second;

            if ( upper < lower )
            {
                int temp=upper;
                upper=lower;
                lower=temp;
            }
            
            if ( index >= lower &&
                 index < upper )
            {
                model_int_diff=upper-lower;
                int_diff=index-lower;
                break;
                
            }

            
        }

        
        // Find mapping point
        int start_index=curve_map[v].first;
        int stop_index=curve_map[v+1].first;

        if ( stop_index < start_index)
        {
            int temp=stop_index;
            stop_index=start_index;
            start_index=temp;
        }

        int query_int_diff=stop_index-start_index;

        double s_map(0.0);

        if ( query_int_diff == 0 )
        {
            s_map=start_index;

        }
        else
        {
            double ratio=((double) query_int_diff)/((double) model_int_diff);
            s_map = start_index+int_diff*ratio;

        }

        double t_rad_model = model_curve->interp_radius(int_pt.x());
        double t_rad_query = query_curve->interp_radius(s_map);

        double t_map = int_pt.y()*(t_rad_query/t_rad_model);

        mapping_pt = query_curve->fragment_pt(s_map,
                                              t_map);

        rt_model.set(int_pt.y(),
                     model_curve->interp_theta(int_pt.x()));

        rt_query.set(t_map,
                     query_curve->interp_theta(s_map));


    }


    mapping_pt.set(vcl_fabs(width-(mapping_pt.x()/query_scale_ratio)),
                   mapping_pt.y()/query_scale_ratio);

    return mapping_pt;
}



vgl_point_2d<double> dbskfg_match_bag_of_fragments::
find_part_correspondences_qm(
    vgl_point_2d<double> query_pt,
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vgl_point_2d<double>& rt_model,
    vgl_point_2d<double>& rt_query,
    int& curve_list_id,
    bool flag,
    double width,
    double model_scale_ratio,
    double query_scale_ratio)
{

    vgl_point_2d<double> mapping_pt;

    vgl_point_2d<double> ps1(query_pt);
    
    if ( width > 0 )
    {
        ps1.set((width-ps1.x())*query_scale_ratio,
                ps1.y()*query_scale_ratio);
    }
    else
    {
        ps1.set(ps1.x()*query_scale_ratio,ps1.y()*query_scale_ratio);
    }

    if ( !flag )
    {
        bool in_part=false;
        unsigned int c=0;
        for ( ; c < curve_list2.size() ; ++c)
        {
            vgl_polygon<double> poly(1);
            dbskr_scurve_sptr sc1=curve_list2[c];
            sc1->get_polygon(poly,width);

            if ( poly.contains(query_pt.x(),query_pt.y()))
            {
                in_part=true;
                break;
            }
            
        }
        
        if ( !in_part)
        {
            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        dbskr_scurve_sptr model_curve=curve_list2[c];
        dbskr_scurve_sptr query_curve=curve_list1[c];

        curve_list_id=c;

        // Find point in model curve
        vgl_point_2d<double> int_pt;
        bool found=model_curve->intrinsinc_pt(ps1,int_pt);

        if ( !found )
        {

            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        vcl_vector<vcl_pair<int,int> > curve_map=map_list[c];

        double index=int_pt.x();

        double index_ratio=0.0;

        unsigned int v=0; 

        int model_int_diff(0);
        double int_diff(0.0);

        for ( ; v < curve_map.size()-1 ; ++v)
        {

            int lower=curve_map[v].second;
            int upper=curve_map[v+1].second;

            if ( upper < lower )
            {
                int temp=upper;
                upper=lower;
                lower=temp;
            }
            
            if ( index >= lower &&
                 index < upper )
            {
                model_int_diff=upper-lower;
                int_diff=index-lower;
                break;
                
            }

            
        }

        
        // Find mapping point
        int start_index=curve_map[v].first;
        int stop_index=curve_map[v+1].first;

        if ( stop_index < start_index)
        {
            int temp=stop_index;
            stop_index=start_index;
            start_index=temp;
        }

        int query_int_diff=stop_index-start_index;

        double s_map(0.0);

        if ( query_int_diff == 0 )
        {
            s_map=start_index;

        }
        else
        {
            double ratio=((double) query_int_diff)/((double) model_int_diff);
            s_map = start_index+int_diff*ratio;

        }

        double t_rad_model = model_curve->interp_radius(int_pt.x());
        double t_rad_query = query_curve->interp_radius(s_map);

        double t_map = int_pt.y()*(t_rad_query/t_rad_model);

        mapping_pt = query_curve->fragment_pt(s_map,
                                              t_map);

        rt_model.set(t_rad_model-vcl_fabs(int_pt.y()),
                     model_curve->interp_theta(int_pt.x()));

        rt_query.set(t_rad_query-vcl_fabs(t_map),
                     query_curve->interp_theta(s_map));

        
    }
    else
    {
        bool in_part=false;
        unsigned int c=0;
        for ( ; c < curve_list1.size() ; ++c)
        {
            vgl_polygon<double> poly(1);
            dbskr_scurve_sptr sc1=curve_list1[c];
            sc1->get_polygon(poly,width);

            if ( poly.contains(query_pt.x(),query_pt.y()))
            {
                in_part=true;
                break;
            }
            
        }

        if ( !in_part )
        {

            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        dbskr_scurve_sptr model_curve=curve_list1[c];
        dbskr_scurve_sptr query_curve=curve_list2[c];

        curve_list_id=c;

        // Find point in model curve
        vgl_point_2d<double> int_pt;
        bool found = model_curve->intrinsinc_pt(ps1,int_pt);

        if ( !found )
        {
            mapping_pt.set(-1.0,-1.0);
            return mapping_pt;
        }

        vcl_vector<vcl_pair<int,int> > curve_map=map_list[c];

        double index=int_pt.x();

        double index_ratio=0.0;

        unsigned int v=0; 

        int model_int_diff(0);
        double int_diff(0.0);

        for ( ; v < curve_map.size()-1 ; ++v)
        {

            int lower=curve_map[v].first;
            int upper=curve_map[v+1].first;

            if ( upper < lower )
            {
                int temp=upper;
                upper=lower;
                lower=temp;
            }
            
            if ( index >= lower &&
                 index < upper )
            {
                model_int_diff=upper-lower;
                int_diff=index-lower;
                break;
                
            }

            
        }

        
        // Find mapping point
        int start_index=curve_map[v].second;
        int stop_index=curve_map[v+1].second;

        if ( stop_index < start_index)
        {
            int temp=stop_index;
            stop_index=start_index;
            start_index=temp;
        }

        int query_int_diff=stop_index-start_index;

        double s_map(0.0);

        if ( query_int_diff == 0 )
        {
            s_map=start_index;

        }
        else
        {
            double ratio=((double) query_int_diff)/((double) model_int_diff);
            s_map = start_index+int_diff*ratio;

        }

        double t_rad_model = model_curve->interp_radius(int_pt.x());
        double t_rad_query = query_curve->interp_radius(s_map);

        double t_map = int_pt.y()*(t_rad_query/t_rad_model);

        mapping_pt = query_curve->fragment_pt(s_map,
                                              t_map);

        rt_model.set(t_rad_model-vcl_fabs(int_pt.y()),
                     model_curve->interp_theta(int_pt.x()));

        rt_query.set(t_rad_query-vcl_fabs(t_map),
                     query_curve->interp_theta(s_map));


    }


    mapping_pt.set(mapping_pt.x()/model_scale_ratio,
                   mapping_pt.y()/model_scale_ratio);

    return mapping_pt;
}

vcl_pair<double,double> dbskfg_match_bag_of_fragments::
compute_dense_rgb_sift_cost(
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_vector< pathtable_key >& path_map,
    vcl_vector<double>& dart_distances,
    vil_image_view<double>& model_channel_1,
    vil_image_view<double>& model_channel_2,
    vil_image_view<double>& model_channel_3,
    vil_image_view<double>& query_channel_1,
    vil_image_view<double>& query_channel_2,
    vil_image_view<double>& query_channel_3,
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
    double query_scale_ratio,
    double model_sift_scale,
    double query_sift_scale,
    vcl_string prefix)
{
 
    // if ( width > 0 )
    // {
    //     vcl_ofstream red_stream("query_red_data.txt");
    //     vcl_ofstream green_stream("query_green_data.txt");
    //     vcl_ofstream blue_stream("query_blue_data.txt");


    //     for ( unsigned int i=0; i < query_channel_1.ni()*
    //               query_channel_1.nj(); ++i)
    //     {
    //         red_stream<<*query_red_grad_data<<" "<<
    //             *(query_red_grad_data+1)<<vcl_endl;
    //         query_red_grad_data+=2;

    //         green_stream<<*query_green_grad_data<<" "<<
    //             *(query_green_grad_data+1)<<vcl_endl;
    //         query_green_grad_data+=2;

    //         blue_stream<<*query_blue_grad_data<<" "<<
    //             *(query_blue_grad_data+1)<<vcl_endl;
    //         query_blue_grad_data+=2;

    //     }

    //     red_stream.close();
    //     green_stream.close();
    //     blue_stream.close();
        
    // }
    bool variable=true;

    double sift_diff= 0.0;
    double color_diff=0.0;
    double color_hist_diff=0.0;

    double arclength_shock_curve1=0.0;
    double arclength_shock_curve2=0.0;

    double splice_cost_shock_curve1=0.0;
    double splice_cost_shock_curve2=0.0;

    double context=0.0;

    unsigned int overall_index=0;
    // Get matching pairs
    for (unsigned i = 0; i < map_list.size(); i++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];

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

        double local_distance=0.0;
        double local_color_distance=0.0;

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
        
        double step_size=1.0;
        unsigned int num_steps=0.0;

        for (unsigned j = 0; j < map_list[i].size(); ++j) 
        {
            vcl_pair<int, int> cor = map_list[i][j];
            
            // Compute sift for both images

            // Shock Point 1 from Model
            double radius_ps1        = sc1->time(cor.first);
            double theta_ps1         = sc1->theta(cor.first);

            // Shock Point 2 from Query
            double radius_ps2        = sc2->time(cor.second);
            double theta_ps2         = sc2->theta(cor.second);

            double ratio=1.0;
            double R1=radius_ps1;
            double R2=radius_ps2;

            ratio=(R2/R1);
                

            double r1 = 0;

            while ( r1 <= R1 )
            {
                double r2=r1*ratio;
                for ( unsigned int p=0; p < 2 ; ++p )
                {
                    vgl_point_2d<double> ps1;
                    vgl_point_2d<double> ps2;

                    if ( p ==  0 )
                    {
                        if ( r1 == 0 )
                        {
                            ps1=sc1->sh_pt(cor.first);
                            ps2=sc2->sh_pt(cor.second);
                        }
                        else
                        {
                            ps1=sc1->fragment_pt(cor.first,r1);
                            ps2=sc2->fragment_pt(cor.second,r2);
                        }
                    }
                    else
                    {
                        if ( r1==0 )
                        {
                            ps1=sc1->sh_pt(cor.first);
                            ps2=sc2->sh_pt(cor.second);
                        }
                        else
                        {
                            ps1=sc1->fragment_pt(cor.first,-1.0*r1);
                            ps2=sc2->fragment_pt(cor.second,-1.0*r2);
                        }
                    }
                    overall_index++;
                    num_steps++;

                    if ( !flag )
                    {

                        ps1.set(ps1.x()/model_scale_ratio,
                                ps1.y()/model_scale_ratio);
                        ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                                ps2.y()/query_scale_ratio);
                    
                        vgl_point_2d<double> ps1_actual=ps1;
                        vgl_point_2d<double> ps2_actual=ps2;

                        if ( width > 0 )
                        {
                            ps2.set(query_channel_1.ni()-1-ps2.x(),
                                    ps2.y());
                        }

                        double model_radius=((R1-r1)/model_scale_ratio)/2.0;
                        double query_radius=((R2-r2)/query_scale_ratio)/2.0;

                        if ( !variable )
                        {
                            model_radius=(model_sift_scale
                                          /model_scale_ratio)/2.0;
                            query_radius=(query_sift_scale
                                          /query_scale_ratio)/2.0;
                    
                        }

                        model_radius+=context;
                        query_radius+=context;

                        // dbskfg_compute_sift sift_process(
                        //     ps1,
                        //     model_radius*2.0,
                        //     theta_ps1,
                        //     model_scale_ratio,
                        //     model_sift_filter,
                        //     ps2,
                        //     query_radius*2.0,
                        //     theta_ps2,
                        //     query_scale_ratio,
                        //     query_sift_filter,
                        //     model_red_grad_data,
                        //     query_red_grad_data,
                        //     model_green_grad_data,
                        //     query_green_grad_data,
                        //     model_blue_grad_data,
                        //     query_blue_grad_data);

                        // local_distance += sift_process.distance();

                        local_distance += descr_cost(
                            ps1,
                            model_radius,
                            theta_ps1,
                            ps2,
                            query_radius,
                            theta_ps2,
                            model_red_grad_data,
                            query_red_grad_data,
                            model_green_grad_data,
                            query_green_grad_data,
                            model_blue_grad_data,
                            query_blue_grad_data,
                            model_sift_filter,
                            query_sift_filter);

                        // local_distance += descr_cost_enriched_sift(
                        //     ps1,
                        //     model_radius,
                        //     theta_ps1,
                        //     ps2,
                        //     query_radius,
                        //     theta_ps2,
                        //     model_channel_1,
                        //     model_channel_2,
                        //     model_channel_3,
                        //     query_channel_1,
                        //     query_channel_2,
                        //     query_channel_3,
                        //     model_red_grad_data,
                        //     query_red_grad_data,
                        //     model_green_grad_data,
                        //     query_green_grad_data,
                        //     model_blue_grad_data,
                        //     query_blue_grad_data,
                        //     model_sift_filter,
                        //     query_sift_filter);

                        // local_color_distance += LAB_distance(
                        //     ps1,
                        //     ps2,
                        //     model_channel_1,
                        //     model_channel_2,
                        //     model_channel_3,
                        //     query_channel_1,
                        //     query_channel_2,
                        //     query_channel_3);

                  
                        // vcl_set<vcl_pair<double,double> > model_sift_samples;
                        // vcl_set<vcl_pair<double,double> > query_sift_samples;
                        
                        // compute_color_over_sift(
                        //     model_sift_filter,
                        //     model_sift_filter->width,
                        //     model_sift_filter->height,
                        //     ps1.x(),
                        //     ps1.y(),
                        //     model_radius,
                        //     theta_ps1,
                        //     model_sift_samples);

                        // compute_color_over_sift(
                        //     query_sift_filter,
                        //     query_sift_filter->width,
                        //     query_sift_filter->height,
                        //     ps2.x(),
                        //     ps2.y(),
                        //     query_radius,
                        //     theta_ps2,
                        //     query_sift_samples);

                        // vcl_vector<double> model_descr;
                        // vcl_vector<double> query_descr;
                        
                        // compute_color_region_hist(
                        //     model_sift_samples,
                        //     model_channel_1,
                        //     model_channel_2,
                        //     model_channel_3,
                        //     model_descr,
                        //     dbskfg_match_bag_of_fragments::DEFAULT);

                        // compute_color_region_hist(
                        //     query_sift_samples,
                        //     query_channel_1,
                        //     query_channel_2,
                        //     query_channel_3,
                        //     query_descr,
                        //     dbskfg_match_bag_of_fragments::DEFAULT);
                        
                        // vnl_vector<double> vec_model(model_descr.size(),0);
                        // vnl_vector<double> vec_query(query_descr.size(),0);

                        // for ( unsigned int m=0; m < model_descr.size(); ++m)
                        // {
                        //     vec_model.put(m,model_descr[m]);
                        //     vec_query.put(m,query_descr[m]);
                        // }

                        // vec_model.normalize(); vec_query.normalize();

                        // color_hist_diff +=
                        //     chi_squared_distance(vec_model,vec_query);

                        vcl_vector<vl_sift_pix> msift;
                        msift.push_back(ps1_actual.x());
                        msift.push_back(ps1_actual.y());
                        msift.push_back(model_radius*2.0);
                        msift.push_back(theta_ps1);

                        vcl_vector<vl_sift_pix> qsift;
                        qsift.push_back(ps2_actual.x());
                        qsift.push_back(ps2_actual.y());
                        qsift.push_back(query_radius*2.0);
                        qsift.push_back(theta_ps2);

                        model_sift.push_back(msift);
                        query_sift.push_back(qsift);

                    }
                    else
                    {
                        ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                                ps1.y()/query_scale_ratio);
                        ps2.set(ps2.x()/model_scale_ratio,
                                ps2.y()/model_scale_ratio);

                        vgl_point_2d<double> ps1_actual=ps1;
                        vgl_point_2d<double> ps2_actual=ps2;

                        if ( width > 0 )
                        {
                            ps1.set(query_channel_1.ni()-1-ps1.x(),
                                    ps1.y());
                        }
                    
                        double query_radius=((R1-r1)/query_scale_ratio)/2.0;
                        double model_radius=((R2-r2)/model_scale_ratio)/2.0;

                        if (!variable )
                        {
                            query_radius=(query_sift_scale
                                          /query_scale_ratio)/2.0;
                            model_radius=(model_sift_scale
                                          /model_scale_ratio)/2.0;
                        }

                        model_radius+=context;
                        query_radius+=context;

                        // dbskfg_compute_sift sift_process(
                        //     ps2,
                        //     model_radius*2.0,
                        //     theta_ps2,
                        //     model_scale_ratio,
                        //     model_sift_filter,
                        //     ps1,
                        //     query_radius*2.0,
                        //     theta_ps1,
                        //     query_scale_ratio,
                        //     query_sift_filter,
                        //     model_red_grad_data,
                        //     query_red_grad_data,
                        //     model_green_grad_data,
                        //     query_green_grad_data,
                        //     model_blue_grad_data,
                        //     query_blue_grad_data);

                        // local_distance += sift_process.distance();

                        local_distance += descr_cost(
                            ps2,
                            model_radius,
                            theta_ps2,
                            ps1,
                            query_radius,
                            theta_ps1,
                            model_red_grad_data,
                            query_red_grad_data,
                            model_green_grad_data,
                            query_green_grad_data,
                            model_blue_grad_data,
                            query_blue_grad_data,
                            model_sift_filter,
                            query_sift_filter);

                        // local_distance += descr_cost_enriched_sift(
                        //     ps2,
                        //     model_radius,
                        //     theta_ps2,
                        //     ps1,
                        //     query_radius,
                        //     theta_ps1,
                        //     model_channel_1,
                        //     model_channel_2,
                        //     model_channel_3,
                        //     query_channel_1,
                        //     query_channel_2,
                        //     query_channel_3,
                        //     model_red_grad_data,
                        //     query_red_grad_data,
                        //     model_green_grad_data,
                        //     query_green_grad_data,
                        //     model_blue_grad_data,
                        //     query_blue_grad_data,
                        //     model_sift_filter,
                        //     query_sift_filter);
 
                        // local_color_distance += LAB_distance(
                        //     ps2,
                        //     ps1,
                        //     model_channel_1,
                        //     model_channel_2,
                        //     model_channel_3,
                        //     query_channel_1,
                        //     query_channel_2,
                        //     query_channel_3);
                  
                        // vcl_set<vcl_pair<double,double> > model_sift_samples;
                        // vcl_set<vcl_pair<double,double> > query_sift_samples;
                        
                        // compute_color_over_sift(
                        //     model_sift_filter,
                        //     model_sift_filter->width,
                        //     model_sift_filter->height,
                        //     ps2.x(),
                        //     ps2.y(),
                        //     model_radius,
                        //     theta_ps2,
                        //     model_sift_samples);

                        // compute_color_over_sift(
                        //     query_sift_filter,
                        //     query_sift_filter->width,
                        //     query_sift_filter->height,
                        //     ps1.x(),
                        //     ps1.y(),
                        //     query_radius,
                        //     theta_ps1,
                        //     query_sift_samples);

                        // vcl_vector<double> model_descr;
                        // vcl_vector<double> query_descr;
                        
                        // compute_color_region_hist(
                        //     model_sift_samples,
                        //     model_channel_1,
                        //     model_channel_2,
                        //     model_channel_3,
                        //     model_descr,
                        //     dbskfg_match_bag_of_fragments::DEFAULT);

                        // compute_color_region_hist(
                        //     query_sift_samples,
                        //     query_channel_1,
                        //     query_channel_2,
                        //     query_channel_3,
                        //     query_descr,
                        //     dbskfg_match_bag_of_fragments::DEFAULT);
                        
                        // vnl_vector<double> vec_model(model_descr.size(),0);
                        // vnl_vector<double> vec_query(query_descr.size(),0);

                        // for ( unsigned int m=0; m < model_descr.size(); ++m)
                        // {
                        //     vec_model.put(m,model_descr[m]);
                        //     vec_query.put(m,query_descr[m]);
                        // }

                        // vec_model.normalize(); vec_query.normalize();

                        // color_hist_diff +=
                        //     chi_squared_distance(vec_model,vec_query);

                        vcl_vector<vl_sift_pix> msift;
                        msift.push_back(ps2_actual.x());
                        msift.push_back(ps2_actual.y());
                        msift.push_back(model_radius*2.0);
                        msift.push_back(theta_ps2);

                        vcl_vector<vl_sift_pix> qsift;
                        qsift.push_back(ps1_actual.x());
                        qsift.push_back(ps1_actual.y());
                        qsift.push_back(query_radius*2.0);
                        qsift.push_back(theta_ps1);

                        model_sift.push_back(msift);
                        query_sift.push_back(qsift);

                
                    }

                }
                r1+=step_size;
            }

            // Test original medial axis point
            {
                vgl_point_2d<double> ps1=sc1->sh_pt(cor.first);
                vgl_point_2d<double> ps2=sc2->sh_pt(cor.second);
                
                if ( !flag )
                {
                    
                    ps1.set(ps1.x()/model_scale_ratio,
                            ps1.y()/model_scale_ratio);
                    ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                            ps2.y()/query_scale_ratio);
                    
                    shock_curve1.push_back(ps1);
                    shock_curve2.push_back(ps2);

                    if ( add_curve )
                    {
                        query_dart_curves_[query_key1].push_back(ps2);
                    }

                    
                }
                else
                {
                    ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                            ps1.y()/query_scale_ratio);
                    ps2.set(ps2.x()/model_scale_ratio,
                            ps2.y()/model_scale_ratio);

                    shock_curve1.push_back(ps1);
                    shock_curve2.push_back(ps2);

                    if ( add_curve )
                    {
                        query_dart_curves_[query_key1].push_back(ps1);
                    }

                }
            }
            
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
        sift_diff+=local_distance;
        color_diff+=local_color_distance;

        arclength_shock_curve1=
            local_arclength_shock_curve1+arclength_shock_curve1;

        arclength_shock_curve2=
            local_arclength_shock_curve2+arclength_shock_curve2;

        dart_distances.push_back(local_distance/num_steps);
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

        // {

        //     vcl_stringstream model_stream;
        //     model_stream<<prefix<<"_dart_model_"<<i<<"_app_correspondence.txt";
        //     vcl_stringstream query_stream;
        //     query_stream<<prefix<<"_dart_query_"<<i<<"_app_correspondence.txt";
        
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

        //     vcl_stringstream model_poly_stream;
        //     model_poly_stream<<prefix<<"_dart_model_poly_"
        //                      <<i<<"_app_correspondence.txt";
        //     vcl_stringstream query_poly_stream;
        //     query_poly_stream<<prefix<<"_dart_query_poly_"
        //                      <<i<<"_app_correspondence.txt";
            
        //     if ( !flag )
        //     {
        //         sc1->write_polygon(model_poly_stream.str());
        //         sc2->write_polygon(query_poly_stream.str(),width);
        //     }
        //     else
        //     {
        //         sc1->write_polygon(query_poly_stream.str(),width);
        //         sc2->write_polygon(model_poly_stream.str());
        //     }
        // }
    }

    double avg_norm  = sift_diff/overall_index;

    double color_norm = color_diff/overall_index;

    double color_hist_norm = color_hist_diff/overall_index;

    double length_norm=sift_diff/(arclength_shock_curve1+
                                  arclength_shock_curve2);
    double splice_norm=sift_diff/(splice_cost_shock_curve1+
                                  splice_cost_shock_curve2);

    vcl_pair<double,double> app_diff(color_norm,avg_norm);

    // vcl_cout<<"Unormalized diff: "<<sift_diff<<vcl_endl;
    // vcl_cout<<"Average diff: "<<avg_norm<<vcl_endl;
    // vcl_cout<<"Splice norm:  "<<splice_norm<<vcl_endl;
    // vcl_cout<<"Length norm:  "<<length_norm<<vcl_endl;

    return app_diff;
}

vnl_vector<vl_sift_pix> dbskfg_match_bag_of_fragments::
compress_sift(vl_sift_pix* red_sift,
              vl_sift_pix* green_sift,
              vl_sift_pix* blue_sift,
              int nbp)
{

    int angle_bins=8;
    vnl_vector<vl_sift_pix> output(3*angle_bins*nbp*nbp,0);


    vnl_matrix_ref<vl_sift_pix> red_model_data(16,8,red_sift);
    vnl_matrix_ref<vl_sift_pix> green_model_data(16,8,green_sift);
    vnl_matrix_ref<vl_sift_pix> blue_model_data(16,8,blue_sift);

    if ( nbp == 2 )
    {
        
        // Start with red
        output.update(
            red_model_data.get_row(0)+
            red_model_data.get_row(1)+
            red_model_data.get_row(4)+
            red_model_data.get_row(5),0*angle_bins);
        
        output.update(
            red_model_data.get_row(2)+
            red_model_data.get_row(3)+
            red_model_data.get_row(6)+
            red_model_data.get_row(7),1*angle_bins);

        output.update(
            red_model_data.get_row(8)+
            red_model_data.get_row(9)+
            red_model_data.get_row(12)+
            red_model_data.get_row(13),2*angle_bins);

        output.update(
            red_model_data.get_row(10)+
            red_model_data.get_row(11)+
            red_model_data.get_row(14)+
            red_model_data.get_row(15),3*angle_bins);


        // Now do green
        output.update(
            green_model_data.get_row(0)+
            green_model_data.get_row(1)+
            green_model_data.get_row(4)+
            green_model_data.get_row(5),4*angle_bins);
        
        output.update(
            green_model_data.get_row(2)+
            green_model_data.get_row(3)+
            green_model_data.get_row(6)+
            green_model_data.get_row(7),5*angle_bins);

        output.update(
            green_model_data.get_row(8)+
            green_model_data.get_row(9)+
            green_model_data.get_row(12)+
            green_model_data.get_row(13),6*angle_bins);

        output.update(
            green_model_data.get_row(10)+
            green_model_data.get_row(11)+
            green_model_data.get_row(14)+
            green_model_data.get_row(15),7*angle_bins);


        // Now do blue
        output.update(
            blue_model_data.get_row(0)+
            blue_model_data.get_row(1)+
            blue_model_data.get_row(4)+
            blue_model_data.get_row(5),8*angle_bins);
        
        output.update(
            blue_model_data.get_row(2)+
            blue_model_data.get_row(3)+
            blue_model_data.get_row(6)+
            blue_model_data.get_row(7),9*angle_bins);

        output.update(
            blue_model_data.get_row(8)+
            blue_model_data.get_row(9)+
            blue_model_data.get_row(12)+
            blue_model_data.get_row(13),10*angle_bins);

        output.update(
            blue_model_data.get_row(10)+
            blue_model_data.get_row(11)+
            blue_model_data.get_row(14)+
            blue_model_data.get_row(15),11*angle_bins);



    }
    else
    {
        // Do red first
        output.update(
            red_model_data.get_row(0)+
            red_model_data.get_row(1)+
            red_model_data.get_row(2)+
            red_model_data.get_row(3)+
            red_model_data.get_row(4)+
            red_model_data.get_row(5)+
            red_model_data.get_row(6)+
            red_model_data.get_row(7)+
            red_model_data.get_row(8)+
            red_model_data.get_row(9)+
            red_model_data.get_row(10)+
            red_model_data.get_row(11)+
            red_model_data.get_row(12)+
            red_model_data.get_row(13)+
            red_model_data.get_row(14)+
            red_model_data.get_row(15),0*angle_bins);

        // Do green next
        output.update(
            green_model_data.get_row(0)+
            green_model_data.get_row(1)+
            green_model_data.get_row(2)+
            green_model_data.get_row(3)+
            green_model_data.get_row(4)+
            green_model_data.get_row(5)+
            green_model_data.get_row(6)+
            green_model_data.get_row(7)+
            green_model_data.get_row(8)+
            green_model_data.get_row(9)+
            green_model_data.get_row(10)+
            green_model_data.get_row(11)+
            green_model_data.get_row(12)+
            green_model_data.get_row(13)+
            green_model_data.get_row(14)+
            green_model_data.get_row(15),1*angle_bins);


        // Do blue next
        output.update(
            blue_model_data.get_row(0)+
            blue_model_data.get_row(1)+
            blue_model_data.get_row(2)+
            blue_model_data.get_row(3)+
            blue_model_data.get_row(4)+
            blue_model_data.get_row(5)+
            blue_model_data.get_row(6)+
            blue_model_data.get_row(7)+
            blue_model_data.get_row(8)+
            blue_model_data.get_row(9)+
            blue_model_data.get_row(10)+
            blue_model_data.get_row(11)+
            blue_model_data.get_row(12)+
            blue_model_data.get_row(13)+
            blue_model_data.get_row(14)+
            blue_model_data.get_row(15),2*angle_bins);
            

    }

    return output;
}


vcl_pair<double,double> dbskfg_match_bag_of_fragments::compute_mi(
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_vector< pathtable_key >& path_map,
    vcl_vector<double>& dart_distances,
    vil_image_view<double>& model_channel_1,
    vil_image_view<double>& model_channel_2,
    vil_image_view<double>& model_channel_3,
    vil_image_view<double>& query_channel_1,
    vil_image_view<double>& query_channel_2,
    vil_image_view<double>& query_channel_3,
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



    vl_sift_pix range= forest_->numData;
    unsigned int bins= forest_->numData;

    bsta_histogram<double> model_hist(range, bins);
    bsta_histogram<double> query_hist(range, bins);
    
    bsta_joint_histogram<double> joint_hist(range, bins);

    // Get matching pairs
    for (unsigned i = 0; i < map_list.size(); i++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];

        
        double step_size=1.0;

        for (unsigned j = 0; j < map_list[i].size(); ++j) 
        {
            vcl_pair<int, int> cor = map_list[i][j];
            
            // Shock Point 1 from Model
            double radius_ps1        = sc1->time(cor.first);
            double theta_ps1         = sc1->theta(cor.first);

            // Shock Point 2 from Query
            double radius_ps2        = sc2->time(cor.second);
            double theta_ps2         = sc2->theta(cor.second);

            double r1=0;
            double R1=radius_ps1;
            double R2=radius_ps2;
            double ratio = (R2/R1);

            do
            {
                double r2=r1*ratio;
                
                vgl_point_2d<double> ps1;
                vgl_point_2d<double> ps2;

                vgl_point_2d<double> ps1_plus;
                vgl_point_2d<double> ps1_minus;

                vgl_point_2d<double> ps2_plus;
                vgl_point_2d<double> ps2_minus;


                if ( r1 == 0 )
                {
                    ps1=sc1->sh_pt(cor.first);
                    ps2=sc2->sh_pt(cor.second);

                    ps1_plus=ps1;
                    ps1_minus=ps1;

                    ps2_plus=ps2;
                    ps2_minus=ps2;

                }
                else
                {
                    ps1_plus=sc1->fragment_pt(cor.first,r1);
                    ps2_plus=sc2->fragment_pt(cor.second,r2);

                    ps1_minus=sc1->fragment_pt(cor.first,-r1);
                    ps2_minus=sc2->fragment_pt(cor.second,-r2);
                    
                    ps1=ps1_plus;
                    ps2=ps2_plus;
                }
 
                if ( !flag )
                {   
                    ps1.set(ps1.x()/model_scale_ratio,
                            ps1.y()/model_scale_ratio);
                    ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                            ps2.y()/query_scale_ratio);

                    ps1_plus.set(ps1_plus.x()/model_scale_ratio,
                                 ps1_plus.y()/model_scale_ratio);
                    ps2_plus.set(vcl_fabs(width-(ps2_plus.x()/
                                                 query_scale_ratio)),
                                 ps2_plus.y()/query_scale_ratio);
                    
                    ps1_minus.set(ps1_minus.x()/model_scale_ratio,
                                  ps1_minus.y()/model_scale_ratio);
                    ps2_minus.set(vcl_fabs(width-(ps2_minus.x()/
                                                  query_scale_ratio)),
                                  ps2_minus.y()/query_scale_ratio);
                                       

                    if ( width > 0 )
                    {
                        ps2.set(query_channel_1.ni()-1-ps2.x(),
                                ps2.y());

                        ps2_plus.set(query_channel_1.ni()-1-
                                     ps2_plus.x(),
                                     ps2_plus.y());
                        
                        ps2_minus.set(query_channel_1.ni()-
                                      1-ps2_minus.x(),
                                      ps2_minus.y());
                    }
                    
                    
                    double model_radius=((R1-r1)/model_scale_ratio)/2.0;
                    double query_radius=((R2-r2)/query_scale_ratio)/2.0;

                    if ( r1 == 0 )
                    {

                        vnl_vector<vl_sift_pix> model_descriptor(384,0.0);
                        vnl_vector<vl_sift_pix> query_descriptor(384,0.0);
   
                        compute_descr(ps1,
                                      model_radius,
                                      theta_ps1,
                                      model_red_grad_data,
                                      model_green_grad_data,
                                      model_blue_grad_data,
                                      model_sift_filter,
                                      model_descriptor);
                        
                        compute_descr(ps2,
                                      query_radius,
                                      theta_ps2,
                                      query_red_grad_data,
                                      query_green_grad_data,
                                      query_blue_grad_data,
                                      query_sift_filter,
                                      query_descriptor);
                        

                        VlKDForestNeighbor model_keyword[0];
                        VlKDForestNeighbor query_keyword[1];

                        int nvisited = vl_kdforestsearcher_query(
                            searcher_, model_keyword, 1, model_descriptor
                            .data_block());

                        nvisited = vl_kdforestsearcher_query(
                            searcher_, query_keyword, 1, query_descriptor
                            .data_block());
                        
                        model_hist.upcount(model_keyword[0].index,1.0f);
                        query_hist.upcount(query_keyword[0].index,1.0f);
                        joint_hist.upcount(model_keyword[0].index,1.0f,
                                           query_keyword[0].index,1.0f);
                    }
                    else
                    {
                        vnl_vector<vl_sift_pix> model_plus_descriptor(384,0.0);
                        vnl_vector<vl_sift_pix> query_plus_descriptor(384,0.0);

                        vnl_vector<vl_sift_pix> model_minus_descriptor(384,0.0);
                        vnl_vector<vl_sift_pix> query_minus_descriptor(384,0.0);
   
                        compute_descr(ps1_plus,
                                      model_radius,
                                      theta_ps1,
                                      model_red_grad_data,
                                      model_green_grad_data,
                                      model_blue_grad_data,
                                      model_sift_filter,
                                      model_plus_descriptor);

                        compute_descr(ps1_minus,
                                      model_radius,
                                      theta_ps1,
                                      model_red_grad_data,
                                      model_green_grad_data,
                                      model_blue_grad_data,
                                      model_sift_filter,
                                      model_minus_descriptor);

                        
                        compute_descr(ps2_plus,
                                      query_radius,
                                      theta_ps2,
                                      query_red_grad_data,
                                      query_green_grad_data,
                                      query_blue_grad_data,
                                      query_sift_filter,
                                      query_plus_descriptor);

                        compute_descr(ps2_minus,
                                      query_radius,
                                      theta_ps2,
                                      query_red_grad_data,
                                      query_green_grad_data,
                                      query_blue_grad_data,
                                      query_sift_filter,
                                      query_minus_descriptor);


                        VlKDForestNeighbor model_plus_keyword[1];
                        VlKDForestNeighbor model_minus_keyword[1];

                        int nvisited = vl_kdforestsearcher_query(
                            searcher_, model_plus_keyword, 1, 
                            model_plus_descriptor.data_block());

                        nvisited = vl_kdforestsearcher_query(
                            searcher_, model_minus_keyword, 1, 
                            model_minus_descriptor.data_block());

                        VlKDForestNeighbor query_plus_keyword[1];
                        VlKDForestNeighbor query_minus_keyword[1];

                        nvisited = vl_kdforestsearcher_query(
                            searcher_, query_plus_keyword, 1, 
                            query_plus_descriptor.data_block());

                        nvisited = vl_kdforestsearcher_query(
                            searcher_, query_minus_keyword, 1, 
                            query_minus_descriptor.data_block());

                        model_hist.upcount(model_plus_keyword[0].index,1.0f);
                        query_hist.upcount(query_plus_keyword[0].index,1.0f);
                        joint_hist.upcount(model_plus_keyword[0].index,1.0f,
                                           query_plus_keyword[0].index,1.0f);

                        model_hist.upcount(model_minus_keyword[0].index,1.0f);
                        query_hist.upcount(query_minus_keyword[0].index,1.0f);
                        joint_hist.upcount(model_minus_keyword[0].index,1.0f,
                                           query_minus_keyword[0].index,1.0f);


                    }
                }
                else
                {
                    ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                            ps1.y()/query_scale_ratio);
                    ps2.set(ps2.x()/model_scale_ratio,
                            ps2.y()/model_scale_ratio);

                    ps1_plus.set(vcl_fabs(
                                     width-(
                                         ps1_plus.x()/query_scale_ratio)),
                                 ps1_plus.y()/query_scale_ratio);
                    ps2_minus.set(ps2_minus.x()/model_scale_ratio,
                                  ps2_minus.y()/model_scale_ratio);

                    if ( width > 0 )
                    {
                        ps1.set(query_channel_1.ni()-1-ps1.x(),
                                ps1.y());

                        ps1_plus.set(query_channel_1.ni()-1-ps1_plus.x(),
                                     ps1_plus.y());
                        
                        ps1_minus.set(query_channel_1.ni()-1-ps1_minus.x(),
                                      ps1_minus.y());
                    }
                    
                    double query_radius=((R1-r1)/query_scale_ratio)/2.0;
                    double model_radius=((R2-r2)/model_scale_ratio)/2.0;
                    
                    if ( r1 == 0 )
                    {

                        vnl_vector<vl_sift_pix> model_descriptor(384,0.0);
                        vnl_vector<vl_sift_pix> query_descriptor(384,0.0);
   
                        compute_descr(ps2,
                                      model_radius,
                                      theta_ps2,
                                      model_red_grad_data,
                                      model_green_grad_data,
                                      model_blue_grad_data,
                                      model_sift_filter,
                                      model_descriptor);
                        
                        compute_descr(ps1,
                                      query_radius,
                                      theta_ps1,
                                      query_red_grad_data,
                                      query_green_grad_data,
                                      query_blue_grad_data,
                                      query_sift_filter,
                                      query_descriptor);

                        VlKDForestNeighbor model_keyword[1];
                        VlKDForestNeighbor query_keyword[1];

                        int nvisited = vl_kdforestsearcher_query(
                            searcher_, model_keyword, 1, model_descriptor
                            .data_block());

                        nvisited = vl_kdforestsearcher_query(
                            searcher_, query_keyword, 1, query_descriptor
                            .data_block());

                        model_hist.upcount(model_keyword[0].index,1.0f);
                        query_hist.upcount(query_keyword[0].index,1.0f);
                        joint_hist.upcount(model_keyword[0].index,1.0f,
                                           query_keyword[0].index,1.0f);
                        
                    }
                    else
                    {
                        vnl_vector<vl_sift_pix> model_plus_descriptor(384,0.0);
                        vnl_vector<vl_sift_pix> query_plus_descriptor(384,0.0);

                        vnl_vector<vl_sift_pix> model_minus_descriptor(384,0.0);
                        vnl_vector<vl_sift_pix> query_minus_descriptor(384,0.0);
   
                        compute_descr(ps2_plus,
                                      model_radius,
                                      theta_ps2,
                                      model_red_grad_data,
                                      model_green_grad_data,
                                      model_blue_grad_data,
                                      model_sift_filter,
                                      model_plus_descriptor);

                        compute_descr(ps2_minus,
                                      model_radius,
                                      theta_ps2,
                                      model_red_grad_data,
                                      model_green_grad_data,
                                      model_blue_grad_data,
                                      model_sift_filter,
                                      model_minus_descriptor);

                        
                        compute_descr(ps1_plus,
                                      query_radius,
                                      theta_ps1,
                                      query_red_grad_data,
                                      query_green_grad_data,
                                      query_blue_grad_data,
                                      query_sift_filter,
                                      query_plus_descriptor);

                        compute_descr(ps1_minus,
                                      query_radius,
                                      theta_ps1,
                                      query_red_grad_data,
                                      query_green_grad_data,
                                      query_blue_grad_data,
                                      query_sift_filter,
                                      query_minus_descriptor);

                        VlKDForestNeighbor model_plus_keyword[1];
                        VlKDForestNeighbor model_minus_keyword[1];

                        int nvisited = vl_kdforestsearcher_query(
                            searcher_, model_plus_keyword, 1, 
                            model_plus_descriptor.data_block());

                        nvisited = vl_kdforestsearcher_query(
                            searcher_, model_minus_keyword, 1, 
                            model_minus_descriptor.data_block());

                        VlKDForestNeighbor query_plus_keyword[1];
                        VlKDForestNeighbor query_minus_keyword[1];

                        nvisited = vl_kdforestsearcher_query(
                            searcher_, query_plus_keyword, 1, 
                            query_plus_descriptor.data_block());

                        nvisited = vl_kdforestsearcher_query(
                            searcher_, query_minus_keyword, 1, 
                            query_minus_descriptor.data_block());

                        model_hist.upcount(model_plus_keyword[0].index,1.0f);
                        query_hist.upcount(query_plus_keyword[0].index,1.0f);
                        joint_hist.upcount(model_plus_keyword[0].index,1.0f,
                                           query_plus_keyword[0].index,1.0f);

                        model_hist.upcount(model_minus_keyword[0].index,1.0f);
                        query_hist.upcount(query_minus_keyword[0].index,1.0f);
                        joint_hist.upcount(model_minus_keyword[0].index,1.0f,
                                           query_minus_keyword[0].index,1.0f);


                    }

                }
                
                r1+=step_size;

            }while (r1 <= R1 );
            
        }


    }
    
    double model_entropy=model_hist.entropy();
    double query_entropy=query_hist.entropy();
    double joint_entropy=joint_hist.entropy();

    double mi=model_entropy+query_entropy-joint_entropy;
    double metric_mi=joint_entropy-mi;
    // vcl_cout<<"Model entropy: "<<model_entropy<<vcl_endl;
    // vcl_cout<<"query entropy: "<<query_entropy<<vcl_endl;
    // vcl_cout<<"Joint entropy: "<<joint_entropy<<vcl_endl;
    // vcl_cout<<"Mutual Information is: "<<mi<<vcl_endl;
    // vcl_cout<<"Mutual Information metric: "<<metric_mi<<vcl_endl;

    vcl_pair<double,double> distance=vcl_make_pair(mi,metric_mi);
    return distance;
}


vcl_pair<double,double> dbskfg_match_bag_of_fragments::compute_bow(
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_vector< pathtable_key >& path_map,
    vcl_vector<double>& dart_distances,
    vil_image_view<double>& model_channel_1,
    vil_image_view<double>& model_channel_2,
    vil_image_view<double>& model_channel_3,
    vil_image_view<double>& query_channel_1,
    vil_image_view<double>& query_channel_2,
    vil_image_view<double>& query_channel_3,
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


    VlDoubleVectorComparisonFunction Chi2_distance =    
      vl_get_vector_comparison_function_d (VlDistanceChi2) ;

    vl_sift_pix range= forest_->numData;
    unsigned int bins= forest_->numData;

    bsta_histogram<double> model_hist(range, bins);
    bsta_histogram<double> query_hist(range, bins);
    
    bsta_joint_histogram<double> joint_hist(range, bins);

    vnl_vector<double> hist_distances(map_list.size(),0);
    vnl_vector<double> sc1_part_hists(map_list.size()*bins,0);
    vnl_vector<double> sc2_part_hists(map_list.size()*bins,0);

    // Get matching pairs
    for (unsigned ii = 0; ii < map_list.size(); ii++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[ii];
        dbskr_scurve_sptr sc2 = curve_list2[ii];

        bsta_histogram<double> sc1_hist(range, bins);
        bsta_histogram<double> sc2_hist(range, bins);


        for ( unsigned int c1=0; c1 < sc1->num_points(); ++c1 )
        {

            // Shock Point 1 from Model
            vgl_point_2d<double> ps1 = sc1->sh_pt(c1);
            double radius_ps1        = sc1->time(c1)/2.0;
            double theta_ps1         = sc1->theta(c1);

            vnl_vector<vl_sift_pix> ps1_descriptor(384,0.0);

            if ( !flag )
            {   
                ps1.set(ps1.x()/model_scale_ratio,
                        ps1.y()/model_scale_ratio);

                compute_descr(ps1,
                              radius_ps1,
                              theta_ps1,
                              model_red_grad_data,
                              model_green_grad_data,
                              model_blue_grad_data,
                              model_sift_filter,
                              ps1_descriptor);

            }
            else
            {
                ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                        ps1.y()/query_scale_ratio);

                if ( width > 0 )
                {
                    ps1.set(query_channel_1.ni()-1-ps1.x(),
                            ps1.y());
                
                }

                compute_descr(ps1,
                              radius_ps1,
                              theta_ps1,
                              query_red_grad_data,
                              query_green_grad_data,
                              query_blue_grad_data,
                              query_sift_filter,
                              ps1_descriptor);
                
            }

            
            VlKDForestNeighbor ps1_keyword[1];
            
            int nvisited = vl_kdforestsearcher_query(
                searcher_, ps1_keyword, 1, 
                ps1_descriptor.data_block());

            sc1_hist.upcount(ps1_keyword[0].index,1.0f);
            model_hist.upcount(ps1_keyword[0].index,1.0f);
        }
        
        for ( unsigned int c2=0; c2 < sc2->num_points(); ++c2 )
        {

            // Shock Point 1 from Model
            vgl_point_2d<double> ps2 = sc2->sh_pt(c2);
            double radius_ps2        = sc2->time(c2)/2.0;
            double theta_ps2         = sc2->theta(c2);

            vnl_vector<vl_sift_pix> ps2_descriptor(384,0.0);

            if ( !flag )
            {   
                ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                        ps2.y()/query_scale_ratio);
                
                if ( width > 0 )
                {
                    ps2.set(query_channel_1.ni()-1-ps2.x(),
                            ps2.y());
                }

                compute_descr(ps2,
                              radius_ps2,
                              theta_ps2,
                              query_red_grad_data,
                              query_green_grad_data,
                              query_blue_grad_data,
                              query_sift_filter,
                              ps2_descriptor);


            }
            else
            {
                ps2.set(ps2.x()/model_scale_ratio,
                        ps2.y()/model_scale_ratio);
                
                compute_descr(ps2,
                              radius_ps2,
                              theta_ps2,
                              model_red_grad_data,
                              model_green_grad_data,
                              model_blue_grad_data,
                              model_sift_filter,
                              ps2_descriptor);

            }
            
            VlKDForestNeighbor ps2_keyword[1];
            
            int nvisited = vl_kdforestsearcher_query(
                searcher_, ps2_keyword, 1, 
                ps2_descriptor.data_block());
            
            sc2_hist.upcount(ps2_keyword[0].index,1.0f);
            query_hist.upcount(ps2_keyword[0].index,1.0f);

        }


        vcl_vector<double> sc1_counts = sc1_hist.count_array();
        vcl_vector<double> sc2_counts = sc2_hist.count_array();

        vnl_vector<double> descr1(sc1_counts.data(),sc1_counts.size());
        vnl_vector<double> descr2(sc2_counts.data(),sc2_counts.size());

        descr1.normalize();
        descr2.normalize();

        double result_final[1];
        
        vl_eval_vector_comparison_on_all_pairs_d(result_final,
                                                 descr1.size(),
                                                 descr1.data_block(),
                                                 1,
                                                 descr2.data_block(),
                                                 1,
                                                 Chi2_distance);
        
        hist_distances.put(ii,(0.5)*result_final[0]);

        unsigned int start_index=ii*bins;

        for ( unsigned int kkk=0; kkk < sc1_counts.size() ; ++kkk)
        {
            sc1_part_hists.put(kkk+start_index,sc1_counts[kkk]);
            sc2_part_hists.put(kkk+start_index,sc2_counts[kkk]);

        }
        
    }

    vcl_vector<double> model_counts = model_hist.count_array();
    vcl_vector<double> query_counts = query_hist.count_array();


    vnl_vector<double> descr1(model_counts.data(),model_counts.size());
    vnl_vector<double> descr2(query_counts.data(),query_counts.size());
    
    descr1.normalize();
    descr2.normalize();
    
    double result_final[1];
    
    vl_eval_vector_comparison_on_all_pairs_d(result_final,
                                             descr1.size(),
                                             descr1.data_block(),
                                             1,
                                             descr2.data_block(),
                                             1,
                                             Chi2_distance);
    
    double overall_dist = (0.5)*result_final[0];

    vcl_pair<double,double> distances(0,0);

    // Lets look at chi squared distance first
    double  average_chi = (hist_distances.sum()+overall_dist)/
        (hist_distances.size()+1);
    
    distances.first=average_chi;

    // Lets look at L2 distance

    double global_l2_distance = vcl_sqrt(vnl_vector_ssd(descr1,descr2));

    sc1_part_hists.normalize();
    sc2_part_hists.normalize();

    double part_l2_distance = vcl_sqrt(vnl_vector_ssd(sc1_part_hists,
                                                      sc2_part_hists));

    distances.second = global_l2_distance+part_l2_distance;
    return distances;
        
}



void dbskfg_match_bag_of_fragments::compute_app_alignment(
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vil_image_view<double>& model_channel_1,
    vil_image_view<double>& query_channel_1,
    bool flag,
    double width,
    vl_sift_pix* model_grad_data,
    VlSiftFilt*  model_sift_filter,
    vl_sift_pix* query_grad_data,
    VlSiftFilt*  query_sift_filter,
    double model_scale_ratio,
    double query_scale_ratio)
{
    
    unsigned int stop=map_list.size();

    map_list.clear();

    // Get matching pairs
    for (unsigned ii = 0; ii < stop; ii++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[ii];
        dbskr_scurve_sptr sc2 = curve_list2[ii];

        vnl_matrix<vl_sift_pix> sc1_matrix(128,sc1->num_points(),0.0);
        vnl_matrix<vl_sift_pix> sc2_matrix(128,sc2->num_points(),0.0);

        for ( unsigned int c1=0; c1 < sc1->num_points(); ++c1 )
        {

            // Shock Point 1 from Model
            vgl_point_2d<double> ps1 = sc1->sh_pt(c1);
            double radius_ps1        = sc1->time(c1)/2.0;
            double theta_ps1         = sc1->theta(c1);

            vnl_vector<vl_sift_pix> ps1_descriptor(128,0.0);

            if ( !flag )
            {   
                ps1.set(ps1.x()/model_scale_ratio,
                        ps1.y()/model_scale_ratio);

                compute_descr(ps1,
                              radius_ps1,
                              theta_ps1,
                              model_grad_data,
                              model_sift_filter,
                              ps1_descriptor);

            }
            else
            {
                ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                        ps1.y()/query_scale_ratio);

                if ( width > 0 )
                {
                    ps1.set(query_channel_1.ni()-1-ps1.x(),
                            ps1.y());
                
                }

                compute_descr(ps1,
                              radius_ps1,
                              theta_ps1,
                              query_grad_data,
                              query_sift_filter,
                              ps1_descriptor);
                
            }

            sc1_matrix.set_column(c1,ps1_descriptor);
        }

        for ( unsigned int c2=0; c2 < sc2->num_points(); ++c2 )
        {

            // Shock Point 1 from Model
            vgl_point_2d<double> ps2 = sc2->sh_pt(c2);
            double radius_ps2        = sc2->time(c2)/2.0;
            double theta_ps2         = sc2->theta(c2);

            vnl_vector<vl_sift_pix> ps2_descriptor(128,0.0);

            if ( !flag )
            {   
                ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                        ps2.y()/query_scale_ratio);
                
                if ( width > 0 )
                {
                    ps2.set(query_channel_1.ni()-1-ps2.x(),
                            ps2.y());
                }

                compute_descr(ps2,
                              radius_ps2,
                              theta_ps2,
                              query_grad_data,
                              query_sift_filter,
                              ps2_descriptor);


            }
            else
            {
                ps2.set(ps2.x()/model_scale_ratio,
                        ps2.y()/model_scale_ratio);
                
                compute_descr(ps2,
                              radius_ps2,
                              theta_ps2,
                              model_grad_data,
                              model_sift_filter,
                              ps2_descriptor);

            }

            sc2_matrix.set_column(c2,ps2_descriptor);
        }

        dbskfg_app_curve_match dpMatch(sc1_matrix,sc2_matrix);
        dpMatch.Match();

        vcl_vector<vcl_pair<int,int> > fmap=*(dpMatch.finalMap());
        map_list.push_back(fmap);
        
    }

        


}

vcl_pair<double,double> dbskfg_match_bag_of_fragments::
compute_o2p_dense(
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
      vl_get_vector_comparison_function_f (VlDistanceChi2);

    double sift_diff= 0.0;
    
    // Get matching pairs
    for (unsigned i = 0; i < map_list.size(); i++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];

        vcl_map< vcl_pair<double,double>, vcl_vector<vl_sift_pix> > model_sift;
        vcl_map< vcl_pair<double,double>, vcl_vector<vl_sift_pix> > query_sift;

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
        
        double step_size=1.0;
        unsigned int num_steps=0.0;

        for (unsigned j = 0; j < map_list[i].size(); ++j) 
        {
            vcl_pair<int, int> cor = map_list[i][j];
            
            // Compute sift for both images

            // Shock Point 1 from Model
            double radius_ps1        = sc1->time(cor.first);
            double theta_ps1         = sc1->theta(cor.first);

            // Shock Point 2 from Query
            double radius_ps2        = sc2->time(cor.second);
            double theta_ps2         = sc2->theta(cor.second);

            double ratio=1.0;
            double R1=radius_ps1;
            double R2=radius_ps2;

            ratio=(R2/R1);
                

            double r1 = step_size;

            while ( r1 <= R1 )
            {
                double r2=r1*ratio;
                for ( unsigned int p=0; p < 2 ; ++p )
                {
                    vgl_point_2d<double> ps1;
                    vgl_point_2d<double> ps2;

                    if ( p ==  0 )
                    {
                        ps1=sc1->fragment_pt(cor.first,r1);
                        ps2=sc2->fragment_pt(cor.second,r2);
                    }
                    else
                    {
                        ps1=sc1->fragment_pt(cor.first,-1.0*r1);
                        ps2=sc2->fragment_pt(cor.second,-1.0*r2);

                    }

                    if ( !flag )
                    {
                        
                        ps1.set(ps1.x()/model_scale_ratio,
                                ps1.y()/model_scale_ratio);
                        ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                                ps2.y()/query_scale_ratio);

                        vcl_pair<double,double> ps1_key(ps1.x(),ps1.y());
                        vcl_pair<double,double> ps2_key(ps2.x(),ps2.y());

                        if ( !model_sift.count(ps1_key) )
                        {
                            vcl_pair<double,double> key(ps1.x(),ps1.y());

                            model_sift[key].push_back(ps1.x());
                            model_sift[key].push_back(ps1.y());
                            model_sift[key].push_back(
                                (R1-r1)/model_scale_ratio);
                            model_sift[key].push_back(theta_ps1);
                        }


                        if ( !query_sift.count(ps2_key) )
                        {
                            vcl_pair<double,double> key(ps2.x(),ps2.y());

                            query_sift[key].push_back(ps2.x());
                            query_sift[key].push_back(ps2.y());
                            query_sift[key].push_back(
                                (R2-r2)/query_scale_ratio);
                            query_sift[key].push_back(theta_ps2);

                        }

                    }
                    else
                    {
                        ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                                ps1.y()/query_scale_ratio);
                        ps2.set(ps2.x()/model_scale_ratio,
                                ps2.y()/model_scale_ratio);

                        vcl_pair<double,double> ps1_key(ps1.x(),ps1.y());
                        vcl_pair<double,double> ps2_key(ps2.x(),ps2.y());
                    
                        if ( !model_sift.count(ps1_key))
                        {
                            vcl_pair<double,double> key(ps1.x(),ps1.y());

                            model_sift[key].push_back(ps1.x());
                            model_sift[key].push_back(ps1.y());
                            model_sift[key].push_back(
                                (R1-r1)/query_scale_ratio);
                            model_sift[key].push_back(theta_ps1);
                        }
                       
                        if ( !query_sift.count(ps2_key))
                        {
                            vcl_pair<double,double> key(ps2.x(),ps2.y());

                            query_sift[key].push_back(ps2.x());
                            query_sift[key].push_back(ps2.y());
                            query_sift[key].push_back(
                                (R2-r2)/model_scale_ratio);
                            query_sift[key].push_back(theta_ps2);
                        }

                
                    }

                }
                r1+=step_size;
            }

            // Test original medial axis point
            {
                vgl_point_2d<double> ps1=sc1->sh_pt(cor.first);
                vgl_point_2d<double> ps2=sc2->sh_pt(cor.second);
                
                r1=0.0;
                double r2=0.0;
                
                if ( !flag )
                {
                    
                    ps1.set(ps1.x()/model_scale_ratio,
                            ps1.y()/model_scale_ratio);
                    ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                            ps2.y()/query_scale_ratio);

                    vcl_pair<double,double> ps1_key(ps1.x(),ps1.y());
                    vcl_pair<double,double> ps2_key(ps2.x(),ps2.y());
                        
                    if ( !model_sift.count(ps1_key) )
                    {
                        vcl_pair<double,double> key(ps1.x(),ps1.y());

                        model_sift[key].push_back(ps1.x());
                        model_sift[key].push_back(ps1.y());
                        model_sift[key].push_back((R1-r1)/model_scale_ratio);
                        model_sift[key].push_back(theta_ps1);
                    }

                    if ( !query_sift.count(ps2_key) )
                    {   
                        vcl_pair<double,double> key(ps2.x(),ps2.y());
                     
                        query_sift[key].push_back(ps2.x());
                        query_sift[key].push_back(ps2.y());
                        query_sift[key].push_back((R2-r2)/query_scale_ratio);
                        query_sift[key].push_back(theta_ps2);
                    }

                    if ( add_curve )
                    {
                        query_dart_curves_[query_key1].push_back(ps2);
                    }

                    
                }
                else
                {
                    ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                            ps1.y()/query_scale_ratio);
                    ps2.set(ps2.x()/model_scale_ratio,
                            ps2.y()/model_scale_ratio);
                    
                    vcl_pair<double,double> ps1_key(ps1.x(),ps1.y());
                    vcl_pair<double,double> ps2_key(ps2.x(),ps2.y());

                    if ( !model_sift.count(ps1_key) )
                    {
                        vcl_pair<double,double> key(ps1.x(),ps1.y());
          
                        model_sift[key].push_back(ps1.x());
                        model_sift[key].push_back(ps1.y());
                        model_sift[key].push_back((R1-r1)/query_scale_ratio);
                        model_sift[key].push_back(theta_ps1);
                    }
                  
                    if (!query_sift.count(ps2_key) )
                    {
                        vcl_pair<double,double> key(ps2.x(),ps2.y());

                        query_sift[key].push_back(ps2.x());
                        query_sift[key].push_back(ps2.y());
                        query_sift[key].push_back((R2-r2)/model_scale_ratio);
                        query_sift[key].push_back(theta_ps2);
                    }

                    if ( add_curve )
                    {
                        query_dart_curves_[query_key1].push_back(ps1);
                    }

                }
            }
            
        }

        vnl_vector<vl_sift_pix> model_triu(73920,0.0);
        vnl_vector<vl_sift_pix> query_triu(73920,0.0);

        // Process model first
        {
            vnl_matrix<vl_sift_pix> model_matrix(384,model_sift.size(),0.0);
            unsigned int index(0);
            
            vcl_map< vcl_pair<double,double>, vcl_vector<vl_sift_pix> >
                ::iterator it;
            for ( it = model_sift.begin() ; it != model_sift.end() ; ++it)
            {
                vcl_vector<vl_sift_pix> model_vec=(*it).second;
                
                vgl_point_2d<double> model_pt(model_vec[0],model_vec[1]);
                double model_radius=model_vec[2]/2.0;
                double model_theta=model_vec[3];
                
                vnl_vector<vl_sift_pix> model_descriptor(384,0.0);
                
                compute_descr(model_pt,
                              model_radius,
                              model_theta,
                              model_red_grad_data,
                              model_green_grad_data,
                              model_blue_grad_data,
                              model_sift_filter,
                              model_descriptor);

                model_matrix.set_column(index,model_descriptor.data_block());
                
                index++;
            }
            
            vnl_matrix<vl_sift_pix> model_matrix_transpose=
                model_matrix.transpose();
            vnl_matrix<vl_sift_pix> o2p_model = 
                (model_matrix*model_matrix_transpose)/(model_sift.size());

            unsigned int position=0;

            // Get upper triangle portion
            for ( unsigned int c=0; c < o2p_model.cols() ; ++c)
            {
                for ( unsigned int r=0; r < c+1 ; ++r)
                {
                    double scaled_value=o2p_model[r][c];
                    model_triu.put(position,scaled_value);
                    position++;
                }
            }

            //Finally normalize descriptor
            model_triu.normalize();
        }


        // Process query first
        {
            vnl_matrix<vl_sift_pix> query_matrix(384,query_sift.size(),0.0);
            unsigned int index(0);
            
            vcl_map< vcl_pair<double,double>, vcl_vector<vl_sift_pix> >
                ::iterator it;
            for ( it = query_sift.begin() ; it != query_sift.end() ; ++it)
            {
                vcl_vector<vl_sift_pix> query_vec=(*it).second;
                
                vgl_point_2d<double> query_pt(query_vec[0],query_vec[1]);
                double query_radius=query_vec[2]/2.0;
                double query_theta=query_vec[3];
                
                vnl_vector<vl_sift_pix> query_descriptor(384,0.0);
                
                compute_descr(query_pt,
                              query_radius,
                              query_theta,
                              query_red_grad_data,
                              query_green_grad_data,
                              query_blue_grad_data,
                              query_sift_filter,
                              query_descriptor);

                query_matrix.set_column(index,query_descriptor.data_block());
                
                index++;
            }
            
            vnl_matrix<vl_sift_pix> query_matrix_transpose=
                query_matrix.transpose();
            vnl_matrix<vl_sift_pix> o2p_query = 
                (query_matrix*query_matrix_transpose)/(query_sift.size());

            unsigned int position=0;

            // Get upper triangle portion
            for ( unsigned int c=0; c < o2p_query.cols() ; ++c)
            {
                for ( unsigned int r=0; r < c+1 ; ++r)
                {
                    double scaled_value=o2p_query[r][c];
                    query_triu.put(position,scaled_value);
                    position++;
                }
            }

            //Finally normalize descriptor
            query_triu.normalize();
        }

        vl_sift_pix local_distance[1];
        vl_eval_vector_comparison_on_all_pairs_f(local_distance,
                                                 model_triu.size(),
                                                 model_triu.data_block(),
                                                 1,
                                                 query_triu.data_block(),
                                                 1,
                                                 Chi2_distance);


        sift_diff+=0.5*local_distance[0];
        dart_distances.push_back(0.5*local_distance[0]);
        // vcl_cout<<"Tree 1 dart ("
        //         <<path_map[i].first.first
        //         <<","
        //         <<path_map[i].first.second
        //         <<") Tree 2 dart ("
        //         <<path_map[i].second.first
        //         <<","
        //         <<path_map[i].second.second
        //         <<") L2 distance: "
        //         <<local_distance[0]<<vcl_endl;
       
    }


    vcl_pair<double,double> app_diff(sift_diff,sift_diff/map_list.size());

    // vcl_cout<<"Unormalized diff: "<<app_diff.first<<vcl_endl;
    // vcl_cout<<"Average diff:     "<<app_diff.second<<vcl_endl;
    return app_diff;
}


vcl_pair<double,double> dbskfg_match_bag_of_fragments::
compute_3d_hist_color(
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_vector< pathtable_key >& path_map,
    vcl_vector<double>& dart_distances,
    vil_image_view<double>& model_channel_1,
    vil_image_view<double>& model_channel_2,
    vil_image_view<double>& model_channel_3,
    vil_image_view<double>& query_channel_1,
    vil_image_view<double>& query_channel_2,
    vil_image_view<double>& query_channel_3,
    double model_scale_ratio,
    double query_scale_ratio,
    bool flag,
    double width)
{
 
    double sift_diff= 0.0;
    double step_size=1.0;
    bool debug=false;

    // Get matching pairs
    for (unsigned index = 0; index < map_list.size(); index++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[index];
        dbskr_scurve_sptr sc2 = curve_list2[index];

        unsigned int midpoint_index=sc1->midpoint_index();

        // Compute three level pyramid histogram

        // Level 0
        vcl_set< vcl_pair<double,double> > model_sift;
        vcl_set< vcl_pair<double,double> > query_sift;

        // Level 1
        vcl_set< vcl_pair<double,double> > model_sift_left;
        vcl_set< vcl_pair<double,double> > model_sift_right;
        vcl_set< vcl_pair<double,double> > query_sift_left;
        vcl_set< vcl_pair<double,double> > query_sift_right;

        // Level 2
        vcl_set< vcl_pair<double,double> > model_sift_left_plus;
        vcl_set< vcl_pair<double,double> > model_sift_left_minus;
        vcl_set< vcl_pair<double,double> > model_sift_right_plus;
        vcl_set< vcl_pair<double,double> > model_sift_right_minus;

        vcl_set< vcl_pair<double,double> > query_sift_left_plus;
        vcl_set< vcl_pair<double,double> > query_sift_left_minus;
        vcl_set< vcl_pair<double,double> > query_sift_right_plus;
        vcl_set< vcl_pair<double,double> > query_sift_right_minus;


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

        vgl_polygon<double> model_polygon(1);
        vgl_polygon<double> query_polygon(1);

        if ( !flag )
        {
            sc1->get_polygon(model_polygon);
            sc2->get_polygon(query_polygon,width);
        }
        else
        {
            sc1->get_polygon(query_polygon,width);
            sc2->get_polygon(model_polygon);
        }


        bool add_curve=true;

        if ( query_dart_curves_.count(query_key1) ||
             query_dart_curves_.count(query_key2) )
        {
            add_curve=false;
        }

        if ( add_curve )
        {
            for (unsigned int s = 0; s < query_polygon.num_sheets(); ++s)
            {
                for (unsigned int p = 0; p < query_polygon[s].size(); ++p)
                {
                    query_dart_curves_[query_key1].push_back(
                        query_polygon[s][p]);
                }
            }
                
            query_dart_curves_[query_key1].push_back(query_polygon[0][0]);
             
        }

        
        for (unsigned j = 0; j < map_list[index].size(); ++j) 
        {
            vcl_pair<int, int> cor = map_list[index][j];

            // Compute sift for both images

            // Shock Point 1 from Model
            double radius_ps1        = sc1->time(cor.first);
            double theta_ps1         = sc1->theta(cor.first);

            // Shock Point 2 from Query
            double radius_ps2        = sc2->time(cor.second);
            double theta_ps2         = sc2->theta(cor.second);

            double ratio=1.0;
            double R1=radius_ps1;
            double R2=radius_ps2;

            ratio=(R2/R1);
                

            double r1 =0.0;

            while ( r1 <= R1 )
            {
                double r2=r1*ratio;
                for ( unsigned int p=0; p < 2 ; ++p )
                {
                    vgl_point_2d<double> ps1;
                    vgl_point_2d<double> ps2;

                    if ( p ==  0 )
                    {
                        if ( r1== 0 )
                        {
                            ps1=sc1->sh_pt(cor.first);
                            ps2=sc2->sh_pt(cor.second);
                        }
                        else
                        {
                            ps1=sc1->fragment_pt(cor.first,r1);
                            ps2=sc2->fragment_pt(cor.second,r2);
                        }
                    }
                    else
                    {
                        if ( r1== 0 )
                        {
                            ps1=sc1->sh_pt(cor.first);
                            ps2=sc2->sh_pt(cor.second);
                        }
                        else
                        {
                            ps1=sc1->fragment_pt(cor.first,-1.0*r1);
                            ps2=sc2->fragment_pt(cor.second,-1.0*r2);
                        }
                    }

                    if ( !flag )
                    {
                        
                        ps1.set(ps1.x()/model_scale_ratio,
                                ps1.y()/model_scale_ratio);
                        ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                                ps2.y()/query_scale_ratio);

                        vcl_pair<double,double> ps1_key(ps1.x(),ps1.y());
                        vcl_pair<double,double> ps2_key(ps2.x(),ps2.y());

                        model_sift.insert(ps1_key);
                        query_sift.insert(ps2_key);

                        if ( cor.first <= midpoint_index )
                        {
                            model_sift_left.insert(ps1_key);
                            query_sift_left.insert(ps2_key);

                            if ( p == 0  )
                            {
                                model_sift_left_plus.insert(ps1_key);
                                query_sift_left_plus.insert(ps2_key);
                            }
                            else
                            {
                                model_sift_left_minus.insert(ps1_key);
                                query_sift_left_minus.insert(ps2_key);
                            }
                        }
                        else
                        {
                            model_sift_right.insert(ps1_key);
                            query_sift_right.insert(ps2_key);

                            if ( p == 0  )
                            {
                                model_sift_right_plus.insert(ps1_key);
                                query_sift_right_plus.insert(ps2_key);
                            }
                            else
                            {
                                model_sift_right_minus.insert(ps1_key);
                                query_sift_right_minus.insert(ps2_key);
                            }

                        }
                    }
                    else
                    {
                        ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                                ps1.y()/query_scale_ratio);
                        ps2.set(ps2.x()/model_scale_ratio,
                                ps2.y()/model_scale_ratio);

                        vcl_pair<double,double> ps1_key(ps1.x(),ps1.y());
                        vcl_pair<double,double> ps2_key(ps2.x(),ps2.y());

                        model_sift.insert(ps2_key);
                        query_sift.insert(ps1_key);

                        if ( cor.first <= midpoint_index )
                        {
                            model_sift_left.insert(ps2_key);
                            query_sift_left.insert(ps1_key);

                            if ( p == 0  )
                            {
                                model_sift_left_plus.insert(ps2_key);
                                query_sift_left_plus.insert(ps1_key);
                            }
                            else
                            {
                                model_sift_left_minus.insert(ps2_key);
                                query_sift_left_minus.insert(ps1_key);
                            }
                        }
                        else
                        {
                            model_sift_right.insert(ps2_key);
                            query_sift_right.insert(ps1_key);

                            if ( p == 0  )
                            {
                                model_sift_right_plus.insert(ps2_key);
                                query_sift_right_plus.insert(ps1_key);
                            }
                            else
                            {
                                model_sift_right_minus.insert(ps2_key);
                                query_sift_right_minus.insert(ps1_key);
                            }


                        }
                                               
                    }
              
                }
                r1+=step_size;
            }
           
        }

        vcl_vector<double> phoc_model;
        vcl_vector<double> phoc_query;

        // Compute Level 0 color distance 
        {
            vcl_string model_sift_title("");
            vcl_string query_sift_title("");

            if ( debug )
            {
                vcl_stringstream model_sift_stream;
                model_sift_stream<<"Model_sift_"<<index;

                vcl_stringstream query_sift_stream;
                query_sift_stream<<"Query_sift_"<<index;
                
                model_sift_title=model_sift_stream.str();
                query_sift_title=query_sift_stream.str();
            }

            unsigned int start=phoc_model.size();
            
            compute_color_region_hist(model_sift,
                                      model_channel_1,
                                      model_channel_2,
                                      model_channel_3,
                                      phoc_model,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      model_sift_title);

            compute_color_region_hist(query_sift,
                                      query_channel_1,
                                      query_channel_2,
                                      query_channel_3,
                                      phoc_query,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      query_sift_title);

            unsigned int stop=phoc_model.size();

            if ( debug )
            {

                vcl_stringstream bstream;
                bstream<<index;

                vcl_string model_string="Model_level_0_hist_dart_"+
                    bstream.str()+".txt";
                vcl_string query_string="Query_level_0_hist_dart_"+
                    bstream.str()+".txt";

                vcl_ofstream model_hist(model_string.c_str());
                vcl_ofstream query_hist(query_string.c_str());

                for ( unsigned int ph=start; ph < stop ; ++ph)
                {
                    model_hist<<phoc_model[ph]<<vcl_endl;
                    query_hist<<phoc_query[ph]<<vcl_endl;
                    
                }

                model_hist.close();
                query_hist.close();

            }

        }

        // Compute Level 1 distance and descriptors
        {
            vcl_string model_sift_left_title("");
            vcl_string query_sift_left_title("");

            vcl_string model_sift_right_title("");
            vcl_string query_sift_right_title("");

            if ( debug )
            {
                vcl_stringstream model_sift_left_stream;
                model_sift_left_stream<<"Model_sift_left_"<<index;

                vcl_stringstream query_sift_left_stream;
                query_sift_left_stream<<"Query_sift_left_"<<index;
                
                model_sift_left_title=model_sift_left_stream.str();
                query_sift_left_title=query_sift_left_stream.str();

                vcl_stringstream model_sift_right_stream;
                model_sift_right_stream<<"Model_sift_right_"<<index;

                vcl_stringstream query_sift_right_stream;
                query_sift_right_stream<<"Query_sift_right_"<<index;
                
                model_sift_right_title=model_sift_right_stream.str();
                query_sift_right_title=query_sift_right_stream.str();

            }

            unsigned int start=phoc_model.size();

            compute_color_region_hist(model_sift_left,
                                      model_channel_1,
                                      model_channel_2,
                                      model_channel_3,
                                      phoc_model,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      model_sift_left_title);

            compute_color_region_hist(model_sift_right,
                                      model_channel_1,
                                      model_channel_2,
                                      model_channel_3,
                                      phoc_model,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      model_sift_right_title);

            compute_color_region_hist(query_sift_left,
                                      query_channel_1,
                                      query_channel_2,
                                      query_channel_3,
                                      phoc_query,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      query_sift_left_title);

            compute_color_region_hist(query_sift_right,
                                      query_channel_1,
                                      query_channel_2,
                                      query_channel_3,
                                      phoc_query,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      query_sift_right_title);

            unsigned int stop=phoc_model.size();

            if ( debug )
            {
                vcl_stringstream bstream;
                bstream<<index;

                vcl_string model_string="Model_level_1_hist_dart_"+
                    bstream.str()+".txt";
                vcl_string query_string="Query_level_1_hist_dart_"+
                    bstream.str()+".txt";

                vcl_ofstream model_hist(model_string.c_str());
                vcl_ofstream query_hist(query_string.c_str());

                for ( unsigned int ph=start; ph < stop ; ++ph)
                {
                    model_hist<<phoc_model[ph]<<vcl_endl;
                    query_hist<<phoc_query[ph]<<vcl_endl;
                    
                }

                model_hist.close();
                query_hist.close();

            }

        }


        // Compute Level 2 distances and descriptors
        {
            vnl_vector<double> model_sift_left_plus_descr;
            vnl_vector<double> model_sift_right_plus_descr;
            vnl_vector<double> model_sift_left_minus_descr;
            vnl_vector<double> model_sift_right_minus_descr;

            vcl_string model_sift_left_plus_title("");
            vcl_string model_sift_left_minus_title("");

            vcl_string model_sift_right_plus_title("");
            vcl_string model_sift_right_minus_title("");

            if ( debug )
            {
                vcl_stringstream model_sift_left_plus_stream;
                model_sift_left_plus_stream<<"Model_sift_left_plus_"<<index;

                vcl_stringstream model_sift_left_minus_stream;
                model_sift_left_minus_stream<<"Model_sift_left_minus_"<<index;
                
                model_sift_left_plus_title=model_sift_left_plus_stream.str();
                model_sift_left_minus_title=model_sift_left_minus_stream.str();

                vcl_stringstream model_sift_right_plus_stream;
                model_sift_right_plus_stream<<"Model_sift_right_plus_"<<index;

                vcl_stringstream model_sift_right_minus_stream;
                model_sift_right_minus_stream<<"Model_sift_right_minus_"<<index;
                
                model_sift_right_plus_title=model_sift_right_plus_stream.str();
                model_sift_right_minus_title=
                    model_sift_right_minus_stream.str();

            }

            unsigned int start=phoc_model.size();

            compute_color_region_hist(model_sift_left_plus,
                                      model_channel_1,
                                      model_channel_2,
                                      model_channel_3,
                                      phoc_model,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      model_sift_left_plus_title);

            compute_color_region_hist(model_sift_right_plus,
                                      model_channel_1,
                                      model_channel_2,
                                      model_channel_3,
                                      phoc_model,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      model_sift_right_plus_title);

            compute_color_region_hist(model_sift_left_minus,
                                      model_channel_1,
                                      model_channel_2,
                                      model_channel_3,
                                      phoc_model,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      model_sift_left_minus_title);

            compute_color_region_hist(model_sift_right_minus,
                                      model_channel_1,
                                      model_channel_2,
                                      model_channel_3,
                                      phoc_model,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      model_sift_right_minus_title);

            vcl_string query_sift_left_plus_title("");
            vcl_string query_sift_left_minus_title("");

            vcl_string query_sift_right_plus_title("");
            vcl_string query_sift_right_minus_title("");

            if ( debug )
            {
                vcl_stringstream query_sift_left_plus_stream;
                query_sift_left_plus_stream<<"Query_sift_left_plus_"<<index;

                vcl_stringstream query_sift_left_minus_stream;
                query_sift_left_minus_stream<<"Query_sift_left_minus_"<<index;
                
                query_sift_left_plus_title=query_sift_left_plus_stream.str();
                query_sift_left_minus_title=query_sift_left_minus_stream.str();

                vcl_stringstream query_sift_right_plus_stream;
                query_sift_right_plus_stream<<"Query_sift_right_plus_"<<index;

                vcl_stringstream query_sift_right_minus_stream;
                query_sift_right_minus_stream<<"Query_sift_right_minus_"<<index;
                
                query_sift_right_plus_title=query_sift_right_plus_stream.str();
                query_sift_right_minus_title=
                    query_sift_right_minus_stream.str();

            }

            compute_color_region_hist(query_sift_left_plus,
                                      query_channel_1,
                                      query_channel_2,
                                      query_channel_3,
                                      phoc_query,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      query_sift_left_plus_title);

            compute_color_region_hist(query_sift_right_plus,
                                      query_channel_1,
                                      query_channel_2,
                                      query_channel_3,
                                      phoc_query,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      query_sift_right_plus_title);

            compute_color_region_hist(query_sift_left_minus,
                                      query_channel_1,
                                      query_channel_2,
                                      query_channel_3,
                                      phoc_query,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      query_sift_left_minus_title);

            compute_color_region_hist(query_sift_right_minus,
                                      query_channel_1,
                                      query_channel_2,
                                      query_channel_3,
                                      phoc_query,
                                      dbskfg_match_bag_of_fragments::DEFAULT,
                                      query_sift_right_minus_title);

            unsigned int stop=phoc_model.size();

            if ( debug )
            {

                vcl_stringstream bstream;
                bstream<<index;

                vcl_string model_string="Model_level_2_hist_dart_"+
                    bstream.str()+".txt";
                vcl_string query_string="Query_level_2_hist_dart_"+
                    bstream.str()+".txt";

                vcl_ofstream model_hist(model_string.c_str());
                vcl_ofstream query_hist(query_string.c_str());

                for ( unsigned int ph=start; ph < stop ; ++ph)
                {
                    model_hist<<phoc_model[ph]<<vcl_endl;
                    query_hist<<phoc_query[ph]<<vcl_endl;
                    
                }

                model_hist.close();
                query_hist.close();

            }

        }

        vnl_vector<double> descr1(phoc_model.size(),0.0);
        vnl_vector<double> descr2(phoc_query.size(),0.0);

        double model_sum(0.0);
        double query_sum(0.0);
        for ( unsigned int p=0; p < phoc_model.size() ; ++p)
        {
            descr1.put(p,phoc_model[p]);
            descr2.put(p,phoc_query[p]);

            model_sum+=phoc_model[p];
            query_sum+=phoc_query[p];
        }

        descr1/=model_sum;
        descr2/=query_sum;

        double dart_distance=chi_squared_distance(descr1,descr2);
        sift_diff+=dart_distance;

        dart_distances.push_back(dart_distance);

        if ( debug )
        {
            vcl_cout<<"Tree 1 dart ("
                    <<path_map[index].first.first
                    <<","
                    <<path_map[index].first.second
                    <<") Tree 2 dart ("
                    <<path_map[index].second.first
                    <<","
                    <<path_map[index].second.second
                    <<") L2 distance: "
                    <<dart_distance<<vcl_endl;
        }

    }


    vcl_pair<double,double> app_diff(sift_diff,sift_diff/map_list.size());

    // vcl_cout<<"Unormalized diff: "<<app_diff.first<<vcl_endl;
    // vcl_cout<<"Average diff:     "<<app_diff.second<<vcl_endl;
    return app_diff;
}

vcl_pair<double,double> dbskfg_match_bag_of_fragments::
compute_body_centric_sift(
    vcl_vector<dbskr_scurve_sptr>& curve_list1,
    vcl_vector<dbskr_scurve_sptr>& curve_list2,
    vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
    vcl_vector< pathtable_key >& path_map,
    vcl_vector<double>& dart_distances,
    vl_sift_pix* model_red_grad_data,
    vl_sift_pix* query_red_grad_data,
    vl_sift_pix* model_green_grad_data,
    vl_sift_pix* query_green_grad_data,
    vl_sift_pix* model_blue_grad_data,
    vl_sift_pix* query_blue_grad_data,
    VlSiftFilt* model_sift_filter,
    VlSiftFilt* query_sift_filter,
    double model_scale_ratio,
    double query_scale_ratio,
    bool flag,
    double width)
{
    
    bool debug=false;

    // Create gradient maps
    vil_image_view<double> red_model_grad_map(
        model_sift_filter->width,
        model_sift_filter->height);
    vil_image_view<double> green_model_grad_map(
        model_sift_filter->width,
        model_sift_filter->height);
    vil_image_view<double> blue_model_grad_map(
        model_sift_filter->width,
        model_sift_filter->height);
    vil_image_view<double> red_model_angle_map(
        model_sift_filter->width,
        model_sift_filter->height);
    vil_image_view<double> green_model_angle_map(
        model_sift_filter->width,
        model_sift_filter->height);
    vil_image_view<double> blue_model_angle_map(
        model_sift_filter->width,
        model_sift_filter->height);

    vil_image_view<double> red_query_grad_map(
        query_sift_filter->width,
        query_sift_filter->height);
    vil_image_view<double> green_query_grad_map(
        query_sift_filter->width,
        query_sift_filter->height);
    vil_image_view<double> blue_query_grad_map(
        query_sift_filter->width,
        query_sift_filter->height);
    vil_image_view<double> red_query_angle_map(
        query_sift_filter->width,
        query_sift_filter->height);
    vil_image_view<double> green_query_angle_map(
        query_sift_filter->width,
        query_sift_filter->height);
    vil_image_view<double> blue_query_angle_map(
        query_sift_filter->width,
        query_sift_filter->height);
        
    unsigned int coord=0;
    for ( unsigned int nj =0 ; nj < red_model_grad_map.nj() ; ++nj)
    {
        for ( unsigned int ni=0; ni < red_model_grad_map.ni() ; ++ni)
        {
            red_model_grad_map(ni,nj)=model_red_grad_data[coord];
            red_model_angle_map(ni,nj)=model_red_grad_data[coord+1];

            green_model_grad_map(ni,nj)=model_green_grad_data[coord];
            green_model_angle_map(ni,nj)=model_green_grad_data[coord+1];

            blue_model_grad_map(ni,nj)=model_blue_grad_data[coord];
            blue_model_angle_map(ni,nj)=model_blue_grad_data[coord+1];

            coord=coord+2;
        }
    }

    coord=0;
    for ( unsigned int nj =0 ; nj < red_query_grad_map.nj() ; ++nj)
    {
        for ( unsigned int ni=0; ni < red_query_grad_map.ni() ; ++ni)
        {
            red_query_grad_map(ni,nj)=query_red_grad_data[coord];
            red_query_angle_map(ni,nj)=query_red_grad_data[coord+1];

            green_query_grad_map(ni,nj)=query_green_grad_data[coord];
            green_query_angle_map(ni,nj)=query_green_grad_data[coord+1];

            blue_query_grad_map(ni,nj)=query_blue_grad_data[coord];
            blue_query_angle_map(ni,nj)=query_blue_grad_data[coord+1];

            coord=coord+2;
        }
    }
 
    VlFloatVectorComparisonFunction Chi2_distance =    
        vl_get_vector_comparison_function_f(VlDistanceChi2);

    double sift_diff= 0.0;
    double step_size=1.0;
    
    // Get matching pairs
    for (unsigned index = 0; index < map_list.size(); index++) 
    {
        dbskr_scurve_sptr sc1 = curve_list1[index];
        dbskr_scurve_sptr sc2 = curve_list2[index];

        unsigned int midpoint_index=sc1->midpoint_index();

        // Compute three level pyramid histogram

        // Level 0
        vcl_set< vcl_pair<double,double> > model_sift;
        vcl_set< vcl_pair<double,double> > query_sift;

        // Level 1
        vcl_set< vcl_pair<double,double> > model_sift_left;
        vcl_set< vcl_pair<double,double> > model_sift_right;
        vcl_set< vcl_pair<double,double> > query_sift_left;
        vcl_set< vcl_pair<double,double> > query_sift_right;

        // Level 2
        vcl_set< vcl_pair<double,double> > model_sift_left_plus;
        vcl_set< vcl_pair<double,double> > model_sift_left_minus;
        vcl_set< vcl_pair<double,double> > model_sift_right_plus;
        vcl_set< vcl_pair<double,double> > model_sift_right_minus;

        vcl_set< vcl_pair<double,double> > query_sift_left_plus;
        vcl_set< vcl_pair<double,double> > query_sift_left_minus;
        vcl_set< vcl_pair<double,double> > query_sift_right_plus;
        vcl_set< vcl_pair<double,double> > query_sift_right_minus;


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

        vgl_polygon<double> model_polygon(1);
        vgl_polygon<double> query_polygon(1);

        if ( !flag )
        {
            sc1->get_polygon(model_polygon);
            sc2->get_polygon(query_polygon,width);
        }
        else
        {
            sc1->get_polygon(query_polygon,width);
            sc2->get_polygon(model_polygon);
        }


        bool add_curve=true;

        if ( query_dart_curves_.count(query_key1) ||
             query_dart_curves_.count(query_key2) )
        {
            add_curve=false;
        }

        if ( add_curve )
        {
            for (unsigned int s = 0; s < query_polygon.num_sheets(); ++s)
            {
                for (unsigned int p = 0; p < query_polygon[s].size(); ++p)
                {
                    query_dart_curves_[query_key1].push_back(
                        query_polygon[s][p]);
                }
            }
                
            query_dart_curves_[query_key1].push_back(query_polygon[0][0]);
             
        }

        
        for (unsigned j = 0; j < map_list[index].size(); ++j) 
        {
            vcl_pair<int, int> cor = map_list[index][j];

            // Compute sift for both images

            // Shock Point 1 from Model
            double radius_ps1        = sc1->time(cor.first);
            double theta_ps1         = sc1->theta(cor.first);

            // Shock Point 2 from Query
            double radius_ps2        = sc2->time(cor.second);
            double theta_ps2         = sc2->theta(cor.second);

            double ratio=1.0;
            double R1=radius_ps1;
            double R2=radius_ps2;

            ratio=(R2/R1);
                

            double r1 =0.0;

            while ( r1 <= R1 )
            {
                double r2=r1*ratio;
                for ( unsigned int p=0; p < 2 ; ++p )
                {
                    vgl_point_2d<double> ps1;
                    vgl_point_2d<double> ps2;

                    if ( p ==  0 )
                    {
                        if ( r1== 0 )
                        {
                            ps1=sc1->sh_pt(cor.first);
                            ps2=sc2->sh_pt(cor.second);
                        }
                        else
                        {
                            ps1=sc1->fragment_pt(cor.first,r1);
                            ps2=sc2->fragment_pt(cor.second,r2);
                        }
                    }
                    else
                    {
                        if ( r1== 0 )
                        {
                            ps1=sc1->sh_pt(cor.first);
                            ps2=sc2->sh_pt(cor.second);
                        }
                        else
                        {
                            ps1=sc1->fragment_pt(cor.first,-1.0*r1);
                            ps2=sc2->fragment_pt(cor.second,-1.0*r2);
                        }
                    }

                    if ( !flag )
                    {
                        
                        ps1.set(ps1.x()/model_scale_ratio,
                                ps1.y()/model_scale_ratio);
                        ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                                ps2.y()/query_scale_ratio);

                        vcl_pair<double,double> ps1_key(ps1.x(),ps1.y());
                        vcl_pair<double,double> ps2_key(ps2.x(),ps2.y());

                        model_sift.insert(ps1_key);
                        query_sift.insert(ps2_key);

                        if ( cor.first <= midpoint_index )
                        {
                            model_sift_left.insert(ps1_key);
                            query_sift_left.insert(ps2_key);

                            if ( p == 0  )
                            {
                                model_sift_left_plus.insert(ps1_key);
                                query_sift_left_plus.insert(ps2_key);
                            }
                            else
                            {
                                model_sift_left_minus.insert(ps1_key);
                                query_sift_left_minus.insert(ps2_key);
                            }
                        }
                        else
                        {
                            model_sift_right.insert(ps1_key);
                            query_sift_right.insert(ps2_key);

                            if ( p == 0  )
                            {
                                model_sift_right_plus.insert(ps1_key);
                                query_sift_right_plus.insert(ps2_key);
                            }
                            else
                            {
                                model_sift_right_minus.insert(ps1_key);
                                query_sift_right_minus.insert(ps2_key);
                            }

                        }
                    }
                    else
                    {
                        ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                                ps1.y()/query_scale_ratio);
                        ps2.set(ps2.x()/model_scale_ratio,
                                ps2.y()/model_scale_ratio);

                        vcl_pair<double,double> ps1_key(ps1.x(),ps1.y());
                        vcl_pair<double,double> ps2_key(ps2.x(),ps2.y());

                        model_sift.insert(ps2_key);
                        query_sift.insert(ps1_key);

                        if ( cor.first <= midpoint_index )
                        {
                            model_sift_left.insert(ps2_key);
                            query_sift_left.insert(ps1_key);

                            if ( p == 0  )
                            {
                                model_sift_left_plus.insert(ps2_key);
                                query_sift_left_plus.insert(ps1_key);
                            }
                            else
                            {
                                model_sift_left_minus.insert(ps2_key);
                                query_sift_left_minus.insert(ps1_key);
                            }
                        }
                        else
                        {
                            model_sift_right.insert(ps2_key);
                            query_sift_right.insert(ps1_key);

                            if ( p == 0  )
                            {
                                model_sift_right_plus.insert(ps2_key);
                                query_sift_right_plus.insert(ps1_key);
                            }
                            else
                            {
                                model_sift_right_minus.insert(ps2_key);
                                query_sift_right_minus.insert(ps1_key);
                            }


                        }
                                               
                    }
              
                }
                r1+=step_size;
            }
                        
        }

        vcl_vector<double> phog_model;
        vcl_vector<double> phog_query;

        // Compute Level 0 distance and descriptors
        {
            vcl_string model_sift_title("");
            vcl_string query_sift_title("");

            if ( debug )
            {
                vcl_stringstream model_sift_stream;
                model_sift_stream<<"Model_sift_"<<index;

                vcl_stringstream query_sift_stream;
                query_sift_stream<<"Query_sift_"<<index;
                
                model_sift_title=model_sift_stream.str();
                query_sift_title=query_sift_stream.str();
            }

            unsigned int start=phog_model.size();

            compute_grad_region_hist(model_sift,
                                     red_model_grad_map,
                                     red_model_angle_map,
                                     green_model_grad_map,
                                     green_model_angle_map,
                                     blue_model_grad_map,
                                     blue_model_angle_map,
                                     phog_model,
                                     model_sift_title);
        
            compute_grad_region_hist(query_sift,
                                     red_query_grad_map,
                                     red_query_angle_map,
                                     green_query_grad_map,
                                     green_query_angle_map,
                                     blue_query_grad_map,
                                     blue_query_angle_map,
                                     phog_query,
                                     query_sift_title);

            unsigned int stop=phog_model.size();

            if ( debug )
            {

                vcl_stringstream bstream;
                bstream<<index;

                vcl_string model_string="Model_level_0_hist_dart_"+
                    bstream.str()+".txt";
                vcl_string query_string="Query_level_0_hist_dart_"+
                    bstream.str()+".txt";

                vcl_ofstream model_hist(model_string.c_str());
                vcl_ofstream query_hist(query_string.c_str());

                for ( unsigned int ph=start; ph < stop ; ++ph)
                {
                    model_hist<<phog_model[ph]<<vcl_endl;
                    query_hist<<phog_query[ph]<<vcl_endl;
                    
                }

                model_hist.close();
                query_hist.close();

            }
        }

        // Compute Level 1 distance and descriptors
        {
            vcl_string model_sift_left_title("");
            vcl_string query_sift_left_title("");

            vcl_string model_sift_right_title("");
            vcl_string query_sift_right_title("");

            if ( debug )
            {
                vcl_stringstream model_sift_left_stream;
                model_sift_left_stream<<"Model_sift_left_"<<index;

                vcl_stringstream query_sift_left_stream;
                query_sift_left_stream<<"Query_sift_left_"<<index;
                
                model_sift_left_title=model_sift_left_stream.str();
                query_sift_left_title=query_sift_left_stream.str();

                vcl_stringstream model_sift_right_stream;
                model_sift_right_stream<<"Model_sift_right_"<<index;

                vcl_stringstream query_sift_right_stream;
                query_sift_right_stream<<"Query_sift_right_"<<index;
                
                model_sift_right_title=model_sift_right_stream.str();
                query_sift_right_title=query_sift_right_stream.str();

            }

            unsigned int start=phog_model.size();

            compute_grad_region_hist(model_sift_left,
                                     red_model_grad_map,
                                     red_model_angle_map,
                                     green_model_grad_map,
                                     green_model_angle_map,
                                     blue_model_grad_map,
                                     blue_model_angle_map,
                                     phog_model,
                                     model_sift_left_title);

            compute_grad_region_hist(model_sift_right,
                                     red_model_grad_map,
                                     red_model_angle_map,
                                     green_model_grad_map,
                                     green_model_angle_map,
                                     blue_model_grad_map,
                                     blue_model_angle_map,
                                     phog_model,
                                     model_sift_right_title);

            compute_grad_region_hist(query_sift_left,
                                     red_query_grad_map,
                                     red_query_angle_map,
                                     green_query_grad_map,
                                     green_query_angle_map,
                                     blue_query_grad_map,
                                     blue_query_angle_map,
                                     phog_query,
                                     query_sift_left_title);

            compute_grad_region_hist(query_sift_right,
                                     red_query_grad_map,
                                     red_query_angle_map,
                                     green_query_grad_map,
                                     green_query_angle_map,
                                     blue_query_grad_map,
                                     blue_query_angle_map,
                                     phog_query,
                                     query_sift_right_title);

            unsigned int stop=phog_model.size();

            if ( debug )
            {

                vcl_stringstream bstream;
                bstream<<index;

                vcl_string model_string="Model_level_1_hist_dart_"+
                    bstream.str()+".txt";
                vcl_string query_string="Query_level_1_hist_dart_"+
                    bstream.str()+".txt";

                vcl_ofstream model_hist(model_string.c_str());
                vcl_ofstream query_hist(query_string.c_str());

                for ( unsigned int ph=start; ph < stop ; ++ph)
                {
                    model_hist<<phog_model[ph]<<vcl_endl;
                    query_hist<<phog_query[ph]<<vcl_endl;
                    
                }

                model_hist.close();
                query_hist.close();

            }
        }


        // Compute Level 2 distances and descriptors
        {
            vcl_string model_sift_left_plus_title("");
            vcl_string model_sift_left_minus_title("");

            vcl_string model_sift_right_plus_title("");
            vcl_string model_sift_right_minus_title("");

            if ( debug )
            {
                vcl_stringstream model_sift_left_plus_stream;
                model_sift_left_plus_stream<<"Model_sift_left_plus_"<<index;

                vcl_stringstream model_sift_left_minus_stream;
                model_sift_left_minus_stream<<"Model_sift_left_minus_"<<index;
                
                model_sift_left_plus_title=model_sift_left_plus_stream.str();
                model_sift_left_minus_title=model_sift_left_minus_stream.str();

                vcl_stringstream model_sift_right_plus_stream;
                model_sift_right_plus_stream<<"Model_sift_right_plus_"<<index;

                vcl_stringstream model_sift_right_minus_stream;
                model_sift_right_minus_stream<<"Model_sift_right_minus_"<<index;
                
                model_sift_right_plus_title=model_sift_right_plus_stream.str();
                model_sift_right_minus_title=
                    model_sift_right_minus_stream.str();

            }

            unsigned int start=phog_model.size();

            compute_grad_region_hist(model_sift_left_plus,
                                     red_model_grad_map,
                                     red_model_angle_map,
                                     green_model_grad_map,
                                     green_model_angle_map,
                                     blue_model_grad_map,
                                     blue_model_angle_map,
                                     phog_model,
                                     model_sift_left_plus_title);

            compute_grad_region_hist(model_sift_right_plus,
                                     red_model_grad_map,
                                     red_model_angle_map,
                                     green_model_grad_map,
                                     green_model_angle_map,
                                     blue_model_grad_map,
                                     blue_model_angle_map,
                                     phog_model,
                                     model_sift_right_plus_title);

            compute_grad_region_hist(model_sift_left_minus,
                                     red_model_grad_map,
                                     red_model_angle_map,
                                     green_model_grad_map,
                                     green_model_angle_map,
                                     blue_model_grad_map,
                                     blue_model_angle_map,
                                     phog_model,
                                     model_sift_left_minus_title);

            compute_grad_region_hist(model_sift_right_minus,
                                     red_model_grad_map,
                                     red_model_angle_map,
                                     green_model_grad_map,
                                     green_model_angle_map,
                                     blue_model_grad_map,
                                     blue_model_angle_map,
                                     phog_model,
                                     model_sift_right_minus_title);

            vcl_string query_sift_left_plus_title("");
            vcl_string query_sift_left_minus_title("");

            vcl_string query_sift_right_plus_title("");
            vcl_string query_sift_right_minus_title("");

            if ( debug )
            {
                vcl_stringstream query_sift_left_plus_stream;
                query_sift_left_plus_stream<<"Query_sift_left_plus_"<<index;

                vcl_stringstream query_sift_left_minus_stream;
                query_sift_left_minus_stream<<"Query_sift_left_minus_"<<index;
                
                query_sift_left_plus_title=query_sift_left_plus_stream.str();
                query_sift_left_minus_title=query_sift_left_minus_stream.str();

                vcl_stringstream query_sift_right_plus_stream;
                query_sift_right_plus_stream<<"Query_sift_right_plus_"<<index;

                vcl_stringstream query_sift_right_minus_stream;
                query_sift_right_minus_stream<<"Query_sift_right_minus_"<<index;
                
                query_sift_right_plus_title=query_sift_right_plus_stream.str();
                query_sift_right_minus_title=
                    query_sift_right_minus_stream.str();

            }

            compute_grad_region_hist(query_sift_left_plus,
                                     red_query_grad_map,
                                     red_query_angle_map,
                                     green_query_grad_map,
                                     green_query_angle_map,
                                     blue_query_grad_map,
                                     blue_query_angle_map,
                                     phog_query,
                                     query_sift_left_plus_title);

            compute_grad_region_hist(query_sift_right_plus,
                                     red_query_grad_map,
                                     red_query_angle_map,
                                     green_query_grad_map,
                                     green_query_angle_map,
                                     blue_query_grad_map,
                                     blue_query_angle_map,
                                     phog_query,
                                     query_sift_right_plus_title);

            compute_grad_region_hist(query_sift_left_minus,
                                     red_query_grad_map,
                                     red_query_angle_map,
                                     green_query_grad_map,
                                     green_query_angle_map,
                                     blue_query_grad_map,
                                     blue_query_angle_map,
                                     phog_query,
                                     query_sift_left_minus_title);

            compute_grad_region_hist(query_sift_right_minus,
                                     red_query_grad_map,
                                     red_query_angle_map,
                                     green_query_grad_map,
                                     green_query_angle_map,
                                     blue_query_grad_map,
                                     blue_query_angle_map,
                                     phog_query,
                                     query_sift_right_minus_title);

            unsigned int stop=phog_model.size();

            if ( debug )
            {

                vcl_stringstream bstream;
                bstream<<index;

                vcl_string model_string="Model_level_2_hist_dart_"+
                    bstream.str()+".txt";
                vcl_string query_string="Query_level_2_hist_dart_"+
                    bstream.str()+".txt";

                vcl_ofstream model_hist(model_string.c_str());
                vcl_ofstream query_hist(query_string.c_str());

                for ( unsigned int ph=start; ph < stop ; ++ph)
                {
                    model_hist<<phog_model[ph]<<vcl_endl;
                    query_hist<<phog_query[ph]<<vcl_endl;
                    
                }

                model_hist.close();
                query_hist.close();

            }

        }

        vnl_vector<double> descr1(phog_model.size(),0.0);
        vnl_vector<double> descr2(phog_query.size(),0.0);

        double model_sum(0.0);
        double query_sum(0.0);
        for ( unsigned int p=0; p < phog_model.size() ; ++p)
        {
            descr1.put(p,phog_model[p]);
            descr2.put(p,phog_query[p]);

            model_sum+=phog_model[p];
            query_sum+=phog_query[p];
        }

        descr1/=model_sum;
        descr2/=query_sum;

        double dart_distance=chi_squared_distance(descr1,descr2);
        sift_diff+=dart_distance;

        dart_distances.push_back(dart_distance);

        if ( debug )
        {
            vcl_cout<<"Tree 1 dart ("
                    <<path_map[index].first.first
                    <<","
                    <<path_map[index].first.second
                    <<") Tree 2 dart ("
                    <<path_map[index].second.first
                    <<","
                    <<path_map[index].second.second
                    <<") L2 distance: "
                    <<dart_distance<<vcl_endl;
        }
    }
    


    vcl_pair<double,double> app_diff(sift_diff,sift_diff/map_list.size());

    // vcl_cout<<"Unormalized diff: "<<app_diff.first<<vcl_endl;
    // vcl_cout<<"Average diff:     "<<app_diff.second<<vcl_endl;
    return app_diff;
}

double dbskfg_match_bag_of_fragments::descr_cost(
    vgl_point_2d<double>& model_pt,
    double& model_radius,
    double& model_theta,
    vgl_point_2d<double>& query_pt,
    double& query_radius,
    double& query_theta,
    vl_sift_pix* model_red_grad_data,
    vl_sift_pix* query_red_grad_data,
    vl_sift_pix* model_green_grad_data,
    vl_sift_pix* query_green_grad_data,
    vl_sift_pix* model_blue_grad_data,
    vl_sift_pix* query_blue_grad_data,
    VlSiftFilt* model_sift_filter,
    VlSiftFilt* query_sift_filter)
{

    VlFloatVectorComparisonFunction Chi2_distance =    
      vl_get_vector_comparison_function_f (VlDistanceChi2) ;
    double sift_diff= 0.0;

    vl_sift_pix descr_ps1_red[128];
    memset(descr_ps1_red, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps1_green[128];
    memset(descr_ps1_green, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps1_blue[128];
    memset(descr_ps1_blue, 0, sizeof(vl_sift_pix)*128);
    

    vl_sift_pix descr_ps2_red[128];
    memset(descr_ps2_red, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps2_green[128];
    memset(descr_ps2_green, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps2_blue[128];
    memset(descr_ps2_blue, 0, sizeof(vl_sift_pix)*128);
    

    vl_sift_calc_raw_descriptor(model_sift_filter,
                                model_red_grad_data,
                                descr_ps1_red,
                                model_sift_filter->width,
                                model_sift_filter->height,
                                model_pt.x(),
                                model_pt.y(),
                                model_radius,
                                model_theta);

    vl_sift_calc_raw_descriptor(model_sift_filter,
                                model_green_grad_data,
                                descr_ps1_green,
                                model_sift_filter->width,
                                model_sift_filter->height,
                                model_pt.x(),
                                model_pt.y(),
                                model_radius,
                                model_theta);

    vl_sift_calc_raw_descriptor(model_sift_filter,
                                model_blue_grad_data,
                                descr_ps1_blue,
                                model_sift_filter->width,
                                model_sift_filter->height,
                                model_pt.x(),
                                model_pt.y(),
                                model_radius,
                                model_theta);

    vl_sift_calc_raw_descriptor(query_sift_filter,
                                query_red_grad_data,
                                descr_ps2_red,
                                query_sift_filter->width,
                                query_sift_filter->height,
                                query_pt.x(),
                                query_pt.y(),
                                query_radius,
                                query_theta);

    vl_sift_calc_raw_descriptor(query_sift_filter,
                                query_green_grad_data,
                                descr_ps2_green,
                                query_sift_filter->width,
                                query_sift_filter->height,
                                query_pt.x(),
                                query_pt.y(),
                                query_radius,
                                query_theta);

    vl_sift_calc_raw_descriptor(query_sift_filter,
                                query_blue_grad_data,
                                descr_ps2_blue,
                                query_sift_filter->width,
                                query_sift_filter->height,
                                query_pt.x(),
                                query_pt.y(),
                                query_radius,
                                query_theta);


  
    double min_radius = vnl_math::min(model_radius*2.0,query_radius*2.0);

    vnl_vector<vl_sift_pix> descr1(384,0.0);
    vnl_vector<vl_sift_pix> descr2(384,0.0);

    for ( unsigned int d=0; d < 128 ; ++d)
    {
        descr1.put(d,descr_ps1_red[d]);
        descr1.put(d+128,descr_ps1_green[d]);
        descr1.put(d+256,descr_ps1_blue[d]);

        descr2.put(d,descr_ps2_red[d]);
        descr2.put(d+128,descr_ps2_green[d]);
        descr2.put(d+256,descr_ps2_blue[d]);

        
    }

    // {
    //     vcl_ofstream model_stream("opp_sift_model.txt");
    //     vcl_ofstream query_stream("opp_sift_query.txt");

    //     for ( unsigned int d=0; d < 128 ; ++d)
    //     {
    //         model_stream<<descr_ps1_red[d]<<vcl_endl;
    //         query_stream<<descr_ps2_red[d]<<vcl_endl;
    //     }

    //     for ( unsigned int d=0; d < 128 ; ++d)
    //     {
    //         model_stream<<descr_ps1_green[d]<<vcl_endl;
    //         query_stream<<descr_ps2_green[d]<<vcl_endl;
    //     }

    //     for ( unsigned int d=0; d < 128 ; ++d)
    //     {
    //         model_stream<<descr_ps1_blue[d]<<vcl_endl;
    //         query_stream<<descr_ps2_blue[d]<<vcl_endl;
    //     }

    //     model_stream.close();
    //     query_stream.close();
    // }

    // vnl_vector<vl_sift_pix> descr1;
    // vnl_vector<vl_sift_pix> descr2;

    // int nbp=4;
    // if ( min_radius >= 8.0 )
    // {
    //     nbp=4;
    //     descr1.set_size(384);
    //     descr2.set_size(384);
    //     for ( unsigned int d=0; d < 128 ; ++d)
    //     {
    //         descr1.put(d,descr_ps1_red[d]);
    //         descr1.put(d+128,descr_ps1_green[d]);
    //         descr1.put(d+256,descr_ps1_blue[d]);
            
    //         descr2.put(d,descr_ps2_red[d]);
    //         descr2.put(d+128,descr_ps2_green[d]);
    //         descr2.put(d+256,descr_ps2_blue[d]);
        
        
    //     }

    // }
    // else if ( min_radius < 8.0 && min_radius >= 4.0 )
    // {
    //     nbp=2;
    //     descr1=compress_sift(descr_ps1_red,
    //                          descr_ps1_green,
    //                          descr_ps1_blue,
    //                          nbp);
    //     descr2=compress_sift(descr_ps2_red,
    //                          descr_ps2_green,
    //                          descr_ps2_blue,
    //                          nbp);
            
    // }
    // else
    // {
    //     nbp=1;
    //     descr1=compress_sift(descr_ps1_red,
    //                          descr_ps1_green,
    //                          descr_ps1_blue,
    //                          nbp);
    //     descr2=compress_sift(descr_ps2_red,
    //                          descr_ps2_green,
    //                          descr_ps2_blue,
    //                          nbp);

    // }


    // descr1.normalize();
    // descr2.normalize();

    vl_sift_pix result_final[1];

    vl_eval_vector_comparison_on_all_pairs_f(result_final,
                                             descr1.size(),
                                             descr1.data_block(),
                                             1,
                                             descr2.data_block(),
                                             1,
                                             Chi2_distance);

    return (0.5)*result_final[0];
    
}

double dbskfg_match_bag_of_fragments::descr_cost_fv(
    vgl_point_2d<double>& model_pt,
    double& model_radius,
    double& model_theta,
    vgl_point_2d<double>& query_pt,
    double& query_radius,
    double& query_theta,
    vl_sift_pix* model_red_grad_data,
    vl_sift_pix* query_red_grad_data,
    vl_sift_pix* model_green_grad_data,
    vl_sift_pix* query_green_grad_data,
    vl_sift_pix* model_blue_grad_data,
    vl_sift_pix* query_blue_grad_data,
    VlSiftFilt* model_sift_filter,
    VlSiftFilt* query_sift_filter)
{

    VlFloatVectorComparisonFunction FV_distance =    
      vl_get_vector_comparison_function_f (VlDistanceL2) ;

    vl_sift_pix descr_ps1_red[128];
    memset(descr_ps1_red, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps1_green[128];
    memset(descr_ps1_green, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps1_blue[128];
    memset(descr_ps1_blue, 0, sizeof(vl_sift_pix)*128);
    

    vl_sift_pix descr_ps2_red[128];
    memset(descr_ps2_red, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps2_green[128];
    memset(descr_ps2_green, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps2_blue[128];
    memset(descr_ps2_blue, 0, sizeof(vl_sift_pix)*128);
    

    vl_sift_calc_raw_descriptor(model_sift_filter,
                                model_red_grad_data,
                                descr_ps1_red,
                                model_sift_filter->width,
                                model_sift_filter->height,
                                model_pt.x(),
                                model_pt.y(),
                                model_radius,
                                model_theta);

    vl_sift_calc_raw_descriptor(model_sift_filter,
                                model_green_grad_data,
                                descr_ps1_green,
                                model_sift_filter->width,
                                model_sift_filter->height,
                                model_pt.x(),
                                model_pt.y(),
                                model_radius,
                                model_theta);

    vl_sift_calc_raw_descriptor(model_sift_filter,
                                model_blue_grad_data,
                                descr_ps1_blue,
                                model_sift_filter->width,
                                model_sift_filter->height,
                                model_pt.x(),
                                model_pt.y(),
                                model_radius,
                                model_theta);

    vl_sift_calc_raw_descriptor(query_sift_filter,
                                query_red_grad_data,
                                descr_ps2_red,
                                query_sift_filter->width,
                                query_sift_filter->height,
                                query_pt.x(),
                                query_pt.y(),
                                query_radius,
                                query_theta);

    vl_sift_calc_raw_descriptor(query_sift_filter,
                                query_green_grad_data,
                                descr_ps2_green,
                                query_sift_filter->width,
                                query_sift_filter->height,
                                query_pt.x(),
                                query_pt.y(),
                                query_radius,
                                query_theta);

    vl_sift_calc_raw_descriptor(query_sift_filter,
                                query_blue_grad_data,
                                descr_ps2_blue,
                                query_sift_filter->width,
                                query_sift_filter->height,
                                query_pt.x(),
                                query_pt.y(),
                                query_radius,
                                query_theta);


  
    double min_radius = vnl_math::min(model_radius*2.0,query_radius*2.0);

    vnl_vector<vl_sift_pix> descr1(384,0.0);
    vnl_vector<vl_sift_pix> descr2(384,0.0);

    for ( unsigned int d=0; d < 128 ; ++d)
    {
        descr1.put(d,descr_ps1_red[d]);
        descr1.put(d+128,descr_ps1_green[d]);
        descr1.put(d+256,descr_ps1_blue[d]);

        descr2.put(d,descr_ps2_red[d]);
        descr2.put(d+128,descr_ps2_green[d]);
        descr2.put(d+256,descr_ps2_blue[d]);

        
    }



    int encoding_size = 2 * descr1.size() * keywords_;
    
    // allocate space for the encoding
    float* model_fv = (float *) vl_malloc(
        sizeof(float) * encoding_size);

    // allocate space for the encoding
    float* query_fv = (float *)
        vl_malloc(sizeof(float) * encoding_size);


    // run fisher encoding
    vl_fisher_encode
        (model_fv, VL_TYPE_FLOAT,
         means_cg_, descr1.size(), keywords_,
         covariances_cg_,
         priors_cg_,
         descr1.data_block(), 1,
         VL_FISHER_FLAG_IMPROVED);

    // run fisher encoding
    vl_fisher_encode
        (query_fv, VL_TYPE_FLOAT,
         means_cg_, descr2.size(), keywords_,
         covariances_cg_,
         priors_cg_,
         descr2.data_block(), 1,
         VL_FISHER_FLAG_IMPROVED);

    vl_sift_pix result_final[1];

    vl_eval_vector_comparison_on_all_pairs_f(result_final,
                                             encoding_size,
                                             model_fv,
                                             1,
                                             query_fv,
                                             1,
                                             FV_distance);


    vl_free(model_fv);
    vl_free(query_fv);

    return result_final[0];
    
}

double dbskfg_match_bag_of_fragments::descr_cost_enriched_sift(
    vgl_point_2d<double>& model_pt,
    double& model_radius,
    double& model_theta,
    vgl_point_2d<double>& query_pt,
    double& query_radius,
    double& query_theta,
    vil_image_view<double>& model_channel_1,
    vil_image_view<double>& model_channel_2,
    vil_image_view<double>& model_channel_3,
    vil_image_view<double>& query_channel_1,
    vil_image_view<double>& query_channel_2,
    vil_image_view<double>& query_channel_3,
    vl_sift_pix* model_red_grad_data,
    vl_sift_pix* query_red_grad_data,
    vl_sift_pix* model_green_grad_data,
    vl_sift_pix* query_green_grad_data,
    vl_sift_pix* model_blue_grad_data,
    vl_sift_pix* query_blue_grad_data,
    VlSiftFilt* model_sift_filter,
    VlSiftFilt* query_sift_filter)
{

    vnl_vector_fixed<double,3> model_values;
    vnl_vector_fixed<double,3> query_values;


    model_values[0] = vil_bilin_interp_safe(model_channel_1,
                                            model_pt.x(),
                                            model_pt.y());

    model_values[1] = vil_bilin_interp_safe(model_channel_2,
                                            model_pt.x(),
                                            model_pt.y());

    model_values[2] = vil_bilin_interp_safe(model_channel_3,
                                            model_pt.x(),
                                            model_pt.y());

    query_values[0] = vil_bilin_interp_safe(query_channel_1,
                                            query_pt.x(),
                                            query_pt.y());

    query_values[1] = vil_bilin_interp_safe(query_channel_2,
                                            query_pt.x(),
                                            query_pt.y());

    query_values[2] = vil_bilin_interp_safe(query_channel_3,
                                            query_pt.x(),
                                            query_pt.y());

    VlFloatVectorComparisonFunction Chi2_distance =    
      vl_get_vector_comparison_function_f (VlDistanceChi2) ;
    double sift_diff= 0.0;

    vl_sift_pix descr_ps1_red[128];
    memset(descr_ps1_red, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps1_green[128];
    memset(descr_ps1_green, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps1_blue[128];
    memset(descr_ps1_blue, 0, sizeof(vl_sift_pix)*128);
    

    vl_sift_pix descr_ps2_red[128];
    memset(descr_ps2_red, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps2_green[128];
    memset(descr_ps2_green, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps2_blue[128];
    memset(descr_ps2_blue, 0, sizeof(vl_sift_pix)*128);
    

    vl_sift_calc_raw_descriptor(model_sift_filter,
                                model_red_grad_data,
                                descr_ps1_red,
                                model_sift_filter->width,
                                model_sift_filter->height,
                                model_pt.x(),
                                model_pt.y(),
                                model_radius,
                                model_theta);

    vl_sift_calc_raw_descriptor(model_sift_filter,
                                model_green_grad_data,
                                descr_ps1_green,
                                model_sift_filter->width,
                                model_sift_filter->height,
                                model_pt.x(),
                                model_pt.y(),
                                model_radius,
                                model_theta);

    vl_sift_calc_raw_descriptor(model_sift_filter,
                                model_blue_grad_data,
                                descr_ps1_blue,
                                model_sift_filter->width,
                                model_sift_filter->height,
                                model_pt.x(),
                                model_pt.y(),
                                model_radius,
                                model_theta);

    vl_sift_calc_raw_descriptor(query_sift_filter,
                                query_red_grad_data,
                                descr_ps2_red,
                                query_sift_filter->width,
                                query_sift_filter->height,
                                query_pt.x(),
                                query_pt.y(),
                                query_radius,
                                query_theta);

    vl_sift_calc_raw_descriptor(query_sift_filter,
                                query_green_grad_data,
                                descr_ps2_green,
                                query_sift_filter->width,
                                query_sift_filter->height,
                                query_pt.x(),
                                query_pt.y(),
                                query_radius,
                                query_theta);

    vl_sift_calc_raw_descriptor(query_sift_filter,
                                query_blue_grad_data,
                                descr_ps2_blue,
                                query_sift_filter->width,
                                query_sift_filter->height,
                                query_pt.x(),
                                query_pt.y(),
                                query_radius,
                                query_theta);


    vl_sift_pix result_red[1];
    vl_sift_pix result_green[1];
    vl_sift_pix result_blue[1];
    vl_sift_pix result_final[1];

    vnl_vector<vl_sift_pix> descr1(387,0.0);
    vnl_vector<vl_sift_pix> descr2(387,0.0);

    for ( unsigned int d=0; d < 128 ; ++d)
    {
        descr1.put(d,descr_ps1_red[d]);
        descr1.put(d+128,descr_ps1_green[d]);
        descr1.put(d+256,descr_ps1_blue[d]);

        descr2.put(d,descr_ps2_red[d]);
        descr2.put(d+128,descr_ps2_green[d]);
        descr2.put(d+256,descr_ps2_blue[d]);

        
    }


    descr1.put(384,model_values[0]);
    descr1.put(385,model_values[1]);
    descr1.put(386,model_values[2]);

    descr2.put(384,query_values[0]);
    descr2.put(385,query_values[1]);
    descr2.put(386,query_values[2]);

    descr1.normalize();
    descr2.normalize();

    vl_eval_vector_comparison_on_all_pairs_f(result_final,
                                             384,
                                             descr1.data_block(),
                                             1,
                                             descr2.data_block(),
                                             1,
                                             Chi2_distance);

    return (0.5)*result_final[0];
    
}

void dbskfg_match_bag_of_fragments::compute_descr(
    vgl_point_2d<double>& pt,
    double& radius,
    double& theta,
    vl_sift_pix* grad_data,
    VlSiftFilt* sift_filter,
    vnl_vector<vl_sift_pix>& descriptor)
{


    vl_sift_pix descr_ps1[128];
    memset(descr_ps1, 0, sizeof(vl_sift_pix)*128);
            

    vl_sift_calc_raw_descriptor(sift_filter,
                                grad_data,
                                descr_ps1,
                                sift_filter->width,
                                sift_filter->height,
                                pt.x(),
                                pt.y(),
                                radius,
                                theta);


    for ( unsigned int d=0; d < 128 ; ++d)
    {
        descriptor.put(d,descr_ps1[d]);
    }

    // descriptor.normalize();

}

void dbskfg_match_bag_of_fragments::compute_descr(
    vgl_point_2d<double>& pt,
    double& radius,
    double& theta,
    vl_sift_pix* red_grad_data,
    vl_sift_pix* green_grad_data,
    vl_sift_pix* blue_grad_data,
    VlSiftFilt* sift_filter,
    vnl_vector<vl_sift_pix>& descriptor)
{


    vl_sift_pix descr_ps1_red[128];
    memset(descr_ps1_red, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps1_green[128];
    memset(descr_ps1_green, 0, sizeof(vl_sift_pix)*128);
    
    vl_sift_pix descr_ps1_blue[128];
    memset(descr_ps1_blue, 0, sizeof(vl_sift_pix)*128);
        

    vl_sift_calc_raw_descriptor(sift_filter,
                                red_grad_data,
                                descr_ps1_red,
                                sift_filter->width,
                                sift_filter->height,
                                pt.x(),
                                pt.y(),
                                radius,
                                theta);

    vl_sift_calc_raw_descriptor(sift_filter,
                                green_grad_data,
                                descr_ps1_green,
                                sift_filter->width,
                                sift_filter->height,
                                pt.x(),
                                pt.y(),
                                radius,
                                theta);

    vl_sift_calc_raw_descriptor(sift_filter,
                                blue_grad_data,
                                descr_ps1_blue,
                                sift_filter->width,
                                sift_filter->height,
                                pt.x(),
                                pt.y(),
                                radius,
                                theta);


    for ( unsigned int d=0; d < 128 ; ++d)
    {
        descriptor.put(d,descr_ps1_red[d]);
        descriptor.put(d+128,descr_ps1_green[d]);
        descriptor.put(d+256,descr_ps1_blue[d]);
        
    }

    // descriptor.normalize();

}


void dbskfg_match_bag_of_fragments::compute_descr_fv(
    vgl_point_2d<double>& pt,
    double& radius,
    double& theta,
    vl_sift_pix* red_grad_data,
    vl_sift_pix* green_grad_data,
    vl_sift_pix* blue_grad_data,
    VlSiftFilt* sift_filter,
    vnl_vector<vl_sift_pix>& fv_descriptor)
{
    
    double scale_1=16;
    double scale_2=12;
    double scale_3=8;
    double scale_4=4;

    vnl_vector<vl_sift_pix> scale_1_descriptor(384,0.0);
    vnl_vector<vl_sift_pix> scale_2_descriptor(384,0.0);
    vnl_vector<vl_sift_pix> scale_3_descriptor(384,0.0);
    vnl_vector<vl_sift_pix> scale_4_descriptor(384,0.0);

    compute_descr(pt,
                  scale_1,
                  theta,
                  red_grad_data,
                  green_grad_data,
                  blue_grad_data,
                  sift_filter,
                  scale_1_descriptor);

    compute_descr(pt,
                  scale_2,
                  theta,
                  red_grad_data,
                  green_grad_data,
                  blue_grad_data,
                  sift_filter,
                  scale_2_descriptor);

    compute_descr(pt,
                  scale_3,
                  theta,
                  red_grad_data,
                  green_grad_data,
                  blue_grad_data,
                  sift_filter,
                  scale_3_descriptor);

    compute_descr(pt,
                  scale_4,
                  theta,
                  red_grad_data,
                  green_grad_data,
                  blue_grad_data,
                  sift_filter,
                  scale_4_descriptor);

    vnl_vector<vl_sift_pix> sift_block(4*scale_1_descriptor.size(),0.0);

    for ( unsigned int s=0; s <scale_1_descriptor.size() ; ++s)
    {
        sift_block.put(s,scale_1_descriptor[s]);
        sift_block.put(s+scale_1_descriptor.size(),scale_2_descriptor[s]);
        sift_block.put(s+2*scale_1_descriptor.size(),scale_3_descriptor[s]);
        sift_block.put(s+3*scale_1_descriptor.size(),scale_4_descriptor[s]);
        
    }

    int encoding_size = 2 * scale_1_descriptor.size() * keywords_;

    fv_descriptor.clear();
    fv_descriptor.set_size(encoding_size);

    // run fisher encoding
    vl_fisher_encode
        (fv_descriptor.data_block(), VL_TYPE_FLOAT,
         means_cg_, scale_1_descriptor.size(), keywords_,
         covariances_cg_,
         priors_cg_,
         sift_block.data_block(), 4,
         VL_FISHER_FLAG_IMPROVED);

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
            double scaled_value=vnl_math::sgn(log_mapping[r][c])*
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
    bool flag,
    double model_scale_ratio,
    double query_scale_ratio,
    double width)
{

    // vcl_ofstream model_stream("model_points.txt");
    // vcl_ofstream query_stream("query_points.txt");

    vcl_vector< rgrl_feature_sptr > pts1;
    vcl_vector< rgrl_feature_sptr > pts2;
    for (unsigned i = 0; i < map_list.size(); i++)
    {
        
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];
        
        for (unsigned j = 0; j < map_list[i].size(); ++j) 
        {
            vcl_pair<int, int> cor = map_list[i][j];
            
            vnl_vector<double> v(2);

            // Shock Point 1 from Model
            vgl_point_2d<double> ps1  = sc1->sh_pt(cor.first);
            double radius_ps1         = sc1->time(cor.first);
            double theta_ps1          = sc1->theta(cor.first);
            double phi_ps1            = sc1->phi(cor.first);

            vgl_point_2d<double> ps2  = sc2->sh_pt(cor.second);
            double radius_ps2         = sc2->time(cor.second);
            double theta_ps2          = sc2->theta(cor.second);
            double phi_ps2            = sc2->phi(cor.second);

            if ( !flag )
            {
                
                ps1.set(ps1.x()/model_scale_ratio,
                        ps1.y()/model_scale_ratio);
                ps2.set(vcl_fabs(width-(ps2.x()/query_scale_ratio)),
                        ps2.y()/query_scale_ratio);
                
                radius_ps1 = radius_ps1/model_scale_ratio;
                radius_ps2 = radius_ps2/query_scale_ratio;

                if ( width )
                {
                    theta_ps2=vnl_math::pi-theta_ps2;
                }

                vgl_point_2d<double> pt_p_sc1 = _translatePoint(
                    ps1, 
                    theta_ps1+phi_ps1, 
                    radius_ps1);
                vgl_point_2d<double> pt_m_sc1 = _translatePoint(
                    ps1, 
                    theta_ps1-phi_ps1, 
                    radius_ps1);

                vgl_point_2d<double> pt_p_sc2 = _translatePoint(
                    ps2, 
                    theta_ps2+phi_ps2, 
                    radius_ps2);
                vgl_point_2d<double> pt_m_sc2 = _translatePoint(
                    ps2, 
                    theta_ps2-phi_ps2, 
                    radius_ps2);

                // model_stream<<pt_p_sc1.x()<<" "  
                //             <<pt_p_sc1.y()<<" "
                //             <<pt_m_sc1.x()<<" "
                //             <<pt_m_sc1.y()<<vcl_endl;

                // query_stream<<pt_p_sc2.x()<<" "  
                //             <<pt_p_sc2.y()<<" "
                //             <<pt_m_sc2.x()<<" "
                //             <<pt_m_sc2.y()<<vcl_endl;
                
                // scurve 1
                v[0] = pt_m_sc1.x();
                v[1] = pt_m_sc1.y();
                pts1.push_back(new rgrl_feature_point(v));
        
                v[0] = pt_p_sc1.x();
                v[1] = pt_p_sc1.y();
                pts1.push_back(new rgrl_feature_point(v));

                // scurve 2
                v[0] = pt_m_sc2.x();
                v[1] = pt_m_sc2.y();
                pts2.push_back(new rgrl_feature_point(v));
        
                v[0] = pt_p_sc2.x();
                v[1] = pt_p_sc2.y();
                pts2.push_back(new rgrl_feature_point(v));
                
            }
            else
            {

                ps1.set(vcl_fabs(width-(ps1.x()/query_scale_ratio)),
                        ps1.y()/query_scale_ratio);
                ps2.set(ps2.x()/model_scale_ratio,
                        ps2.y()/model_scale_ratio);

                radius_ps1 = radius_ps1/query_scale_ratio;
                radius_ps2 = radius_ps2/model_scale_ratio;

                if ( width )
                {
                    theta_ps1=vnl_math::pi-theta_ps1;
                }

                vgl_point_2d<double> pt_p_sc1 = _translatePoint(
                    ps1, 
                    theta_ps1+phi_ps1, 
                    radius_ps1);
                vgl_point_2d<double> pt_m_sc1 = _translatePoint(
                    ps1, 
                    theta_ps1-phi_ps1, 
                    radius_ps1);

                vgl_point_2d<double> pt_p_sc2 = _translatePoint(
                    ps2, 
                    theta_ps2+phi_ps2, 
                    radius_ps2);
                vgl_point_2d<double> pt_m_sc2 = _translatePoint(
                    ps2, 
                    theta_ps2-phi_ps2, 
                    radius_ps2);

                // query_stream<<pt_p_sc1.x()<<" "  
                //             <<pt_p_sc1.y()<<" "
                //             <<pt_m_sc1.x()<<" "
                //             <<pt_m_sc1.y()<<vcl_endl;

                // model_stream<<pt_p_sc2.x()<<" "  
                //             <<pt_p_sc2.y()<<" "
                //             <<pt_m_sc2.x()<<" "
                //             <<pt_m_sc2.y()<<vcl_endl;

                // scurve 1
                v[0] = pt_m_sc1.x();
                v[1] = pt_m_sc1.y();
                pts1.push_back(new rgrl_feature_point(v));
        
                v[0] = pt_p_sc1.x();
                v[1] = pt_p_sc1.y();
                pts1.push_back(new rgrl_feature_point(v));

                // scurve 2
                v[0] = pt_m_sc2.x();
                v[1] = pt_m_sc2.y();
                pts2.push_back(new rgrl_feature_point(v));
        
                v[0] = pt_p_sc2.x();
                v[1] = pt_p_sc2.y();
                pts2.push_back(new rgrl_feature_point(v));

 
            }
        }
    }
 
    // model_stream.close();
    // query_stream.close();

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
    double frob_norm(0.0);

    // Match model to query
    match_two_graphs_root_node_orig(model_os_tree,
                                    query_os_tree,
                                    norm_shape_cost,
                                    norm_shape_cost_length,
                                    app_diff,
                                    norm_app_cost,
                                    rgb_avg_cost,
                                    frob_norm);

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
    GradColorSpace color_space)
{
    vil_image_view<vxl_byte> image = input_image->get_view();
    unsigned int w = image.ni(); 
    unsigned int h = image.nj();
    o1.set_size(w,h);
    o2.set_size(w,h);
    o3.set_size(w,h);

    if ( color_space == LAB_2 )
    {
        convert_RGB_to_Lab(image,
                           o1,
                           o2,
                           o3);
    }
    else if ( color_space == RGB )
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
                    if ( o3(c,r) > 0.0 )
                    {
                        o1(c,r)=o1(c,r)/o3(c,r);
                        o2(c,r)=o2(c,r)/o3(c,r);
                    }
                }
            }
        }
    }


}

void dbskfg_match_bag_of_fragments::write_out_dart_data()
{
    
    vcl_string dart_file=vul_file::strip_extension(output_binary_file_);
    dart_file=dart_file+"_part_distances.txt";

    vcl_ofstream model_file(dart_file.c_str());

    vcl_map<int,vcl_vector<double> >::iterator it;
    for ( it = part_distances_.begin(); it != part_distances_.end() ; ++it)
    {

        vcl_vector<double> dd=(*it).second;

        for ( unsigned int v=0; v < dd.size() ; ++v)
        {
            if ( v == dd.size() -1 )
            {
                model_file<<dd[v]<<vcl_endl;
            }
            else
            {
                model_file<<dd[v]<<" ";
            }
        }
    }

    model_file.close();

    // {
    //     vcl_map<vcl_pair<unsigned int,unsigned int>,
    //         vcl_vector<vgl_point_2d<double> > >
    //         ::iterator it;
        
        
    //     for ( it = query_dart_curves_.begin() ; it != query_dart_curves_.end();
    //           ++it)
    //     {
    //         vcl_pair<int,int> pair=(*it).first;
    //         vcl_vector<vgl_point_2d<double> > curve=(*it).second;
    //         model_file<<pair.first<<","<<pair.second<<vcl_endl;
    //         model_file<<curve.size()<<vcl_endl;
    //         for ( unsigned int c=0; c < curve.size() ; ++c)
    //         {
    //             model_file<<curve[c].x()<<","<<curve[c].y()<<vcl_endl;
    //         }
    //     }
    // }

    // // Write out model
    // {

    //     model_file<<model_dart_distances_.size()<<vcl_endl;
    //     vcl_map<unsigned int,
    //         vcl_vector< vcl_pair<vcl_pair<unsigned int,unsigned int>,
    //         double> > >::
    //         iterator it;
    //     for ( it = model_dart_distances_.begin() ; it != model_dart_distances_
    //               .end(); ++it)
    //     {
    //         vcl_vector< vcl_pair<vcl_pair<unsigned int,unsigned 
    //             int>,double> > vec=
    //             (*it).second;
    //         vcl_vector< vcl_pair<vcl_pair<unsigned int,
    //             unsigned int>, dbskr_scurve_sptr > > p2 =
    //             model_dart_curves_[(*it).first];

    //         model_file<<vec.size()<<vcl_endl;
    //         for ( unsigned int v=0; v < vec.size() ; ++v)
    //         {
    //             vcl_pair<vcl_pair<unsigned int,unsigned int>,
    //                 double> pair=vec[v];
    //             model_file<<pair.first.first<<","<<pair.first.second<<vcl_endl;
    //             model_file<<pair.second<<vcl_endl;

                
    //             dbskr_scurve_sptr curve=p2[v].second;
    //             vgl_polygon<double> model_polygon(1);
    //             curve->get_polygon(model_polygon);
                
    //             model_file<<model_polygon[0].size()+1<<vcl_endl;

    //             for (unsigned int s = 0; s < model_polygon.num_sheets(); ++s)
    //             {
    //                 for (unsigned int p = 0; p < model_polygon[s].size(); ++p)
    //                 {
    //                     model_file<<model_polygon[s][p].x()<<","
    //                               <<model_polygon[s][p].y()<<vcl_endl;
    //                 }
    //             }

    //             model_file<<model_polygon[0][0].x()<<","
    //                       <<model_polygon[0][0].y()<<vcl_endl;
                
    //         }
            
    //     }
        
        
    // }
    // model_file.close();








}


inline void dbskfg_match_bag_of_fragments::compute_color_over_sift
(VlSiftFilt const *f,
 int width, int height,
 double x, double y,
 double sigma,
 double angle0,
 vcl_set<vcl_pair<double,double> >& samples)
{
  double const magnif = f-> magnif ;

  int NBO=8;
  int NBP=4;
  int          w      = width ;
  int          h      = height ;
  int const    xo     = 2 ;         /* x-stride */
  int const    yo     = 2 * w ;     /* y-stride */

  int          xi     = (int) (x + 0.5) ;
  int          yi     = (int) (y + 0.5) ;

  double const st0    = sin (angle0) ;
  double const ct0    = cos (angle0) ;
  double const SBP    = magnif * sigma + VL_EPSILON_D ;
  int    const W      = floor
      (sqrt(2.0) * SBP * (NBP + 1) / 2.0 + 0.5) ;

  int const binto = 1 ;          /* bin theta-stride */
  int const binyo = NBO * NBP ;  /* bin y-stride */
  int const binxo = NBO ;        /* bin x-stride */

  int dxi, dyi ;

  /* check bounds */
  if(xi    <  0               ||
     xi    >= w               ||
     yi    <  0               ||
     yi    >= h -    1        )
      return ;
  /*
   * Process pixels in the intersection of the image rectangle
   * (1,1)-(M-1,N-1) and the keypoint bounding box.
   */
  for(dyi =  VL_MAX(- W,   - yi   ) ;
      dyi <= VL_MIN(+ W, h - yi -1) ; ++ dyi) 
  {

      for(dxi =  VL_MAX(- W,   - xi   ) ;
          dxi <= VL_MIN(+ W, w - xi -1) ; ++ dxi) 
      {

          /* fractional displacement */
          vl_sift_pix dx = xi + dxi - x;
          vl_sift_pix dy = yi + dyi - y;

          /* get the displacement normalized w.r.t. the keypoint
             orientation and extension */
          vl_sift_pix nx = ( ct0 * dx + st0 * dy) / SBP ;
          vl_sift_pix ny = (-st0 * dx + ct0 * dy) / SBP ;

          /* The sample will be distributed in 8 adjacent bins.
             We start from the ``lower-left'' bin. */
          int         binx = (int)vl_floor_f (nx - 0.5) ;
          int         biny = (int)vl_floor_f (ny - 0.5) ;
          int         dbinx ;
          int         dbiny ;
          int         dbint ;

          /* Distribute the current sample into the 8 adjacent bins*/
          for(dbinx = 0 ; dbinx < 2 ; ++dbinx)
          {
              for(dbiny = 0 ; dbiny < 2 ; ++dbiny)
              {
                  for(dbint = 0 ; dbint < 2 ; ++dbint)
                  {

                      if (binx + dbinx >= - (NBP/2) &&
                          binx + dbinx <    (NBP/2) &&
                          biny + dbiny >= - (NBP/2) &&
                          biny + dbiny <    (NBP/2) ) 
                      {

                          vcl_pair<double,double> 
                              pair(xi+dxi,yi+dyi);
                          samples.insert(pair);
                      }
                  }
              }
          }
      }
  }


  // vcl_ofstream stream("sift_samples.txt");

  // vcl_set<vcl_pair<double,double> >::iterator it;
  // for ( it = samples.begin() ; it != samples.end() ; ++it)
  // {
  //     stream<<(*it).first<<" "<<(*it).second<<vcl_endl;

  // }
  // stream.close();

}

void dbskfg_match_bag_of_fragments::compute_sift_along_curve(
    dbskr_scurve_sptr scurve,
    vnl_matrix<vl_sift_pix>& descriptors,
    vl_sift_pix* red_grad_data,
    vl_sift_pix* green_grad_data,
    vl_sift_pix* blue_grad_data,
    VlSiftFilt* sift_filter,
    double sift_scale,
    double scale_ratio,
    double width)
{
    for ( unsigned int i = 0 ; i < descriptors.cols() ; ++i)
    {
        vgl_point_2d<double> ps1 = scurve->sh_pt(i);
        double radius_ps1        = sift_scale;
        double theta_ps1         = scurve->theta(i);

        ps1.set(vcl_fabs(width-(ps1.x()/scale_ratio)),
                ps1.y()/scale_ratio);
        radius_ps1 = (radius_ps1/scale_ratio)/2.0;


        vnl_vector<vl_sift_pix> descriptor(384,0.0);
        
        compute_descr(ps1,
                      radius_ps1,
                      theta_ps1,
                      red_grad_data,
                      green_grad_data,
                      blue_grad_data,
                      sift_filter,
                      descriptor);
        
        descriptors.set_column(i,descriptor);
    }







}

void dbskfg_match_bag_of_fragments::compute_grad_region_hist(
    vcl_set<vcl_pair<double,double> >& samples,
    vil_image_view<double>& o1_grad_map,
    vil_image_view<double>& o1_angle_map,
    vil_image_view<double>& o2_grad_map,
    vil_image_view<double>& o2_angle_map,
    vil_image_view<double>& o3_grad_map,
    vil_image_view<double>& o3_angle_map,
    vcl_vector<double>& descr,
    vcl_string title)
{

    bsta_histogram<double> o1_hist(2.0*vnl_math::pi,8);
    bsta_histogram<double> o2_hist(2.0*vnl_math::pi,8);
    bsta_histogram<double> o3_hist(2.0*vnl_math::pi,8);
    
    vcl_set<vcl_pair<double,double> >::iterator pit;
    for ( pit = samples.begin() ; pit != samples.end() ; ++pit)
    {
        double o1_mag = vil_bilin_interp_safe(o1_grad_map,
                                               (*pit).first,
                                               (*pit).second);
        double o1_angle = vil_bilin_interp_safe(o1_angle_map,
                                                (*pit).first,
                                                (*pit).second);
        
        double o2_mag = vil_bilin_interp_safe(o2_grad_map,
                                              (*pit).first,
                                              (*pit).second);
        double o2_angle = vil_bilin_interp_safe(o2_angle_map,
                                                (*pit).first,
                                                (*pit).second);
                
        double o3_mag = vil_bilin_interp_safe(o3_grad_map,
                                              (*pit).first,
                                              (*pit).second);
        double o3_angle = vil_bilin_interp_safe(o3_angle_map,
                                                (*pit).first,
                                                (*pit).second);
              
        o1_hist.upcount(o1_angle,o1_mag);
        o2_hist.upcount(o2_angle,o2_mag);
        o3_hist.upcount(o3_angle,o3_mag);
    }

    vcl_vector<double> o1_counts=o1_hist.count_array();
    vcl_vector<double> o2_counts=o2_hist.count_array();
    vcl_vector<double> o3_counts=o3_hist.count_array();

    for ( unsigned int i=0; i < o1_counts.size() ; ++i)
    {
        descr.push_back(o1_counts[i]);
    }

    for ( unsigned int i=0; i < o2_counts.size() ; ++i)
    {
        descr.push_back(o2_counts[i]);
    }

    for ( unsigned int i=0; i < o3_counts.size() ; ++i)
    {
        descr.push_back(o3_counts[i]);
    }

    if ( title.size() )
    {
        vcl_string samp_title=title+"_samples.txt";
        vcl_ofstream output(samp_title.c_str());
        for ( pit = samples.begin() ; pit != samples.end() ; ++pit)
        {
            output<<(*pit).first<<" "<<(*pit).second<<vcl_endl;


        }
        output.close();
        
    }






}



void dbskfg_match_bag_of_fragments::compute_color_region_hist(
    vcl_set<vcl_pair<double,double> >& samples,
    vil_image_view<double>& o1,
    vil_image_view<double>& o2,
    vil_image_view<double>& o3,
    vcl_vector<double>& descr,
    LabBinType bintype,
    vcl_string title)
{

    double min_l(0.0),max_l(0.0);
    double min_a(0.0),max_a(0.0);
    double min_b(0.0),max_b(0.0);
    unsigned int bins_l(0),bins_a(0),bins_b(0);

    if ( raw_color_space_ == dbskfg_match_bag_of_fragments::LAB )
    {
        min_l=0; max_l=100;
        min_a=-110; max_a=110;
        min_b=-110; max_b=110;
        bins_l=5;
        bins_a=10;
        bins_b=10;
    }
    else if( raw_color_space_ == dbskfg_match_bag_of_fragments::RGB_2 )
    {
        min_l=0; max_l=255;
        min_a=0; max_a=255;
        min_b=0; max_b=255;

        bins_l=8;
        bins_a=8;
        bins_b=8;


    }
    else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::OPP_2 )
    {
        min_l=-180; max_l=180;
        min_a=-208; max_a=208;
        min_b=0; max_b=441;

        bins_l=10;
        bins_a=10;
        bins_b=10;


    }
    else
    {
        min_b=0; max_b=441;
        min_l=-180/max_b; max_l=180/max_b;
        min_a=-208/max_b; max_a=208/max_b;
            

        bins_l=10;
        bins_a=10;
        bins_b=10;


    }

   
    bsta_joint_histogram_3d<double> color_hist(
        min_l,max_l,bins_l,
        min_a,max_a,bins_a,
        min_b,max_b,bins_b);

    bsta_histogram<double> l_hist(min_l,max_l,bins_l);
    bsta_histogram<double> a_hist(min_a,max_a,bins_a);
    bsta_histogram<double> b_hist(min_b,max_b,bins_b);

    vcl_set<vcl_pair<double,double> >::iterator pit;
    for ( pit = samples.begin() ; pit != samples.end() ; ++pit)
    {
        double L_value = vil_bilin_interp_safe(o1,
                                              (*pit).first,
                                               (*pit).second);
        double a_value = vil_bilin_interp_safe(o2,
                                               (*pit).first,
                                               (*pit).second);        
        double b_value = vil_bilin_interp_safe(o3,
                                               (*pit).first,
                                               (*pit).second);
        
        color_hist.upcount(L_value,0,a_value,0,b_value,1);

        l_hist.upcount(L_value,1);
        a_hist.upcount(a_value,1);
        b_hist.upcount(b_value,1);

    }

    if ( bintype == dbskfg_match_bag_of_fragments::DEFAULT )
    {
        for (unsigned int l = 0; l<bins_l; l++)
        {
            for (unsigned int a = 0; a<bins_a; a++)
            {
                for (unsigned int b = 0; b<bins_b; b++)
                {
                    double value=color_hist.get_count(l,a,b);
                    descr.push_back(value);
                }
            }
        }
    }
    else
    {
        vcl_vector<double> l_counts=l_hist.count_array();
        vcl_vector<double> a_counts=a_hist.count_array();
        vcl_vector<double> b_counts=b_hist.count_array();

        {
            for ( unsigned int k=0; k < l_counts.size() ; ++k)
            {
                descr.push_back(l_counts[k]);
            }
        }

        {
            for ( unsigned int k=0; k < a_counts.size() ; ++k)
            {
                descr.push_back(a_counts[k]);
            }
        }

        {
            for ( unsigned int k=0; k < b_counts.size() ; ++k)
            {
                descr.push_back(b_counts[k]);
            }
        }

    }
    if ( title.size() )
    {
        vcl_string samp_title=title+"_samples.txt";
        vcl_ofstream output(samp_title.c_str());
        for ( pit = samples.begin() ; pit != samples.end() ; ++pit)
        {
            output<<(*pit).first<<" "<<(*pit).second<<vcl_endl;


        }
        output.close();
        
    }

}

void dbskfg_match_bag_of_fragments::compute_color_region_hist_fv(
    vcl_set<vcl_pair<double,double> >& samples,
    vil_image_view<double>& o1,
    vil_image_view<double>& o2,
    vil_image_view<double>& o3,
    vcl_vector<vl_sift_pix>& fv_descriptor,
    LabBinType bintype,
    vcl_string title)
{

    double min_l(0.0),max_l(0.0);
    double min_a(0.0),max_a(0.0);
    double min_b(0.0),max_b(0.0);
    unsigned int bins_l(0),bins_a(0),bins_b(0);

    if ( raw_color_space_ == dbskfg_match_bag_of_fragments::LAB )
    {
        min_l=0; max_l=100;
        min_a=-110; max_a=110;
        min_b=-110; max_b=110;
        bins_l=5;
        bins_a=10;
        bins_b=10;
    }
    else if( raw_color_space_ == dbskfg_match_bag_of_fragments::RGB_2 )
    {
        min_l=0; max_l=255;
        min_a=0; max_a=255;
        min_b=0; max_b=255;

        bins_l=8;
        bins_a=8;
        bins_b=8;


    }
    else if ( raw_color_space_ == dbskfg_match_bag_of_fragments::OPP_2 )
    {
        min_l=-180; max_l=180;
        min_a=-208; max_a=208;
        min_b=0; max_b=441;

        bins_l=10;
        bins_a=10;
        bins_b=10;


    }
    else
    {
        min_b=0; max_b=441;
        min_l=-180/max_b; max_l=180/max_b;
        min_a=-208/max_b; max_a=208/max_b;
            

        bins_l=10;
        bins_a=10;
        bins_b=10;


    }

   
    bsta_joint_histogram_3d<double> color_hist(
        min_l,max_l,bins_l,
        min_a,max_a,bins_a,
        min_b,max_b,bins_b);

    bsta_histogram<double> l_hist(min_l,max_l,bins_l);
    bsta_histogram<double> a_hist(min_a,max_a,bins_a);
    bsta_histogram<double> b_hist(min_b,max_b,bins_b);

    vcl_set<vcl_pair<double,double> >::iterator pit;
    for ( pit = samples.begin() ; pit != samples.end() ; ++pit)
    {
        double L_value = vil_bilin_interp_safe(o1,
                                              (*pit).first,
                                               (*pit).second);
        double a_value = vil_bilin_interp_safe(o2,
                                               (*pit).first,
                                               (*pit).second);        
        double b_value = vil_bilin_interp_safe(o3,
                                               (*pit).first,
                                               (*pit).second);
        
        color_hist.upcount(L_value,0,a_value,0,b_value,1);

        l_hist.upcount(L_value,1);
        a_hist.upcount(a_value,1);
        b_hist.upcount(b_value,1);

    }

    vcl_vector<vl_sift_pix> unrolled_hist; 
    if ( bintype == dbskfg_match_bag_of_fragments::DEFAULT )
    {
        for (unsigned int l = 0; l<bins_l; l++)
        {
            for (unsigned int a = 0; a<bins_a; a++)
            {
                for (unsigned int b = 0; b<bins_b; b++)
                {
                    double value=color_hist.get_count(l,a,b);
                    unrolled_hist.push_back(value);
                }
            }
        }
    }
    else
    {
        vcl_vector<double> l_counts=l_hist.count_array();
        vcl_vector<double> a_counts=a_hist.count_array();
        vcl_vector<double> b_counts=b_hist.count_array();

        {
            for ( unsigned int k=0; k < l_counts.size() ; ++k)
            {
                unrolled_hist.push_back(l_counts[k]);
            }
        }

        {
            for ( unsigned int k=0; k < a_counts.size() ; ++k)
            {
                unrolled_hist.push_back(a_counts[k]);
            }
        }

        {
            for ( unsigned int k=0; k < b_counts.size() ; ++k)
            {
                unrolled_hist.push_back(b_counts[k]);
            }
        }

    }
    if ( title.size() )
    {
        vcl_string samp_title=title+"_samples.txt";
        vcl_ofstream output(samp_title.c_str());
        for ( pit = samples.begin() ; pit != samples.end() ; ++pit)
        {
            output<<(*pit).first<<" "<<(*pit).second<<vcl_endl;


        }
        output.close();
        
    }


    int encoding_size = 2 * unrolled_hist.size() * keywords_;

    fv_descriptor.clear();
    fv_descriptor.resize(encoding_size);

    // run fisher encoding
    vl_fisher_encode
        (fv_descriptor.data(), VL_TYPE_FLOAT,
         means_color_, unrolled_hist.size(), keywords_,
         covariances_color_,
         priors_color_,
         unrolled_hist.data(), 1,
         VL_FISHER_FLAG_IMPROVED);

}


double dbskfg_match_bag_of_fragments::LAB_distance(
    vgl_point_2d<double> model_pt,
    vgl_point_2d<double> query_pt,
    vil_image_view<double>& model_channel_1,
    vil_image_view<double>& model_channel_2,
    vil_image_view<double>& model_channel_3,
    vil_image_view<double>& query_channel_1,
    vil_image_view<double>& query_channel_2,
    vil_image_view<double>& query_channel_3
    )
{

    vnl_vector_fixed<double,3> model_values;
    vnl_vector_fixed<double,3> query_values;



    model_values[0] = vil_bilin_interp_safe(model_channel_1,
                                            model_pt.x(),
                                            model_pt.y());

    model_values[1] = vil_bilin_interp_safe(model_channel_2,
                                            model_pt.x(),
                                            model_pt.y());

    model_values[2] = vil_bilin_interp_safe(model_channel_3,
                                            model_pt.x(),
                                            model_pt.y());

    query_values[0] = vil_bilin_interp_safe(query_channel_1,
                                            query_pt.x(),
                                            query_pt.y());

    query_values[1] = vil_bilin_interp_safe(query_channel_2,
                                            query_pt.x(),
                                            query_pt.y());

    query_values[2] = vil_bilin_interp_safe(query_channel_3,
                                            query_pt.x(),
                                            query_pt.y());

    return 1.0-distance_LAB(model_values,query_values,14);

}


void dbskfg_match_bag_of_fragments::compute_mean_std_color_descr(
    vgl_point_2d<double>& center,
    double& scale,
    vil_image_view<double>& o1,
    vil_image_view<double>& o2,
    vil_image_view<double>& o3,
    vcl_vector<double>& descr)
{

    vgl_box_2d<double> bbox(0,scale-1,0,scale-1);

    bbox.set_centroid_x(center.x());
    bbox.set_centroid_y(center.y());

    double l2_sum=0.0;

    unsigned int mini_boxes=1;
    for ( int y=bbox.min_y(); y <= bbox.max_y(); y=y+16)
    {
        for ( int x=bbox.min_x(); x <= bbox.max_x() ; x=x+16) 
        {
            vgl_point_2d<double> new_center(x+8,y+8);

            vgl_box_2d<double> mini_box(0,16,0,16);
            
            mini_box.set_centroid_x(x);
            mini_box.set_centroid_y(y);

            vcl_vector<double> chan1, chan2,chan3;
            for ( int yy=mini_box.min_y(); yy <= mini_box.max_y(); ++yy)
            {
                for ( int xx=mini_box.min_x(); xx <= mini_box.max_x(); ++xx) 
                {
                    double L_value = vil_bilin_interp_safe(o1,
                                                           xx,
                                                           yy);
                    double a_value = vil_bilin_interp_safe(o2,
                                                           xx,
                                                           yy);        
                    double b_value = vil_bilin_interp_safe(o3,
                                                           xx,
                                                           yy);

                    chan1.push_back(L_value);
                    chan2.push_back(a_value);
                    chan3.push_back(b_value);
                }
            }

            vnl_vector<double> stats_chan1(chan1.data(),chan1.size());
            vnl_vector<double> stats_chan2(chan2.data(),chan2.size());
            vnl_vector<double> stats_chan3(chan3.data(),chan3.size());
            
            double mean_chan1=stats_chan1.mean();
            double mean_chan2=stats_chan2.mean();
            double mean_chan3=stats_chan3.mean();

            stats_chan1 -= mean_chan1;
            stats_chan2 -= mean_chan2;
            stats_chan3 -= mean_chan3;
            
            double sum_chan1 = dot_product(stats_chan1,stats_chan1);
            double sum_chan2 = dot_product(stats_chan2,stats_chan2);
            double sum_chan3 = dot_product(stats_chan3,stats_chan3);

            double std_chan1 = vcl_sqrt(sum_chan1/(stats_chan1.size()-1));
            double std_chan2 = vcl_sqrt(sum_chan2/(stats_chan2.size()-1));
            double std_chan3 = vcl_sqrt(sum_chan3/(stats_chan3.size()-1));

            // Power law normalization
            mean_chan1 = vnl_math::sgn(mean_chan1)*
                vcl_sqrt(vcl_abs(mean_chan1));
            mean_chan2 = vnl_math::sgn(mean_chan2)*
                vcl_sqrt(vcl_abs(mean_chan2));
            mean_chan3 = vnl_math::sgn(mean_chan3)*
                vcl_sqrt(vcl_abs(mean_chan3));

            std_chan1 = vnl_math::sgn(std_chan1)*
                vcl_sqrt(vcl_abs(std_chan1));
            std_chan2 = vnl_math::sgn(std_chan2)*
                vcl_sqrt(vcl_abs(std_chan2));
            std_chan3 = vnl_math::sgn(std_chan3)*
                vcl_sqrt(vcl_abs(std_chan3));

            descr.push_back(mean_chan1);
            descr.push_back(std_chan1);

            descr.push_back(mean_chan2);
            descr.push_back(std_chan2);

            descr.push_back(mean_chan3);
            descr.push_back(std_chan3);

            l2_sum += mean_chan1*mean_chan1+
                      mean_chan2*mean_chan2+
                      mean_chan3*mean_chan3+
                      std_chan1*std_chan1+
                      std_chan2*std_chan2+
                      std_chan3*std_chan3;

        }
        
        
    }

    double l2_distance=vcl_sqrt(l2_sum);

    for ( unsigned int i=0; i < descr.size() ; ++i)
    {
        descr[i] = descr[i]/l2_distance;
    }
}
