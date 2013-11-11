// This is brcv/seg/dbdet/dbdet_scale_space_peaks.cxx
//:
// \file

#include "dbdet_scale_space_peaks.h"
#include <vil/vil_image_view.h>
#include <vcl_cstddef.h>

#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_qr.h>

static const float pi = 3.1415927f;


// compute (r+1)^2/r where r is the ratio of pricipal curvatures
static inline float
curvature_ratio(const float *center, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
  float n_m1_m1 = center[-i_step-j_step];
  float n_m1_0  = center[-i_step];
  float n_m1_1  = center[-i_step+j_step];
  float n_0_m1  = center[-j_step];
  float n_0_0   = center[0];
  float n_0_1   = center[j_step];
  float n_1_m1  = center[i_step-j_step];
  float n_1_0   = center[i_step];
  float n_1_1   = center[i_step+j_step];

  //Compute the 2nd order quadratic coefficients;
  //      1/3 * [ +1 -2 +1 ]
  // Dxx =      [ +1 -2 +1 ]
  //            [ +1 -2 +1 ]
  float Dxx = (   ( n_m1_m1+n_0_m1+n_1_m1
                 +n_m1_1 +n_0_1 +n_1_1 )
             -2.0f*(n_m1_0 +n_0_0 +n_1_0) )/3.0f;
  //      1/4 * [ +1  0 -1 ]
  // Dxy =      [  0  0  0 ]
  //            [ -1  0 +1 ]
  float Dxy = (n_m1_m1-n_m1_1-n_1_m1+n_1_1)/4.0f;
  //      1/3 * [ +1 +1 +1 ]
  // Dyy =      [ -2 -2 -2 ]
  //            [ +1 +1 +1 ]
  float Dyy = (   ( n_m1_m1+n_m1_0+n_m1_1
                 +n_1_m1 +n_1_0 +n_1_1 )
             -2.0f*(n_0_m1 +n_0_0 +n_0_1) )/3.0f;

  float TrH = Dxx + Dyy;
  float DetH = Dxx*Dyy - Dxy*Dxy;
  return TrH*TrH/DetH;
}


// refine the position based on a 3x3x3 image of neighbors
static float
refine_peak( const vil_image_view<float> & neighbors, vnl_double_3& delta)
{
  vnl_double_3 D;

  // Dx
  D(0) = (neighbors(2,1,1) - neighbors(0,1,1))/2.0;

  // Dy
  D(1) = (neighbors(1,2,1) - neighbors(1,0,1))/2.0;

  // Dz
  D(2) = (neighbors(1,1,2) - neighbors(1,1,0))/2.0;

  // The Hessian
  vnl_double_3x3 H;

  // Dxx
  H(0,0) = (neighbors(2,1,1) - 2.0*neighbors(1,1,1) + neighbors(0,1,1));

  // Dyy
  H(1,1) = (neighbors(1,2,1) - 2.0*neighbors(1,1,1) + neighbors(1,0,1));

  // Dzz
  H(2,2) = (neighbors(1,1,2) - 2.0*neighbors(1,1,1) + neighbors(1,1,0));

  // Dxy
  H(0,1) = H(1,0) = (neighbors(2,2,1) + neighbors(0,0,1)
                    -neighbors(0,2,1) - neighbors(2,0,1))/4.0;

  // Dxz
  H(0,2) = H(2,0) =  (neighbors(2,1,2) + neighbors(0,1,0)
                     -neighbors(0,1,2) - neighbors(2,1,0))/4.0;

  // Dyz
  H(1,2) = H(2,1) =  (neighbors(1,2,2) + neighbors(1,0,0)
                     -neighbors(1,0,2) - neighbors(1,2,0))/4.0;

  delta = -vnl_qr<double>(H).solve(D);//vnl_inverse(H)*D;

  return neighbors(1,1,1) + float(dot_product(D,delta))/2.0f;
}


//: DoG Peak finding helper function
static inline bool
is_max_3x3(const float* im, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
   if (*im <= im[i_step]) return false;
   if (*im <= im[-i_step]) return false;
   if (*im <= im[j_step]) return false;
   if (*im <= im[-j_step]) return false;
   if (*im <= im[i_step+j_step]) return false;
   if (*im <= im[i_step-j_step]) return false;
   if (*im <= im[j_step-i_step]) return false;
   if (*im <= im[-i_step-j_step]) return false;
   return true;
}


//: DoG Peak finding helper function
static inline bool
is_min_3x3(const float* im, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
   if (*im >= im[i_step]) return false;
   if (*im >= im[-i_step]) return false;
   if (*im >= im[j_step]) return false;
   if (*im >= im[-j_step]) return false;
   if (*im >= im[i_step+j_step]) return false;
   if (*im >= im[i_step-j_step]) return false;
   if (*im >= im[j_step-i_step]) return false;
   if (*im >= im[-i_step-j_step]) return false;
   return true;
}


//: DoG Peak finding helper function
static inline bool
is_more_3x3(const float value, const float* im, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
   if (value <= im[0]) return false;
   if (value <= im[i_step]) return false;
   if (value <= im[-i_step]) return false;
   if (value <= im[j_step]) return false;
   if (value <= im[-j_step]) return false;
   if (value <= im[i_step+j_step]) return false;
   if (value <= im[i_step-j_step]) return false;
   if (value <= im[j_step-i_step]) return false;
   if (value <= im[-i_step-j_step]) return false;
   return true;
}


//: DoG Peak finding helper function
static inline bool
is_less_3x3(const float value, const float* im, vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step)
{
   if (value >= im[0]) return false;
   if (value >= im[i_step]) return false;
   if (value >= im[-i_step]) return false;
   if (value >= im[j_step]) return false;
   if (value >= im[-j_step]) return false;
   if (value >= im[i_step+j_step]) return false;
   if (value >= im[i_step-j_step]) return false;

   if (value >= im[j_step-i_step]) return false;
   if (value >= im[-i_step-j_step]) return false;
   return true;
}


//: Find the peaks in the scale space
void dbdet_scale_space_peaks(const bil_scale_image<float>& scale_image,
                             vcl_vector<vgl_point_3d<float> >& peak_pts,
                             float curve_ratio,
                             float contrast_thresh)
{
  unsigned int num_lvl = scale_image.levels();
  int first_oct = scale_image.first_octave();
  int last_oct = first_oct + int(scale_image.octaves())-1;
  float max_curve = (curve_ratio+1.0f)*(curve_ratio+1.0f)/curve_ratio;

  for(int oct=first_oct; oct<=last_oct; ++oct){
    for(unsigned int lvl=1; lvl<=num_lvl; ++lvl){
      vcl_cout << "oct = "<<oct << " lvl = "<<lvl<<vcl_endl;    
      const vil_image_view<float> & above = scale_image(oct,lvl+1);
      const vil_image_view<float> & image = scale_image(oct,lvl);
      const vil_image_view<float> & below = scale_image(oct,lvl-1);


      float ps = scale_image.image_scale(oct);

      unsigned int ni = image.ni(), nj = image.nj();
      vcl_ptrdiff_t istep=image.istep(), jstep=image.jstep();
      const float* row = image.top_left_ptr() + istep + jstep;
      for (unsigned j=1; j<nj-1; ++j,row+=jstep){
        const float* pixel = row;
        for (unsigned i=1; i<ni-1; ++i,pixel+=istep){

          assert(image(i,j) == *pixel);
          int sign = 0;
          // check for maxima
          if( is_max_3x3(pixel, istep, jstep) && 
              is_more_3x3(*pixel, &above(i,j), above.istep(), above.jstep()) &&
              is_more_3x3(*pixel, &below(i,j), below.istep(), below.jstep()) ){

            sign = 1;
          }
          // check for minima
          else if( is_min_3x3(pixel, istep, jstep) &&
                   is_less_3x3(*pixel, &above(i,j), above.istep(), above.jstep()) &&
                   is_less_3x3(*pixel, &below(i,j), below.istep(), below.jstep()) ){
            sign = -1;
          }

          if( sign == 0 ) continue; // this pixel is not a peak

          // refined indices
          unsigned int ri = i, rj = j, rlvl = lvl;
          vnl_double_3 offset(0,0,0);
          vil_image_view<float> neighbors = scale_image.neighbors(oct,rlvl,ri,rj);
          float peak_val = refine_peak( neighbors, offset );

          // offset is more than one pixel away, reestimate
          // This usually finds a stable peak within a few iterations
          // If it takes more than 5, assume it is oscillating 
          bool peak_valid = true;
          int cnt=0;        
          for(; cnt<5&&peak_valid; ++cnt){ 
            // if not within a pixel
            if(vcl_fabs(offset(0)) >= 0.5 || 
               vcl_fabs(offset(1)) >= 0.5 || 
               vcl_fabs(offset(2)) >= 0.5 ){
              // compute the new pixel
              ri = (unsigned int)(double(ri)+offset(0)+0.5);
              rj = (unsigned int)(double(rj)+offset(1)+0.5);
              rlvl = (unsigned int)(double(rlvl)+offset(2)+0.5);
              // verify that the new pixel is within bounds
              if( rlvl>=1 && rlvl<=num_lvl && ri>=1 && ri<ni-1 && rj>=1 && rj<nj-1){
                neighbors = scale_image.neighbors(oct,rlvl,ri,rj);
                peak_val = refine_peak( neighbors, offset );
              }
              else
                peak_valid = false;
            }
            else 
              break;
          }
          
          if( !peak_valid ) continue;
          
          // if we oscillated for a while between neighboring points 
          // that's close enough, otherwise reject
          if(cnt >= 5 && (vcl_fabs(offset(0)) >= 1.0 || 
                          vcl_fabs(offset(1)) >= 1.0 || 
                          vcl_fabs(offset(2)) >= 1.0 ) ) continue;
                    
          // peaks in the images at the most course scale image are ignored
          // because later code round up into the next octave which does not exist
          if(oct == last_oct && rlvl == num_lvl) continue;
           
          
          // ignore low contrast peaks
          if( sign*peak_val <  contrast_thresh ) continue;

          // ignore peaks with high principle curvature ratio
          const vil_image_view<float> & rimage = scale_image(oct,rlvl);
          float curv_rat = curvature_ratio(&rimage(ri,rj), rimage.istep(), rimage.jstep());
          if ( curv_rat > max_curve || curv_rat < 0.0f ) continue;
          
          peak_pts.push_back(vgl_point_3d<float>(float(ri+offset(0))*ps, float(rj+offset(1))*ps,
                                                 float(scale_image.init_scale()
                                                       *vcl_pow(2.0, oct + double(rlvl+offset(2))/(num_lvl))) ));
        }
      }
    }
  }
}


//================================== Orientations ========================================


//: Constructor using a scale space image
dbdet_ssp_orientation_params::dbdet_ssp_orientation_params(
                               const bil_scale_image<float>& scale_image,
                               unsigned int bins, float sig, float thresh)
 : peak_thresh(thresh), sigma(sig), num_bins(bins)
{
  scale_image.compute_gradients(scale_grad_dir, scale_grad_mag);
}


//: Constructor using precomputed gradients
dbdet_ssp_orientation_params::dbdet_ssp_orientation_params(
                               const bil_scale_image<float>& si_grad_dir,
                               const bil_scale_image<float>& si_grad_mag,
                               unsigned int bins, float sig, float thresh)
 : scale_grad_dir(si_grad_dir), scale_grad_mag(si_grad_mag),
   peak_thresh(thresh), sigma(sig), num_bins(bins)
{
}


//: Compute the peak orientations of a scale space point
vcl_vector<float>
dbdet_ssp_orientations(vgl_point_3d<float> ssp,
                       const dbdet_ssp_orientation_params& params)
{
  // extract the gradient images for this scale
  float init_scale = params.scale_grad_dir.init_scale();
  unsigned int num_lvl = params.scale_grad_dir.levels();
  int first_oct = params.scale_grad_dir.first_octave();
  
  double log2_scale = vcl_log(ssp.z()/init_scale)/vcl_log(2.0)-first_oct;
  unsigned int index = (unsigned int)(log2_scale*num_lvl +0.5);
  int oct = index/num_lvl;
  unsigned int lvl = index%num_lvl;
  oct += first_oct;
  
  const vil_image_view<float> & grad_orient = params.scale_grad_dir(oct,lvl);
  const vil_image_view<float> & grad_mag =  params.scale_grad_mag(oct,lvl);

  // compute position relative to the current gradient image size
  float img_scale = params.scale_grad_dir.image_scale(oct);

  float x = ssp.x()/img_scale;
  float y = ssp.y()/img_scale;
  int x_int = int(x+0.5);
  int y_int = int(y+0.5);

  // compute scale relative to the current image size
  float rel_scale = ssp.z()/img_scale;
  float sigma = params.sigma * rel_scale;
  // trucate the gaussian region after 3 sigma
  int size = int(3.0*sigma)+1;
  float bin_scale = (2*params.num_bins-1)/(2.0f*pi);

  
  // the orientation histogram
  vcl_vector<float> histogram(params.num_bins, 0.0);

  // compute the orientation histogram in a size-by-size box around the point
  for (int i=x_int-size; i<=x_int+size; ++i){
    for (int j=y_int-size; j<=y_int+size; ++j){
      // only consider pixels within the image
      if (i>=0 && i<int(grad_orient.ni()) &&
          j>=0 && j<int(grad_orient.nj()) ){
        float x_dist = i-x;
        float y_dist = j-y;
        float dist_2 = x_dist*x_dist + y_dist*y_dist;
        float sig_2 = sigma*sigma;
        // ignore pixels outside a radius of 3 sigma
        if( dist_2 <= 9.0*sig_2 ){
          // compute the weight of the contribution
          float weight = grad_mag(i,j)*vcl_exp(-dist_2/(2.0f*sig_2));
          // compute the bin number
          int bin = ((int((grad_orient(i,j)+pi)*bin_scale)+1)/2) % params.num_bins;
          // add to the bin
          histogram[bin] += weight;
        }
      }
    }
  }

  
  float max = 0.0;
  vcl_vector<unsigned int> peaks;
  
  // find the maximum peak
  for (unsigned int i=0; i<params.num_bins; ++i){
    if( histogram[i] > histogram[(i-1)%params.num_bins] &&
        histogram[i] > histogram[(i+1)%params.num_bins] ){
      if( histogram[i] > max ) max = histogram[i];
      peaks.push_back(i);
    }
  }

  // must have found at least 1 peak unless the histogram was perfectly flat
  assert(!peaks.empty());

  // the vector of values to return
  vcl_vector<float> orientations;
  
  // find all peaks within peak_thresh of the max peak
  // and use parabolic interpolation to compute the peak orientation
  max *= params.peak_thresh;
  for (unsigned int i=0; i<peaks.size(); ++i){
    if (histogram[ peaks[i] ] > max){
      //parabolic interpolation
      float ypos = histogram[ (peaks[i]+1)%params.num_bins];
      float yneg = histogram[ (peaks[i]-1)%params.num_bins];
      float dy   = (ypos - yneg)/2.0f;
      float d2y  = 2.0f*histogram[ peaks[i] ] - ypos - yneg;
      float dx = (pi*2.0f)/params.num_bins;
      float angle = (float(peaks[i])+dy/d2y)*dx;
      orientations.push_back(angle);
    }
  }
  return orientations;
}


