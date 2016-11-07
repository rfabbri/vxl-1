// This is brcv/rec/dbskr/dbskr_utilities.cxx

#include "dbskr_utilities.h"
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbsk2d/algo/dbsk2d_shock_transforms.h>

#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_file_io.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>
#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>
#include <dbskr/dbskr_shock_patch.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <dbsol/dbsol_file_io.h>

#include <vil/vil_image_resource.h>
#include <bil/algo/bil_color_conversions.h>
#include <vil/algo/vil_threshold.h>

#include <dbdet/algo/dbdet_third_order_color_detector.h>
#include <dbdet/algo/dbdet_third_order_edge_det.h>
#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/algo/dbdet_sel.h>

#include <dbsol/algo/dbsol_img_curve_algs.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/dbsol_file_io.h>

#include <bbas/bsol/bsol_algs.h>

#include<vul/vul_psfile.h>
#include<vul/vul_timer.h>

#include <dbdet/tracer/dbdet_contour_tracer.h>

#include <dbgl/algo/dbgl_curve_smoothing.h>

vsol_polyline_2d_sptr smooth_poly(vsol_polyline_2d_sptr new_curve, unsigned smoothing_nsteps)
{
  vcl_vector<vgl_point_2d<double> > pv;
  for (unsigned i = 0; i < new_curve->size(); i++)
    pv.push_back(new_curve->vertex(i)->get_p());

  dbgl_csm( pv, 1.0, smoothing_nsteps);

  vcl_vector<vsol_point_2d_sptr> vv;
  for (unsigned i = 0; i < pv.size(); i++)
    vv.push_back(new vsol_point_2d(pv[i]));

  return new vsol_polyline_2d(vv);
}

vsol_polygon_2d_sptr smooth_polygon(vsol_polygon_2d_sptr new_curve, unsigned smoothing_nsteps)
{
  vcl_vector<vgl_point_2d<double> > pv;
  for (unsigned i = 0; i < new_curve->size(); i++)
    pv.push_back(new_curve->vertex(i)->get_p());

  dbgl_csm( pv, 1.0, smoothing_nsteps);

  vcl_vector<vsol_point_2d_sptr> vv;
  for (unsigned i = 0; i < pv.size(); i++)
    vv.push_back(new vsol_point_2d(pv[i]));

  return new vsol_polygon_2d(vv);
}

bool write_contours_to_ps(vcl_vector< vsol_spatial_object_2d_sptr > image_curves, 
                          vcl_vector< vsol_spatial_object_2d_sptr > euler_sps,
                          vcl_string output_name)
{
  //vcl_ostringstream s;
  //s<<frameno;
  vcl_string outputfile=output_name+".ps";
  vul_psfile psfile(outputfile.c_str(), false);
  psfile.set_scale_x(50);
  psfile.set_scale_y(50);

  if(image_curves.size()>0)
  {
    for (unsigned int i=0;i<image_curves.size();i++)
    {
      if (image_curves[i]->cast_to_curve())
        if (image_curves[i]->cast_to_curve()->cast_to_polyline()) {
          vsol_polyline_2d_sptr poly = image_curves[i]->cast_to_curve()->cast_to_polyline();
          psfile.set_line_width(1.0);
          psfile.set_fg_color(0,0,1);
          for(int k=0;k<int(poly->size())-1;k++)
            psfile.line((float)poly->vertex(k)->x(),
                        (float)poly->vertex(k)->y(),
                        (float)poly->vertex(k+1)->x(),
                        (float)poly->vertex(k+1)->y());
        }

    }
  }

  if(euler_sps.size()>0)
  {
    for (unsigned int i=0;i<euler_sps.size();i++)
    {
      if (euler_sps[i]->cast_to_curve())
        if (euler_sps[i]->cast_to_curve()->cast_to_polyline()) {
          vsol_polyline_2d_sptr poly = euler_sps[i]->cast_to_curve()->cast_to_polyline();
          psfile.set_line_width(1.0);
          psfile.set_fg_color(1,0,0);
          for(int k=0;k<int(poly->size())-1;k++)
            psfile.line((float)poly->vertex(k)->x(),
                        (float)poly->vertex(k)->y(),
                        (float)poly->vertex(k+1)->x(),
                        (float)poly->vertex(k+1)->y());
        }
    }
  }

  psfile.close();
  return true;
}

// length_thres            : prune out the polylines that are shorter in length than this threshold before shock computation, typically 2 pixels
// pruning_region_width    : the width of the region to consider on both sides of a polyline to determine the color contrast, typically 5 pixels
// pruning_color_threshold : the threshold for LAB space color contrast, typically 0.4
// rms                     : root mean square error used in line fitting process before shock computation, typically 0.05
dbsk2d_shock_graph_sptr extract_shock(vil_image_resource_sptr img_sptr,
                                      unsigned min_size_to_keep,
                                      double avg_grad_mag_threshold,
                                      double length_thresh, 
                                      double pruning_region_width, 
                                      double pruning_color_threshold, 
                                      double rms, unsigned smoothing_nsteps,
                                      double cont_t, double app_t, 
                                      bool write_output, vcl_string out_shock_name, double edge_detection_sigma) 
{
  vil_image_view<vxl_byte> I = img_sptr->get_view(0, img_sptr->ni(), 0, img_sptr->nj() );
  vil_image_view<float> L, A, B;

  dbdet_edgemap_sptr edge_map;
  
  bool color_image = false;
  
  // extract third order edges  ---------------------------------------------------------------------------------------
  unsigned grad_op = 0; // Gaussian
  unsigned convolution_algo = 0; // 2-D
  unsigned parabola_fit = 0; // 3-point fit
  //double sigma = 1.0f;
  double sigma = edge_detection_sigma;
  double thresh = 0.1f;  // threshold for gradient magnitude in edge detection, we always set this 1.0 then eliminate based on the linked contours
                         // avg_grad_mag_threshold which is typically higher than 1.0f
  int N = 1;
  
  vul_timer t;
  t.mark();
  vcl_cout << "edges... ";
    
  //make sure these images are one plane images
  if (I.nplanes() != 3){
    vcl_cout << "In edge_detector - GREY image!!! \n";
    edge_map = dbdet_detect_third_order_edges(I, sigma, thresh, N, parabola_fit, grad_op, false);  // reduce_tokens is false
  } 
  else {
    color_image = true;
    convert_RGB_to_Lab(I, L, A, B);   
    edge_map = dbdet_third_order_color(grad_op, convolution_algo, N, sigma, thresh, parabola_fit, L, A, B);
  }

  //-------------------------------------------------------------------------------------------------------------------
  vcl_cout << t.real()/1000.0f << " secs. linking... ";

  // link the edges ---------------------------------------------------------------------------------------------------
  double nrad = 2.0f, dx = 0.2f, dt = 15.0f;
  unsigned max_size_to_group = 7, min_size_to_link = 3;

  dbdet_curvelet_map cvlet_map; 
  dbdet_edgel_link_graph edge_link_graph; 
  dbdet_curve_fragment_graph curve_frag_graph;

  //dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_ES_curve_model>(edge_map, cvlet_map, edge_link_graph, curve_frag_graph, 
  //                                                                 dbdet_curvelet_params(dbdet_curve_model::ES, nrad, dt, dx));  // ES model w/o perturb

  // curve model choice = Circular Arc model w/o perturbations
  dbdet_sel_sptr edge_linker = new dbdet_sel<dbdet_CC_curve_model>( edge_map, cvlet_map, edge_link_graph, curve_frag_graph,
                                                                    dbdet_curvelet_params(dbdet_curve_model::CC, nrad, dt, dx, false));
  
  edge_linker->build_curvelets_greedy(max_size_to_group); // grouping algo -- greedy local grouping
  edge_linker->use_anchored_curvelets_only();
  edge_linker->construct_the_link_graph(min_size_to_link, 0);  // linkgraph algo = 0 (TODO: ask Amir ? this zero seems redundant..?)
  edge_linker->extract_image_contours_from_the_link_graph();

  vcl_vector< vsol_spatial_object_2d_sptr > image_curves;
  vcl_vector< vsol_spatial_object_2d_sptr > image_curves_all;
  vcl_vector< vsol_spatial_object_2d_sptr > image_curves_avg_mag_pruned;
  vcl_vector< vsol_spatial_object_2d_sptr > image_curves_length_pruned;
  
  vsol_box_2d_sptr bbox = new vsol_box_2d();
  double mean_color_dist_all = 0;
  int cnt_all = 0;
  double mean_color_dist_selected = 0;
  int cnt_sel = 0;

  vcl_list<dbdet_edgel_chain*>::iterator f_it = curve_frag_graph.frags.begin();
  for (; f_it != curve_frag_graph.frags.end(); f_it++)
  {
    dbdet_edgel_chain* chain = (*f_it);

    //only keep the longer contours
    if (chain->edgels.size() < min_size_to_keep)
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

    //smooth this contour
    dbgl_csm(pts, 1.0f, smoothing_nsteps);   // psi = 1.0f

    vcl_vector<vsol_point_2d_sptr> vsol_pts;
    vsol_pts.reserve(pts.size());
    for (unsigned i=0; i<pts.size(); ++i)
      vsol_pts.push_back(new vsol_point_2d(pts[i]));
    vsol_polyline_2d_sptr new_curve = new vsol_polyline_2d(vsol_pts);

    image_curves_all.push_back(new_curve->cast_to_spatial_object());

    //only keep the contours that are above the threshold
    if (avg_contrast<avg_grad_mag_threshold)
      continue;

    image_curves_avg_mag_pruned.push_back(new_curve->cast_to_spatial_object());

    if (new_curve->length() > length_thresh) {

      image_curves_length_pruned.push_back(new_curve->cast_to_spatial_object());

      dbsol_interp_curve_2d_sptr c = new dbsol_interp_curve_2d();
      dbsol_curve_algs::interpolate_linear(c.ptr(), new_curve); // open curve
      double dist;
      if (color_image)
        dist = get_color_distance_of_curve_regions(c, float(pruning_region_width), L, A, B, 14.0f);  // color_gamma = 14
      else
        dist = get_intensity_distance_of_curve_regions(c, float(pruning_region_width), I, 14.0f);
      mean_color_dist_all += dist;
      cnt_all++;
      if (dist < pruning_color_threshold) {
        mean_color_dist_selected += dist;
        cnt_sel++;
        vsol_polyline_2d_sptr fitted_poly = fit_lines_to_contour(new_curve, rms);
        image_curves.push_back(fitted_poly->cast_to_spatial_object());
        fitted_poly->compute_bounding_box();
        bbox->grow_minmax_bounds(fitted_poly->get_bounding_box());
      }
    }

#if 0  // before April 25
      vcl_vector<vsol_point_2d_sptr> pts;
      for (unsigned j=0; j<chain->edgels.size(); j++)
        pts.push_back(new vsol_point_2d(chain->edgels[j]->pt));
      vsol_polyline_2d_sptr new_curve = new vsol_polyline_2d(pts);
      if (new_curve->length() > length_thresh) {
        dbsol_interp_curve_2d_sptr c = new dbsol_interp_curve_2d();
        dbsol_curve_algs::interpolate_linear(c.ptr(), new_curve); // open curve
        double dist;
        if (color_image)
          dist = get_color_distance_of_curve_regions(c, float(pruning_region_width), L, A, B, 14.0f);  // color_gamma = 14
        else
          dist = get_intensity_distance_of_curve_regions(c, float(pruning_region_width), I, 14.0f);
        mean_color_dist_all += dist;
        cnt_all++;
        if (dist < pruning_color_threshold) {
          mean_color_dist_selected += dist;
          cnt_sel++;
          vsol_polyline_2d_sptr spoly = smooth_poly(new_curve, smoothing_nsteps);
          vsol_polyline_2d_sptr fitted_poly = fit_lines_to_contour(spoly, rms);
          image_curves.push_back(fitted_poly->cast_to_spatial_object());
          fitted_poly->compute_bounding_box();
          bbox->grow_minmax_bounds(fitted_poly->get_bounding_box());
        }
      }
#endif
  
  }
  vcl_cout << "mean color dist of all curves: " << mean_color_dist_all/cnt_all << " of selected: " << mean_color_dist_selected/cnt_sel << " threshold was: " << pruning_color_threshold << vcl_endl;
  vcl_cout << "bbox (minx, miny) (width, height): " << "(" << bbox->get_min_x() << ", " << bbox->get_min_y() << ") (" << bbox->width() << ", " << bbox->height() << ")\n";
  int offset = 3;
  bbox->add_point(bbox->get_min_x()-offset, bbox->get_min_y()-offset);
  bbox->add_point(bbox->get_max_x()+offset, bbox->get_max_y()+offset);  // enlarge the box with 3 pixels
  vsol_polygon_2d_sptr box_poly = bsol_algs::poly_from_box(bbox);
  image_curves.push_back(box_poly->cast_to_spatial_object());

  if (write_output) {
    // save the boundary curves
    dbsk2d_file_io::save_bnd_v3_0(out_shock_name+"_boundary.bnd", image_curves);
    
    dbsol_save_cem(image_curves, out_shock_name+"_boundary.cem");
    dbsol_save_cem(image_curves_all, out_shock_name+"_all_boundary.cem");
    dbsol_save_cem(image_curves_avg_mag_pruned, out_shock_name+"_after_avg_mag_pruned_boundary.cem");
    dbsol_save_cem(image_curves_length_pruned, out_shock_name+"_after_length_pruned_boundary.cem");
  }
  
  edge_map = 0;
  edge_linker = 0;

  //-------------------------------------------------------------------------------------------------------------------
  vcl_cout << t.real()/1000.0f << " secs. computing shocks... ";
  // compute shocks ---------------------------------------------------------------------------------------------------
  float xmin=0, ymin=0, cell_width=1000.0f, cell_height=1000.0f; int num_rows=1, num_cols=1;
  dbsk2d_boundary_sptr boundary = dbsk2d_create_boundary(image_curves, false, xmin, ymin, num_rows, num_cols, cell_width, cell_height, true, true);
  dbsk2d_ishock_graph_sptr isg = dbsk2d_compute_ishocks(boundary);
  if (!isg) {
    vcl_cout << "Problem in intrinsic shock computation! Exiting!\n";
    return 0;
  }
  dbsk2d_shock_graph_sptr sg = new dbsk2d_shock_graph();
  dbsk2d_prune_ishock ishock_pruner(isg, sg);
  ishock_pruner.prune(1.0f);  // prune threshold is 1.0f
  ishock_pruner.compile_coarse_shock_graph();
  if (!sg->number_of_vertices() || !sg->number_of_edges()) {
    vcl_cout << "Problem in coarse shock computation!\n";
    return 0;
  }
  // -------------------------------------------------------------------------------------------------------------------
  vcl_cout << (t.real()/1000.0f)/60.0 << " mins. " << " gap transforms... ";
  // gap transforms ----------------------------------------------------------------------------------------------------
  //float low_cont_t = 0.1f, high_cont_t = 0.5f, low_app_t = 0.1f, high_app_t = 0.5f;
  float alpha_cont = 1.0f, alpha_app = 1.0f;
  dbsk2d_shock_transforms transformer(isg, sg);
  transformer.set_image(img_sptr);
  transformer.set_curve_length_gamma(2.0);
  transformer.perform_all_gap_transforms(cont_t, app_t, alpha_cont, alpha_app, true);
  vcl_vector< vsol_spatial_object_2d_sptr > euler_sps;
  transformer.get_eulerspirals(euler_sps);
  transformer.clear_eulerspirals();  // is this cleaning the spirals pointed by normal pointers??? TODO: check!!

  if (!sg->number_of_vertices() || !sg->number_of_edges()) {
    vcl_cout << "Problem in gap transforms!\n";
    return 0;
  }

  dbsk2d_sample_ishock ishock_sampler(sg);
  ishock_sampler.sample(1.0f, BOTHSIDE);
  dbsk2d_shock_graph_sptr sampled_sg = ishock_sampler.extrinsic_shock_graph();

  // -------------------------------------------------------------------------------------------------------------------
  if (write_output) {
    //write_contours_to_ps(image_curves, euler_sps, out_shock_name+"_curves");
    dbsk2d_xshock_graph_fileio writer;
    writer.save_xshock_graph(sampled_sg, out_shock_name+".esf");
    dbsk2d_file_io::save_bnd_v3_0(out_shock_name+"_boundary_gaps.bnd", euler_sps);
    dbsol_save_cem(euler_sps, out_shock_name+"_boundary_gaps.cem");
  }
  
  vcl_cout << "total: " << (t.real()/1000.0f)/60.0 << " mins. Done!\n";

  return sampled_sg;
}

dbsk2d_shock_graph_sptr extract_shock_from_mask(vil_image_resource_sptr image_sptr, 
                                      bool write_output, vcl_string out_shock_name, int smoothing_nsteps, double rms) 
{

  vul_timer t;
  t.mark();
  
  vil_image_view<bool> binary_img;

  // no conversion necessary when input is binary image
  if (image_sptr->pixel_format()==binary_img.pixel_format() && 
    image_sptr->nplanes() == 1)
  {
    binary_img = image_sptr->get_view();
  }
  else
  {
    // get the grey view
    vil_image_view<vxl_byte> image;  
    if (image_sptr->nplanes()==1)
    {
      image = *vil_convert_cast(vxl_byte(), image_sptr->get_view());
    }
    else
    {
      image = *vil_convert_cast(vxl_byte(), 
      vil_convert_to_grey_using_rgb_weighting(image_sptr->get_view()));
    }

    // threshold the image to make sure that it is binary
    vxl_byte threshold_value = 128;
    vxl_byte min_value = 0, max_value = 0;
    vil_math_value_range(image, min_value, max_value);
    threshold_value = (min_value+max_value)/2;
    vil_threshold_above<vxl_byte>(image, binary_img, threshold_value);
  }

  vcl_cout << "image ok..";
  float sigma = 1.0f;
  int nsteps = 1;
  float beta = 0.3f;
  
  //invoke the tracer
  dbdet_contour_tracer ctracer;
  ctracer.set_sigma(sigma);
  ctracer.set_curvature_smooth_nsteps(nsteps);
  ctracer.set_curvature_smooth_beta(beta);
  ctracer.trace(binary_img);

  if (ctracer.largest_contour().size() > 3) {
    vcl_cout << "contour ok..";
    vsol_polygon_2d_sptr poly = new vsol_polygon_2d(ctracer.largest_contour());

    vsol_polygon_2d_sptr spoly = smooth_polygon(poly, smoothing_nsteps);
    vsol_polygon_2d_sptr traced_poly_ = fit_lines_to_contour(spoly, rms);

    if (!traced_poly_) 
      return 0;

    vcl_vector< vsol_spatial_object_2d_sptr > conts;
    conts.push_back(traced_poly_->cast_to_spatial_object());

    if (write_output) {
      // save the boundary curves
      vcl_string out_name_str = out_shock_name+"_boundary.con";
      dbsol_save_con_file(out_name_str.c_str(), poly);
    }

    // compute shocks ---------------------------------------------------------------------------------------------------
    float xmin=0, ymin=0, cell_width=1000.0f, cell_height=1000.0f; int num_rows=1, num_cols=1;
    dbsk2d_boundary_sptr boundary = dbsk2d_create_boundary(conts, false, xmin, ymin, num_rows, num_cols, cell_width, cell_height, true, true);
    dbsk2d_ishock_graph_sptr isg = dbsk2d_compute_ishocks(boundary);
    if (!isg) {
      return 0;
    }
    dbsk2d_shock_graph_sptr sg = new dbsk2d_shock_graph();
    dbsk2d_prune_ishock ishock_pruner(isg, sg);
    ishock_pruner.prune(1.0f);  // prune threshold is 1.0f
    ishock_pruner.compile_coarse_shock_graph();
    if (!sg->number_of_vertices() || !sg->number_of_edges()) {
      return 0;
    }

    dbsk2d_sample_ishock ishock_sampler(sg);
    ishock_sampler.sample(1.0f);
    dbsk2d_shock_graph_sptr sampled_sg = ishock_sampler.extrinsic_shock_graph();

    // -------------------------------------------------------------------------------------------------------------------
    if (write_output) {
      dbsk2d_xshock_graph_fileio writer;
      writer.save_xshock_graph(sampled_sg, out_shock_name+".esf");
    }

    vcl_cout << "total: " << (t.real()/1000.0f)/60.0 << " mins. Done!\n";
    return sampled_sg;

  } else
    return 0;
 
}

                   
dbsk2d_shock_graph_sptr compute_shock(vsol_polygon_2d_sptr poly,float shock_pruning_threshold) {


  vcl_vector< vsol_spatial_object_2d_sptr > conts;
  conts.push_back(poly->cast_to_spatial_object());
  float xmin=0, ymin=0, cell_width=1000.0f, cell_height=1000.0f; int num_rows=1, num_cols=1;
  dbsk2d_boundary_sptr boundary = dbsk2d_create_boundary(conts, false, xmin, ymin, num_rows, num_cols, cell_width, cell_height, true, true);
  dbsk2d_ishock_graph_sptr isg = dbsk2d_compute_ishocks(boundary);
  if (!isg) {
    return 0;
  }

  dbsk2d_shock_graph_sptr sg = new dbsk2d_shock_graph();
  dbsk2d_prune_ishock ishock_pruner(isg, sg);
  ishock_pruner.prune(shock_pruning_threshold);  // prune threshold is 1.0f
  ishock_pruner.compile_coarse_shock_graph();
  if (!sg->number_of_vertices() || !sg->number_of_edges()) {
    return 0;
  }

  dbsk2d_sample_ishock ishock_sampler(sg);
  ishock_sampler.sample(1.0f);
  dbsk2d_shock_graph_sptr final_shock = ishock_sampler.extrinsic_shock_graph();

  return final_shock;
}
