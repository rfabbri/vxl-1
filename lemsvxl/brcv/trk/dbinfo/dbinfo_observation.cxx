#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_vector_io.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/vil_crop.h>
#include <bsol/bsol_algs.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data_base.h>
#include <dbinfo/dbinfo_feature_base.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_ihs_feature.h>
#include <dbinfo/dbinfo_observation.h>
//: get the margin from each feature and choose the largest to 
//  extract the region of interest image chip
void dbinfo_observation::set_margin()
{
  margin_=0;
  for(vcl_vector<dbinfo_feature_base_sptr>::iterator fit = features_.begin();
      fit != features_.end(); ++fit)
    if((*fit)->margin()>margin_)
      margin_ = (*fit)->margin();
}

void dbinfo_observation::compute_roi()
{
  if(!geom_)
    ex_roi_ = (brip_roi*)0;
  if(margin_)
    ex_roi_ = new brip_roi(*(geom_->roi()), (float)margin_);
  else
    ex_roi_ = geom_->roi();
}
//: margin set by user
void dbinfo_observation::set_margin(unsigned m)
{
  margin_=m;
}
//: constructor from basic elements
// no cached data, just stored data in the feature set
dbinfo_observation::
dbinfo_observation(dbinfo_region_geometry_sptr const& geom,
                   vcl_vector<dbinfo_feature_base_sptr> const& features)
  : id_(0), score_(0), doc_(""), current_frame_(0), geom_(geom), features_(features), image_cached_(0), image_crp_cached_(0)
{
  this->set_margin();
  this->compute_roi();
}
//: constructor from polygons
dbinfo_observation::
dbinfo_observation(const unsigned frame,
                   vil_image_resource_sptr const& image,
                   vcl_vector<vsol_polygon_2d_sptr> const& polys,
                   vcl_vector<dbinfo_feature_base_sptr> const& features)
  :  id_(0), score_(0), doc_(""), image_cached_(0), image_crp_cached_(0)
{
  assert(image);
  geom_ = new dbinfo_region_geometry(image->ni(), image->nj(), polys);
  features_ = features;
  this->set_margin();
  this->compute_roi();
  current_frame_ = frame;
  this->scan(frame, image);
}

//: constructor from a single polygon
dbinfo_observation::
dbinfo_observation(const unsigned frame,
                   vil_image_resource_sptr const& image, 
                   vsol_polygon_2d_sptr const& poly,
                   vcl_vector<dbinfo_feature_base_sptr> const& features):
  id_(0),  score_(0), doc_(""), image_cached_(0), image_crp_cached_(0)
{
  assert(image);
  geom_ = new dbinfo_region_geometry(image->ni(), image->nj(), poly);
  features_ = features;
  this->set_margin();
  this->compute_roi();
  current_frame_ = frame;
  this->scan(frame, image);
}
//: constructor from multiple polygons, no features specified
dbinfo_observation::dbinfo_observation(const unsigned frame,
                                       vil_image_resource_sptr const& image,
                                       vcl_vector<vsol_polygon_2d_sptr> const& polys)
  : id_(0), score_(0), doc_(""), margin_(0), current_frame_(frame), image_cached_(0), image_crp_cached_(0)
{
  assert(image);
  geom_ = new dbinfo_region_geometry(image->ni(), image->nj(), polys);
  this->compute_roi();
}

//: constructor from a single polygon, no features specified
dbinfo_observation::dbinfo_observation(const unsigned frame,
                                       vil_image_resource_sptr const& image, 
                                       vsol_polygon_2d_sptr const& poly)
  : id_(0), score_(0), doc_(""), margin_(0), current_frame_(frame), image_cached_(0), image_crp_cached_(0) 
{
  assert(image);
  geom_ = new dbinfo_region_geometry(image->ni(), image->nj(), poly);
  this->compute_roi();
}
//: constructor from multiple polygons, features constructed according to spec
dbinfo_observation::dbinfo_observation(const unsigned frame,
                                       vil_image_resource_sptr const& image,
                                       vcl_vector<vsol_polygon_2d_sptr> const& polys,
                                       bool intensity_info,
                                       bool gradient_info,
                                       bool color_info)
  : id_(0), score_(0), doc_(""), current_frame_(frame), image_cached_(0), image_crp_cached_(0)
{
  assert(image);
  geom_ = new dbinfo_region_geometry(image->ni(), image->nj(), polys);
  if(intensity_info)
    {
      dbinfo_feature_base_sptr intf = new dbinfo_intensity_feature();
      features_.push_back(intf);
    }
  if(gradient_info)
    {
      dbinfo_feature_base_sptr gradf = new dbinfo_gradient_feature();
      features_.push_back(gradf);
    }  
  if(color_info)
    {
      dbinfo_feature_data_base_sptr data;
      dbinfo_feature_base_sptr ihsf = new dbinfo_ihs_feature();
      features_.push_back(ihsf);
    }  
  this->set_margin();
  this->compute_roi();
  this->scan(frame, image);
}

//: constructor from a single polygon, features constructed according to spec
dbinfo_observation::dbinfo_observation(const unsigned frame,
                                       vil_image_resource_sptr const& image, 
                                       vsol_polygon_2d_sptr const& poly,
                                       bool intensity_info,
                                       bool gradient_info,
                                       bool color_info)
  : id_(0), score_(0), doc_(""), current_frame_(frame), image_cached_(0), image_crp_cached_(0) 
{
  assert(image);
  unsigned n_i = image->ni(), n_j = image->nj();
  //poly is null then use the image border as the geometry
  vsol_polygon_2d_sptr p = poly;
  if(!p)
    {
      vsol_box_2d_sptr b = new vsol_box_2d();
      b->add_point(0, 0);
      b->add_point(n_i, n_j);
      p = bsol_algs::poly_from_box(b);
    }
  geom_ = new dbinfo_region_geometry(n_i, n_j, p);
  if(intensity_info)
    {
      dbinfo_feature_base_sptr intf = new dbinfo_intensity_feature();
      features_.push_back(intf);
    }
  if(gradient_info)
    {
      dbinfo_feature_base_sptr gradf = new dbinfo_gradient_feature();
      features_.push_back(gradf);
    }  
  if(color_info)
    {
      dbinfo_feature_data_base_sptr data;
      dbinfo_feature_base_sptr ihsf = new dbinfo_ihs_feature();
      features_.push_back(ihsf);
    }  
  this->set_margin();
  this->compute_roi();
  this->scan(frame, image);
}

void 
dbinfo_observation::image_to_roi(vcl_vector<vgl_point_2d<unsigned> >& points,
                                 vcl_vector<bool>& valid)
{
  assert(geom_);
  unsigned npts = geom_->size();
  points.resize(npts);
  valid.resize(npts);
  unsigned cmn = ex_roi_->cmin(0)+margin_, cmx = ex_roi_->cmax(0)-margin_;
  unsigned rmn = ex_roi_->rmin(0)+margin_, rmx = ex_roi_->rmax(0)-margin_;

  for(unsigned i = 0; i<npts; ++i)
    {
      float c = (geom_->point(i)).x(), r = (geom_->point(i)).y();
      bool vld = geom_->point_mask(i)&&c>=cmn&&c<=cmx&&r>=rmn&&r<=rmx;
      valid[i] = vld;
      points[i].set(ex_roi_->lc((unsigned)c, 0),
                    ex_roi_->lr((unsigned)r, 0));
    }
}
//scan the image data into the feature list from the current frame
bool dbinfo_observation::scan(const unsigned frame, 
                              vil_image_resource_sptr const& image)
{
  assert(ex_roi_);
  if(!image) return false;
  bool valid_scan = true;
  current_frame_ = frame;
  //extract the image chip
  vil_image_resource_sptr temp;
  if(!brip_vil_float_ops::chip(image, ex_roi_, temp))
    return false;

  //Check if the chip is too small
  unsigned min_length = (unsigned)(1.0 + 2.0*margin_);
  if(temp->ni()<min_length||temp->nj()<min_length)
    return false;

  vcl_vector<vgl_point_2d<unsigned> > points;
  vcl_vector<bool> valid;
  this->image_to_roi(points, valid);

  for(vcl_vector<dbinfo_feature_base_sptr>::const_iterator fit =
        features_.begin(); fit != features_.end(); ++fit)
    {
      valid_scan = valid_scan && (*fit)->scan(frame, points, valid, temp);
    }
  return valid_scan;
}
// A quick kludge JLM -- FIXME!
vil_image_resource_sptr dbinfo_observation::image(bool background_noise)
{
  if (image_cached_)
    return image_cached_;
  dbinfo_feature_base_sptr feat = features_[0];
  if(feat->format()!=DBINFO_INTENSITY_FEATURE)
    return 0;
  vcl_vector<vgl_point_2d<float> > points = geom_->points();
  vcl_vector<bool> valid = geom_->masks();
  unsigned n_i = geom_->cols(), n_j = geom_->rows();
  image_cached_ = feat->image(points, valid, n_i, n_j, 0, 0, background_noise);
  return image_cached_;
}
/*
//:Construct an image of the observation
// cropped with respect to the geometry
vil_image_resource_sptr dbinfo_observation::image_cropped()
{
  if (image_crp_cached_)
    return image_crp_cached_;
  vil_image_resource_sptr big = this->image();
  unsigned int nr = geom_->roi()->n_regions();
  if (nr <= 0) 
    return 0;
  vsol_box_2d_sptr big_region = geom_->roi()->region(0);
  for (unsigned i = 1; i<nr; i++) {
    vsol_box_2d_sptr box = geom_->roi()->region(i);
    big_region->add_point(box->get_min_x(), box->get_min_y());
    big_region->add_point(box->get_max_x(), box->get_max_y());
  }
  unsigned int topx = int(vcl_floor(big_region->get_min_x()+0.5));
  unsigned int topy = int(vcl_floor(big_region->get_min_y()+0.5));
  unsigned int lenx = int(vcl_floor(big_region->width()+0.5));
  unsigned int leny = int(vcl_floor(big_region->height()+0.5));
  image_crp_cached_ = vil_crop(big,topx, lenx, topy, leny);
  return image_crp_cached_;
}
*/

//:Construct an image of the observation
// cropped with respect to the geometry
vil_image_resource_sptr dbinfo_observation::image_cropped(bool background_noise)
{
  if (image_crp_cached_)
    return image_crp_cached_;

  unsigned int nr = geom_->roi()->n_regions();
  if (nr <= 0) 
    return 0;

  dbinfo_feature_base_sptr feat = features_[0];
  if(feat->format()!=DBINFO_INTENSITY_FEATURE)
    return 0;
  vcl_vector<vgl_point_2d<float> > points = geom_->points();
  vcl_vector<bool> valid = geom_->masks();
 // unsigned n_i = geom_->cols(), n_j = geom_->rows();
  
  vsol_box_2d_sptr big_region = geom_->roi()->region(0);
  for (unsigned i = 1; i<nr; i++) {
    vsol_box_2d_sptr box = geom_->roi()->region(i);
    big_region->add_point(box->get_min_x(), box->get_min_y());
    big_region->add_point(box->get_max_x(), box->get_max_y());
  }
  float topx = float(big_region->get_min_x());
  float topy = float(big_region->get_min_y());
  unsigned int lenx = int(vcl_floor(big_region->width()+0.5));
  unsigned int leny = int(vcl_floor(big_region->height()+0.5));

  image_crp_cached_ = feat->image(points, valid, lenx, leny, -topx, -topy, background_noise);
  
  return image_crp_cached_;
}


void dbinfo_observation::print(vcl_ostream& os) const
{
  os << this->is_a() << " [\n"
     << "id " << id_ << '\n'
     << "score " << score_ << '\n'
     << "margin " << margin_ << '\n';
  os << "frame " << current_frame_ << '\n';
  if(geom_)
    os << *geom_ << '\n';
  for(vcl_vector<dbinfo_feature_base_sptr>::const_iterator fit =
        features_.begin(); fit != features_.end(); ++fit)
    os << *(*fit) ;
  os << "]\n";
}
//Is intensity information channel used?
bool dbinfo_observation::intensity_info()
{
  for(vcl_vector<dbinfo_feature_base_sptr>::iterator fit = features_.begin();
      fit != features_.end(); ++fit)
    if((*fit)->format()==DBINFO_INTENSITY_FEATURE)
      return true;
  return false;
}

//:Is gradient information channel used?
bool dbinfo_observation::gradient_info()
{
  for(vcl_vector<dbinfo_feature_base_sptr>::iterator fit = features_.begin();
      fit != features_.end(); ++fit)
    if((*fit)->format()==DBINFO_GRADIENT_FEATURE)
      return true;
  return false;

}

//:Is color information channel used?
bool dbinfo_observation::color_info()
{
  for(vcl_vector<dbinfo_feature_base_sptr>::iterator fit = features_.begin();
      fit != features_.end(); ++fit)
    if((*fit)->format()==DBINFO_IHS_FEATURE)
      return true;
  return false;
}
void dbinfo_observation::set_edge_points(vcl_vector<vgl_point_2d<double> > edgeps)
{
    for(int i=0;i<edgeps.size();i++)
        edgepoints.push_back(edgeps[i]);
}
void dbinfo_observation::set_edge_dirs(vcl_vector<double> edgeds)
{
    for(int i=0;i<edgeds.size();i++)
        edgedirs.push_back(edgeds[i]);
}

//: Binary save self to stream.
void dbinfo_observation::b_write(vsl_b_ostream &os) const
{
  dbinfo_observation& fb = (dbinfo_observation&)*this;  
  assert(fb.geometry());
  vsl_b_write(os, version());
  vsl_b_write(os, fb.id());
  vsl_b_write(os, fb.score());
  vsl_b_write(os, fb.doc());
  vsl_b_write(os, fb.margin());
  vsl_b_write(os, fb.frame());
  vsl_b_write(os, fb.geometry());
  vsl_b_write(os, fb.features());
}

//: Binary load self from stream.
void dbinfo_observation::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        vsl_b_read(is, id_);
        vsl_b_read(is, score_);
#if 1   // unfortunately some binary files are saved as version 1 even though they're actually version 2
        // if the reader gives problems open this directive temporarily to read and then save that binary 
        // before using so that observations are saved as version 2, then after that use the saved new binary file
        // and close this directive in the code
        vsl_b_read(is, doc_);  // this is the only addition of version 2
#endif
        vsl_b_read(is, margin_);
        vsl_b_read(is, current_frame_);
        vsl_b_read(is, geom_);
        vcl_vector<dbinfo_feature_base_sptr> features;
        vsl_b_read(is, features);
        for(vcl_vector<dbinfo_feature_base_sptr>::iterator fit =
              features.begin(); fit != features.end(); ++fit)
          {
            dbinfo_feature_format fmt = (*fit)->format();
            switch (fmt)
              {
              case DBINFO_INTENSITY_FEATURE:
                features_.push_back(new dbinfo_intensity_feature((*fit)->data()));
                break;
              case DBINFO_GRADIENT_FEATURE:
                features_.push_back(new dbinfo_gradient_feature((*fit)->data()));
                break;
              case DBINFO_IHS_FEATURE:
                features_.push_back(new dbinfo_ihs_feature((*fit)->data()));
                break;
              default:
                features_.push_back(*fit);
              }
          }
        this->compute_roi();
        break;
      }
    case 2:
      {
        vsl_b_read(is, id_);
        vsl_b_read(is, score_);
        vsl_b_read(is, doc_);  // this is the only addition of version 2
        vsl_b_read(is, margin_);
        vsl_b_read(is, current_frame_);
        vsl_b_read(is, geom_);
        vcl_vector<dbinfo_feature_base_sptr> features;
        vsl_b_read(is, features);
        for(vcl_vector<dbinfo_feature_base_sptr>::iterator fit =
              features.begin(); fit != features.end(); ++fit)
          {
            dbinfo_feature_format fmt = (*fit)->format();
            switch (fmt)
              {
              case DBINFO_INTENSITY_FEATURE:
                features_.push_back(new dbinfo_intensity_feature((*fit)->data()));
                break;
              case DBINFO_GRADIENT_FEATURE:
                features_.push_back(new dbinfo_gradient_feature((*fit)->data()));
                break;
              case DBINFO_IHS_FEATURE:
                features_.push_back(new dbinfo_ihs_feature((*fit)->data()));
                break;
              default:
                features_.push_back(*fit);
              }
          }
        this->compute_roi();
        break;
      }
    }
}

bool compute_observation_snippet(dbinfo_observation * obs,
                                 vil_image_resource_sptr &im)
{
    
    vil_image_resource_sptr snippet;
    if(!obs)
        return false;
    brip_roi_sptr ex_roi=obs->ex_roi();
    if(!im.ptr()) 
        return false;
    if(!brip_vil_float_ops::chip(im, ex_roi, snippet))
        return false;
    else 
    {
        obs->set_obs_snippet(snippet);     
        return true;
    }
}

//vil_image_resource_sptr dbinfo_observation::get_clip_image()
//    {
//        return clip_image_;
//    }
//
//bool dbinfo_observation::compute_clip_image(vil_image_resource_sptr const& image)
//    {
//    if(!ex_roi_)return 0;
//    if(!image) return 0;
//    if(!brip_vil_float_ops::chip(image, ex_roi_, clip_image_))
//        return false;
//    else 
//        return true;
//    }

