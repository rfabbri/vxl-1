// classify shock patch sets

#include "dborl_dataset_procs.h"
#include <dborl/algo/dborl_utilities.h>
#include <dborl/dborl_category_info_sptr.h>
#include <dborl/dborl_category_info.h>
#include <dborl/dborl_dataset.h>
#include <dborl/algo/dborl_category_info_parser.h>
#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_image_bbox_description.h>
#include <dborl/dborl_image_description.h>
#include <dborl/dborl_image_object_sptr.h>
#include <dborl/dborl_image_object.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dborl/dborl_index_sptr.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/algo/dbskr_shock_patch_match_sptr.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/dbskr_shock_patch.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <vil/vil_convert.h>
#include <bil/algo/bil_color_conversions.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vul/vul_psfile.h>
#include <vul/vul_file_iterator.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_plane.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_threshold.h>
#include <brip/brip_vil_float_ops.h>
#include <bxml/bxml_document.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <dbskr/algo/io/dbskr_detect_patch_params.h>
#include <bsol/bsol_algs.h>

#include <dbdet/algo/dbdet_third_order_color_detector.h>
//#include <dbdet/edge/dbdet_third_order_edge_det.h>
//#include <dbdet/sel/dbdet_sel_base.h>
//#include <dbdet/sel/dbdet_sel_sptr.h>
//#include <dbdet/sel/dbdet_curve_model.h>
//#include <dbdet/sel/dbdet_sel.h>
//#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/tracer/dbdet_contour_tracer.h>

#include <dbgl/algo/dbgl_curve_smoothing.h>
#include <dbsk2d/dbsk2d_file_io.h>
#include <dbsol/dbsol_file_io.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_img_curve_algs.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
//#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_utilities.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsk2d/algo/dbsk2d_shock_transforms.h>
#include <dbskr/algo/io/dbskr_extract_bnd_params.h>
#include <dbskr/algo/io/dbskr_extract_shock_params.h>
#include <dbskr/algo/dbskr_shock_patch_selector.h>

bool write_def_params(vcl_string bnd_param_file, vcl_string shock_param_file)
{
  if (bnd_param_file.compare("") == 0)
    return false;
  if (shock_param_file.compare("") == 0)
    return false;

   //: save the bnd extraction params
  bxml_document doc;
  dbskr_extract_bnd_params bnd_params;
  bxml_element * root_doc = bnd_params.create_default_document_data();
  doc.set_root_element(root_doc);
  bxml_write(bnd_param_file, doc);

  bxml_document doc2;
  dbskr_extract_shock_params shock_params;
  bxml_element * root_doc2 = shock_params.create_default_document_data();
  doc2.set_root_element(root_doc2);
  bxml_write(shock_param_file, doc2);

  return true;
}

bool prepare_ethz_boundaries(vcl_string index_file, vcl_string image_ext, vcl_string bnd_param_xml, vcl_string sh_param_xml, bool check_existence)
{
  /*
  if (index_file.compare("") == 0)
    return false;

  if (image_ext.compare("") == 0)
    return false;

  if (bnd_param_xml.compare("") == 0)
    return false;

  if (sh_param_xml.compare("") == 0)
    return false;

  bxml_document param_doc = bxml_read(bnd_param_xml);
  dbskr_extract_bnd_params bnd_params;
  if (!param_doc.root_element())
    return false;
  if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
    vcl_cout << bnd_param_xml << " root is not ELEMENT\n";
    return false;
  }
  bnd_params.parse_from_data(param_doc.root_element());

  bxml_document param_doc2 = bxml_read(sh_param_xml);
  dbskr_extract_shock_params sh_params;
  if (!param_doc2.root_element())
    return false;
  if (param_doc2.root_element()->type() != bxml_data::ELEMENT) {
    vcl_cout << sh_param_xml << " root is not ELEMENT\n";
    return false;
  }
  sh_params.parse_from_data(param_doc2.root_element());

  dborl_index_parser parser;
  parser.clear();
  dborl_index_sptr ind = parse(index_file, parser);
  if (!ind)
    return false;

  dborl_index_node_sptr root = ind->root_->cast_to_index_node();

  for (unsigned int i = 0; i<root->names().size(); i++) {
    //: load the edge image
    vcl_string im_name = root->paths()[i] + root->names()[i] + image_ext;
    vcl_cout << im_name << vcl_endl;
    vil_image_resource_sptr img = vil_load_image_resource(im_name.c_str());
    vil_image_view<vxl_byte> I = img->get_view(0, img->ni(), 0, img->nj() );
    vil_image_view<float> L, A, B;

    vcl_string out_shock_name = root->paths()[i] + root->names()[i];
    if (check_existence && vul_file::exists(out_shock_name+".esf")) {
      vcl_cout << out_shock_name + ".esf exists, skipping!!!!!!\n";
      continue;
    }

    dbdet_sel_sptr edge_linker;
    bool color_image = false;
    
    vul_timer t;
    t.mark();

    vcl_vector< vsol_spatial_object_2d_sptr > image_curves;
    vcl_vector< vsol_spatial_object_2d_sptr > image_curves_all;
    vcl_vector< vsol_spatial_object_2d_sptr > image_curves_avg_mag_pruned;
    vcl_vector< vsol_spatial_object_2d_sptr > image_curves_length_pruned;

    if (bnd_params.run_contour_tracing_) { // need to binarize the image..  assumes that the figure is black and the background is white

      vcl_cout << " contour tracing...\n";
      float sigma=1.0f, beta=0.3f;
      int nsteps=1;
      vil_image_view<bool> binary_img;

      // no conversion necessary when input is binary image
      if (img->pixel_format()==binary_img.pixel_format() && 
        img->nplanes() == 1)
      {
        binary_img = img->get_view();
      }
      else
      {
        // get the grey view
        vil_image_view<vxl_byte> image;  
        if (img->nplanes()==1)
        {
          image = *vil_convert_cast(vxl_byte(), img->get_view());
        }
        else
        {
          image = *vil_convert_cast(vxl_byte(), 
          vil_convert_to_grey_using_rgb_weighting(img->get_view()));
        }

        // threshold the image to make sure that it is binary
        vxl_byte threshold_value = 128;
        vxl_byte min_value = 0, max_value = 0;
        vil_math_value_range(image, min_value, max_value);
        threshold_value = (min_value+max_value)/2;
        vil_threshold_above<vxl_byte >(image, binary_img, threshold_value);
      }
  
      //invoke the tracer
      dbdet_contour_tracer ctracer;
      ctracer.set_sigma(sigma);
      ctracer.set_curvature_smooth_nsteps(nsteps);
      ctracer.set_curvature_smooth_beta(beta);
      ctracer.trace(binary_img);
      
      //get the interesting contours
      vcl_vector< vsol_spatial_object_2d_sptr > contours;
      
      //vsol_box_2d_sptr bbox = new vsol_box_2d();
      for (unsigned i=0; i<ctracer.contours().size(); i++)
      {
        if ((int)(ctracer.contours()[i].size()) >= bnd_params.length_thresh_)
        {
          vsol_polygon_2d_sptr newContour = new vsol_polygon_2d (ctracer.contours()[i]);//ctracer.largest_contour()
        //  newContour->compute_bounding_box();
        //  bbox->grow_minmax_bounds(newContour->get_bounding_box());

          if (bnd_params.smooth_bnds_) {
            
            vcl_vector<vgl_point_2d<double> > pts;
            pts.reserve(newContour->size());
            for (unsigned j=0; j<newContour->size(); j++)
              pts.push_back(newContour->vertex(j)->get_p());
            //smooth this contour
            dbgl_csm(pts, 1.0f, bnd_params.smoothing_nsteps_);   // psi = 1.0f
            vcl_vector<vsol_point_2d_sptr> vsol_pts;
            vsol_pts.reserve(pts.size());
            for (unsigned i=0; i<pts.size(); ++i)
              vsol_pts.push_back(new vsol_point_2d(pts[i]));
            vsol_polygon_2d_sptr new_curve = new vsol_polygon_2d(vsol_pts);
            image_curves.push_back(new_curve->cast_to_spatial_object());

          } else {
            image_curves.push_back(newContour->cast_to_spatial_object());
          }
        }
      }

      //int offset = 3;
      //bbox->add_point(bbox->get_min_x()-offset, bbox->get_min_y()-offset);
      //bbox->add_point(bbox->get_max_x()+offset, bbox->get_max_y()+offset);  // enlarge the box with 3 pixels
      //vsol_polygon_2d_sptr box_poly = bsol_algs::poly_from_box(bbox);
      //image_curves.push_back(box_poly->cast_to_spatial_object());

    } else {

    if (bnd_params.extract_from_edge_img_) {
      //convert to grayscale
      vil_image_view<vxl_byte> image_view = img->get_view(0, img->ni(), 0, img->nj() );
      vil_image_view<vxl_byte> greyscale_view;

      if(image_view.nplanes() == 3){
        vil_convert_planes_to_grey(image_view, greyscale_view );
      }
      else if (image_view.nplanes() == 1){
        greyscale_view = image_view;
      }

      //create a new edgemap
      dbdet_edgemap_sptr edge_map = new dbdet_edgemap(greyscale_view.ni(), greyscale_view.nj());

      for (unsigned x = 0; x < greyscale_view.ni(); x++){
        for (unsigned y = 0; y < greyscale_view.nj(); y++)
        {
          if (greyscale_view(x,y) > bnd_params.edge_img_threshold_){ //edge pixel found
            dbdet_edgel* e = new dbdet_edgel(vgl_point_2d<double>(x,y), 0.0, greyscale_view(x,y));
            e->gpt = vgl_point_2d<int>(x,y);
            edge_map->insert(e);
          }
        }
      }

      vcl_cout << " there are " << edge_map->num_edgels << " in the edge map\n";

      //construct the linker
      edge_linker = new dbdet_sel<dbdet_CC_curve_model>(edge_map, bnd_params.nrad_, bnd_params.dt_*vnl_math::pi/180, bnd_params.dx_, bnd_params.max_k_);

      if (bnd_params.generic_linker_) {
        //everything below is false cause we're linking edgels with no orientation info
        edge_linker->require_appearance_consistency(false);//orientation consistency
        edge_linker->set_appearance_threshold(2.0f);  // this threshold is meaningless
        edge_linker->require_orientation_consistency(false);//orientation consistency
        edge_linker->require_smooth_continuation(false);//smooth continuation
        edge_linker->require_low_curvatures(false);//low curvatures
        //Extract the contours using the image grid (Greedy)
        edge_linker->extract_image_contours_by_tracing();
      } else 
        return false;  // we don't know what to do yet..

    } else {  // extract from real image using Amir's edge detection and linker

      dbdet_edgemap_sptr edge_map;
  
      // extract third order edges  ---------------------------------------------------------------------------------------
      unsigned grad_op = 0; // Gaussian
      unsigned convolution_algo = 0; // 2-D
      unsigned parabola_fit = 0; // 3-point fit
      //double sigma = 1.0f;
      //double sigma = edge_detection_sigma;
      //double thresh = 0.1f;  // threshold for gradient magnitude in edge detection, we always set this 1.0 then eliminate based on the linked contours
                         // avg_grad_mag_threshold which is typically higher than 1.0f
      int N = 1;
  
      
      vcl_cout << "edges... ";
    
      if (I.nplanes() != 3){
        vcl_cout << "In edge_detector - GREY image!!! \n";
        edge_map = dbdet_detect_third_order_edges(I, bnd_params.edge_detection_sigma_, bnd_params.edge_detection_thresh_, N, parabola_fit, grad_op, false);  // reduce_tokens is false
      } else {
        color_image = true;
        convert_RGB_to_Lab(I, L, A, B);
        edge_map = dbdet_third_order_color(grad_op, convolution_algo, N, bnd_params.edge_detection_sigma_, bnd_params.edge_detection_thresh_, parabola_fit, L, A, B);
      }

      //-------------------------------------------------------------------------------------------------------------------
      vcl_cout << t.real()/1000.0f << " secs. linking... ";
      // link the edges ---------------------------------------------------------------------------------------------------
      //double nrad = 2.0f, dx = 0.2f, dt = 15.0f;
      //unsigned bnd_params.max_size_to_group_ = 7, bnd_params.min_size_to_link_ = 3;

      edge_linker = new dbdet_sel<dbdet_CC_curve_model>(edge_map, bnd_params.nrad_, bnd_params.dt_*vnl_math::pi/180, bnd_params.dx_); // curve model choice = Circular Arc model w/o perturbations
      
      if (bnd_params.generic_linker_) {
        //everything below is false cause we're linking edgels with no orientation info
        edge_linker->require_appearance_consistency(bnd_params.require_appearance_consistency_);
        edge_linker->set_appearance_threshold(bnd_params.appearance_consistency_threshold_);  // this threshold was 2.0f on the order of edge strength
        edge_linker->require_orientation_consistency(false);//orientation consistency
        edge_linker->require_smooth_continuation(false);//smooth continuation
        edge_linker->require_low_curvatures(false);//low curvatures
        //Extract the contours using the image grid (Greedy)
        edge_linker->extract_image_contours_by_tracing();
      } else {
        edge_linker->build_curvelets_greedy(bnd_params.max_size_to_group_); // grouping algo -- greedy local grouping
        edge_linker->use_anchored_curvelets_only();
        edge_linker->construct_the_link_graph(bnd_params.min_size_to_link_, 0);  // linkgraph algo = 0 (TODO: ask Amir ? this zero seems redundant..?)
        edge_linker->extract_image_contours_from_the_link_graph();
      }
    }

    double link_time = t.real() / 1000.0;
    vcl_cout << "Time taken to link: " << link_time << " sec" << vcl_endl;

    //construct vsol objects from the linked contours
    vcl_cout << " after linking there are " << edge_linker->get_curve_fragment_graph().frags.size() << " fragments \n";

     vsol_box_2d_sptr bbox = new vsol_box_2d();

    double mean_color_dist_all = 0;
    int cnt_all = 0;
    double mean_color_dist_selected = 0;
    int cnt_sel = 0;

    dbdet_edgel_chain_list_iter f_it = edge_linker->get_curve_fragment_graph().frags.begin();
    for (; f_it != edge_linker->get_curve_fragment_graph().frags.end(); f_it++)
    {
      dbdet_edgel_chain* chain = (*f_it);

      //only keep the longer contours
      if (int(chain->edgels.size()) < bnd_params.min_size_to_keep_)
        continue;

      //compute the average contrast of an edgel chain
      double avg_contrast = 0.0;
      for (unsigned j=0; j<chain->edgels.size(); j++)
        avg_contrast += chain->edgels[j]->strength;
      avg_contrast /= chain->edgels.size();

      //create a polyline out of the edgel chain
      vcl_vector<vgl_point_2d<double> > pts;
      pts.reserve(chain->edgels.size());
      for (unsigned j=0; j<chain->edgels.size(); j++)
        pts.push_back(chain->edgels[j]->pt);

      if (bnd_params.smooth_bnds_) {
        //smooth this contour
        dbgl_csm(pts, 1.0f, bnd_params.smoothing_nsteps_);   // psi = 1.0f
      }

      vcl_vector<vsol_point_2d_sptr> vsol_pts;
      vsol_pts.reserve(pts.size());
      for (unsigned i=0; i<pts.size(); ++i)
        vsol_pts.push_back(new vsol_point_2d(pts[i]));
      vsol_polyline_2d_sptr new_curve = new vsol_polyline_2d(vsol_pts);

      image_curves_all.push_back(new_curve->cast_to_spatial_object());

      //only keep the contours that are above the threshold
      if (avg_contrast<bnd_params.avg_grad_mag_threshold_) {
        pts.clear();
        for (unsigned jj = 0; jj <vsol_pts.size(); jj++)
          vsol_pts[jj] = 0;
        vsol_pts.clear();
        continue;
      }

      if (new_curve->length() > bnd_params.length_thresh_) {

        image_curves_length_pruned.push_back(new_curve->cast_to_spatial_object());
        
        vsol_polyline_2d_sptr fitted_poly;
        if (bnd_params.fit_lines_) 
          fitted_poly = fit_lines_to_contour(new_curve, bnd_params.rms_);
        else  
          fitted_poly = new_curve;

        dbsol_interp_curve_2d_sptr c = new dbsol_interp_curve_2d();
        dbsol_curve_algs::interpolate_linear(c.ptr(), fitted_poly); // open curve
        double dist;
        if (color_image)
          dist = get_color_distance_of_curve_regions(c, float(bnd_params.pruning_region_width_), L, A, B, 14.0f);  // color_gamma = 14
        else
          dist = get_intensity_distance_of_curve_regions(c, float(bnd_params.pruning_region_width_), I, 14.0f);
        mean_color_dist_all += dist;
        
        cnt_all++;
        if (dist < bnd_params.pruning_color_threshold_) {
          mean_color_dist_selected += dist;
          cnt_sel++;
          image_curves.push_back(fitted_poly->cast_to_spatial_object());
          fitted_poly->compute_bounding_box();
          bbox->grow_minmax_bounds(fitted_poly->get_bounding_box());
        }
      }
      
      pts.clear();
      for (unsigned jj = 0; jj <vsol_pts.size(); jj++)
        vsol_pts[jj] = 0;
      vsol_pts.clear();
    }

   vcl_cout << "mean color dist of all curves: " << mean_color_dist_all/cnt_all << " of selected: " << mean_color_dist_selected/cnt_sel << " threshold was: " << bnd_params.pruning_color_threshold_ << vcl_endl;
   vcl_cout << "bbox (minx, miny) (width, height): " << "(" << bbox->get_min_x() << ", " << bbox->get_min_y() << ") (" << bbox->width() << ", " << bbox->height() << ")\n";
 
    int offset = 3;
    bbox->add_point(bbox->get_min_x()-offset, bbox->get_min_y()-offset);
    bbox->add_point(bbox->get_max_x()+offset, bbox->get_max_y()+offset);  // enlarge the box with 3 pixels
    vsol_polygon_2d_sptr box_poly = bsol_algs::poly_from_box(bbox);
    image_curves.push_back(box_poly->cast_to_spatial_object());

    dbsol_save_cem(image_curves_all, root->paths()[i] + root->names()[i]+"_all_boundary.cem");
    dbsol_save_cem(image_curves_avg_mag_pruned, root->paths()[i] + root->names()[i]+"_after_avg_mag_pruned_boundary.cem");
    dbsol_save_cem(image_curves_length_pruned, root->paths()[i] + root->names()[i]+"_after_length_pruned_boundary.cem");    
    }

    vcl_string bnd_name = root->paths()[i] + root->names()[i] + "_boundary.bnd";
    vcl_cout << bnd_name << vcl_endl;
    // save the boundary curves
    dbsk2d_file_io::save_bnd_v3_0(bnd_name, image_curves);
    dbsol_save_cem(image_curves, root->paths()[i] + root->names()[i]+"_boundary.cem");
    vcl_cout << "saved " << image_curves.size() << " polylines\n";

    //: save the bnd extraction params
    bxml_document doc;
    bxml_element * root_doc = bnd_params.create_document_data();
    doc.set_root_element(root_doc);
    vcl_string bnd_param_file = root->paths()[i] + root->names()[i] + "_boundary_params.xml";
    bxml_write(bnd_param_file, doc);


    //: find the shock graph
    //-------------------------------------------------------------------------------------------------------------------
    vcl_cout << t.real()/1000.0f << " secs. computing shocks... ";
    // compute shocks ---------------------------------------------------------------------------------------------------
    float xmin=0, ymin=0, cell_width=1000.0f, cell_height=1000.0f; int num_rows=1, num_cols=1;
    dbsk2d_boundary_sptr boundary = dbsk2d_create_boundary(image_curves, false, xmin, ymin, num_rows, num_cols, cell_width, cell_height, true, true);
    dbsk2d_ishock_graph_sptr isg = dbsk2d_compute_ishocks(boundary);
    if (!isg) {
      vcl_cout << "Problem in intrinsic shock computation! Exiting!\n";
      return false;
    }
    dbsk2d_shock_graph_sptr sg = new dbsk2d_shock_graph();
    dbsk2d_prune_ishock ishock_pruner(isg, sg);
    ishock_pruner.prune(sh_params.prune_threshold_);  // prune threshold is 1.0f
    ishock_pruner.compile_coarse_shock_graph();
    if (!sg->number_of_vertices() || !sg->number_of_edges()) {
      vcl_cout << "Problem in coarse shock computation!\n";
      return false;
    }

    dbsk2d_sample_ishock ishock_sampler_t(sg);
    if (bnd_params.run_contour_tracing_)
      ishock_sampler_t.sample(sh_params.sampling_ds_, INSIDE);
    else 
      ishock_sampler_t.sample(sh_params.sampling_ds_, BOTHSIDE);
    dbsk2d_shock_graph_sptr sampled_sg_t = ishock_sampler_t.extrinsic_shock_graph();
    dbsk2d_xshock_graph_fileio writer_t;
    
      

    // -------------------------------------------------------------------------------------------------------------------
    vcl_cout << (t.real()/1000.0f)/60.0 << " mins. " << " gap transforms... ";
    // gap transforms ----------------------------------------------------------------------------------------------------

    if (sh_params.perform_gap_transforms_) {
      writer_t.save_xshock_graph(sampled_sg_t, out_shock_name+"_before_gaps.esf");

    //: need the color image for this
    vcl_string im_c_name = root->paths()[i] + root->names()[i] + ".jpg";
    vcl_cout << im_c_name << vcl_endl;
    vil_image_resource_sptr img_sptr = vil_load_image_resource(im_c_name.c_str());

    //float low_cont_t = 0.1f, high_cont_t = 0.5f, low_app_t = 0.1f, high_app_t = 0.5f;
    //float alpha_cont = 0.4f, alpha_app = 0.6f;
    dbsk2d_shock_transforms transformer(isg, sg);
    transformer.set_image(img_sptr);
    transformer.set_curve_length_gamma(sh_params.curve_length_gamma_);
    vcl_cout << "cont_t: " << sh_params.cont_thres_ << " app_t: " << sh_params.app_thres_ << " performing gap transforms\n";
    transformer.perform_all_gap_transforms(sh_params.cont_thres_, sh_params.app_thres_, sh_params.alpha_cont_, sh_params.alpha_app_, true);
    vcl_vector< vsol_spatial_object_2d_sptr > euler_sps;
    transformer.get_eulerspirals(euler_sps);
    transformer.clear_eulerspirals();  // is this cleaning the spirals pointed by normal pointers??? TODO: check!!

    if (!sg->number_of_vertices() || !sg->number_of_edges()) {
      vcl_cout << "Problem in gap transforms!\n";
      return 0;
    }

    dbsk2d_sample_ishock ishock_sampler(sg);
    ishock_sampler.sample(sh_params.sampling_ds_, BOTHSIDE);
    dbsk2d_shock_graph_sptr sampled_sg = ishock_sampler.extrinsic_shock_graph();

    // -------------------------------------------------------------------------------------------------------------------
    //if (write_output) {
      //write_contours_to_ps(image_curves, euler_sps, out_shock_name+"_curves");
      dbsk2d_xshock_graph_fileio writer;
      //vcl_string out_shock_name = root->paths()[i] + root->names()[i];
      writer.save_xshock_graph(sampled_sg, out_shock_name+".esf");
      //use the generic linker in SEL instead
      
      dbsk2d_file_io::save_bnd_v3_0(out_shock_name+"_boundary_gaps.bnd", euler_sps);
      dbsol_save_cem(euler_sps, out_shock_name+"_boundary_gaps.cem");

      //: save the shock extraction params
      bxml_document doc2;
      bxml_element * root2 = sh_params.create_document_data();
      doc2.set_root_element(root2);
      bxml_write(vcl_string(out_shock_name + "_esf_params.xml"), doc2);
    //}
    } else {
      writer_t.save_xshock_graph(sampled_sg_t, out_shock_name+".esf");
    }
    
    vcl_cout << "total: " << (t.real()/1000.0f)/60.0 << " mins. Done!\n";

    for (unsigned jj = 0; jj < image_curves.size(); jj++)
      image_curves[jj] = 0;
    image_curves.clear();  
    
    for (unsigned jj = 0; jj < image_curves_all.size(); jj++)
      image_curves_all[jj] = 0;
    image_curves_all.clear();
    
    for (unsigned jj = 0; jj < image_curves_avg_mag_pruned.size(); jj++)
      image_curves_avg_mag_pruned[jj] = 0;
    image_curves_avg_mag_pruned.clear();

    for (unsigned jj = 0; jj < image_curves_length_pruned.size(); jj++)
      image_curves_length_pruned[jj] = 0;
    image_curves_length_pruned.clear();

  }

*/
  return true;
  
}

bool create_image(vil_image_resource_sptr img, vcl_string filename, vcl_vector<vsol_polygon_2d_sptr>& polys,
                  vil_rgb<int>& outer, vil_rgb<int>& model, vil_rgb<int>& others)
{
  //1)If file open fails, return.
  vul_psfile psfile1((filename+".ps").c_str(), false);

  if (!psfile1){
    vcl_cout << " Error opening file  " << filename.c_str() << vcl_endl;
    return false;
  }

  vil_image_view<vxl_byte> image = img->get_view(0, img->ni(), 0, img->nj());
  int sizex = image.ni();
  int sizey = image.nj();
  int planes = image.nplanes();

  unsigned char *buf = new unsigned char[sizex*sizey*3];
  if (planes == 3) {
    vcl_cout << "processing color image\n";
    for (int x=0; x<sizex; ++x) 
      for (int y=0; y<sizey; ++y) {
        buf[3*(x+sizex*y)  ] = image(x,y,0);
        buf[3*(x+sizex*y)+1] = image(x,y,1);
        buf[3*(x+sizex*y)+2] = image(x,y,2);
    }
    
    
  } else if (planes == 1) {
    vcl_cout << "processing grey image\n";
    for (int x=0; x<sizex; ++x) 
      for (int y=0; y<sizey; ++y) {
        buf[3*(x+sizex*y)  ] = image(x,y,0);
        buf[3*(x+sizex*y)+1] = image(x,y,0);
        buf[3*(x+sizex*y)+2] = image(x,y,0);
    }
  }
  psfile1.print_color_image(buf,sizex,sizey);
  delete [] buf;
  psfile1.reset_bounding_box();

  psfile1.set_scale_x(50);
  psfile1.set_scale_y(50);
  
  

  // parse through all the vsol classes and save curve objects only
  for (unsigned kk = 0; kk < polys.size(); kk++) {
    vsol_polygon_2d_sptr poly = polys[kk];

    psfile1.set_fg_color(1, 1, 1);
    psfile1.set_line_width(4.0);
    for (unsigned int i=1; i<poly->size();i++)
    {
        vsol_point_2d_sptr p1 = poly->vertex(i-1);
        vsol_point_2d_sptr p2 = poly->vertex(i);
        psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }
    
    if (kk == 0) {
      psfile1.set_line_width(2.0);
      psfile1.set_fg_color((float)outer.R(),(float)outer.G(),(float)outer.B());
    } else if (kk == 1) {
      psfile1.set_line_width(2.0);
      psfile1.set_fg_color((float)model.R(),(float)model.G(),(float)model.B());
    } else {
      psfile1.set_line_width(2.0);
      psfile1.set_fg_color((float)others.R(),(float)others.G(),(float)others.B());
    }

    for (unsigned int i=1; i<poly->size();i++)
    {
      vsol_point_2d_sptr p1 = poly->vertex(i-1);
      vsol_point_2d_sptr p2 = poly->vertex(i);
      psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }
  }

  //close file
  psfile1.close();
  return true;
}

bool create_image2(vil_image_resource_sptr img, vcl_string filename, vcl_vector<vsol_polygon_2d_sptr>& polys,
                  vcl_vector<vil_rgb<int> >& colors)
{
  //1)If file open fails, return.
  vul_psfile psfile1((filename+".ps").c_str(), false);

  if (!psfile1){
    vcl_cout << " Error opening file  " << filename.c_str() << vcl_endl;
    return false;
  }

  if (!(polys.size() == colors.size())) {
    vcl_cout << "poly vector is not the same size as color vector!!!\n";
    return false;
  }

  vil_image_view<vxl_byte> image = img->get_view(0, img->ni(), 0, img->nj());
  int sizex = image.ni();
  int sizey = image.nj();
  int planes = image.nplanes();

  unsigned char *buf = new unsigned char[sizex*sizey*3];
  if (planes == 3) {
    vcl_cout << "processing color image\n";
    for (int x=0; x<sizex; ++x) 
      for (int y=0; y<sizey; ++y) {
        buf[3*(x+sizex*y)  ] = image(x,y,0);
        buf[3*(x+sizex*y)+1] = image(x,y,1);
        buf[3*(x+sizex*y)+2] = image(x,y,2);
    }
    
    
  } else if (planes == 1) {
    vcl_cout << "processing grey image\n";
    for (int x=0; x<sizex; ++x) 
      for (int y=0; y<sizey; ++y) {
        buf[3*(x+sizex*y)  ] = image(x,y,0);
        buf[3*(x+sizex*y)+1] = image(x,y,0);
        buf[3*(x+sizex*y)+2] = image(x,y,0);
    }
  }
  psfile1.print_color_image(buf,sizex,sizey);
  delete [] buf;
  psfile1.reset_bounding_box();

  psfile1.set_scale_x(50);
  psfile1.set_scale_y(50);
  
  // parse through all the vsol classes and save curve objects only
  for (unsigned kk = 0; kk < polys.size(); kk++) {
    vsol_polygon_2d_sptr poly = polys[kk];

    if (kk == 0 || kk == 1) {
      psfile1.set_fg_color(1, 1, 1);
      psfile1.set_line_width(4.0);
      for (unsigned int i=1; i<poly->size();i++)
      {
          vsol_point_2d_sptr p1 = poly->vertex(i-1);
          vsol_point_2d_sptr p2 = poly->vertex(i);
          psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
      }
    }
  
    if (kk == 0) // query model
      psfile1.set_line_width(2.0);
    else if (kk == 1)  // whole model
      psfile1.set_line_width(1.0);
    else if (kk == 2)  // m1
      psfile1.set_line_width(3.0);
    else if (kk == 3)
      psfile1.set_line_width(1.0);

    psfile1.set_fg_color((float)colors[kk].R(), (float)colors[kk].G(), (float)colors[kk].B());
    for (unsigned int i=1; i<poly->size();i++)
    {
      vsol_point_2d_sptr p1 = poly->vertex(i-1);
      vsol_point_2d_sptr p2 = poly->vertex(i);
      psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }
  }

  //close file
  psfile1.close();
  return true;
}

bool save_patch_images(vcl_string image_file, vcl_string st_file, vcl_string out_name)
{
  if (image_file.compare("") == 0)
    return false;

  if (st_file.compare("") == 0)
    return false;

  if (out_name.compare("") == 0)
    return false;

  vil_image_resource_sptr img;
  img = vil_load_image_resource(image_file.c_str());
  if (!img)
    return false;

   //: load the image
  vil_image_view<float> L_, A_, B_;
  vil_image_view<vxl_byte> I_;
  vil_image_resource_sptr img_r, img_g, img_b;
  set_images(img, I_, L_, A_, B_, img_r, img_g, img_b);     

  dbskr_shock_patch_storage_sptr st = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(st_file.c_str());
  st->b_read(ifs);
  ifs.close();

  vcl_cout << "loaded: " << st->size() << " patches in the storage\n";

  //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
  //vcl_cout << model_st->size() << " patches in model storage, reading shocks..\n";
  dbsk2d_xshock_graph_fileio loader;
  //: load esfs for each patch
 
  vcl_string storage_end = "patch_strg.bin";
  vcl_vector<vsol_polygon_2d_sptr> polys;
  for (unsigned iii = 0; iii < st->size(); iii++) {
    dbskr_shock_patch_sptr sp = st->get_patch(iii);
    vcl_string patch_esf_name = st_file.substr(0, st_file.length()-storage_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
    //sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
    //                        false,//params_.shock_match_params_.circular_ends_,
    //                        params_.shock_match_params_.combined_edit_, 
    //                        params_.shock_match_params_.scurve_sample_ds_, 
    //                        params_.shock_match_params_.scurve_interpolate_ds_); // so that prepare_tree() and tree() methods of patch have the needed params
    save_image_poly(sp, out_name, img_r, img_g, img_b);
    /*polys.push_back(sp->get_outer_boundary());
    
    vil_rgb<int> outer(1,0,0); vil_rgb<int> model(0,0,0); vil_rgb<int> others(0,0,0);
    create_image(img, (patch_esf_name + ".ps"), polys, outer, model, others);
    
    polys[0] = 0;
    polys.clear();
*/
  }

  return true;
}

bool prepare_match_html(vcl_string model_image_file, vcl_string model_st_file, 
                        vcl_string query_image_file, vcl_string query_st_file, 
                        vcl_string match_file, vcl_string detection_param_xml, vcl_string out_name, vcl_string width_str)
{
  if (model_image_file.compare("") == 0 || model_st_file.compare("") == 0 || query_image_file.compare("") == 0 || 
      query_st_file.compare("") == 0 || match_file.compare("") == 0 || detection_param_xml.compare("") == 0 || out_name.compare("") == 0 ||
      width_str.compare("") == 0)
    return false;

  vil_image_resource_sptr model_img;
  model_img = vil_load_image_resource(model_image_file.c_str());
  if (!model_img)
    return false;

  vil_image_resource_sptr query_img;
  query_img = vil_load_image_resource(query_image_file.c_str());

  if (!query_img)
    return false;

  /////////////////
  bxml_document param_doc = bxml_read(detection_param_xml);
  if (!param_doc.root_element())
    return false;
  
  if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
    vcl_cout << "params root is not ELEMENT\n";
    return false;
  }

  dbskr_detect_patch_params params_;
  if (!params_.parse_from_data(param_doc.root_element()))
    return false;
  //////////////////

  vul_timer t;
  t.mark();

  dbskr_shock_patch_storage_sptr model_st = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(model_st_file.c_str());
  model_st->b_read(ifs);
  ifs.close();

  vcl_cout << "loaded: " << model_st->size() << " patches in the model\n";

  dbskr_shock_patch_storage_sptr query_st = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifsq(query_st_file.c_str());
  query_st->b_read(ifsq);
  ifsq.close();

  vcl_cout << "loaded: " << query_st->size() << " patches in the query\n";
  
  //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
  //vcl_cout << model_st->size() << " patches in model storage, reading shocks..\n";
  dbsk2d_xshock_graph_fileio loader;
  //: load esfs for each patch
  vcl_string storage_end = "patch_strg.bin";
  vsol_polygon_2d_sptr model_poly;
  if (model_st->size() > 0)
    model_poly = model_st->get_patch(0)->get_outer_boundary();
  for (unsigned iii = 0; iii < model_st->size(); iii++) {
    dbskr_shock_patch_sptr sp = model_st->get_patch(iii);
    vcl_string patch_esf_name = model_st_file.substr(0, model_st_file.length()-storage_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
    sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
                            params_.shock_match_params_.circular_ends_,
                            params_.shock_match_params_.combined_edit_, 
                            params_.shock_match_params_.scurve_sample_ds_, 
                            params_.shock_match_params_.scurve_interpolate_ds_); // so that prepare_tree() and tree() methods of patch have the needed params
    if (model_poly->size() < sp->get_outer_boundary()->size())
      model_poly = sp->get_outer_boundary();
  }

  for (unsigned iii = 0; iii < query_st->size(); iii++) {
    dbskr_shock_patch_sptr sp = query_st->get_patch(iii);
    vcl_string patch_esf_name = query_st_file.substr(0, query_st_file.length()-storage_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
    sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
                            params_.shock_match_params_.circular_ends_,
                            params_.shock_match_params_.combined_edit_, 
                            params_.shock_match_params_.scurve_sample_ds_, 
                            params_.shock_match_params_.scurve_interpolate_ds_);  // so that prepare_tree() and tree() methods of patch have the needed params
  }


  //: load the match
  dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
  vsl_b_ifstream ifsm(match_file.c_str());
  match->b_read(ifsm);
  ifsm.close();

  vcl_cout << "loaded the match file\n";

  //: prepare id maps for this match
  vcl_map<int, dbskr_shock_patch_sptr> model_map;
  for (unsigned ii = 0; ii < model_st->size(); ii++) 
    model_map[model_st->get_patch(ii)->id()] = model_st->get_patch(ii);
  match->set_id_map1(model_map);

  vcl_map<int, dbskr_shock_patch_sptr> query_map;
  for (unsigned ii = 0; ii < query_st->size(); ii++) 
    query_map[query_st->get_patch(ii)->id()] = query_st->get_patch(ii);
  match->set_id_map2(query_map);

  if (params_.use_normalized_costs_) {
    if (params_.use_reconst_boundary_length_)
      if (!match->compute_length_norm_costs_of_cors()) // sorts the matches after recomputation of the normalized cost
        return false;
    else if (params_.use_total_splice_cost_)
      if (!match->compute_splice_norm_costs_of_cors()) // sorts the matches after recomputation of the normalized cost
        return false;
  }

  vcl_cout << " loading took " << t.real() / 1000.0f << " seconds " << t.real() / (60*1000.0f) << " mins.\n";
  t.mark();

  //: create all the similariy transformations
  match->compute_similarity_transformations();

  vcl_cout << " similarity computations took " << t.real() / 1000.0f << " seconds " << t.real() / (60*1000.0f) << " mins.\n";
  t.mark();
  
  vcl_string out_images = out_name + "/";
  vul_file::make_directory(out_images);

  vcl_cout << "patches are sorted wrt parameters in the detection param file\n";
  vcl_ofstream tf((out_name + ".html").c_str(), vcl_ios::app);
  
  if (!tf) {
    vcl_cout << "Unable to open output html file " << out_name << " for write " << vcl_endl;
    return false;
  }
  int visualization_n = 50;

  vil_rgb<int> outer(1, 0, 1);
  vil_rgb<int> model(0, 0, 0);
  vil_rgb<int> others(1, 1, 1);

  patch_cor_map_iterator iter;
  patch_cor_map_type& map = match->get_map();
  vcl_map<int, dbskr_shock_patch_sptr>& id_map_test = match->get_id_map2();
  vcl_map<int, dbskr_shock_patch_sptr>& id_map_model = match->get_id_map1();  // first one is model
  vsol_point_2d_sptr center = new vsol_point_2d(0, 0);

#if 0     // don't create the similarity matrix
  tf << "<TABLE BORDER=\"1\">\n";
  tf << "<caption align=\"top\">" << match_file << "</caption>\n";
  tf << "<TR> <TH>   ";  // leave the first cell of first row empty
  // write top n to the first row
  for (unsigned i = 0; int(i) < visualization_n; i++) 
    tf << "<TH> Match " << i+1 << " ";
  tf << "</TH> </TR>\n";

  for (iter = map.begin(); iter != map.end(); iter++) {
    dbskr_shock_patch_sptr msp = id_map_model[iter->first];
    vcl_ostringstream oss1;
    oss1 << msp->id();
   
    vcl_string patch_image_file = out_images + "model_" + oss1.str() + "_";
    vcl_vector<vsol_polygon_2d_sptr> dummy(1, msp->get_traced_boundary());
    if (!create_image(model_img, patch_image_file, dummy, outer, model, others))
      return false;

    tf << "<TR> <TD> <img src=\"" << patch_image_file << "001.png\" width = \"" << width_str << "\"" << "\"> ";
    tf << msp->id() << " " << msp->depth() << " </TD> "; 
    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = iter->second;
    for (unsigned nn = 0; int(nn) < visualization_n; nn++) {
      if (nn >= match_vec->size()) {
        tf << "<TD> <img src=\"unknown\"> </TD> ";
      } else {
        dbskr_shock_patch_sptr tsp = id_map_test[(*match_vec)[nn].first];
        dbskr_sm_cor_sptr sm = (*match_vec)[nn].second;

        sm->set_tree1(msp->tree()); // assuming tree parameters are already set properly
        sm->set_tree2(tsp->tree()); // assuming tree parameters are already set properly  // all the maps are recomputed based on dart correspondence

        vgl_h_matrix_2d<double> H; 
        if (sm->get_similarity_trans(H)) {  //, true, 5, false, true)) {  // similarity that maps model onto test image
          vcl_vector<vsol_polygon_2d_sptr> polys;
          polys.push_back(tsp->get_traced_boundary());

          vcl_ostringstream oss11;      
          oss11 << tsp->id();

          vcl_string out_img1 = out_images + "query_patch_" + oss11.str() + "_";
          if (!create_image(query_img, out_img1, polys, outer, model, others))
            return false;

          //: map the model polygon onto the query image
          vsol_polygon_2d_sptr trans_model_poly = bsol_algs::transform_about_point(model_poly, center, H);
          polys.push_back(trans_model_poly);

          vcl_string out_img = out_images + "query_" + oss1.str() + "_mapped_with_test_patch_" + oss11.str() + "_";

          if (!create_image(query_img, out_img, polys, outer, model, others))
            return false;
          
          //vil_image_resource_sptr new_img = msp->mapped_image(img_model, img_test, H, true); // no need to recompute observation if already exists
          
          tf << "<TD> <img src=\"" << out_img << "001.png\" width = \"" << width_str << "\"" << "\"> ";
          tf << " <img src=\"" << out_img1 << "001.png\" width = \"" << width_str << "\"" << "\"> ";
          tf << "model: " << msp->id() << " test patch: " << tsp->id() << " sim: " << sm->final_norm_cost() << " </TD> ";
        } else {
          vcl_cout << " similarity transformation not computed!!\n";
          tf << "<TD> <img src=\"unknown\"> sim trans not computed!! </TD> ";
        } 
      }
    }
    tf << "</TR>\n";
  }
  tf << "</TABLE>\n";

  vcl_cout << " image creations took " << t.real() / 1000.0f << " seconds " << t.real() / (60*1000.0f) << " mins.\n";
  t.mark();
#endif
#if 0
  //: make a second table
  tf << "<TABLE BORDER=\"1\">\n";
  tf << "<caption align=\"top\"> pairs ranked wrt weighted distances of the sim transform's angle, scale and translation</caption>\n";

  //: pick the best match of the first model patch and rank order all the remaining patch pairs based on saved transformations
  vil_rgb<int> model_p1(0, 1, 0);  
  vil_rgb<int> model_p2(1, 0, 1);    // magenta
  vil_rgb<int> whole_model(0, 0, 0);   // model is black
  vil_rgb<int> query_p1(0, 0, 1);  
  vil_rgb<int> query_p2(1, 1, 0);  // yellow  
  vil_rgb<int> dummy_color(0, 0, 0);  // yellow  
  
  for (iter = map.begin(); iter != map.end(); iter++) {
    int model_id = iter->first;
    dbskr_shock_patch_sptr msp = id_map_model[iter->first];
    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = iter->second;

    vcl_ostringstream oss1;
    oss1 << msp->id();
    vcl_string patch_image_file = out_images + "model_" + oss1.str() + "_col1_";
    vcl_string patch_image_file2 = out_images + "model_" + oss1.str() + "_col2_";
     vcl_vector<vsol_polygon_2d_sptr> dummy(1, msp->get_traced_boundary());
    if (!create_image(model_img, patch_image_file, dummy, model_p1, dummy_color, dummy_color))
      return false;

    if (!create_image(model_img, patch_image_file2, dummy, model_p2, dummy_color, dummy_color))
      return false;

    if (match_vec->size() <=0 )
      continue;

    tf << "<TR> <TH>   ";  // leave the first cell of first row empty
    // write top n to the first row
    tf << "<TH> Match 1 </TH> ";
    tf << "</TH> </TR>\n";
    
    tf << "<TR> <TD> <img src=\"" << patch_image_file << "001.png\" width = \"" << width_str << "\"" << "\"> ";
    tf << msp->id() << " </TD> "; 
    
    // get the first match of this model patch
    int query_id = (*match_vec)[0].first;
    dbskr_shock_patch_sptr query_sp = id_map_test[query_id];
    vcl_ostringstream oss11;
    oss11 << query_id;
    vcl_string out_img = out_images + "query_" + oss1.str() + "_mapped_with_test_patch_" + oss11.str() + "_col1_";
    vcl_string out_img2 = out_images + "query_" + oss1.str() + "_mapped_with_test_patch_" + oss11.str() + "_col2_";
    tf << "<TD> <img src=\"" << out_img << "001.png\" width = \"" << width_str << "\"" << "\"> ";
    tf << model_id << " mapped with test patch: " << query_id << " </TD> ";

    vcl_vector<vsol_polygon_2d_sptr> polys;
    polys.push_back(query_sp->get_traced_boundary());

    vgl_h_matrix_2d<double> H; 
    if (!(*match_vec)[0].second->get_similarity_trans(H))   //, t
      return false;
    
    //: map the model polygon onto the query image
    vsol_polygon_2d_sptr trans_model_poly = bsol_algs::transform_about_point(model_poly, center, H);
    polys.push_back(trans_model_poly);
    vsol_polygon_2d_sptr trans_model_patch = bsol_algs::transform_about_point(msp->get_traced_boundary(), center, H);
    polys.push_back(trans_model_patch);
    
    if (!create_image(query_img, out_img, polys, query_p1, whole_model, model_p1))
        return false;
    if (!create_image(query_img, out_img2, polys, query_p2, whole_model, model_p2))
        return false;

    vcl_vector<vcl_pair< vcl_pair<int, int>, vcl_pair<double, vnl_matrix<double>* > > > out_vec;
    if (match->rank_order_other_patch_pairs_wrt_sim_trans(model_id, query_id, out_vec, params_.max_thres_)) {
      //: put all the match pairs in this row
      for (unsigned kkk = 0; kkk < out_vec.size(); kkk++) {
        int mod_id = out_vec[kkk].first.first;
        dbskr_shock_patch_sptr mod_msp = id_map_model[mod_id];
        int q_id = out_vec[kkk].first.second;
        dbskr_shock_patch_sptr q_msp = id_test_model[q_id];

        double dist = out_vec[kkk].second.first;
        vnl_matrix<double> * pair_trans = out_vec[kkk].second.second;  // T

        vcl_ostringstream oss_mod, oss_q;
        oss_mod << mod_id;
        oss_q << q_id;

        vcl_string patch_image_file = out_images + "model_" + oss_mod.str() + "_col1_";
        
        tf << "<TD> <img src=\"" << patch_image_file << "001.png\" width = \"" << width_str << "\"" << "\"> ";
        tf << mod_id << " </TD> "; 

        vsol_polygon_2d_sptr trans_mod_patch = bsol_algs::transform_about_point(mod_msp->get_traced_boundary(), center, H);
       
        vcl_string out_img = out_images + "query_m1_" + oss1.str() + "_m2_" + oss_mod.str() + "_q1_" + oss11.str() + "_q2_" + oss_q.str() + "_col1_green_col2_mag_";
        //: transform m1 wrt T

        tf << "<TD> <img src=\"" << out_img << "001.png\" width = \"" << width_str << "\"" << "\"> ";
        tf << " query: " << q_id << " pair dist: " << dist << " </TD> ";
      }

    }

    tf << "</TR>\n";
  }
#endif
 
  //: pick the best match of the first model patch and rank order all the remaining patch pairs based on saved transformations
  vil_rgb<int> model_p1(0, 1, 0);  
  vil_rgb<int> model_p2(1, 0, 1);    // magenta
  vil_rgb<int> whole_model(0, 0, 0);   // model is black
  vil_rgb<int> query_p1(0, 0, 1);  
  vil_rgb<int> query_p2(1, 1, 0);  // yellow  
  vil_rgb<int> dummy_color(0, 0, 0);  // yellow  

  vcl_vector<int> query_patches, model_patches;
  //query_patches.push_back(6116109);
  
  query_patches.push_back(6152206);
  
  //query_patches.push_back(5986406); 
  
  query_patches.push_back(5791612);
  
  query_patches.push_back(5791615);
  query_patches.push_back(5791618);
  query_patches.push_back(5750006); 
  query_patches.push_back(5750009); 

  model_patches.push_back(229001);
  model_patches.push_back(193501);
  
  //for (iter = map.begin(); iter != map.end(); iter++) {
    //int model_id = iter->first;
    //dbskr_shock_patch_sptr msp = id_map_model[iter->first];
    //vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = iter->second;

  for (unsigned mm = 0; mm < model_patches.size(); mm++) {
    int model_id = model_patches[mm];
    dbskr_shock_patch_sptr msp = id_map_model[model_id];
    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = map[model_id];
    
    //if (msp->id() != 229001)
    //  continue;

    vcl_ostringstream oss1;
    oss1 << msp->id();
    vcl_string patch_image_file = out_images + "model_" + oss1.str() + "_col1_";
    vcl_string patch_image_file2 = out_images + "model_" + oss1.str() + "_col2_";
     vcl_vector<vsol_polygon_2d_sptr> dummy(1, msp->get_traced_boundary());
    if (!create_image(model_img, patch_image_file, dummy, model_p1, dummy_color, dummy_color))
      return false;

    if (!create_image(model_img, patch_image_file2, dummy, model_p2, dummy_color, dummy_color))
      return false;

    if (match_vec->size() <=0 )
      continue;

    //: iterate over all matches of this model patch
    for (unsigned kkk = 0; int(kkk) < (*match_vec).size(); kkk++) {
      // get the match of this model patch
      int query_id = (*match_vec)[kkk].first;

      //: if not in query list go on
      bool found_it = false;
      for (unsigned qq = 0; qq < query_patches.size(); qq++) {
        if (query_id == query_patches[qq]) {
          found_it = true;
          break;
        }
      }
      if (!found_it)
        continue;

      dbskr_shock_patch_sptr query_sp = id_map_test[query_id];
      vcl_ostringstream oss11;
      oss11 << query_id;

      vgl_h_matrix_2d<double> H; 
      if (!(*match_vec)[kkk].second->get_similarity_trans(H))   //, t
        return false;

      //: map the model polygon onto the query image
      vsol_polygon_2d_sptr trans_model_poly = bsol_algs::transform_about_point(model_poly, center, H);
      vsol_polygon_2d_sptr trans_model_patch = bsol_algs::transform_about_point(msp->get_traced_boundary(), center, H);

      patch_cor_map_iterator iter2 = map.begin();
      for ( ; iter2 != map.end(); iter2++) {
        int model_id2 = iter2->first;
        dbskr_shock_patch_sptr msp2 = id_map_model[iter2->first];

        vsol_polygon_2d_sptr trans_m2_patch = bsol_algs::transform_about_point(msp2->get_traced_boundary(), center, H);

        vcl_ostringstream oss_m2;
        oss_m2 << model_id2;

        for (unsigned qq = 0; qq < query_patches.size(); qq++) {
          int other_query_id = query_patches[qq];
          dbskr_shock_patch_sptr other_qp = id_map_test[other_query_id];
          vcl_ostringstream oss_other_q;
          oss_other_q << other_query_id;

          vcl_string out_img = out_images + "im_m1_" + oss1.str() + "_green_mapped_with_test_patch_" + oss11.str() + "_blue_other_test_patch_" + oss_other_q.str() + "_yellow_m2_" + oss_m2.str() + "_magenta_";
        
          vcl_vector<vsol_polygon_2d_sptr> polys;
          polys.push_back(query_sp->get_traced_boundary());
          polys.push_back(other_qp->get_traced_boundary());

          polys.push_back(trans_model_poly);
          polys.push_back(trans_model_patch);
          
          polys.push_back(trans_m2_patch);

          vcl_vector<vil_rgb<int> > colors;
          colors.push_back(query_p1);
          colors.push_back(query_p2);

          colors.push_back(whole_model);
          colors.push_back(model_p1);
          
          colors.push_back(model_p2);

          if (!create_image2(query_img, out_img, polys, colors))
            return false;

          vcl_string out_img2 = out_images + "im_m1_" + oss1.str() + "_green_mapped_with_test_patch_" + oss11.str() + "_yellow_other_test_patch_" + oss_other_q.str() + "_blue_m2_" + oss_m2.str() + "_green_";
          colors.clear();
          colors.push_back(query_p2);
          colors.push_back(query_p1);
          colors.push_back(whole_model);
          colors.push_back(model_p2);
          
          colors.push_back(model_p1);

          if (!create_image2(query_img, out_img2, polys, colors))
            return false;

        }

      }
    }
  }
    
    

  vcl_cout << " ranking took " << t.real() / 1000.0f << " seconds " << t.real() / (60*1000.0f) << " mins.\n";
  t.mark();
#if 0
  for (iter = map.begin(); iter != map.end(); iter++) {
    int model_id = iter->first;
    dbskr_shock_patch_sptr msp = id_map_model[iter->first];
    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = iter->second;

    vcl_ostringstream oss1;
    oss1 << msp->id();
    vcl_string patch_image_file = out_images + "model_" + oss1.str() + "_";

    if (match_vec->size() <=1 )
      continue;

    tf << "<TR> <TH>   ";  // leave the first cell of first row empty
    // write top n to the first row
    tf << "<TH> Match 2 </TH> ";
    tf << "</TH> </TR>\n";
    
    tf << "<TR> <TD> <img src=\"" << patch_image_file << "001.png\" width = \"" << width_str << "\"" << "\"> ";
    tf << msp->id() << " </TD> "; 
    
    // get the first match of this model patch
    int query_id = (*match_vec)[1].first;
    vcl_ostringstream oss11;
    oss11 << query_id;
    vcl_string out_img = out_images + "query_" + oss1.str() + "_mapped_with_test_patch_" + oss11.str() + "_";
    tf << "<TD> <img src=\"" << out_img << "001.png\" width = \"" << width_str << "\"" << "\"> ";
    tf << model_id << " mapped with test patch: " << query_id << " </TD> ";


    vcl_vector<vcl_pair< vcl_pair<int, int>, double > > out_vec;
    if (match->rank_order_other_patch_pairs_wrt_sim_trans(model_id, query_id, out_vec, params_.max_thres_)) {
      //: put all the match pairs in this row
      for (unsigned kkk = 0; kkk < out_vec.size(); kkk++) {
        int mod_id = out_vec[kkk].first.first;
        int q_id = out_vec[kkk].first.second;
        double dist = out_vec[kkk].second;

        vcl_ostringstream oss1, oss11;
        oss1 << mod_id;
        oss11 << q_id;

        vcl_string patch_image_file = out_images + "model_" + oss1.str() + "_";
        tf << "<TD> <img src=\"" << patch_image_file << "001.png\" width = \"" << width_str << "\"" << "\"> ";
        tf << mod_id << " </TD> "; 

        vcl_string out_img = out_images + "query_" + oss1.str() + "_mapped_with_test_patch_" + oss11.str() + "_";
        tf << "<TD> <img src=\"" << out_img << "001.png\" width = \"" << width_str << "\"" << "\"> ";
        tf << " query: " << q_id << " pair dist: " << dist << " </TD> ";
      }

    }

    tf << "</TR>\n";
  }

  vcl_cout << " ranking took " << t.real() / 1000.0f << " seconds " << t.real() / (60*1000.0f) << " mins.\n";
  t.mark();

  for (iter = map.begin(); iter != map.end(); iter++) {
    int model_id = iter->first;
    dbskr_shock_patch_sptr msp = id_map_model[iter->first];
    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = iter->second;

    vcl_ostringstream oss1;
    oss1 << msp->id();
    vcl_string patch_image_file = out_images + "model_" + oss1.str() + "_";

    if (match_vec->size() <=2 )
      continue;

    tf << "<TR> <TH>   ";  // leave the first cell of first row empty
    // write top n to the first row
    tf << "<TH> Match 3 </TH> ";
    tf << "</TH> </TR>\n";
    
    tf << "<TR> <TD> <img src=\"" << patch_image_file << "001.png\" width = \"" << width_str << "\"" << "\"> ";
    tf << msp->id() << " </TD> "; 
    
    // get the first match of this model patch
    int query_id = (*match_vec)[2].first;
    vcl_ostringstream oss11;
    oss11 << query_id;
    vcl_string out_img = out_images + "query_" + oss1.str() + "_mapped_with_test_patch_" + oss11.str() + "_";
    tf << "<TD> <img src=\"" << out_img << "001.png\" width = \"" << width_str << "\"" << "\"> ";
    tf << model_id << " mapped with test patch: " << query_id << " </TD> ";


    vcl_vector<vcl_pair< vcl_pair<int, int>, double > > out_vec;
    if (match->rank_order_other_patch_pairs_wrt_sim_trans(model_id, query_id, out_vec, params_.max_thres_)) {
      //: put all the match pairs in this row
      for (unsigned kkk = 0; kkk < out_vec.size(); kkk++) {
        int mod_id = out_vec[kkk].first.first;
        int q_id = out_vec[kkk].first.second;
        double dist = out_vec[kkk].second;

        vcl_ostringstream oss1, oss11;
        oss1 << mod_id;
        oss11 << q_id;

        vcl_string patch_image_file = out_images + "model_" + oss1.str() + "_";
        tf << "<TD> <img src=\"" << patch_image_file << "001.png\" width = \"" << width_str << "\"" << "\"> ";
        tf << mod_id << " </TD> "; 

        vcl_string out_img = out_images + "query_" + oss1.str() + "_mapped_with_test_patch_" + oss11.str() + "_";
        tf << "<TD> <img src=\"" << out_img << "001.png\" width = \"" << width_str << "\"" << "\"> ";
        tf << " query: " << q_id << " pair dist: " << dist << " </TD> ";
      }

    }

    tf << "</TR>\n";
  }
#endif

  vcl_cout << " ranking took " << t.real() / 1000.0f << " seconds " << t.real() / (60*1000.0f) << " mins.\n";
  t.mark();

  tf << "</TABLE>\n";
  tf.close();

  return true;

}

bool batch_convert(vcl_string input_dir)
{

  vcl_string file_set = input_dir + "*.ps";
  vcl_cout << "iterating over the files: " << file_set << vcl_endl;
  for (vul_file_iterator fi(file_set); fi; ++fi)
  {
    vcl_cout << fi() << vcl_endl;
    if (!vul_file::exists(fi()))
      continue;
    
    // find the object name
    vcl_string command = "pstopnm -portrait -xborder 0 -yborder 0 -ppm -verbose ";
    command = command + vcl_string(fi());
    vcl_cout << "\tcommand: " << command << vcl_endl;
    system(command.c_str());
  }

  vcl_cout << " done!\n";
  return true;
}

bool detect_instance(vcl_string model_image_file, vcl_string model_st_file, 
                        vcl_string query_image_file, vcl_string query_st_file, 
                        vcl_string match_file, vcl_string detection_param_xml, vcl_string out_name, vcl_string width_str)
{
  if (model_image_file.compare("") == 0 || model_st_file.compare("") == 0 || query_image_file.compare("") == 0 || 
      query_st_file.compare("") == 0 || match_file.compare("") == 0 || detection_param_xml.compare("") == 0 || out_name.compare("") == 0 ||
      width_str.compare("") == 0)
    return false;

  vil_image_resource_sptr model_img;
  model_img = vil_load_image_resource(model_image_file.c_str());
  if (!model_img)
    return false;

  vil_image_resource_sptr query_img;
  query_img = vil_load_image_resource(query_image_file.c_str());

  if (!query_img)
    return false;

  /////////////////
  bxml_document param_doc = bxml_read(detection_param_xml);
  if (!param_doc.root_element())
    return false;
  
  if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
    vcl_cout << "params root is not ELEMENT\n";
    return false;
  }

  dbskr_detect_patch_params params_;
  if (!params_.parse_from_data(param_doc.root_element()))
    return false;
  //////////////////

  vul_timer t;
  t.mark();

  dbskr_shock_patch_storage_sptr model_st = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(model_st_file.c_str());
  model_st->b_read(ifs);
  ifs.close();

  vcl_cout << "loaded: " << model_st->size() << " patches in the model\n";

  dbskr_shock_patch_storage_sptr query_st = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifsq(query_st_file.c_str());
  query_st->b_read(ifsq);
  ifsq.close();

  vcl_cout << "loaded: " << query_st->size() << " patches in the query\n";
  
  //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
  //vcl_cout << model_st->size() << " patches in model storage, reading shocks..\n";
  dbsk2d_xshock_graph_fileio loader;
  //: load esfs for each patch
  vcl_string storage_end = "patch_strg.bin";
  vsol_polygon_2d_sptr model_poly;
  if (model_st->size() > 0)
    model_poly = model_st->get_patch(0)->get_outer_boundary();
  for (unsigned iii = 0; iii < model_st->size(); iii++) {
    dbskr_shock_patch_sptr sp = model_st->get_patch(iii);
    vcl_string patch_esf_name = model_st_file.substr(0, model_st_file.length()-storage_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
    sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
                            params_.shock_match_params_.circular_ends_,
                            params_.shock_match_params_.combined_edit_, 
                            params_.shock_match_params_.scurve_sample_ds_, 
                            params_.shock_match_params_.scurve_interpolate_ds_); // so that prepare_tree() and tree() methods of patch have the needed params
    if (model_poly->size() < sp->get_outer_boundary()->size())
      model_poly = sp->get_outer_boundary();
  }

  for (unsigned iii = 0; iii < query_st->size(); iii++) {
    dbskr_shock_patch_sptr sp = query_st->get_patch(iii);
    vcl_string patch_esf_name = query_st_file.substr(0, query_st_file.length()-storage_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
    sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
                            params_.shock_match_params_.circular_ends_,
                            params_.shock_match_params_.combined_edit_, 
                            params_.shock_match_params_.scurve_sample_ds_, 
                            params_.shock_match_params_.scurve_interpolate_ds_);  // so that prepare_tree() and tree() methods of patch have the needed params
  }


  //: load the match
  dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
  vsl_b_ifstream ifsm(match_file.c_str());
  match->b_read(ifsm);
  ifsm.close();

  vcl_cout << "loaded the match file\n";

  //: prepare id maps for this match
  vcl_map<int, dbskr_shock_patch_sptr> model_map;
  for (unsigned ii = 0; ii < model_st->size(); ii++) 
    model_map[model_st->get_patch(ii)->id()] = model_st->get_patch(ii);
  match->set_id_map1(model_map);

  vcl_map<int, dbskr_shock_patch_sptr> query_map;
  for (unsigned ii = 0; ii < query_st->size(); ii++) 
    query_map[query_st->get_patch(ii)->id()] = query_st->get_patch(ii);
  match->set_id_map2(query_map);

  if (params_.use_normalized_costs_) {
    if (params_.use_reconst_boundary_length_)
      if (!match->compute_length_norm_costs_of_cors()) // sorts the matches after recomputation of the normalized cost
        return false;
    else if (params_.use_total_splice_cost_)
      if (!match->compute_splice_norm_costs_of_cors()) // sorts the matches after recomputation of the normalized cost
        return false;
  }

  vcl_cout << " loading took " << t.real() / 1000.0f << " seconds " << t.real() / (60*1000.0f) << " mins.\n";
  t.mark();

  //: create all the similariy transformations
  match->compute_similarity_transformations();

  vcl_cout << " similarity computations took " << t.real() / 1000.0f << " seconds " << t.real() / (60*1000.0f) << " mins.\n";
  t.mark();
  
  vcl_string out_images = out_name + "/";
  vul_file::make_directory(out_images);

  vcl_cout << "patches are sorted wrt parameters in the detection param file\n";
  vcl_ofstream tf((out_name + ".html").c_str(), vcl_ios::app);
  
  if (!tf) {
    vcl_cout << "Unable to open output html file " << out_name << " for write " << vcl_endl;
    return false;
  }
  int visualization_n = 10;

  tf << "<TABLE BORDER=\"1\">\n";
  tf << "<caption align=\"top\">" << match_file << "</caption>\n";
  tf << "<TR> <TH>   ";  // leave the first cell of first row empty
  // write top n to the first row
  tf << "</TH> </TR>\n";

   //vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* > 
  vsol_point_2d_sptr center = new vsol_point_2d(0, 0);

  vcl_vector<vsol_box_2d_sptr> detection_boxes;
  match->detect_instance_wrt_trans(detection_boxes, params_.det_params_.k_, params_.min_thres_);
  
  vcl_vector<vsol_polygon_2d_sptr> polys;
  for (unsigned i = 0; i < detection_boxes.size(); i++) {
    vsol_polygon_2d_sptr poly = bsol_algs::poly_from_box(detection_boxes[i]);
    polys.push_back(poly);
  }

  vil_rgb<int> outer(1, 1, 0);
  vil_rgb<int> model(1, 1, 0);
  vil_rgb<int> others(1, 1, 0);

  vcl_string out_img = out_images + "query_detection_";
  if (!create_image(query_img, out_img, polys, outer, model, others))
    return false;

  tf << "<TR> <TD> <img src=\"" << out_img << "001.png\" width = \"" << width_str << "\"" << "\"> ";
  tf << "detection boxes at " << params_.min_thres_ << " in yellow </TD> "; 
  tf << "</TR>\n";
  
  vcl_cout << " detection took " << t.real() / 1000.0f << " seconds " << t.real() / (60*1000.0f) << " mins.\n";
  t.mark();

  tf << "</TABLE>\n";
  tf.close();

  return true;

}




