// This is brcv/seg/dbdet/dbdet_lowe_keypoint.cxx
//:
// \file

#include "dbdet_lowe_keypoint.h"
#include <vnl/io/vnl_io_vector.h>


static const float pi = 3.1415927f;


//: Normalize, threshold, and renormalize
void
dbdet_lowe_keypoint::normalize_descriptor(double max_comp)
{
  descriptor_.normalize();
  bool changed = false;
  for( int i=0; i<128; ++i){
    if( descriptor_[i] > max_comp ){
      descriptor_[i] = max_comp;
      changed = true;
    }
  }
  
  if( changed )
    descriptor_.normalize();
}


static inline float gaussian( float x, float y)
{
  return vcl_exp(-((x*x)+(y*y))/(128.0f));
}

//: compute the descriptor
void
dbdet_lowe_keypoint::compute_descriptor(const bil_scale_image<float>& scale_grad_dir,
                                        const bil_scale_image<float>& scale_grad_mag)
{
  descriptor_ = vnl_vector<double>(128,0.0);

  // extract the gradient images for this scale
  float init_scale = scale_grad_dir.init_scale();
  unsigned int num_lvl = scale_grad_dir.levels();
  int first_oct = scale_grad_dir.first_octave();

  double log2_scale = vcl_log(scale_/init_scale)/vcl_log(2.0)-first_oct;
  unsigned int index = (unsigned int)(log2_scale*num_lvl +0.5);
  int oct = index/num_lvl;
  unsigned int lvl = index%num_lvl;
  oct += first_oct;
  
  const vil_image_view<float> & grad_orient = scale_grad_dir(oct,lvl);
  const vil_image_view<float> & grad_mag =  scale_grad_mag(oct,lvl);

  // compute position relative to the current gradient image size
  float img_scale = scale_grad_dir.image_scale(oct);
  double key_x = this->x()/img_scale;
  double key_y = this->y()/img_scale;

  // compute scale relative to the current image size
  float rel_scale = float(scale_/init_scale)/img_scale;
  
  for (int hi=0; hi<4; ++hi){
    for (int hj=0; hj<4; ++hj){
      for (int i=4*hi; i<4*(hi+1); ++i){
        for (int j=4*hj; j<4*(hj+1); ++j){
          double x = ( (i-7.5)*vcl_cos(orientation_)
                      -(j-7.5)*vcl_sin(orientation_)) * rel_scale;
          double y = ( (i-7.5)*vcl_sin(orientation_)
                      +(j-7.5)*vcl_cos(orientation_)) * rel_scale;
          for(int c=0; c<4; ++c){
            int xc = int(x+key_x) + c/2;
            int yc = int(y+key_y) + c%2;
            if ( xc>=0 && xc<int(grad_orient.ni()) &&
                 yc>=0 && yc<int(grad_orient.nj()) ){

              float interp_x = 1.0f - vcl_fabs( x+key_x - float(xc) );
              float interp_y = 1.0f - vcl_fabs( y+key_y - float(yc) );
              double weight = grad_mag(xc,yc) * interp_x * interp_y
                            * gaussian((xc-key_x)/rel_scale, (yc-key_y)/rel_scale);
              float orient = grad_orient(xc,yc)-float(orientation_)+pi;

              while(orient > 2.0f*pi) orient -= 2.0f*pi;
              while(orient < 0.0f) orient += 2.0f*pi;
              int bin = ((int(orient*15/(2.0f*pi))+1)/2)%8;
              descriptor_[hi*32+hj*8+bin] += weight;
            }
          }
        }
      }
    }
  }

  this->normalize_descriptor();
}


//: Binary save self to stream.
void
dbdet_lowe_keypoint::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  dbdet_keypoint::b_write(os);
  vsl_b_write(os, scale_);
  vsl_b_write(os, orientation_);
  vsl_b_write(os, descriptor_);
}


//: Binary load self from stream.
void
dbdet_lowe_keypoint::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    dbdet_keypoint::b_read(is);
    vsl_b_read(is, scale_);
    vsl_b_read(is, orientation_);
    vsl_b_read(is, descriptor_);
    break;

  default:
    vcl_cerr << "I/O ERROR: dbdet_lowe_keypoint::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Return IO version number;
short
dbdet_lowe_keypoint::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
dbdet_keypoint*
dbdet_lowe_keypoint::clone() const
{
  return new dbdet_lowe_keypoint(*this);
}


//: Print an ascii summary to the stream
void
dbdet_lowe_keypoint::print_summary(vcl_ostream &os) const
{
  os << "dbdet_lowe_keypoint("<<x()<<", "<<y()<<", "<<scale_<<", "<<orientation_<<")";
}

