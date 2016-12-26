#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/dbskr_utilities.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_box_2d.h>

#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon.h>

#include <bsol/bsol_algs.h>
#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <vgl/vgl_polygon_scan_iterator.h>

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>
#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>

#include <dbskr/dbskr_tree.h>

#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/vil_save.h>

#include <dbsol/algo/dbsol_img_curve_algs.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_generator.h>

#include <vil/vil_plane.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>

void dbskr_shock_patch::set_outer_boundary(vsol_polygon_2d_sptr poly) 
{
  if (poly) {
    poly_ = poly; 
    poly_->compute_bounding_box(); 
    bounding_box_ = new vsol_box_2d(*(poly_->get_bounding_box()));
    p_ = bsol_algs::vgl_from_poly(poly_);
  }
}

bool dbskr_shock_patch::inside_box(double x, double y) 
{ 
  return bounding_box_->inside(x, y);
}

bool dbskr_shock_patch::inside(double x, double y) 
{ 
  return p_.contains(x, y);
}

vsol_box_2d_sptr dbskr_shock_patch::union_box(dbskr_shock_patch& other) {
  vsol_box_2d_sptr ob = new vsol_box_2d();
  ob->grow_minmax_bounds(bounding_box_);
  ob->grow_minmax_bounds(other.bounding_box_);
  return ob;
}

#define NEGLIGIBLE  (1e-2)
//: this function is used to determine if any of the real boundaries is on this box
//  the input box is usually the bounding box of the scene (all curve fragments in the scene) 
//  then this patch is usually eliminated 
bool dbskr_shock_patch::boundary_on_the_box(vsol_box_2d_sptr bbox)
{
  double minx = bbox->get_min_x();
  double miny = bbox->get_min_y();
  double maxx = bbox->get_max_x();
  double maxy = bbox->get_max_y();

  for (unsigned i = 0; i < real_boundaries_.size(); i++) {
    if (real_boundaries_[i]->size()) {
      for (unsigned k = 0; k < real_boundaries_[i]->size(); k++) {
        vsol_point_2d_sptr p = real_boundaries_[i]->vertex(k);
        if (vcl_abs(p->x() - minx) < NEGLIGIBLE || 
            vcl_abs(p->x() - maxx) < NEGLIGIBLE || 
            vcl_abs(p->y() - miny) < NEGLIGIBLE || 
            vcl_abs(p->y() - maxy) < NEGLIGIBLE )
          return true;
      }
    }
  }
  return false;
}

//: clean the real boundary set from the bbox contours
void dbskr_shock_patch::clean_real_boundaries(vsol_box_2d_sptr bbox)
{
  double minx = bbox->get_min_x();
  double miny = bbox->get_min_y();
  double maxx = bbox->get_max_x();
  double maxy = bbox->get_max_y();

  for (unsigned i = 0; i < real_boundaries_.size(); i++) {
    if (real_boundaries_[i]->size()) {
      for (unsigned k = 0; k < real_boundaries_[i]->size(); k++) {
        vsol_point_2d_sptr p = real_boundaries_[i]->vertex(k);
        if (vcl_abs(p->x() - minx) < NEGLIGIBLE || 
            vcl_abs(p->x() - maxx) < NEGLIGIBLE || 
            vcl_abs(p->y() - miny) < NEGLIGIBLE || 
            vcl_abs(p->y() - maxy) < NEGLIGIBLE ) {
            real_boundaries_[i] = 0;
            break;
        }
      }
    }
  }

  vcl_vector<vsol_polyline_2d_sptr> kept;
  for (unsigned i = 0; i < real_boundaries_.size(); i++) {
    if (real_boundaries_[i])
      kept.push_back(real_boundaries_[i]);
  }

  real_boundaries_.clear();
  real_boundaries_.insert(real_boundaries_.begin(), kept.begin(), kept.end());

  return;
}

//: bool create a binary image and contour trace it to get a non-overlapping outer boundary for shock extraction
bool dbskr_shock_patch::trace_outer_boundary()
{
  if (!poly_)  // if there is a poly_, there is a p_
    return false;

  //: create an image 4 times bigger to trace a more accurate boundary
  //int ni = int(vcl_floor(bounding_box_->width()+20.5));
  //int nj = int(vcl_ceil(bounding_box_->height()+20.5));
  //float off_x = float(bounding_box_->get_min_x()-10);
  //float off_y = float(bounding_box_->get_min_y()-10);

  int ni = int(vcl_floor(bounding_box_->width()*4+20.5));
  int nj = int(vcl_ceil(bounding_box_->height()*4+20.5));
  float off_x = float(bounding_box_->get_min_x()*4-10);
  float off_y = float(bounding_box_->get_min_y()*4-10);

  vil_image_view<bool> /*fg(ni, nj, 1), */temp(ni, nj, 1); 
  temp.fill(false);
  //fg.fill(false);

  //: scan iterator does not include the boundary currently,
  //  however theoretically it is better if it does, especially if
  //  appearance based methods will be used
  //  but there is a bug, when the boundary is included, it is touching to the image
  //  edge, and causing another bug in contour tracer to trigger 
  //  so currently boundary is not included.
  //  TODO: check what's going wrong in polygon scan iterator when boundary is included.

  //create a new polygon that has 4 times the coordinates
  vgl_polygon<double> pp(p_);
  for (unsigned i = 0; i < pp.num_sheets(); i++)
    for (unsigned j = 0; j < pp[i].size(); j++)
      pp[i][j] = vgl_point_2d<double>(pp[i][j].x()*4, pp[i][j].y()*4);

  //vgl_polygon_scan_iterator<double> psi(p_, false);  // do not include boundary
  vgl_polygon_scan_iterator<double> psi(pp, false);  // do not include boundary
  for (psi.reset(); psi.next(); ) {
    int y = psi.scany();
    for (int x = psi.startx(); x <= psi.endx(); ++x) {
      float xx = x - off_x;
      float yy = y - off_y;
      if (xx < 0 || yy < 0)
        continue;
      int xxx = static_cast<int>(vcl_floor(xx+0.5f));
      int yyy = static_cast<int>(vcl_floor(yy+0.5f));
      if (xxx >= ni || yyy >= nj) 
        continue;
      temp(xxx,yyy) = true;
    }
  }
#if 0
  vil_image_view<unsigned char> temp1(ni, nj, 1);
  for (unsigned i=0;i<temp.ni();i++)
      for(unsigned j=0;j<temp.nj();j++)
          {
          if(temp(i,j))
              temp1(i,j)=255;

          else 
              temp1(i,j)=0;
          }
  
   vcl_ostringstream oss, oss2;
   oss << id_;
   oss2 << depth_;
   vil_save(temp1,("d:\\projects\\temp\\temp-"+oss.str()+"-"+oss2.str()+".tiff").c_str());
#endif
  /*double erosionrad = 2, dilationrad = 0.5, postdilationerosionrad = 2;
  vil_structuring_element se;  
  //se.set_to_disk(erosionrad);
  //vil_binary_erode(temp,fg,se);
  se.set_to_disk(dilationrad);
  //vil_binary_dilate(fg,temp,se);
  vil_binary_dilate(temp,fg,se);
  //se.set_to_disk(postdilationerosionrad);
  //vil_binary_erode(temp,fg,se);

  /*temp1.fill(false);
  for (unsigned i=0;i<fg.ni();i++)
      for(unsigned j=0;j<fg.nj();j++)
          {
          if(fg(i,j))
              temp1(i,j)=255;

          else 
              temp1(i,j)=0;
          }
  vil_save(temp1,"d:\\projects\\temp\\temp_ade.tiff");*/
  
  //invoke the tracer
  dbdet_contour_tracer ctracer;
  ctracer.set_sigma(contour_tracer_sigma_);
  //ctracer.trace(fg);
  ctracer.trace(temp);

  if (!ctracer.contours().size())
    return false;

  if (ctracer.largest_contour().size() > 3) {
    vsol_polygon_2d_sptr newp = new vsol_polygon_2d(ctracer.largest_contour());
    vgl_vector_2d<double> v(off_x, off_y);
    for (unsigned i = 0; i < newp->size(); i++) {
      //newp->vertex(i)->add_vector(v);
      newp->vertex(i)->add_vector(v);
      newp->vertex(i)->set_x(newp->vertex(i)->x()/4);
      newp->vertex(i)->set_y(newp->vertex(i)->y()/4);
    }

    traced_poly_ = fit_lines_to_contour(newp, line_fitting_rms_);
  } else
    return false;

  return true;
}

//: extract the shock graph from the outer poly if wanted
bool dbskr_shock_patch::extract_shock_from_outer_boundary() 
{
  if (!poly_)  // if there is a poly_, there is a p_
    return false;

  vcl_vector< vsol_spatial_object_2d_sptr > conts;
  conts.push_back(poly_->cast_to_spatial_object());
  
  // compute shocks ---------------------------------------------------------------------------------------------------
  float xmin=0, ymin=0, cell_width=1000.0f, cell_height=1000.0f; int num_rows=1, num_cols=1;
  dbsk2d_boundary_sptr boundary = dbsk2d_create_boundary(conts, false, xmin, ymin, num_rows, num_cols, cell_width, cell_height, true, true);
  dbsk2d_ishock_graph_sptr isg = dbsk2d_compute_ishocks(boundary);
  if (!isg) {
    return false;
  }

  dbsk2d_shock_graph_sptr sg = new dbsk2d_shock_graph();
  dbsk2d_prune_ishock ishock_pruner(isg, sg);
  ishock_pruner.prune(shock_pruning_threshold_);  // prune threshold is 1.0f
  ishock_pruner.compile_coarse_shock_graph();
  if (!sg->number_of_vertices() || !sg->number_of_edges()) {
    return false;
  }

  dbsk2d_sample_ishock ishock_sampler(sg);
  ishock_sampler.sample(1.0f);
  traced_shock_ = ishock_sampler.extrinsic_shock_graph();
  
  if (!traced_shock_)
    return false;

  return true;

}

//: bool extract shocks from the simple closed traced boundary
bool dbskr_shock_patch::extract_simple_shock()
{
  traced_poly_ = get_traced_boundary();  // to force computation if not cached
  if (!traced_poly_) 
    return false;

  vcl_vector< vsol_spatial_object_2d_sptr > conts;

  //fit lines to the traced poly!!
  //vsol_polygon_2d_sptr new_poly = fit_lines_to_contour(traced_poly_, 0.05f);
  conts.push_back(traced_poly_->cast_to_spatial_object());
  //conts.push_back(new_poly->cast_to_spatial_object());

  // compute shocks ---------------------------------------------------------------------------------------------------
  float xmin=0, ymin=0, cell_width=1000.0f, cell_height=1000.0f; int num_rows=1, num_cols=1;
  dbsk2d_boundary_sptr boundary = dbsk2d_create_boundary(conts, false, xmin, ymin, num_rows, num_cols, cell_width, cell_height, true, true);
  dbsk2d_ishock_graph_sptr isg = dbsk2d_compute_ishocks(boundary);
  if (!isg) {
    return false;
  }

  dbsk2d_shock_graph_sptr sg = new dbsk2d_shock_graph();
  dbsk2d_prune_ishock ishock_pruner(isg, sg);
  ishock_pruner.prune(shock_pruning_threshold_);  // prune threshold is 1.0f
  ishock_pruner.prune_based_on_support(real_boundaries_, bounding_box_);
  ishock_pruner.compile_coarse_shock_graph();
  if (!sg->number_of_vertices() || !sg->number_of_edges()) {
    return false;
  }

  dbsk2d_sample_ishock ishock_sampler(sg);
  ishock_sampler.sample(1.0f);
  //traced_shock_ = ishock_sampler.extrinsic_shock_graph();
  traced_shock_ = ishock_sampler.extrinsic_shock_graph();
  
  if (!traced_shock_)
    return false;

  return true;
}

//: bool extract shocks from the simple closed traced boundary
bool dbskr_shock_patch::prepare_tree()
{
  dbsk2d_shock_graph_sptr sg = shock_graph();  // to force computation if not available
  if (!sg)
    return false;
  else {
    tree_ = new dbskr_tree(scurve_sample_ds_, scurve_interp_ds_);
    if (tree_->acquire(sg, elastic_splice_cost_, construct_circular_ends_, combined_edit_))
      return true;
    else {
      tree_ = 0;  
      return false;
    }
  }
}

int dbskr_shock_patch::start_node_id(int factor)
{
  int rem = id_%factor;
  return (int) (id_-rem)/factor;
}


/*
//: create the observation
bool dbskr_shock_patch::create_observation(vil_image_resource_sptr img, bool intensity_channel, bool gradient_channel, bool color_channel)
{
  if (poly_) {
    obs_ = new dbinfo_observation(0, img, poly_, intensity_channel, gradient_channel, color_channel);
    return true;
  } else
    return false;
}
*/

float dbskr_shock_patch::real_boundary_length(void)
{
  if (real_len_ == 0) {
    real_len_ = 0;
    for (unsigned i = 0; i < real_boundaries_.size(); i++)
      real_len_ += (float)real_boundaries_[i]->length();
  }
  return real_len_;
}

float dbskr_shock_patch::outer_boundary_length(void)
{
  if (poly_len_ < 0) {
    if (poly_) 
    {
      poly_len_ = 0;
      int i = 0;
      for ( ; i < int(p_[0].size())-1; i++)
        poly_len_ += (float)vgl_distance((p_[0])[i], (p_[0])[i+1]);
      poly_len_ += (float)vgl_distance((p_[0])[i], (p_[0])[0]);    
    }
  }

  return poly_len_;
}

float dbskr_shock_patch::traced_boundary_length(void)
{
  if (traced_poly_len_ < 0) {
    if (traced_poly_) 
    {
      vgl_polygon<double> pp = bsol_algs::vgl_from_poly(traced_poly_);
      traced_poly_len_ = 0;
      int i = 0;
      for ( ; i < int(pp[0].size())-1; i++)
        traced_poly_len_ += (float)vgl_distance((pp[0])[i], (pp[0])[i+1]);
      traced_poly_len_ += (float)vgl_distance((pp[0])[i], (pp[0])[0]);    
    }
  }

  return traced_poly_len_;
}


float dbskr_shock_patch::contour_ratio(void)
{
  if (contour_ratio_ < 0) {
    contour_ratio_ = real_boundary_length()/outer_boundary_length();
  } 

  return contour_ratio_;
}

void dbskr_shock_patch::output_info()
{
  vcl_cout << "----------------------------- patch " << id() << " info -----------------------\n";
  vcl_cout << "outer boundary length: " << outer_boundary_length() << " area: " << poly_->area() << vcl_endl;
  vcl_cout << "real boundaries length: " << real_boundary_length() << " real/outer length ratio: " << contour_ratio() << vcl_endl;
  
  if (traced_poly_) {
    vcl_cout << "traced boundary length: " << traced_boundary_length() << " area: " << traced_poly_->area() << vcl_endl;
  }

  if (color_contrast_ > 0)
    vcl_cout << "color contrast around the traced boundary: " << color_contrast_ << vcl_endl;
  if (grey_contrast_ > 0)
    vcl_cout << "appearance contrast around the traced boundary: " << grey_contrast_ << vcl_endl;
  
  vcl_cout << "----------------------------------------------------------------\n";
}

//: finding color contrast around "real boundaries" of this shock patch
void dbskr_shock_patch::find_color_contrast(vil_image_view<float>& L, vil_image_view<float>& A, vil_image_view<float>& B, float region_width)
{
  if (!real_boundaries_.size())
    return;

  color_contrast_ = 0;
  for (unsigned k = 0; k  < real_boundaries_.size(); k++) {
    dbsol_interp_curve_2d_sptr c = new dbsol_interp_curve_2d();
    dbsol_curve_algs::interpolate_linear(c.ptr(), real_boundaries_[k]); // open curve
    color_contrast_ += (float)get_color_distance_of_curve_regions(c, region_width, L, A, B, 14.0f);  // color_gamma = 14
  }

  //: return the mean contrast
  color_contrast_ /= real_boundaries_.size();
}

void dbskr_shock_patch::find_grey_contrast(vil_image_view<vxl_byte>& I, float region_width)
{
  if (!real_boundaries_.size())
    return;

  grey_contrast_ = 0;
 
  for (unsigned k = 0; k  < real_boundaries_.size(); k++) {
    dbsol_interp_curve_2d_sptr c = new dbsol_interp_curve_2d();
    dbsol_curve_algs::interpolate_linear(c.ptr(), real_boundaries_[k]); // open curve

    grey_contrast_ += (float)get_intensity_distance_of_curve_regions(c, region_width, I, 14.0f);  // color_gamma = 14
  }
 
  //: return the mean contrast
  grey_contrast_ /= real_boundaries_.size();

}

//: compute the mapped image of this patch onto the target image using the homography which is computed usign some correspondence
//  e.g. using edit distance correspondence of this patch with some other patch extracted from the target image.
vil_image_resource_sptr dbskr_shock_patch::mapped_image(vil_image_resource_sptr img_source, 
                                                        vil_image_resource_sptr img_target, 
                                                        const vgl_h_matrix_2d<double>& H, bool recompute_obs)
{
  //: make the green and blue channels in the output image same as the target image,
  vil_image_resource_sptr out_img_g = vil_plane(img_target, 1);
  vil_image_resource_sptr out_img_b = vil_plane(img_target, 2);

  vil_image_view<vxl_byte> out_r(img_target->ni(), img_target->nj(), 1);  // view with a single plane
  out_r.fill(0);
  // take the values from source and put onto the corresponding locations on target

  if (recompute_obs || !obs_)
    extract_observation(img_source, true, false, false);

  vcl_cout << "homography:\n " << H << vcl_endl; 
/*
  dbinfo_observation_sptr new_obs = dbinfo_observation_generator::generate(obs_, H, 0.0);  // even if no points are sufficient return it

  dbinfo_feature_base_sptr feat = (new_obs->features())[0];  
  if(feat->format()!=DBINFO_INTENSITY_FEATURE) {
    vcl_cout << "not found the intensity feature!!\n";
    return 0;
  }
  vcl_vector<vgl_point_2d<float> > points = new_obs->geometry()->points();
  vcl_vector<bool> valid = new_obs->geometry()->masks();
  vcl_vector<bool> valid_all(points.size(), true);
  vil_image_resource_sptr out_img_r = feat->image(points, valid_all, img_target->ni(), img_target->nj(), 0, 0, false);
*/

  vcl_vector<vgl_point_2d<float> > points = obs_->geometry()->points();
  vcl_cout << "# of points in patch: " << points.size() << vcl_endl;
  vcl_vector<bool> valid(points.size(), false);

  int i = 0;
  for(vcl_vector<vgl_point_2d<float> >::iterator pit = points.begin();
      pit != points.end(); ++pit, ++i)
  {
    vgl_homg_point_2d<double> hp((*pit).x(), (*pit).y()), thp;
    thp = H(hp);
    vgl_point_2d<double> tep(thp);
    float x = static_cast<float>(tep.x()),
      y = static_cast<float>(tep.y());
    if(x>=0 && x<img_target->ni() && y>0 && y<img_target->nj()/*y<image_rows_*/)
      {
        valid[i]=true;
        (*pit).set(x, y);
      }
  }

  dbinfo_feature_base_sptr feat = (obs_->features())[0];  
  if(feat->format()!=DBINFO_INTENSITY_FEATURE) {
    vcl_cout << "not found the intensity feature!!\n";
    return 0;
  }
  vil_image_resource_sptr out_img_r = feat->image(points, valid, img_target->ni(), img_target->nj(), 0, 0, false);

  //: set the mapped patch pixels in the target to the red channel
  vil_image_view<vil_rgb<vxl_byte> > combined = brip_vil_float_ops::combine_color_planes(out_img_r, out_img_g, out_img_b);
  vil_image_resource_sptr out_img = vil_new_image_resource_of_view(combined);

  return out_img;
}

//: Binary save self to stream.
void dbskr_shock_patch::b_write(vsl_b_ostream &os) const
{
  unsigned ver = version();
  vsl_b_write(os, ver);

  if (version() >= 3)
    vsl_b_write(os, construct_circular_ends_);

  if (version() == 3 || version() == 2) {
    unsigned size = real_boundaries_.size();
    vsl_b_write(os, size);
    for (unsigned i = 0; i < size; i++)
      real_boundaries_[i]->b_write(os);
  }

  vsl_b_write(os, this->id_);
  vsl_b_write(os, this->depth_);

  if (poly_) {
    vsl_b_write(os, true);
    poly_->b_write(os);
  } else 
    vsl_b_write(os, false);

  vsl_b_write(os, contour_tracer_sigma_);
  vsl_b_write(os, line_fitting_rms_);
  vsl_b_write(os, shock_pruning_threshold_);
  vsl_b_write(os, elastic_splice_cost_);
  vsl_b_write(os, scurve_sample_ds_);
    
  if (traced_poly_) {
    vsl_b_write(os, true);
    traced_poly_->b_write(os);
    if (traced_shock_) {
      vsl_b_write(os, true);
      if (tree_) {
        vsl_b_write(os, true);
      } else
        vsl_b_write(os, false);
    } else
      vsl_b_write(os, false);
  } else 
    vsl_b_write(os, false);
    
  return;
}

//: Binary load self from stream.
void dbskr_shock_patch::b_read(vsl_b_istream &is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
  case 4: 
    {
      vsl_b_read(is, construct_circular_ends_);
      vsl_b_read(is, id_);
      vsl_b_read(is, depth_);
    
      bool poly_available;
      vsl_b_read(is, poly_available);
      if (poly_available) {
        vsol_polygon_2d_sptr poly = new vsol_polygon_2d();
        poly->b_read(is);   
        set_outer_boundary(poly); 
      } else 
        poly_ = 0;

      vsl_b_read(is, contour_tracer_sigma_);
      vsl_b_read(is, line_fitting_rms_);
      vsl_b_read(is, shock_pruning_threshold_);
      vsl_b_read(is, elastic_splice_cost_);
      vsl_b_read(is, scurve_sample_ds_);

      bool tpoly_available;
      vsl_b_read(is, tpoly_available);
      if (tpoly_available) {
        traced_poly_ = new vsol_polygon_2d();
        traced_poly_->b_read(is);
        bool shock_available;
        vsl_b_read(is, shock_available);
        if (shock_available) {
          extract_simple_shock();
          bool tree_available;
          vsl_b_read(is, tree_available);
          tree_ = 0;  // do not prepare tree at this stage
        } else
          traced_shock_ = 0;
      } else 
        traced_poly_ = 0;  
      break;
    }
  case 3:
      vsl_b_read(is, construct_circular_ends_);
  case 2:
    {
      unsigned real_cnt;
      vsl_b_read(is, real_cnt);
      for (unsigned i = 0; i < real_cnt; i++) {
        vsol_polyline_2d_sptr poly = new vsol_polyline_2d();
        poly->b_read(is);
        real_boundaries_.push_back(poly);
      }

    }
    case 1:
      {
        vsl_b_read(is, id_);
        vsl_b_read(is, depth_);
      
        bool poly_available;
        vsl_b_read(is, poly_available);
        if (poly_available) {
          vsol_polygon_2d_sptr poly = new vsol_polygon_2d();
          poly->b_read(is);   
          set_outer_boundary(poly); 
        } else 
          poly_ = 0;

        vsl_b_read(is, contour_tracer_sigma_);
        vsl_b_read(is, line_fitting_rms_);
        vsl_b_read(is, shock_pruning_threshold_);
        vsl_b_read(is, elastic_splice_cost_);
        vsl_b_read(is, scurve_sample_ds_);
  
        bool tpoly_available;
        vsl_b_read(is, tpoly_available);
        if (tpoly_available) {
          traced_poly_ = new vsol_polygon_2d();
          traced_poly_->b_read(is);
          bool shock_available;
          vsl_b_read(is, shock_available);
          if (shock_available) {
            extract_simple_shock();
            bool tree_available;
            vsl_b_read(is, tree_available);
            tree_ = 0;
          } else
            traced_shock_ = 0;
        } else 
          traced_poly_ = 0;       
        break;
      }
  }
}

void dbskr_shock_patch::kill_tree() 
{ 
  if (tree_) 
    tree_ = 0; 
}

void dbskr_shock_patch::kill_shock_graph()
{
  if (traced_shock_)
    traced_shock_ = 0;
}

void dbskr_shock_patch::kill_v_graph()
{
  if (vg_)
    vg_ = 0;
}

//: return the ratio of number of overlaping nodes in the v_graphs of the given patches
float dbskr_shock_patch::v_graph_node_overlap(dbskr_shock_patch_sptr sp)
{
  dbskr_v_graph_sptr sp_vg = sp->get_v_graph();
  return vg_->node_overlap(*sp_vg);
}

float dbskr_v_graph::node_overlap(dbskr_v_graph& other)
{
  float same_id_cnt = 0;
  for (vertex_iterator v_itr = vertices_begin(); v_itr != vertices_end(); v_itr++)
  { 
    for (vertex_iterator v_itr2 = other.vertices_begin(); v_itr2 != other.vertices_end(); v_itr2++) {
      if ((*v_itr)->id_ == (*v_itr2)->id_) 
        same_id_cnt++;
    }
  }
  return (same_id_cnt/number_of_vertices());
}

vsol_box_2d_sptr dbskr_v_graph::bounding_box()
{
  vsol_box_2d_sptr box = new vsol_box_2d();
  for (vertex_iterator v_itr = vertices_begin(); v_itr != vertices_end(); v_itr++)
  {
    if ((*v_itr)->original_shock_node_ && (*v_itr)->original_shock_node_->ex_pts().size() > 0) {
      vgl_point_2d<double> pt = ((*v_itr)->original_shock_node_->ex_pts())[0];
      box->add_point(pt.x(), pt.y());
    }
  }

  return box;
}

dbskr_v_node::dbskr_v_node(dbsk2d_shock_node_sptr n) : dbgrl_vertex<dbskr_v_edge>() {
  original_shock_node_ = n;
  id_ = n->id();
}

dbskr_v_node::dbskr_v_node(const dbskr_v_node& n) : dbgrl_vertex<dbskr_v_edge>() 
{
  original_shock_node_ = n.original_shock_node_;
  id_ = n.id_;
}

dbskr_v_edge::dbskr_v_edge(const dbskr_v_edge& other) : dbgrl_edge<dbskr_v_node>(other.source_, other.target_) {
  edges_ = other.edges_;
  start_node_id_ = other.start_node_id_;
  end_node_id_ = other.end_node_id_;
  length_ = other.length_;
}

float dbskr_v_edge::length()
{
  if (length_ >= 0)
    return length_;

  length_ = 0;
  //traverse through the path, interpolating where necessary
  for (vcl_vector<dbsk2d_shock_edge_sptr>::iterator e_it = edges_.begin();
       e_it != edges_.end(); e_it++)
  {
    dbsk2d_xshock_edge* cur_edge = dynamic_cast<dbsk2d_xshock_edge*>(e_it->ptr());
    if (!cur_edge->num_samples())
      continue;

    vgl_point_2d<double> prev_pt = cur_edge->sample(0)->pt;
    //go through the edge samples and append it to the shock curve
    for (int i = 1; i < cur_edge->num_samples(); i++)
    {
      dbsk2d_xshock_sample_sptr sample = cur_edge->sample(i);
      length_ += (float)vgl_distance(sample->pt, prev_pt);
      prev_pt = sample->pt;
    }
  }

  return length_;
}

//: return true if the two v_graphs are exactly the same
//  checks whether the number of children are exactly the same as well 
bool dbskr_shock_patch::v_graph_same(dbskr_shock_patch_sptr sp)
{
  dbskr_v_graph_sptr sp_vg = sp->get_v_graph();
  return vg_->same(*sp_vg);
}

bool dbskr_v_graph::same(dbskr_v_graph& other)
{
  if (number_of_vertices() != other.number_of_vertices())
    return false;
  if (number_of_edges() != other.number_of_edges())
    return false;

  int same_id_cnt = 0;
  for (vertex_iterator v_itr = vertices_begin(); v_itr != vertices_end(); v_itr++)
  { 
    bool found_same_id = false;
    for (vertex_iterator v_itr2 = other.vertices_begin(); v_itr2 != other.vertices_end(); v_itr2++) {
      if ((*v_itr)->id_ == (*v_itr2)->id_) {
        found_same_id = true;
        if ((*v_itr)->out_degree() == (*v_itr2)->out_degree()) {
          int same_id_children_cnt = 0;
          for (edge_iterator e = (*v_itr)->out_edges_begin(); e != (*v_itr)->out_edges_end(); e++) {
            for (edge_iterator e2 = (*v_itr2)->out_edges_begin(); e2 != (*v_itr2)->out_edges_end(); e2++) {
              if ((*e)->opposite((*v_itr))->id_ == (*e2)->opposite((*v_itr2))->id_)
                same_id_children_cnt++;
            }
          }
          
          if (same_id_children_cnt == (*v_itr)->out_degree())
            same_id_cnt++;
        }
      }
    }
    if (!found_same_id)
      return false;
  }

  return same_id_cnt == number_of_vertices();
}

//: does not check the edge list on the vertices but only the vertex ids and the edge source and target ids
bool dbskr_v_graph::same_vertex_and_edges(dbskr_v_graph& other)
{
  if (number_of_vertices() != other.number_of_vertices())
    return false;
  if (number_of_edges() != other.number_of_edges())
    return false;

  vcl_map<int, bool> check;
  for (vertex_iterator v_itr = vertices_begin(); v_itr != vertices_end(); v_itr++)
  { 
    check[(*v_itr)->id_] = true;
  }

  for (vertex_iterator v_itr2 = other.vertices_begin(); v_itr2 != other.vertices_end(); v_itr2++) {
    vcl_map<int, bool>::iterator it = check.find((*v_itr2)->id_);
    if (it == check.end())
      return false;
  }
  vcl_map<vcl_pair<int, int>, bool> check2;
  for (edge_iterator e_itr = edges_begin(); e_itr != edges_end(); e_itr++) {
    vcl_pair<int, int> p((*e_itr)->source()->id_, (*e_itr)->target()->id_);
    check2[p] = true;
  }
  for (edge_iterator e_itr = other.edges_begin(); e_itr != other.edges_end(); e_itr++) {
    vcl_pair<int, int> p((*e_itr)->source()->id_, (*e_itr)->target()->id_);
    vcl_map<vcl_pair<int, int>, bool>::iterator it = check2.find(p);
    if (it == check2.end())
      return false;
  }

  return true;
}


vsol_box_2d_sptr dbskr_shock_patch::bounding_box_real()
{
  if (!bounding_box_real_) {
    bounding_box_real_ = new vsol_box_2d();
    for (unsigned i = 0; i < real_boundaries_.size(); i++) {
      real_boundaries_[i]->compute_bounding_box();
      bounding_box_real_->grow_minmax_bounds(real_boundaries_[i]->get_bounding_box());
    }
  } 
  
  return bounding_box_real_;
}

//:  there are cases such that subgraphs are different but after shock trimming the real boundaries
//   and hence the trimmed shock is almost exactly the same
//   or e.g. real boundaries traced from outer shocks of an object, and traced from inner shocks are the same
//   check if areas and bounding box centers are almost exactly the same
bool dbskr_shock_patch::same_real_boundaries(dbskr_shock_patch_sptr sp)
{
  //: find the bbox of real boundaries
  vsol_box_2d_sptr bbox = bounding_box_real();
  vsol_box_2d_sptr bbox2 = sp->bounding_box_real();

  if (vcl_abs(bbox->get_min_x()-bbox2->get_min_x()) < NEGLIGIBLE && 
      vcl_abs(bbox->get_min_y()-bbox2->get_min_y()) < NEGLIGIBLE && 
      vcl_abs(bbox->get_max_x()-bbox2->get_max_x()) < NEGLIGIBLE && 
      vcl_abs(bbox->get_max_y()-bbox2->get_max_y()) < NEGLIGIBLE)
      return true;

  return false;
}

//: if not computed yet, compute it!
vsol_polygon_2d_sptr dbskr_shock_patch::get_traced_boundary()
{
  if (!traced_poly_) 
    trace_outer_boundary();

  return traced_poly_;
}
/*
//: if not computed yet, compute it!
dbsk2d_shock_graph_sptr dbskr_shock_patch::shock_graph()
{
  if (!traced_shock_)
    extract_simple_shock();

  return traced_shock_;
}
*/
//: if not computed yet, compute it!
dbskr_tree_sptr dbskr_shock_patch::tree()
{
  if (!tree_)
    prepare_tree();  // set the shock matching parameters before calling this method so that local params
                     // of tree construction in prepare_tree are set properly

  return tree_;
}


void dbskr_shock_patch::extract_observation(vil_image_resource_sptr img, bool app, bool grad, bool color) 
{ 
  obs_ = new dbinfo_observation(0, img, traced_poly_, app, grad, color); 
}
