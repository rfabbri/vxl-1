// This is brl/bseg/dbinfo/dbinfo_region_geometry.cxx
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_float_3x3.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <vsol/vsol_point_2d.h>
#include <bsol/bsol_algs.h>
//constructor
dbinfo_region_geometry::
dbinfo_region_geometry(const unsigned image_cols,
                       const unsigned image_rows,
                       vcl_vector<vsol_polygon_2d_sptr> const& track_boundary,
                       const float thresh):
  fraction_inside_thresh_(thresh), image_cols_(image_cols),
  image_rows_(image_rows), boundaries_(track_boundary)
{
  points_valid_ = false;
  this->compute_points();
}

dbinfo_region_geometry::
dbinfo_region_geometry(const unsigned image_cols, const unsigned image_rows,
                       vsol_polygon_2d_sptr const& track_boundary,
                       const float thresh) :
  fraction_inside_thresh_(thresh), image_cols_(image_cols),
  image_rows_(image_rows)
{
  boundaries_.push_back(track_boundary);
  points_valid_ = false;
  this->compute_points();
}  

//: get the overall centroid weighted by area
vsol_point_2d_sptr dbinfo_region_geometry::centroid() {
  double xc=0, yc=0;
  double tarea = 0;
  for(vcl_vector<vsol_polygon_2d_sptr>::const_iterator pit = 
        boundaries_.begin(); pit != boundaries_.end(); pit++)
    {
      vsol_point_2d_sptr cent = (*pit)->centroid();
      double area = (*pit)->area();
      xc += cent->x()*area;
      yc += cent->y()*area;
      tarea += area;
    }
  xc /= tarea;
  yc /= tarea;

  vsol_point_2d_sptr gc = new vsol_point_2d(xc, yc);
  return gc;
}

//: Copy constructor with transformation.  The same point set is kept, but
//  the coordinates are transformed
dbinfo_region_geometry::dbinfo_region_geometry(const dbinfo_region_geometry & rhs,
                                               vgl_h_matrix_2d<float> const& trans,
                                               const float thresh):
  fraction_inside_thresh_(thresh), points_valid_(rhs.points_valid_), 
  sufficient_points_(rhs.sufficient_points_), image_cols_(rhs.image_cols_),
  image_rows_(rhs.image_rows_), point_masks_(rhs.point_masks_), points_(rhs.points_)
{

  //cast to double
  vnl_double_3x3 md;
  vnl_float_3x3 mf = trans.get_matrix();
  for(unsigned r = 0; r<3; ++r)
    for(unsigned c = 0; c<3; ++c)
      md[r][c]=mf[r][c];
  vgl_h_matrix_2d<double> Htemp(md);

  double xc=0, yc=0;
  //Get the overall centroid
  double tarea = 0;
  for(vcl_vector<vsol_polygon_2d_sptr>::const_iterator pit = 
        rhs.boundaries_.begin(); pit != rhs.boundaries_.end(); pit++)
    {
      vsol_point_2d_sptr cent = (*pit)->centroid();
      double area = (*pit)->area();
      xc += cent->x()*area;
      yc += cent->y()*area;
      tarea += area;
    }
  xc /= tarea;
  yc /= tarea;

  vsol_point_2d_sptr gc = new vsol_point_2d(xc, yc);

  for(vcl_vector<vsol_polygon_2d_sptr>::const_iterator pit = 
        rhs.boundaries_.begin(); pit != rhs.boundaries_.end(); pit++)
    {
      vsol_polygon_2d_sptr Hpoly = 
        bsol_algs::transform_about_point(*pit,gc, Htemp);       
      boundaries_.push_back(Hpoly);
    }
  //  this->adjust_image_bounds(); //JLM
  if(points_valid_)
    {
      int i = 0;
      for(vcl_vector<vgl_point_2d<float> >::iterator pit = points_.begin();
          pit != points_.end(); ++pit, ++i)
        {
          vgl_homg_point_2d<double> hp((*pit).x()-xc, (*pit).y()-yc), thp;
          thp = Htemp(hp);
          vgl_point_2d<double> tep(thp);
          float x = static_cast<float>(tep.x()+xc),
            y = static_cast<float>(tep.y() + yc);
          //: Ozge changed the following line from
          //  if(x>=0||x<image_cols_||y>0||y<image_rows_)
          if(x>=0 && x<image_cols_ && y>0 && y<image_rows_)
            {
              point_masks_[i]=true;
              (*pit).set(x, y);
            }
          else
            {
              point_masks_[i]=false;
              (*pit).set(0, 0);
            }
    
        }
      // Ozge added this line, since sufficient_points_ variable was being 
      // copied directly from right hand side and never updated again for the new region
      this->compute_sufficient();
    }
  else
    this->compute_points();
}

dbinfo_region_geometry::dbinfo_region_geometry(const dbinfo_region_geometry & rhs)

  :  fraction_inside_thresh_(rhs.fraction_inside_thresh_),
     points_valid_(false), image_cols_(rhs.image_cols_),
     image_rows_(rhs.image_rows_), boundaries_(rhs.boundaries_)
{
  this->compute_points();
}

brip_roi_sptr dbinfo_region_geometry::roi() const
{
  brip_roi* roi = new brip_roi(image_cols_, image_rows_);
  for(vcl_vector<vsol_polygon_2d_sptr>::const_iterator pit = boundaries_.begin();
      pit != boundaries_.end(); ++pit)
    {
      vsol_box_2d_sptr box = (*pit)->get_bounding_box();
      roi->add_region(box);
    }
  return roi;
}

//compute if a sufficient number of region points lie inside the image bounds
void dbinfo_region_geometry::compute_sufficient()
{
  if(!points_valid_)
    {sufficient_points_ = false; return;}
  float count = 0;
  for(vcl_vector<bool>::iterator mit = point_masks_.begin();
      mit != point_masks_.end(); ++mit)
    if((*mit)) count++;
  float ratio = count/(float)points_.size();
  sufficient_points_ =  ratio >= fraction_inside_thresh_;
}

//compute discrete pixel locations inside the track boundary
//for now, it includes the points within the union of the tracking polygons
//there could be duplicate points but we don't worry about that now
void dbinfo_region_geometry::compute_points()
{
  points_.clear();
  point_masks_.clear();
  for(vcl_vector<vsol_polygon_2d_sptr>::iterator pit = boundaries_.begin();
      pit != boundaries_.end(); ++pit)
    {
      //convert the bounding vsol polygons to a vgl_polygon
      vgl_polygon<double> vgp = bsol_algs::vgl_from_poly(*pit);
  
      //scan convert the interior points
      vgl_polygon_scan_iterator<double> psi(vgp, false);
      //iterate through the interior
      for (psi.reset(); psi.next();)
        {
        for (int x = psi.startx(); x<=psi.endx(); x++)
          {
            int y = psi.scany();
            float fx = (float)x, fy = (float)y;
            vgl_point_2d<float> p(fx, fy);
            points_.push_back(p);
            point_masks_.push_back(x>=0&&x<static_cast<int>(image_cols_)&&y>=0&&y<static_cast<int>(image_rows_));
    }}
    }
  points_valid_ = true;
  //see if the number of points inside the image is sufficient
  this->compute_sufficient();
}
void dbinfo_region_geometry::adjust_image_bounds()
{
  bool mutated = false;
  vsol_box_2d_sptr updated_bounds = new vsol_box_2d();   
  for(vcl_vector<vsol_polygon_2d_sptr>::iterator pit = boundaries_.begin();
      pit != boundaries_.end(); ++pit)
    {
      vsol_box_2d_sptr bb = (*pit)->get_bounding_box();
      updated_bounds->grow_minmax_bounds(bb);
      mutated = true;
    }
  if(mutated)
    {
      double w = updated_bounds->width(), h = updated_bounds->height();
      if(w<0||h<0)
        return;
      image_cols_ = static_cast<unsigned>(w);
      image_rows_ = static_cast<unsigned>(h);
    }
}
bool dbinfo_region_geometry::transform(vgl_h_matrix_2d<float> const& trans)
{
  //cast the transform to double
  vnl_double_3x3 md;
  vnl_float_3x3 mf = trans.get_matrix();
  for(unsigned r = 0; r<3; ++r)
    for(unsigned c = 0; c<3; ++c)
      md[r][c]=mf[r][c];
  vgl_h_matrix_2d<double> Htemp(md);
  vcl_vector<vsol_polygon_2d_sptr> btemp;
  //transform the boundary polygons
  for(vcl_vector<vsol_polygon_2d_sptr>::iterator pit = boundaries_.begin();
      pit != boundaries_.end(); ++pit)
    {
      vsol_polygon_2d_sptr Hpoly;
      if(!bsol_algs::homography(*pit,Htemp, Hpoly))
        return false;
      btemp.push_back(Hpoly);
    }
  boundaries_ = btemp;
  //  this->adjust_image_bounds(); //JLM
  this->compute_points();
  return true;
}
// Compute the center of gravity of the region
vsol_point_2d_sptr dbinfo_region_geometry::cog()
{
  if(!points_valid_)
    this->compute_points();
  float sumx = 0, sumy = 0;
  for(vcl_vector<vgl_point_2d<float> >::const_iterator pit = points_.begin();
      pit != points_.end(); ++pit)
    {
      sumx += (*pit).x();
      sumy += (*pit).y();
    }
  float npts = (float)(points_.size());
  float scale = 1.0;
  if(npts!=0)
    scale = 1.0f/npts;
  sumx *= scale;
  sumy *= scale;
  return new vsol_point_2d(sumx, sumy);
}

double dbinfo_region_geometry::diameter()
{
  brip_roi_sptr roi = this->roi();
  unsigned n = roi->n_regions();
  int cmin = roi->cmin(0), rmin = roi->rmin(0); 
  int cmax = roi->cmax(0), rmax = roi->rmax(0); 
  for(unsigned i =1; i<n; ++i)
    {
      if(roi->cmin(i)<cmin)
        cmin = roi->cmin(i);
      if(roi->cmax(i)>cmax)
        cmax = roi->cmax(i);
      if(roi->rmin(i)<rmin)
        rmin = roi->rmin(i);
      if(roi->rmax(i)>rmax)
        rmax = roi->rmax(i);
    }
  vgl_point_2d<double> pmin(static_cast<double>(cmin), static_cast<double>(rmin));
  vgl_point_2d<double> pmax(static_cast<double>(cmax), static_cast<double>(rmax));
  return vgl_distance(pmin, pmax);
}

void dbinfo_region_geometry::print(vcl_ostream &strm)
{
  vsol_point_2d_sptr cog = this->cog();
  brip_roi_sptr roi = this->roi();
  strm << "dbinfo_region_geometry [\n"
       << " image_cols" <<  image_cols_
       << "  image_rows" <<  image_rows_ << '\n'
       << "  cog( "  << cog->x() << ' ' << cog->y() << ")\n";
  unsigned n = roi->n_regions();
  for(unsigned i = 0; i<n; ++i)
    {
      strm << " bounds(" << i <<  ")|(" <<  roi->cmin(i) << ' '<< roi->rmin(i)
           <<  "):(" <<  roi->cmax(i)<< ' ' << roi->rmax(i) << ")\n";
    }
  strm << "]\n";
}
//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void dbinfo_region_geometry::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, image_cols_);
  vsl_b_write(os, image_rows_);
  vsl_b_write(os, boundaries_);
  vsl_b_write(os, points_);
  vsl_b_write(os, point_masks_);
}

//: Binary load self from stream (not typically used)
void dbinfo_region_geometry::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        vsl_b_read(is, image_cols_);
        vsl_b_read(is, image_rows_);
        vsl_b_read(is, boundaries_);
        vsl_b_read(is, points_);
        vsl_b_read(is, point_masks_);
        if(points_.size())
          {
            this->points_valid_ = true;
            this->sufficient_points_ = true;
          }
        break;
      }
    default:
      vcl_cerr << "dbinfo_region_geometry: unknown I/O version " << ver << '\n';
    }
}
