// This is contrib/mleotta/modrec/vidreg_detector.cxx

//:
// \file


#include <vcl_algorithm.h>
#include <vcl_set.h>
#include "vidreg_detector.h"
#include <vidreg/vidreg_feature_pt_desc.h>
#include <vidreg/vidreg_feature_edgel.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_sobel_1x3.h>
#include <vil/algo/vil_orientations.h>
#include <vil/algo/vil_find_peaks.h>
#include <gevd/gevd_float_operators.h>
#include <gevd/gevd_pixel.h>

#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_face_pt.h>
#include <rgrl/rgrl_cast.h>
#include <vnl/vnl_math.h>


//: Constructor
vidreg_detector::vidreg_detector()
  : edgel(NULL),
    dx(NULL),
    dy(NULL),
    mag(NULL),
    direction(NULL),
    locationx(NULL),
    locationy(NULL),
    features_(NULL),
    point_sigma_(1.5),
    peak_thresh_(0.8),
    max_pts_to_describe_(300),
    num_bins_(8)
{
}


//: Destructor
vidreg_detector::~vidreg_detector()
{
  delete dx;
  delete dy;
  delete mag;
  delete edgel;
  delete direction;
  delete locationx;
  delete locationy;
}


//: Make sure buf is the right size and wrap the memory in view
void
vidreg_detector::make_valid_image(gevd_bufferxy*& buf, vil_image_view<float>& view,
                                  int ni, int nj)
{
  if(!buf || buf->GetSizeX() != ni || buf->GetSizeY() != nj){
    delete buf;
    buf = new gevd_bufferxy(ni, nj, sizeof(float)*8);
    view = vil_image_view<float>(static_cast<const float*>(buf->GetBuffer()),
                                 ni,nj,1,1,ni,ni*nj);
  }
}

namespace {
class sqr_functor
{
  public:
    float operator()(float x) const { return x*x; }
    double operator()(double x) const { return x*x; }
};

template<class inT, class outT>
inline void corner_function(const vil_image_view<inT>& in,
                                  vil_image_view<outT>& out)
{
  unsigned ni = in.ni(), nj = in.nj();
  assert(in.nplanes() == 3);
  out.set_size(ni,nj,1);

  vcl_ptrdiff_t istep_in=in.istep(), jstep_in=in.jstep(), pstep_in = in.planestep();
  vcl_ptrdiff_t istep_out=out.istep(), jstep_out=out.jstep();
  const inT* row_in = in.top_left_ptr();
        outT* row_out = out.top_left_ptr();
  for (unsigned j=0; j<nj; ++j, row_in += jstep_in, row_out += jstep_out)
  {
    const inT* pixel_in = row_in;
          outT* pixel_out = row_out;
    for (unsigned i=0; i<ni; ++i, pixel_in+=istep_in, pixel_out+=istep_out){
      const inT& a = *pixel_in;
      const inT& b = *(pixel_in+pstep_in);
      const inT& c = *(pixel_in+2*pstep_in);
      double trace = a+b;
      *pixel_out = a*b-c*c - .04*trace*trace;
    }
  }
}

template<class T>
bool interpolate_center(const T* pixel,
                        vcl_ptrdiff_t istep, vcl_ptrdiff_t jstep,
                        double& dx, double& dy, double& val)
{
  dx = 0; dy=0;
  //extract the neighborhood
  // +-----+-----+-----+
  // | p00 | p10 | p20 |
  // +-----+-----+-----+
  // | p01 | p11 | p21 |
  // +-----+-----+-----+
  // | p02 | p12 | p22 |
  // +-----+-----+-----+
  const T& p11 = *pixel;
  const T& p01 = *(pixel-istep);
  const T& p21 = *(pixel+istep);
  const T& p10 = *(pixel-jstep);
  const T& p12 = *(pixel+jstep);
  const T& p00 = *(&p10-istep);
  const T& p20 = *(&p10+istep);
  const T& p02 = *(&p12-istep);
  const T& p22 = *(&p12+istep);

  //Compute the 2nd order quadratic coefficients
  //      1/6 * [ -1  0 +1 ]
  // Ix =       [ -1  0 +1 ]
  //            [ -1  0 +1 ]
  double Ix =(-p00-p01-p02 +p20+p21+p22)/6.0;
  //      1/6 * [ -1 -1 -1 ]
  // Iy =       [  0  0  0 ]
  //            [ +1 +1 +1 ]
  double Iy =(-p00-p10-p20 +p02+p12+p22)/6.0;
  //      1/3 * [ +1 -2 +1 ]
  // Ixx =      [ +1 -2 +1 ]
  //            [ +1 -2 +1 ]
  double Ixx = ((p00+p01+p02 +p20+p21+p22)-2.0*(p10+p11+p12))/3.0;
  //      1/4 * [ +1  0 -1 ]
  // Ixy =      [  0  0  0 ]
  //            [ -1  0 +1 ]
  double Ixy = (p00+p22 -p02-p20)/4.0;
  //      1/3 * [ +1 +1 +1 ]
  // Iyy =      [ -2 -2 -2 ]
  //            [ +1 +1 +1 ]
  double Iyy = ((p00+p10+p20 +p02+p12+p22)-2.0*(p01+p11+p21))/3.0;

  //
  // The next bit is to find the extremum of the fitted surface by setting its
  // partial derivatives to zero. We need to solve the following linear system :
  // Given the fitted surface is
  // I(x,y) = Io + Ix x + Iy y + 1/2 Ixx x^2 + Ixy x y + 1/2 Iyy y^2
  // we solve for the maximum (x,y),
  //
  //  [ Ixx Ixy ] [ dx ] + [ Ix ] = [ 0 ]      (dI/dx = 0)
  //  [ Ixy Iyy ] [ dy ]   [ Iy ]   [ 0 ]      (dI/dy = 0)
  //
  double det = Ixx*Iyy - Ixy*Ixy;
  // det>0 corresponds to a true local extremum otherwise a saddle point
  if(det<=0)
    return false;

  dx = (Iy*Ixy - Ix*Iyy) / det;
  dy = (Ix*Ixy - Iy*Ixx) / det;
  // more than one pixel away
  if (vcl_fabs(dx) > 1.0 || vcl_fabs(dy) > 1.0)
    return false;

  double Io =(p00+p01+p02 +p10+p11+p12 +p20+p21+p22)/9.0;

  val = Io + (Ix + 0.5*Ixx*dx + Ixy*dy)*dx + (Iy + 0.5*Iyy*dy)*dy;

  return true;
}

unsigned trace_order[][2] = { {2,1}, {2,3}, {0,1}, {0,3},
                              {1,2}, {3,2}, {1,0}, {3,0} };


inline vidreg_feature_edgel** find_next(vidreg_feature_edgel** curr,
                                        vcl_ptrdiff_t d1, vcl_ptrdiff_t d2)
{
  vidreg_feature_edgel** next = NULL;
  if( !(*(next = curr + d1)) )                // first choice (4-con)
    if( !(*(next = curr + d2)) )              // second choice (4-con)
      if( !(*(next += d1)) )                 // third choice (8-con)
        if( !(*(next = curr + d1 - d2)) )     // fourth choice (8-con)
          if( !(*(next = curr - d1 + d2)) )   // last choice (8-con)
            return NULL;
  return next;
}


inline vidreg_feature_edgel** find_prev(vidreg_feature_edgel** curr,
                                        vcl_ptrdiff_t d1, vcl_ptrdiff_t d2)
{
  vidreg_feature_edgel** next = NULL;
  if( !(*(next = curr - d1)) )                // first choice (4-con)
    if( !(*(next = curr - d2)) )              // second choice (4-con)
      if( !(*(next -= d1)) )                 // third choice (8-con)
        if( !(*(next = curr - d1 + d2)) )     // fourth choice (8-con)
          if( !(*(next = curr + d1 - d2)) )   // last choice (8-con)
            return NULL;
  return next;
}


void link_edgels(vbl_array_2d<vidreg_feature_edgel*>& edgel_grid,
                 vcl_vector<rgrl_feature_sptr>& features)
{
  vcl_sort(features.begin(), features.end(), vidreg_feature_edgel::dec_mag_order);
  typedef vcl_vector<rgrl_feature_sptr>::iterator Fitr;

  //the neighborhood pointer offsets
  // +----+----+----+
  // |    | d3 |    |
  // +----+----+----+
  // | d2 |    | d0 |
  // +----+----+----+
  // |    | d1 |    |
  // +----+----+----+
  vcl_ptrdiff_t d[] = {1, edgel_grid.cols(), -1, -edgel_grid.cols()};


  vidreg_feature_edgel **seed = NULL, **curr = NULL;

  for(Fitr itr = features.begin(); itr!=features.end(); ++itr){
    vidreg_feature_edgel* edgel = rgrl_cast<vidreg_feature_edgel*>(*itr);
    if(edgel->magnitude() < 4.0)
      break;

    int x = static_cast<int>((edgel->location()[0]+0.5));
    int y = static_cast<int>((edgel->location()[1]+0.5));
    seed = edgel_grid.begin() + x*d[0] + y*d[1];
    assert(seed < edgel_grid.end());
    assert(seed >= edgel_grid.begin());
    if(*seed != edgel)
      continue;

    // Link forward along the edge
    curr = seed;
    for(vidreg_feature_edgel **next = NULL; !((*curr)->next()); curr = next){
      const vnl_vector<double>& n = (*curr)->normal();
      unsigned order = (n[0]>0) + ((n[1]>0)<<1) + ((vcl_abs(n[0])>vcl_abs(n[1]))<<2);
      assert( order >=0 && order <8);
      unsigned *t_ord = &trace_order[0][0] + order*2;
      if( !(next = find_next(curr,d[t_ord[0]],d[t_ord[1]])) )
        break;

      assert(next < edgel_grid.end());
      assert(next >= edgel_grid.begin());

      // already found this path
      if((*next)->prev()){
        double old_path = dot_product((*next)->prev()->normal(), (*next)->normal());
        double new_path = dot_product(n, (*next)->normal());
        if(new_path > old_path){
          // break old path and connect
          (*next)->prev()->set_next(NULL);
          (*curr)->set_next(*next);
        } 
        break;
      }

      (*curr)->set_next(*next);
    }

    // Link backward along the edge
    curr = seed;
    for(vidreg_feature_edgel **prev = NULL; !((*curr)->prev()); curr = prev){
      const vnl_vector<double>& n = (*curr)->normal();
      unsigned order = (n[0]>0) + ((n[1]>0)<<1) + ((vcl_abs(n[0])>vcl_abs(n[1]))<<2);
      assert( order >=0 && order <8);
      unsigned *t_ord = &trace_order[0][0] + order*2;
      if( !(prev = find_prev(curr,d[t_ord[0]],d[t_ord[1]])) )
        break;

      assert(prev < edgel_grid.end());
      assert(prev >= edgel_grid.begin());

      // already found this path
      if((*prev)->next()){
        double old_path = dot_product((*prev)->next()->normal(), (*prev)->normal());
        double new_path = dot_product(n, (*prev)->normal());
        if(new_path > old_path){
          // break old path and connect
          (*prev)->next()->set_prev(NULL);
          (*curr)->set_prev(*prev);
        } 
        break;
      }

      (*curr)->set_prev(*prev);
    }

  }

}


void find_edges(vcl_vector<rgrl_feature_sptr>& features,
                vcl_vector<vidreg_edge>& edges)
{
  vcl_set<vidreg_feature_edgel*> edgels;
  for(vcl_vector<rgrl_feature_sptr>::const_iterator i=features.begin(); i!=features.end(); ++i)
    if(vidreg_feature_edgel* e = dynamic_cast<vidreg_feature_edgel*>(i->ptr()))
      edgels.insert(e);

  vcl_vector<rgrl_feature_sptr> new_features;

  while(!edgels.empty()){
    vidreg_feature_edgel* e = *edgels.begin();
    for(; e->prev() && e->prev() != *edgels.begin(); e=e->prev());

    vidreg_feature_edgel* head = e;
    unsigned count = 0;
    for(; e; e=e->next(), ++count){
      vcl_set<vidreg_feature_edgel*>::iterator eit = edgels.find(e);
      if(eit != edgels.end())
        edgels.erase(eit);
      else
        break;
    }
    if(count > 3){
      edges.push_back(vidreg_edge(head));
      e = head;
      for(unsigned i=0; i<count; ++i, e=e->next())
        new_features.push_back(e);
    }
  }
  features = new_features;
}

}; // end anonymous namespace

bool vidreg_detector::detect_features(const vil_image_resource_sptr& image)
{
  features_ = new vidreg_feature_group();

  return detect_edgels(image) && detect_corners(image);
}


//: Detect edgel features only
bool vidreg_detector::detect_edgels(const vil_image_resource_sptr& image)
{
  if(!features_)
    features_ = new vidreg_feature_group();

  unsigned ni = image->ni(), nj = image->nj();
  assert(ni>0  && nj>0);
  assert(image->nplanes() == 1);
  assert(image->pixel_format() == VIL_PIXEL_FORMAT_BYTE);

  make_valid_image(dx,vdx,ni,nj);
  make_valid_image(dy,vdy,ni,nj);
  make_valid_image(mag,vmag,ni,nj);
  make_valid_image(edgel,vedgel,ni,nj);


  vil_image_view<vxl_byte> view = image->get_view();
  vil_gauss_filter_5tap_params sigma(1.0);
  vil_gauss_filter_5tap(view,smooth,sigma,work);

  vil_sobel_1x3(smooth,vdx,vdy);
  vil_orientations(vdx,vdy,vorient,vmag,num_bins_);

  gevd_float_operators::NonMaximumSuppression(*mag, *dx, *dy,
                                              2.0, // above noise
                                              edgel, direction,
                                              locationx, locationy);
  gevd_float_operators::FillFrameX(*edgel, 0, 4); // erase pixels in frame border
  gevd_float_operators::FillFrameY(*edgel, 0, 4);


  vnl_vector<double> pt(2), n(2);
  vbl_array_2d<vidreg_feature_edgel*> edgel_grid(nj,ni,NULL);
  for (int y = 0; y < edgel->GetSizeY(); ++y){
    for (int x = 0; x < edgel->GetSizeX(); ++x){
      const float& pixel = floatPixel(*edgel, x, y);
      if(pixel > 0){
        pt[0] = x+floatPixel(*locationx, x, y);
        pt[1] = y+floatPixel(*locationy, x, y);
        double m = floatPixel(*mag, x, y);
        double m_inv = 1.0/m;
        n[0] = floatPixel(*dx, x, y)*m_inv;
        n[1] = floatPixel(*dy, x, y)*m_inv;
        vidreg_feature_edgel* edgel = new vidreg_feature_edgel(pt,n,m);
        features_->edgels.push_back(rgrl_feature_sptr(edgel));
        edgel_grid(y,x) = edgel;
      }
    }
  }
  vcl_cout << "number of edgels: "<< features_->edgels.size() << vcl_endl;

  link_edgels(edgel_grid, features_->edgels);
  find_edges(features_->edgels, features_->edges);
  vcl_cout << "number of edges: "<< features_->edges.size() << vcl_endl;

  return true;
}


//: Detect corner features only
bool vidreg_detector::detect_corners(const vil_image_resource_sptr& image)
{
  if(!features_)
    features_ = new vidreg_feature_group();

  unsigned ni = image->ni(), nj = image->nj();
  assert(ni>0  && nj>0);
  assert(image->nplanes() == 1);
  assert(image->pixel_format() == VIL_PIXEL_FORMAT_BYTE);

  corners.set_size(ni,nj,3);
  vil_image_view<float> vdx2 = vil_plane(corners,0);
  vil_image_view<float> vdy2 = vil_plane(corners,1);
  vil_image_view<float> vdxdy = vil_plane(corners,2);
  vil_transform(vdx,vdx2,sqr_functor());
  vil_transform(vdy,vdy2,sqr_functor());
  vil_math_image_product(vdx,vdy,vdxdy);
  vil_gauss_filter_5tap_params sigma(1.0);
  vil_gauss_filter_5tap(corners,corners,sigma,work);
  corner_function(corners,work);

  float threshold = 8.0;
  double pdx,pdy,val;
  vnl_vector<double> pt(2);
  vcl_ptrdiff_t istep = work.istep(),jstep=work.jstep();
  const float* row = work.top_left_ptr()+istep+jstep;
  for (unsigned j=1; j<nj-1; ++j, row+=jstep)
  {
    const float* pixel = row;
    for (unsigned i=1; i<ni-1; ++i, pixel+=istep)
    {
      if (*pixel>=threshold &&
          vil_is_peak_3x3(pixel,istep,jstep) &&
          interpolate_center(pixel,istep,jstep,pdx,pdy,val)){
        pt[0] = i+pdx;
        pt[1] = j+pdy;
        //vcl_vector<double> angles = orient_point(pt[0],pt[1]);
        features_->corners.push_back(new vidreg_feature_pt_desc(pt,val));
      }
    }
  }

  // compute descriptors for the top 300 corners
  vnl_vector<double> desc(128,0.0);
  typedef vcl_vector<rgrl_feature_sptr>::iterator Fitr;
  const unsigned int desc_size = vcl_min(max_pts_to_describe_,unsigned(features_->corners.size()));
  Fitr middle = features_->corners.begin()+desc_size;
  vcl_partial_sort(features_->corners.begin(), middle, features_->corners.end(),
                   vidreg_feature_pt_desc::dec_mag_order);
  for(Fitr i = features_->corners.begin(); i!=middle; ++i){
    vidreg_feature_pt_desc* curr_pt = rgrl_cast<vidreg_feature_pt_desc*>(*i);
    make_descriptor(curr_pt->location()[0],curr_pt->location()[1],desc);
    curr_pt->set_descriptor(desc);
  }

  vcl_cout << "number of corners: "<< features_->corners.size() << vcl_endl;
  return true;
}


vcl_vector<double> vidreg_detector::orient_point(double x, double y)
{
  const int x_int = static_cast<int>(x+0.5);
  const int y_int = static_cast<int>(y+0.5);

  // trucate the gaussian region after 3 sigma
  const int size = int(3.0*point_sigma_)+1;
  const double inv_sig_2 = 1.0/(point_sigma_*point_sigma_);

  // the orientation histogram
  vcl_vector<double> histogram(num_bins_, 0.0);

  // compute the orientation histogram in a size-by-size box around the point
  const int i_min = x_int-size,  i_max = x_int+size;
  const int j_min = y_int-size,  j_max = y_int+size;
  for (int i=i_min; i<=i_max; ++i){
    for (int j=j_min; j<=j_max; ++j){
      // only consider pixels within the image
      if (i>=0 && i<int(vorient.ni()) &&
          j>=0 && j<int(vorient.nj()) ){
        double x_dist = i-x;
        double y_dist = j-y;
        double dist_2_norm = x_dist*x_dist + y_dist*y_dist;
        dist_2_norm *= inv_sig_2;
        // ignore pixels outside a radius of 3 sigma
        if( dist_2_norm <= 9.0 ){
          // compute the weight of the contribution
          double weight = vmag(i,j)*vcl_exp(-0.5*dist_2_norm);
          // add to the bin
          histogram[vorient(i,j)] += weight;
        }
      }
    }
  }


  double max = 0.0;
  vcl_vector<vxl_byte> peaks;

  // find the maximum peak
  for (vxl_byte i=0; i<num_bins_; ++i){
    if( histogram[i] > histogram[(i-1)%num_bins_] &&
        histogram[i] > histogram[(i+1)%num_bins_] ){
      if( histogram[i] > max ) max = histogram[i];
      peaks.push_back(i);
    }
  }

  // must have found at least 1 peak unless the histogram was perfectly flat
  assert(!peaks.empty());

  // the vector of values to return
  vcl_vector<double> orientations;

  // find all peaks within peak_thresh of the max peak
  // and use parabolic interpolation to compute the peak orientation
  max *= peak_thresh_;
  double dx = (vnl_math::pi*2.0)/num_bins_;
  for (unsigned int i=0; i<peaks.size(); ++i){
    if (histogram[ peaks[i] ] > max){
      //parabolic interpolation
      double ypos = histogram[ (peaks[i]+1)%num_bins_];
      double yneg = histogram[ (peaks[i]-1)%num_bins_];
      double dy   = (ypos - yneg)/2.0f;
      double d2y  = 2.0*histogram[ peaks[i] ] - ypos - yneg;
      double angle = (peaks[i]+dy/d2y)*dx;
      orientations.push_back(angle);
    }
  }
  return orientations;
}


void
vidreg_detector::make_descriptor(double x, double y,
                                 vnl_vector<double>& descriptor,
                                 double max_comp)
{
  descriptor.fill(0.0);

  const int x_int = static_cast<int>(x+0.5);
  const int y_int = static_cast<int>(y+0.5);
  const double x_offset = x+0.5-x_int,  y_offset = y+0.5-y_int;

  const double inv_sig_2 = 1.0/128.0;
  const int i_min = vcl_max(0,x_int-8),  i_max = vcl_min(int(vorient.ni()),x_int+9);
  const int j_min = vcl_max(0,y_int-8),  j_max = vcl_min(int(vorient.nj()),y_int+9);
  for (int i=i_min; i<i_max; ++i){
    double x_dist = i-x;
    int i1 = i-x_int+7,  i2 = i-x_int+8;
    int bin1 = (i1>=0)?(i1>>2)<<5:-1;
    int bin2 = (i2<16)?(i2>>2)<<5:-1;
    for (int j=j_min; j<j_max; ++j){
      double y_dist = j-y;
      int j1 = j-y_int+7,  j2 = j-y_int+8;

      double dist_2_norm = x_dist*x_dist + y_dist*y_dist;
      dist_2_norm *= inv_sig_2;
      double weight = vmag(i,j) * vcl_exp(-dist_2_norm);
      vxl_byte o = vorient(i,j);


      if(bin1>0){
        if(j1>=0){
          int bin = bin1+((j1>>2)<<3)+o;
          assert(bin >0 && bin<128);
          descriptor[bin] += weight*x_offset*y_offset;
        }
        if(j2<16){
          int bin = bin1+((j2>>2)<<3)+o;
          assert(bin >0 && bin<128);
          descriptor[bin] += weight*x_offset*(1-y_offset);
        }
      }
      if(bin2>0){
        if(j1>=0){
          int bin = bin2+((j1>>2)<<3)+o;
          assert(bin >0 && bin<128);
          descriptor[bin] += weight*(1-x_offset)*y_offset;
        }
        if(j2<16){
          int bin = bin2+((j2>>2)<<3)+o;
          assert(bin >0 && bin<128);
          descriptor[bin] += weight*(1-x_offset)*(1-y_offset);
        }
      }
    }
  }

  descriptor.normalize();
  bool changed = false;
  for( int i=0; i<128; ++i){
    if( descriptor[i] > max_comp ){
      descriptor[i] = max_comp;
      changed = true;
    }
  }

  if( changed )
    descriptor.normalize();
}
